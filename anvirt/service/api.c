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
 *  api.c
 *  Created by Chen Zhen <cz.worker@gmail.com> on 2021/06/23.
 */

#define ANV_VMM_CORE
#define ANV_LOG_TAG "anvs-api"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <anvirt_emu/service/anvs_api.h>
#include <anvirt_emu/base/log.h>

#if defined(__APPLE__)
#define DLL_NAME_FMT_LEN (9)
#elif defined(__linux__)
#define DLL_NAME_FMT_LEN (6)
#elif defined(_WIN32)
#define DLL_NAME_FMT_LEN (4)
#endif

#if defined(_WIN32)
// TODO: windows support
#error windows support
#define OS_PATH_SEPARATORS "\\/"
#define OS_PATH_SEPARATOR '\\'
#define OS_PATH_SEPARATOR_STR "\\"
#define ENV_PATH_SEPARATOR_STR ";"
#else
#define OS_PATH_SEPARATORS "/"
#define OS_PATH_SEPARATOR '/'
#define OS_PATH_SEPARATOR_STR "/"
#define ENV_PATH_SEPARATOR_STR ":"

#include <dlfcn.h>
static inline void *_lib_load_impl(const char *path) {
  void *lib_handle = dlopen(path, RTLD_LOCAL | RTLD_LAZY);
  return lib_handle;
}

#if 0
static inline void _lib_free_impl(void* native_ptr) {
    int ret = dlclose(native_ptr);
    if (ret) {
        ALOGE("[cvst] err in close plugin: %s", dlerror());
    }
}
#endif

static inline void *_lib_find_symbol_impl(void *native_ptr, const char *name) {
  return dlsym(native_ptr, name);
}
#endif

struct anvs_context_handle {
  anvs_context_t *ctx;
  void *lib;
};
typedef struct anvs_context_handle anvs_context_handle_t;

ANV_VMM_API anvs_context_t *anvs_context_init(const char *name, int argc, const char *argv[]) {
  const char *env_dl_path = getenv("ANVS_SEARCH_PATH");
  if (!env_dl_path) {
    ALOGE("require service search path");
    return NULL;
  }

  size_t pl = strlen(env_dl_path);
  char *dl_path = strdup(env_dl_path);
  if (dl_path[pl - 1] == OS_PATH_SEPARATOR) {
    dl_path[pl - 1] = '\0';
    pl -= 1;
  }

  size_t fl = pl + DLL_NAME_FMT_LEN + 2 + strlen(name);
  char *dl_file = malloc(fl);
#if defined(__APPLE__)
  snprintf(dl_file, fl, "%s" OS_PATH_SEPARATOR_STR "lib%s.dylib", dl_path, name);
#elif defined(__linux__)
  snprintf(dl_file, fl, "%s" OS_PATH_SEPARATOR_STR "lib%s.so", dl_path, name);
#elif defined(_WIN32)
  snprintf(dl_file, fl, "%s" OS_PATH_SEPARATOR_STR "%s.dll", dl_path, name);
#endif

  SAFE_FREE(dl_path);

  void *lib_handle = _lib_load_impl(dl_file);
  SAFE_FREE(dl_file);
  if (!lib_handle) {
    ALOGE("dylib load failed: invalid service lib");
    return NULL;
  }
  anvs_main_fn main = (anvs_main_fn) _lib_find_symbol_impl(lib_handle, "anvs_main");
  if (!main) {
    main = (anvs_main_fn) _lib_find_symbol_impl(lib_handle, "AnvsMain");
    if (!main) {
      ALOGE("service load failed: no main func found");
      dlclose(lib_handle);
      return NULL;
    }
  }

  anvs_context_t *ctx = main(argc, argv);
  if (!ctx) {
    ALOGE("service load failed: invalid context");
    dlclose(lib_handle);
    return NULL;
  }

  anvs_context_handle_t *handle = TALLOC(anvs_context_handle_t);
  handle->ctx = ctx;
  handle->lib = lib_handle;
  ctx->handle = handle;
  return ctx;
}

ANV_VMM_API int anvs_context_run_loop(anvs_context_t *ctx) {
  return ctx->loop(ctx);
}
