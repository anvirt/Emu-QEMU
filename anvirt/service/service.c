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
 *  service.c
 *  Created by Chen Zhen <cz.worker@gmail.com> on 2021/06/23.
 */

#define ANV_VMM_CORE
#define ANV_LOG_TAG "service"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>

#define DEFAULT_ANVIRT_SERVICE

#include <anvirt_emu/base/log.h>
#include <anvirt_emu/service/service.h>
#include <anvirt_emu/service/anvs_api.h>
#include <anvirt_emu/ipc/endpoint.h>

static const int kMaxNameLen = 256;

#if 0
static char SERVICE_PID_FILE[32] = {'\0'};

static void write_pid() {
  FILE *f = fopen(SERVICE_PID_FILE, "w");
  pid_t pid = getpid();
  fprintf(f, "%d", pid);
  fclose(f);
}

static void clean_pid() {
  unlink(SERVICE_PID_FILE);
}
#endif

struct anv_service {
  char name[kMaxNameLen];
  anv_ipc_endpoint_t *end;
  anvs_context_t *ctx;

  anv_service_on_recv_msg_fn on_recv;
  void *on_recv_ctx;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// API
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern anvs_context_t *anvs_context_init(const char *name, int argc, const char *argv[]);
extern int anvs_context_run_loop(anvs_context_t *ctx);

ANV_VMM_API anv_service_t *anv_service_init(const char *name) {
  anv_service_t *s = TALLOC(anv_service_t);
  s->end = NULL;
  s->ctx = NULL;
  s->on_recv = NULL;
  s->on_recv_ctx = NULL;

  int rfd = -1, wfd = -1;
  anv_ipc_endpoint_t *end = NULL;
  do {
    if (strlcpy(s->name, name, kMaxNameLen) >= kMaxNameLen) {
      ALOGE("service name too long");
      break;
    }

    const char *in_fd = getenv("ANVS_IN_FD");
    if (!in_fd) {
      break;
    }
    sscanf(in_fd, "%d", &rfd);
    if (rfd == -1) {
      break;
    }

    const char *out_fd = getenv("ANVS_OUT_FD");
    if (!out_fd) {
      break;
    }
    sscanf(out_fd, "%d", &wfd);
    if (wfd == -1) {
      break;
    }

    ALOGT("fds: r/%d w/%d", rfd, wfd);
    end = anv_ipc_endpoint_new(rfd, wfd);
    if (!end) {
      ALOGE("can not create endpoint: %s", strerror(errno));
      break;
    }

    if (setsid() == -1 && errno != EPERM) {
      ALOGE("setsid() failed");
      break;
    }

// TIP: may be should limit name len, later. [zhen.chen]
#if 0 // TODO: 需要确定working path或者指定可写入的路径
    sprintf(SERVICE_PID_FILE, "%s.pid", name);
    write_pid();
    atexit(clean_pid);
#endif

    // success
    s->end = end;
    return s;
  } while (0);

  // failed
  SAFE_CLOSE(rfd);
  SAFE_CLOSE(wfd);
  anv_service_free(s);
  return NULL;
}

ANV_VMM_API void anv_service_free(anv_service_t *s) {
  if (!s)
    return;
  anv_ipc_endpoint_free(s->end);

  // TODO: free s->ctx here

  free(s);
}

static int on_recv_msg(anv_ipc_endpoint_t *end, void *ctx, const anv_ipc_msg_t *msg) {
  ANV_UNUSED(end);
  int rc = 0;
  anv_service_t *s = (anv_service_t *) ctx;
  if (s->on_recv) {
    rc = s->on_recv(s, s->on_recv_ctx, msg);
  }

  if (msg->hdr.t == anvs_msg_quit) {
    ALOGT("i am quit, bye!");
    exit(0);
  }
  return rc;
}

ANV_VMM_API int anv_service_on_recv_msg(anv_service_t *s, anv_service_on_recv_msg_fn on_recv,
                                        void *ctx) {
  if (!s) {
    return -EINVAL;
  } else if (s->on_recv || s->on_recv_ctx) {
    return -EINVAL;
  } else {
    s->on_recv = on_recv;
    s->on_recv_ctx = ctx;
    return anv_ipc_endpoint_start_recv_message(s->end, on_recv_msg, s);
  }
}

ANV_VMM_API int anv_service_send_msg(anv_service_t *s, const anv_ipc_msg_t *msg) {
  if (!s) {
    return -EINVAL;
  } else if (!s->end) {
    return -EBUSY;
  } else if (!msg) {
    return 0; // skip null msg
  } else {
    return anv_ipc_endpoint_send_message(s->end, msg);
  }
}

ANV_VMM_API int anv_service_ack(anv_service_t *s) {
  anv_service_msg_ack_t msg;
  msg.base.hdr.t = anvs_msg_ack;
  msg.base.hdr.payload_size = 0;
  return anv_service_send_msg(s, &msg.base);
}

ANV_VMM_API int anv_service_load(anv_service_t *s, int argc, const char *argv[]) {
  anvs_context_t *sctx = anvs_context_init(s->name, argc, argv);
  if (!sctx) {
    return -1;
  }

  s->ctx = sctx;

  anvs_context_run_loop(sctx);
  ALOGI("service loaded");
  return 0;
}
