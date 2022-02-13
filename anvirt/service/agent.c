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
 *  agent.c
 *  Created by Chen Zhen <cz.worker@gmail.com> on 2021/10/09.
 */

// TIP: this src is not linked in service.

#define ANV_VMM_CORE

#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#define ANV_LOG_TAG "agent"
#include <anvirt_emu/base/log.h>
#include <anvirt_emu/service/agent.h>
#include <anvirt_emu/ipc/endpoint.h>

// define DEBUG_STDOUT to debug stdout/stderr
#ifdef DEBUG_STDOUT
#define CLOSE_STDOUT (0)
#else
#define CLOSE_STDOUT (1)
#endif

static const int kMaxPathLen = 256;
static const int kMaxNameLen = 256;

#define SERVICE_PROC_NAME "anvs"

struct anv_service_agent {
  char exec[kMaxPathLen];
  char name[kMaxNameLen];
  int pid;

  anv_ipc_endpoint_t *end;
  anv_bool ready;
  // on recv
  anv_service_agent_on_recv_msg_fn on_recv;
  void *on_recv_ctx;

  void (*free)(anv_service_agent_t *agent);
  // const char *(*exec)(anv_service_agent_t *agent);
  void (*child_process_init)(anv_service_agent_t *agent);
};

typedef struct anv_service_agent_default {
  struct anv_service_agent base;
  char search_path[kMaxPathLen];
} anv_service_agent_default_t;

static void agent_default_free(anv_service_agent_t *agent) {
  ANV_UNUSED(agent);
}

static void agent_default_child_process_init(anv_service_agent_t *agent) {
  anv_service_agent_default_t *agent_default = (anv_service_agent_default_t *) agent;
  setenv("ANVS_SEARCH_PATH", agent_default->search_path, 1);
}

typedef struct anv_service_agent_custom {
  struct anv_service_agent base;
} anv_service_agent_custom_t;

static void agent_custom_free(anv_service_agent_t *agent) {
  ANV_UNUSED(agent);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// API
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ANV_VMM_API anv_service_agent_t *anv_service_new_agent(const char *name, const char *exec,
                                                       const char *search_path) {
  anv_service_agent_default_t *agent = TALLOC(anv_service_agent_default_t);
  agent->base.pid = -1;
  agent->base.end = NULL;
  agent->base.ready = ANV_FALSE;
  agent->base.on_recv = NULL;
  agent->base.on_recv_ctx = NULL;
  do {
    if (strlcpy(agent->base.name, name, kMaxNameLen) >= kMaxNameLen) {
      ALOGE("service name too long");
      break;
    } else if (strlcpy(agent->base.exec, exec, kMaxPathLen) >= kMaxPathLen) {
      ALOGE("service exec path too long");
      break;
    } else if (strlcpy(agent->search_path, search_path, kMaxPathLen) >= kMaxPathLen) {
      ALOGE("service search path too long");
      break;
    }

    // success
    agent->base.free = agent_default_free;
    agent->base.child_process_init = agent_default_child_process_init;
    return &agent->base;
  } while (0);

  // failed
  anv_service_agent_free(&agent->base);
  return NULL;
}
ANV_VMM_API anv_service_agent_t *anv_service_new_custom_agent(const char *name, const char *exec) {
  anv_service_agent_custom_t *agent = TALLOC(anv_service_agent_custom_t);
  agent->base.pid = -1;
  agent->base.end = NULL;
  agent->base.ready = ANV_FALSE;
  agent->base.on_recv = NULL;
  agent->base.on_recv_ctx = NULL;
  do {
    if (strlcpy(agent->base.name, name, kMaxNameLen) >= kMaxNameLen) {
      ALOGE("service name too long");
      break;
    } else if (strlcpy(agent->base.exec, exec, kMaxPathLen) >= kMaxPathLen) {
      ALOGE("service exec path too long");
      break;
    }

    ALOGT("agent: %s (%s)", agent->base.name, agent->base.exec);

    // success
    agent->base.free = agent_custom_free;
    agent->base.child_process_init = NULL;
    return &agent->base;
  } while (0);

  // failed
  anv_service_agent_free(&agent->base);
  return NULL;
}

ANV_VMM_API void anv_service_agent_free(anv_service_agent_t *agent) {
  if (!agent)
    return;

  int pid = agent->pid;
  if (agent->free) {
    agent->free(agent);
  }

  if (pid != -1) {
    // notify service quit
    ALOGT("notify service [%s] quit", agent->name);
    anv_service_msg_quit_t quit_msg;
    quit_msg.base.hdr.t = anvs_msg_quit;
    quit_msg.base.hdr.payload_size = 0;
    anv_service_send_msg(agent, &quit_msg.base);
    // kill(pid, SIGTERM);

    // TODO: do not care about exit status for now. check later. [zhen.chen]
    // waitpid() ref: https://linux.die.net/man/2/waitpid
#if 0
    int status;
    ALOGT("wait service [%s][%d] quit", agent->name, pid);
    waitpid(pid, &status, 0);
    ALOGT("service [%s][%d] quit with status: %d", agent->name, pid, status);
#endif
  }

  anv_ipc_endpoint_free(agent->end);
  free(agent);
}

ANV_VMM_API int anv_service_agent_on_recv_msg(anv_service_agent_t *agent,
                                              anv_service_agent_on_recv_msg_fn on_recv, void *ctx) {
  if (!agent) {
    return -EINVAL;
  } else if (agent->on_recv || agent->on_recv_ctx) {
    return -EINVAL;
  } else {
    agent->on_recv = on_recv;
    agent->on_recv_ctx = ctx;
    return 0;
  }
}

ANV_VMM_API int anv_service_send_msg(anv_service_agent_t *agent, const anv_ipc_msg_t *msg) {
  if (!agent) {
    return -EINVAL;
  } else if (!agent->end) {
    return -EBUSY;
  } else if (!msg) {
    return 0; // skip null msg
  } else {
    return anv_ipc_endpoint_send_message(agent->end, msg);
  }
}

static int on_recv_msg(anv_ipc_endpoint_t *end, void *ctx, const anv_ipc_msg_t *msg) {
  ANV_UNUSED(end);
  anv_service_agent_t *agent = (anv_service_agent_t *) ctx;
  switch (msg->hdr.t) {
  case anvs_msg_ack:
    agent->ready = ANV_TRUE;
    ALOGT("service [%s] ready!", agent->name);
    break;
  default:
    break;
  }
  if (agent->on_recv) {
    return agent->on_recv(agent, agent->on_recv_ctx, msg);
  }
  return 0;
}

ANV_VMM_API int anv_service_agent_start(anv_service_agent_t *agent, int argc, const char *argv[]) {
  int rc = 0;
  int rfds[2], wfds[2];
  anv_ipc_endpoint_t *end = NULL;
  do {
    rfds[0] = rfds[1] = wfds[0] = wfds[1] = -1;

    if ((rc = pipe(rfds)) == -1) {
      ALOGE("can not setup input pipe: %s", strerror(errno));
      break;
    }
    if ((rc = pipe(wfds)) == -1) {
      ALOGE("can not setup output pipe: %s", strerror(errno));
      break;
    }

    ALOGT("pipe: r: %d-%d", rfds[0], rfds[1]);
    ALOGT("pipe: w: %d-%d", wfds[0], wfds[1]);
    end = anv_ipc_endpoint_new(rfds[0], wfds[1]);
    if (!end) {
      ALOGE("can not create endpoint: %s", strerror(errno));
      break;
    } else if ((rc = anv_ipc_endpoint_start_recv_message(end, on_recv_msg, agent))) {
      ALOGE("can not init endpoint: %s", strerror(errno));
      break;
    }

    //
    pid_t pid = fork();
    if (pid == -1) {
      ALOGE("can not fork service process: %s", strerror(errno));
      rc = -1;
      break;
    }

    if (pid == 0) {
      // 子进程 (service)
      close(rfds[0]);
      close(wfds[1]);

      char in_fd[16], out_fd[16];
      snprintf(in_fd, 16, "%d", wfds[0]);
      snprintf(out_fd, 16, "%d", rfds[1]);
      setenv("ANVS_IN_FD", in_fd, 1);
      setenv("ANVS_OUT_FD", out_fd, 1);

      if (agent->child_process_init) {
        agent->child_process_init(agent);
      }

      // chdir("/");
      const char *null_file = "/dev/null";
      // close stdin
      {
        int fd = open(null_file, O_RDONLY);
        if (fd == -1) {
          ALOGE("failed to open: %s", null_file);
        } else {
          if (TEMP_FAILURE_RETRY(dup2(fd, STDIN_FILENO)) == -1) {
            ALOGE("failed to redirect stdin to %s", null_file);
          }
          close(fd);
        }
      }
#if CLOSE_STDOUT
      // close stdout/stderr
      {
        int fd = open(null_file, O_WRONLY);
        if (fd == -1) {
          ALOGE("failed to open: %s", null_file);
        } else {
          if (TEMP_FAILURE_RETRY(dup2(fd, STDOUT_FILENO)) == -1) {
            ALOGE("failed to redirect stdout to %s", null_file);
          }
          if (TEMP_FAILURE_RETRY(dup2(fd, STDERR_FILENO)) == -1) {
            ALOGE("failed to redirect stderr to %s", null_file);
          }
          close(fd);
        }
      }
#endif
      const char **exec_args = malloc((argc + 4) * sizeof(char *));
      exec_args[0] = SERVICE_PROC_NAME;
      exec_args[1] = "--name";
      exec_args[2] = agent->name;
      for (int i = 0; i < argc; ++i) {
        exec_args[i + 3] = argv[i];
      }
      exec_args[argc + 3] = NULL;
      // child process
      int result = execv(agent->exec, (char **) exec_args);
      // this should not return
      ALOGE("service exec return %d: %s (%s)", result, strerror(errno), agent->exec);
      // return 0;
    } else {
      // 父进程(agent)
      close(rfds[1]);
      close(wfds[0]);
      agent->end = end;
      agent->pid = pid;
      ALOGT("proc: %d", agent->pid);
    }

  } while (0);

  if (rc) {
    // start failed, clear
    SAFE_CLOSE(rfds[0]);
    SAFE_CLOSE(rfds[1]);
    SAFE_CLOSE(wfds[0]);
    SAFE_CLOSE(wfds[1]);

    anv_ipc_endpoint_free(end);
  }
  return rc;
}

ANV_VMM_API anv_bool anv_service_agent_is_service_ready(anv_service_agent_t *agent) {
  return agent && agent->ready;
}