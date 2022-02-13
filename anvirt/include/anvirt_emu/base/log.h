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
 *  log.h
 *  Created by Chen Zhen <cz.worker@gmail.com> on 2021/09/17.
 */

#ifndef anv_base_log_h
#define anv_base_log_h

#include <stdarg.h>
#include <stdio.h>
#include <anvirt_emu/vmm.h>

#ifdef __cplusplus
extern "C" {
#endif

enum AnvLogLevel {
  AnvLogLevelError = 1,
  AnvLogLevelWarn,
  AnvLogLevelInfo,
  AnvLogLevelDebug,
  AnvLogLevelTrace,
};

ANV_VMM_API anv_i32 anv_log_init(const char *config_file);
ANV_VMM_API void anv_log_log(anv_i32 level, const char *tag, const char *msg);

ANV_VMM_API void anv_log_log_in(anv_i32 level, const char *tag, const char *fmt, ...);

#ifdef ANV_NO_LOG

#define ALOGE(_fmt, ...)
#define ALOGW(_fmt, ...)
#define ALOGI(_fmt, ...)
#define ALOGD(_fmt, ...)
#define ALOGT(_fmt, ...)

#else

#define ALOGE(_fmt, ...)                                                  \
  do {                                                                    \
    anv_log_log_in(AnvLogLevelError, ANV_LOG_TAG, (_fmt), ##__VA_ARGS__); \
  } while (0);

#define ALOGW(_fmt, ...)                                                 \
  do {                                                                   \
    anv_log_log_in(AnvLogLevelWarn, ANV_LOG_TAG, (_fmt), ##__VA_ARGS__); \
  } while (0);

#define ALOGI(_fmt, ...)                                                 \
  do {                                                                   \
    anv_log_log_in(AnvLogLevelInfo, ANV_LOG_TAG, (_fmt), ##__VA_ARGS__); \
  } while (0);

#define ALOGD(_fmt, ...)                                                  \
  do {                                                                    \
    anv_log_log_in(AnvLogLevelDebug, ANV_LOG_TAG, (_fmt), ##__VA_ARGS__); \
  } while (0);

#define ALOGT(_fmt, ...)                                                  \
  do {                                                                    \
    anv_log_log_in(AnvLogLevelTrace, ANV_LOG_TAG, (_fmt), ##__VA_ARGS__); \
  } while (0);

#endif

#ifndef ANV_LOG_TAG
// default log tag, can define another different tag for each src
#define ANV_LOG_TAG "-"
#endif

#ifdef __cplusplus
}
#endif

#endif // anv_base_log_h
