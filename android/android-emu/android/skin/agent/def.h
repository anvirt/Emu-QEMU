/* Copyright (C) AnVirt Emu
** def.h
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

#ifndef __anvirt_emu_agent_def_h__
#define __anvirt_emu_agent_def_h__

// ANVIRT_EMU_AGENT_DLL should define if build shared lib on windows.
#if defined(ANVIRT_EMU_AGENT_DLL)
// ANVIRT_EMU_AGENT_CORE should define in core src
#if defined(ANVIRT_EMU_AGENT_CORE)
#define ANVIRT_EMU_AGENT_API __declspec(dllexport)
#else
#define ANVIRT_EMU_AGENT_API __declspec(dllimport)
#endif
#else
#define ANVIRT_EMU_AGENT_API extern __attribute__((visibility("default")))
#endif

#ifndef __BEGIN_DECLS
#ifdef __cplusplus
#define __BEGIN_DECLS extern "C" {
#else
#define __BEGIN_DECLS
#endif
#endif

#ifndef __END_DECLS
#ifdef __cplusplus
#define __END_DECLS }
#else
#define __END_DECLS
#endif
#endif

#ifndef NULL
#define NULL ((void *) 0x0L)
#endif

#ifndef HAVE_ANV_BOOL
typedef unsigned char anv_bool;
#define ANV_TRUE ((anv_bool) 0x1)
#define ANV_FALSE ((anv_bool) 0x0)
#define HAVE_ANV_BOOL
#endif

#endif // __anvirt_emu_agent_def_h__
