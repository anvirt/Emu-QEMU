// Copyright 2016 The Android Open Source Project
//
// This software is licensed under the terms of the GNU General Public
// License version 2, as published by the Free Software Foundation, and
// may be copied, distributed, and modified under those terms.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

#include "android/emulation/AdbHostListener.h"

#include "android/base/async/AsyncSocketServer.h"
#include "android/base/async/ThreadLooper.h"
#include "android/base/sockets/ScopedSocket.h"
#include "android/base/sockets/SocketUtils.h"
#include "android/emulation/AdbHostServer.h"
#include "android/emulation/AdbTypes.h"

#include <memory>
#include <vector>

#ifdef ADB_INTERNAL
#include <unistd.h>
#endif

namespace android {
namespace emulation {

static bool systemSupportsIPv4() {
    int socket = base::socketCreateTcp4();
    if (socket < 0 && errno == EAFNOSUPPORT)
        return false;
    base::socketClose(socket);
    return true;
}

using android::base::AsyncSocketServer;
using android::base::ScopedSocket;

bool AdbHostListener::reset(int adbPort) {
    if (adbPort < 0) {
        mRegularAdbServer.reset();
        mJdwpServer.reset();
    } else if (!mRegularAdbServer || adbPort != mRegularAdbServer->port()) {
        CHECK(adbPort < 65536);
        AsyncSocketServer::LoopbackMode mode =
            AsyncSocketServer::kIPv4AndOptionalIPv6;
        if (!systemSupportsIPv4()) {
            mode = AsyncSocketServer::kIPv6;
        }
        mRegularAdbServer = AsyncSocketServer::createTcpLoopbackServer(
                adbPort,
                [this](int socket) {
                    return onHostServerConnection(socket, AdbPortType::RegularAdb);
                },
                mode, android::base::ThreadLooper::get());
        if (!mJdwpServer) {
            mJdwpServer = AsyncSocketServer::createTcpLoopbackServer(
                    0,  // Get a random port
                    [this](int socket) {
                        return onHostServerConnection(socket, AdbPortType::Jdwp);
                    },
                    mode, android::base::ThreadLooper::get());
            if (!mJdwpServer) {
                dwarning("jdwp port creation fails,"
                        " Icebox will not work.");
            }
        }
        // Don't start listening until startListening() is called.
        if (!mRegularAdbServer) {
            // This can happen when the emulator is probing for a free port
            // at startup, so don't print a warning here.
            return false;
        }
    }
    return true;
}

#ifdef ADB_INTERNAL
bool AdbHostListener::reset(const char *adb_socket_path) {
    unlink(adb_socket_path);
    mRegularAdbServer = AsyncSocketServer::createUnixDomainServer(adb_socket_path,
            [this](int socket) {
                return onHostServerConnection(socket, AdbPortType::RegularAdb);
            });
    // Don't start listening until startListening() is called.
    if (!mRegularAdbServer) {
        // This can happen when the emulator is probing for a free port
        // at startup, so don't print a warning here.
        return false;
    }
    mAdbSockPath = adb_socket_path;
    return true;
}
#endif

void AdbHostListener::startListening() {
    if (mRegularAdbServer) {
        mRegularAdbServer->startListening();
    }
    if (mJdwpServer) {
        mJdwpServer->startListening();
    }
}

void AdbHostListener::stopListening() {
    if (mRegularAdbServer) {
        mRegularAdbServer->stopListening();
    }
    if (mJdwpServer) {
        mJdwpServer->stopListening();
    }
}

void AdbHostListener::notifyServer() {
#ifdef ADB_INTERNAL
    const char *sock_path = getenv("ANVIRT_EMU_ADB_SOCK_PATH");
    if (mRegularAdbServer && sock_path) {
        AdbHostServer::notify_un(mAdbSockPath.c_str(), sock_path);
    }
#else
    if (mRegularAdbServer && mAdbClientPort > 0) {
        AdbHostServer::notify(mRegularAdbServer->port(), mAdbClientPort);
    }
#endif
}

bool AdbHostListener::onHostServerConnection(int socket, AdbPortType portType) {
    mGuestAgent->onHostConnection(ScopedSocket(socket), portType);
    return true;
}

}  // namespace emulation
}  // namespace android
