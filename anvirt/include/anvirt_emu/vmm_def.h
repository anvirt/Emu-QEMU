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
 *  vmm_def.h
 *  Created by Chen Zhen <cz.worker@gmail.com> on 2021/05/17.
 */

#ifndef anvirt_vmm_def_h
#define anvirt_vmm_def_h

// ANV_VMM_DLL should define if build shared lib on windows.
#if defined(ANV_VMM_DLL)
// WAVEFORMCODEC_CORE should define in core src
#if defined(ANV_VMM_CORE)
#define ANV_VMM_API __declspec(dllexport)
#else
#define ANV_VMM_API __declspec(dllimport)
#endif
#else
#define ANV_VMM_API extern __attribute__((visibility("default")))
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

typedef long long anv_i64;
typedef int anv_i32;
typedef short anv_i16;
typedef char anv_i8;
typedef unsigned long long anv_u64;
typedef unsigned int anv_u32;
typedef unsigned short anv_u16;
typedef unsigned char anv_u8;

typedef unsigned long anv_size;

#ifndef NULL
#define NULL (0x0)
#endif

#ifndef HAVE_ANV_BOOL
#define HAVE_ANV_BOOL
typedef unsigned char anv_bool;
#define ANV_TRUE ((anv_bool) 0x1)
#define ANV_FALSE ((anv_bool) 0x0)
#endif

// mark var to unused
#define ANV_UNUSED(_var) ((void) _var)

// empty str check
#define IS_STR_EMPTY(str) ((str) == NULL || (str)[0] == '\0')

// safe free object from alloc family (malloc/calloc...).
#ifndef SAFE_FREE
#define SAFE_FREE(buf) \
  do {                 \
    if (buf) {         \
      free(buf);       \
      buf = NULL;      \
    }                  \
  } while (0)
#endif

// safe close file descriptor (open/pipe/...).
#ifndef SAFE_CLOSE
#define SAFE_CLOSE(_fd) \
  do {                  \
    if (_fd != -1) {    \
      close(_fd);       \
      _fd = -1;         \
    }                   \
  } while (0)
#endif

// alloc for struct via malloc
#define TALLOC(_type) (_type *) malloc(sizeof(_type))
// alloc array for type ptr via malloc
#define TALLOC_PTR_A(_type, _count) (_type **) malloc(sizeof(_type *) * (_count))

#ifndef TEMP_FAILURE_RETRY
#define TEMP_FAILURE_RETRY(exp)            \
  ({                                       \
    int _rc;                               \
    do {                                   \
      _rc = (exp);                         \
    } while (_rc == -1 && errno == EINTR); \
    _rc;                                   \
  })
#endif

#ifndef container_of
#define container_of(_ptr, _type, _member) \
  ((_type *) (((char *) (_ptr)) - (char *) &((((_type *) 0x0)->_member))))
#endif

#endif // anvirt_vmm_def_h
