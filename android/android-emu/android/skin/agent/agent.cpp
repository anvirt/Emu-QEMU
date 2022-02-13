/* Copyright (C) AnVirt Emu
** agent.cpp
** Created by Chen Zhen <cz.worker@gmail.com> on 2021/11/03.
**
** This software is licensed under the terms of the GNU General Public
** License version 2, as published by the Free Software Foundation, and
** may be copied, distributed, and modified under those terms.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
*/

#include <string.h>
#include <stdlib.h>
#include "agent.h"

#include "android/emulation/control/ApkInstaller.h"
#include "android/base/memory/OnDemand.h"

#ifndef container_of
#include <stddef.h> // offsetof
#define container_of(ptr, type, field) \
	((type *)(((char *)(ptr)) - offsetof(type, field)))
#endif

class AgentHelper;

__BEGIN_DECLS

typedef struct anv_emu_agent_impl {
  anv_emu_agent_t base;
  AgentHelper *helper;
} anv_emu_agent_impl_t;

__END_DECLS

class AgentHelper {
public:
  AgentHelper():
    mAdbInterface([this] { return android::emulation::AdbInterface::createGlobalOwnThread(); }),
    mApkInstaller([this] { return (*mAdbInterface); }) {}

  ~AgentHelper() {
    cancel();
  }

public:
  int install(const char *apk_file) {
    if (mApkInstallCommand && mApkInstallCommand->inFlight()) {
      return;
    }

    mApkInstallCommand = mApkInstaller->install(
      apk_file,
      [this](android::emulation::ApkInstaller::Result result, std::string errorString) {
        dinfo("[agent] install apk result: %d, msg: %s", result, errorString.c_str());
        // 结果回调
        anv_emu_agent_install_progress(anv_emu_agent_get(), .0f, 1);
        if (result != android::emulation::ApkInstaller::Result::kSuccess) {
          // 错误信息
          anv_emu_agent_show_error(anv_emu_agent_get(), "Install APK", errorString.c_str());
        }
      });
  }

  void cancel() {
    if (mApkInstallCommand && mApkInstallCommand->inFlight()) {
        mApkInstallCommand->cancel();
    }
  }

  void request_shutdown() {
    (*mAdbInterface)->runAdbCommand(
      {"reboot", "-p"},
      [](const android::emulation::OptionalAdbCommandResult&) {
        // TODO: shutdown host maybe
        dinfo("request shutdown finish");
      },
      5000,
      false
    );
  }

private:
  android::base::MemberOnDemandT<
          android::emulation::AdbInterface*,
          android::emulation::AdbInterface*>
          mAdbInterface;
  android::emulation::AdbCommandPtr mApkInstallCommand;
  android::base::MemberOnDemandT<android::emulation::ApkInstaller,
                                  android::emulation::AdbInterface*>
          mApkInstaller;
};

anv_emu_agent_impl_t *g_agent = NULL;

static void _close(anv_emu_agent_t *agt) {
  if (!agt) return;

  anv_emu_agent_impl_t *impl = container_of(agt, anv_emu_agent_impl_t, base);
  delete impl->helper;
  free(impl);
  if (g_agent == impl) {
    g_agent = NULL;
  }
}

static inline void anv_emu_agent_setup(anv_emu_agent_impl_t *agt) {
  if (!agt) return;
  memset(agt, 0x0, sizeof(anv_emu_agent_impl_t));
  agt->base.magic = ANV_EMU_AGENT_MAGIC;
  agt->base.version = ANV_EMU_AGENT_VERSION;
  agt->base.close = _close;

  agt->helper = new AgentHelper;
}

ANVIRT_EMU_AGENT_API anv_emu_agent_t *anv_emu_agent_get(void) {
  if (!g_agent) {
    g_agent = (anv_emu_agent_impl_t *) malloc(sizeof(anv_emu_agent_impl_t));
    anv_emu_agent_setup(g_agent);
  }
  return &g_agent->base;
}

ANVIRT_EMU_AGENT_API anv_bool anv_emu_agent_is_alive(void) { return g_agent != NULL; }

ANVIRT_EMU_AGENT_API int anv_emu_agent_install(anv_emu_agent *agt, const char *apk_file) {
  if (!agt) return;
  anv_emu_agent_impl_t *impl = container_of(agt, anv_emu_agent_impl_t, base);
  return impl->helper->install(apk_file);
}

ANVIRT_EMU_AGENT_API void anv_emu_agent_cancel_install(anv_emu_agent *agt) {
  if (!agt) return;
  anv_emu_agent_impl_t *impl = container_of(agt, anv_emu_agent_impl_t, base);
  impl->helper->cancel();
}

ANVIRT_EMU_AGENT_API int anv_emu_agent_shutdown(anv_emu_agent_t *agt) {
  if (!agt) return;
  anv_emu_agent_impl_t *impl = container_of(agt, anv_emu_agent_impl_t, base);
  impl->helper->request_shutdown();
}
