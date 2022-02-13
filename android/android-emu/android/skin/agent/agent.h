/* Copyright (C) AnVirt Emu
** agent.h
** Created by Chen Zhen <cz.worker@gmail.com> on 2021/10/26.
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

#ifndef __anvirt_emu_agent_h__
#define __anvirt_emu_agent_h__

#include <errno.h>
#include <stdint.h>

#include "android/skin/event.h" // qemu/android/android-emu
#include "def.h"
#include "version.h"

#define ANV_EMU_AGENT_MAGIC 0x54474153 // AEGT

__BEGIN_DECLS

typedef struct anv_emu_agent anv_emu_agent_t;
typedef void (*anv_emu_start_fn)(int argc, char **argv);
typedef void (*anv_emu_ui_thread_fn)(void *data);

#pragma pack(push, 1)

struct anv_emu_agent {
  int magic;   // ANV_EMU_AGENT_MAGIC
  int version; // ANV_EMU_AGENT_VERSION

  void (*close)(anv_emu_agent_t *agt);
  int (*win_id)(anv_emu_agent_t *agt, void **pwid);
  int (*start)(anv_emu_agent_t *agt);
  int (*run_main_loop)(anv_emu_agent_t *agt);
  int (*spawn_thread)(anv_emu_agent_t *agt, anv_emu_start_fn f, int argc, char **argv);

  int (*run_on_ui_thread)(anv_emu_agent_t *agt, anv_emu_ui_thread_fn f, void *data, int wait);
  void (*show_error)(anv_emu_agent_t *agt, const char *title, const char *msg);
  void (*request_close)(anv_emu_agent_t *agt);
  int (*set_window_pos)(anv_emu_agent_t *agt, int x, int y);
  int (*set_window_size)(anv_emu_agent_t *agt, int w, int h);

  int (*poll_event)(anv_emu_agent_t *agt, SkinEvent *event, int *has_event);

  void (*install_progress)(anv_emu_agent_t *agt, double progress, int done);
  void (*push_progress)(anv_emu_agent_t *agt, double progress, int done);

  void (*boot_complete)(anv_emu_agent_t *agt);
};

#pragma pack(pop)

ANVIRT_EMU_AGENT_API anv_emu_agent_t *anv_emu_agent_get(void);
ANVIRT_EMU_AGENT_API anv_bool anv_emu_agent_is_alive(void);
ANVIRT_EMU_AGENT_API int anv_emu_agent_install(anv_emu_agent_t *agt, const char *apk_file);
ANVIRT_EMU_AGENT_API void anv_emu_agent_cancel_install(anv_emu_agent_t *agt);
ANVIRT_EMU_AGENT_API int anv_emu_agent_shutdown(anv_emu_agent_t *agt);

static inline int anv_emu_agent_is_valid(anv_emu_agent_t *agt) {
  return agt && agt->magic == ANV_EMU_AGENT_MAGIC;
}

static inline int anv_emu_agent_win_id(anv_emu_agent_t *agt, void **pwid) {
  return agt && agt->win_id ? agt->win_id(agt, pwid) : -EINVAL;
}

static inline int anv_emu_agent_start(anv_emu_agent_t *agt) {
  return agt && agt->start ? agt->start(agt) : -EINVAL;
}

static inline int anv_emu_agent_run_main_loop(anv_emu_agent_t *agt) {
  return agt && agt->run_main_loop ? agt->run_main_loop(agt) : -EINVAL;
}

static inline int anv_emu_agent_spawn_thread(anv_emu_agent_t *agt, anv_emu_start_fn f, int argc, char **argv) {
  return agt && agt->spawn_thread ? agt->spawn_thread(agt, f, argc, argv) : -EINVAL;
}

static inline int anv_emu_agent_run_on_ui_thread(anv_emu_agent_t *agt, anv_emu_ui_thread_fn f, void *data, int wait) {
  return agt && agt->run_on_ui_thread ? agt->run_on_ui_thread(agt, f, data, wait): -EINVAL;
}

static inline int anv_emu_agent_set_window_pos(anv_emu_agent_t *agt, int x, int y) {
  return agt && agt->set_window_pos ? agt->set_window_pos(agt, x, y): -EINVAL;
}

static inline int anv_emu_agent_set_window_size(anv_emu_agent_t *agt, int w, int h) {
  return agt && agt->set_window_size ? agt->set_window_size(agt, w, h): -EINVAL;
}

static inline int anv_emu_agent_poll_event(anv_emu_agent_t *agt, SkinEvent *event, int *has_event) {
  return agt && agt->poll_event ? agt->poll_event(agt, event, has_event): -EINVAL;
}

static inline void anv_emu_agent_show_error(anv_emu_agent_t *agt, const char *title, const char *msg) {
  if (!agt) return;
  if (agt->show_error) agt->show_error(agt, title, msg);
}

static inline void anv_emu_agent_request_close(anv_emu_agent_t *agt) {
  if (!agt) return;
  if (agt->request_close) agt->request_close(agt);
}

static inline void anv_emu_agent_install_progress(anv_emu_agent_t *agt, double prog, int done) {
  if (agt && agt->install_progress) agt->install_progress(agt, prog, done);
}

static inline void anv_emu_agent_push_progress(anv_emu_agent_t *agt, double prog, int done) {
  if (agt && agt->push_progress) agt->push_progress(agt, prog, done);
}

static inline void anv_emu_agent_boot_complete(anv_emu_agent_t *agt) {
  if (agt && agt->boot_complete) agt->boot_complete(agt);
}

#define SAFE_FREE_AGENT(_agt) \
  do {                        \
    if (!_agt)                \
      break;                  \
    if (_agt->close)          \
      _agt->close(_agt);      \
    _agt = NULL;              \
  } while (0);

__END_DECLS

#endif // __anvirt_emu_agent_h__
