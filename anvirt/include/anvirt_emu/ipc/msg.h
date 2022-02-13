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
 *  Created by Chen Zhen <cz.worker@gmail.com> on 2021/09/30.
 */

#ifndef __anvirt_ipc_msg_h__
#define __anvirt_ipc_msg_h__

#include <anvirt_emu/vmm.h>

__BEGIN_DECLS

#pragma pack(push, 1)

struct anv_ipc_msg_header {
  anv_u32 t;
  anv_u32 payload_size;
};

typedef struct anv_ipc_msg {
  struct anv_ipc_msg_header hdr;
  char payload[0];
} anv_ipc_msg_t;

#pragma pack(pop)

ANV_VMM_API anv_ipc_msg_t *anv_ipc_msg_alloc(anv_u16 t, anv_u16 payload_size);
ANV_VMM_API void anv_ipc_msg_free(anv_ipc_msg_t *msg);

__END_DECLS

#endif // __anvirt_ipc_msg_h__