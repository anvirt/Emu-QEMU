
set(LIB anvirt-log-if)

set(LIB_LOG_SRC
    log.c
)

if (WIN32)
# TODO: win32 impl
elseif (APPLE)
# TODO: darwin impl
endif ()

set(SRC 
    ${LIB_LOG_SRC}
)

android_add_library(
  TARGET ${LIB} STATIC INSTALL . LICENSE Apache-2.0
  SRC ${SRC}
)

set_target_properties(${LIB} PROPERTIES COMPILE_FLAGS "-Wall -Werror -Wextra")
