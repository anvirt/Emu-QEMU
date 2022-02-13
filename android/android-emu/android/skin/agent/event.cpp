/* Copyright (C) AnVirt Emu
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
#include "android/skin/event.h"                        // for SkinEvent, ski...
#include "android/skin/qt/emulator-no-qt-no-window.h"  // for EmulatorNoQtNo...
#include "agent.h"

#define  DEBUG  1

#if DEBUG
#include "android/utils/debug.h"                       // for VERBOSE_PRINT

#define  D(...)   VERBOSE_PRINT(surface,__VA_ARGS__)
#else
#define  D(...)   ((void)0)
#endif

extern bool skin_event_poll(SkinEvent* event) {
    int has_event = 0;
    if (anv_emu_agent_poll_event(anv_emu_agent_get(), event, &has_event)) {
        return false;
    }
    return has_event;
}


extern void skin_enable_mouse_tracking(bool enable) {
    (void)enable;
    // TODO: code here
}

extern void skin_event_add(SkinEvent* event) {
    // TODO: queue qemu internal event
}
