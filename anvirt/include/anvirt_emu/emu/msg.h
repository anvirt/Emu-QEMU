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
 *  msg.h
 *  Created by Chen Zhen <cz.worker@gmail.com> on 2021/10/10.
 */

#ifndef __anvirt_emu_msg_h__
#define __anvirt_emu_msg_h__

#include <anvirt_emu/ipc/msg.h>

__BEGIN_DECLS

#define ANV_EMU_MSG(_t) ((_t) | (0b10 << 24))

enum anv_emu_msg_type {
  AnvEmuMsgRunVM = ANV_EMU_MSG(1),
};

#pragma pack(push, 1)

typedef struct anv_emu_msg_run_vm {
  anv_ipc_msg_t base;
  // TODO: params here
} anv_emu_msg_run_vm_t;

#pragma pack(pop)

__END_DECLS

#endif // __anvirt_emu_msg_h__
