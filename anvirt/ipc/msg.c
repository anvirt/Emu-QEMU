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
 *  msg.c
 *  Created by Chen Zhen <cz.worker@gmail.com> on 2021/09/30.
 */

#define ANV_VMM_CORE

#include <stdlib.h>

#define ANV_LOG_TAG "app-proc-msg"
#include <anvirt_emu/base/log.h>

#include <anvirt_emu/ipc/msg.h>

ANV_VMM_API anv_ipc_msg_t *anv_ipc_msg_alloc(anv_u16 t, anv_u16 payload_size) {
  anv_ipc_msg_t *msg = malloc(sizeof(anv_ipc_msg_t) + payload_size);
  msg->hdr.t = t;
  msg->hdr.payload_size = payload_size;
  return msg;
}

ANV_VMM_API void anv_ipc_msg_free(anv_ipc_msg_t *msg) {
  SAFE_FREE(msg);
}
