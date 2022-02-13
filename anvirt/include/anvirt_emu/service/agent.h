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
 *  agent.h
 *  Created by Chen Zhen <cz.worker@gmail.com> on 2021/10/09.
 */

#ifndef __anvirt_service_agent_h__
#define __anvirt_service_agent_h__

#include <anvirt_emu/vmm.h>
#include "msg.h"

__BEGIN_DECLS

typedef struct anv_service_agent anv_service_agent_t;

ANV_VMM_API anv_service_agent_t *anv_service_new_agent(const char *name, const char *exec,
                                                       const char *search_path);
ANV_VMM_API anv_service_agent_t *anv_service_new_custom_agent(const char *name, const char *exec);
ANV_VMM_API void anv_service_agent_free(anv_service_agent_t *agent);

typedef int (*anv_service_agent_on_recv_msg_fn)(anv_service_agent_t *agent, void *ctx,
                                                const anv_ipc_msg_t *msg);
ANV_VMM_API int anv_service_on_recv_msg(anv_service_agent_t *agent,
                                        anv_service_agent_on_recv_msg_fn on_recv, void *ctx);
ANV_VMM_API int anv_service_send_msg(anv_service_agent_t *agent, const anv_ipc_msg_t *msg);

ANV_VMM_API int anv_service_agent_start(anv_service_agent_t *agent, int argc, const char *argv[]);
ANV_VMM_API anv_bool anv_service_agent_is_service_ready(anv_service_agent_t *agent);

__END_DECLS

#endif // __anvirt_service_agent_h__