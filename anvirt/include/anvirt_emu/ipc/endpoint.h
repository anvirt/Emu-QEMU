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
 *  endpoint.h
 *  Created by Chen Zhen <cz.worker@gmail.com> on 2021/10/01.
 */

// IPC节点，提供基础的消息收发机制
// MacOS平台下基于file descriptor读写消息数据，通常使用管道(pipe)

#ifndef __anvirt_ipc_endpoint_h__
#define __anvirt_ipc_endpoint_h__

#include <anvirt_emu/vmm.h>
#include "msg.h"

__BEGIN_DECLS

typedef struct anv_ipc_endpoint anv_ipc_endpoint_t;
typedef int (*anv_ipc_endpoint_on_recv_msg_fn)(anv_ipc_endpoint_t *end, void *ctx,
                                               const anv_ipc_msg_t *msg);

ANV_VMM_API anv_ipc_endpoint_t *anv_ipc_endpoint_new(int rfd, int wfd);
ANV_VMM_API void anv_ipc_endpoint_free(anv_ipc_endpoint_t *end);

ANV_VMM_API int anv_ipc_endpoint_start_recv_message(anv_ipc_endpoint_t *end,
                                                    anv_ipc_endpoint_on_recv_msg_fn on_recv,
                                                    void *ctx);
ANV_VMM_API int anv_ipc_endpoint_send_message(anv_ipc_endpoint_t *end, const anv_ipc_msg_t *msg);

__END_DECLS

#endif // __anvirt_ipc_endpoint_h__