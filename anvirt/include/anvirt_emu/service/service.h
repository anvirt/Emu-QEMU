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
 *  service.h
 *  Created by Chen Zhen <cz.worker@gmail.com> on 2021/06/23.
 */

#ifndef __anvirt_service_h__
#define __anvirt_service_h__

#include <anvirt_emu/vmm.h>
#include "msg.h"

__BEGIN_DECLS

typedef struct anv_service anv_service_t;

ANV_VMM_API anv_service_t *anv_service_init(const char *name);
ANV_VMM_API void anv_service_free(anv_service_t *s);

typedef int (*anv_service_on_recv_msg_fn)(anv_service_t *s, void *ctx, const anv_ipc_msg_t *msg);
ANV_VMM_API int anv_service_on_recv_msg(anv_service_t *s, anv_service_on_recv_msg_fn on_recv,
                                        void *ctx);
ANV_VMM_API int anv_service_send_msg(anv_service_t *s, const anv_ipc_msg_t *msg);
ANV_VMM_API int anv_service_ack(anv_service_t *s);

#ifdef DEFAULT_ANVIRT_SERVICE
ANV_VMM_API int anv_service_load(anv_service_t *s, int argc, const char *argv[]);
#endif

__END_DECLS

#endif // __anvirt_service_h__