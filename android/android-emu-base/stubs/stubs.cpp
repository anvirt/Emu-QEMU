// Copyright 2018 The Android Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Need to include this header in case the compiler decides to mangle the
// symbol.
#include "android/globals.h"

int guest_data_partition_mounted = 0;
int guest_boot_completed = 0;

int changing_language_country_locale = 0;
const char* to_set_language = 0;
const char* to_set_country = 0;
const char* to_set_locale = 0;
int is_fuchsia = 0;
