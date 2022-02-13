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
 *  log.c
 *  Created by Chen Zhen <cz.worker@gmail.com> on 2021/09/17.
 */

#define ANV_VMM_CORE

#include <string.h>
#include <anvirt_emu/base/log.h>

static const int LOG_BUF_LEN = 1024;

// TODO: 临时处理
#define anv_log_log(_l, _t, _d) fprintf(stderr, "%d [%s] %s\n", _l, _t, _d)

ANV_VMM_API void anv_log_log_in(anv_i32 level, const char *tag, const char *fmt, ...) {
  char log_buf[LOG_BUF_LEN];
  va_list args;
  va_start(args, fmt);
  vsnprintf(log_buf, LOG_BUF_LEN - 3, fmt, args);
  va_end(args);

  if (log_buf[LOG_BUF_LEN - 4] == '\0') {
    strncpy(log_buf + LOG_BUF_LEN - 4, "...", 4);
  }

  anv_log_log(level, tag, log_buf);
}
