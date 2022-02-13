/*
 *  Copyright (C) 2021 AnVirt
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public
 *  License along with this library; if not, see <http://www.gnu.org/licenses/>.
 * 
 *  endpoint.c
 *  Created by Chen Zhen <cz.worker@gmail.com> on 2021/10/01.
 */

#define ANV_VMM_CORE

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

#define ANV_LOG_TAG "IPC-END"
#include <anvirt_emu/base/log.h>

#include <anvirt_emu/ipc/endpoint.h>

#ifndef SZ_4K
#define SZ_4K 0x1000
#endif

struct anv_ipc_endpoint {
  int rfd;
  int wfd;

  anv_ipc_endpoint_on_recv_msg_fn on_recv;
  void *on_recv_context;
  pthread_t recv_thd;
  int quit_notify_fds[2];
};

ANV_VMM_API anv_ipc_endpoint_t *anv_ipc_endpoint_new(int rfd, int wfd) {
  anv_ipc_endpoint_t *end = TALLOC(anv_ipc_endpoint_t);
  end->on_recv = end->on_recv_context = NULL;
  end->rfd = rfd;
  end->wfd = wfd;

  end->quit_notify_fds[0] = end->quit_notify_fds[1] = -1;
  if (pipe(end->quit_notify_fds) == -1) {
    ALOGE("init quit notify failed: %s", strerror(errno));
    anv_ipc_endpoint_free(end);
    return NULL;
  }

  return end;
}

ANV_VMM_API void anv_ipc_endpoint_free(anv_ipc_endpoint_t *end) {
  if (!end)
    return;

  // wait IO loop exit
  write(end->quit_notify_fds[1], "Q", 1);
  pthread_join(end->recv_thd, NULL);

  SAFE_CLOSE(end->rfd);
  SAFE_CLOSE(end->wfd);
  SAFE_CLOSE(end->quit_notify_fds[0]);
  SAFE_CLOSE(end->quit_notify_fds[1]);
  SAFE_FREE(end);
}

static inline int __max_nfd(int a, int b) {
  return a > b ? a : b;
}

static void *recv_thread_main(void *td) {
  fd_set rfds;
  int nfds;
  int retval;

  anv_ipc_endpoint_t *end = td;

  ALOGT("wait incoming msg...");
  while (ANV_TRUE) {
    FD_ZERO(&rfds);

    nfds = 0;
    FD_SET(end->rfd, &rfds);
    nfds = __max_nfd(nfds, end->rfd);
    FD_SET(end->quit_notify_fds[0], &rfds);
    nfds = __max_nfd(nfds, end->quit_notify_fds[0]);

    retval = select(nfds + 1, &rfds, NULL, NULL, NULL);
    if (retval == -1) {
      ALOGE("IO error: %s", strerror(errno));
    } else if (retval == 0) {
      // nothing got, continue
    }

    if (FD_ISSET(end->quit_notify_fds[0], &rfds)) {
      // handle notify, just quit
      ALOGT("notify quit...");
      break;
    }

    if (FD_ISSET(end->rfd, &rfds)) {
      char rbuf[SZ_4K];
      ssize_t rs = read(end->rfd, rbuf, SZ_4K);
      if (rs < (ssize_t) sizeof(anv_ipc_msg_t) ||
          rs < (ssize_t) sizeof(anv_ipc_msg_t) + ((anv_ipc_msg_t *) rbuf)->hdr.payload_size) {
        if (rs == -1) {
          ALOGE("read error: [%d] %s", errno, strerror(errno));
          if (errno == EPIPE) {
            // pipe close, notify exit
            // TODO: just exit for now.
            exit(0);
          }
        } else if (rs == 0) {
          // EOF, notify exit
          // TODO: just exit for now.
          exit(0);
        } else {
          ALOGE("bad notify data");
        }
      } else {
        anv_ipc_msg_t *msg = (anv_ipc_msg_t *) rbuf;
        end->on_recv(end, end->on_recv_context, msg);
      }
    }
  }
  ALOGT("proc IO exit");
  return NULL;
}

ANV_VMM_API int anv_ipc_endpoint_start_recv_message(anv_ipc_endpoint_t *end,
                                                    anv_ipc_endpoint_on_recv_msg_fn on_recv,
                                                    void *ctx) {
  if (!end) {
    return -EINVAL;
  } else if (end->on_recv) {
    ALOGE("already started");
    return -EINVAL;
  }

  end->on_recv = on_recv;
  end->on_recv_context = ctx;
  int rc = pthread_create(&end->recv_thd, NULL, recv_thread_main, end);
  if (rc) {
    // reset callback
    end->on_recv = NULL;
    end->on_recv_context = NULL;
  }
  return rc;
}

ANV_VMM_API int anv_ipc_endpoint_send_message(anv_ipc_endpoint_t *end, const anv_ipc_msg_t *msg) {
  if (!end)
    return -EINVAL;
  else if (end->wfd == -1) {
    return -EBADF;
  }

  return write(end->wfd, msg, sizeof(anv_ipc_msg_t) + msg->hdr.payload_size);
}