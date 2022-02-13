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
 *  anvs_api.h
 *  Created by Chen Zhen <cz.worker@gmail.com> on 2021/06/23.
 */

#ifndef __anvirt_anvs_api_h__
#define __anvirt_anvs_api_h__

#include <anvirt_emu/vmm.h>

#ifdef __cplusplus
extern "C" {
#endif

struct anvs_context;

typedef void (*anvs_free_fn)(struct anvs_context *ctx);
typedef int (*anvs_loop_fn)(struct anvs_context *ctx);

struct anvs_context {
  anvs_free_fn free;
  anvs_loop_fn loop;
  void *handle;
};
typedef struct anvs_context anvs_context_t;

typedef anvs_context_t *(*anvs_main_fn)(int argc, const char *argv[]);

#ifdef __cplusplus
}
#endif

#endif // __anvirt_anvs_api_h__