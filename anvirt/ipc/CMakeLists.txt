set(LIB anvirt-ipc)

set(SRC
    msg.c
    endpoint.c
)

android_add_library(
  TARGET ${LIB} STATIC INSTALL . LICENSE Apache-2.0
  SRC ${SRC}
)

set_target_properties(${LIB} PROPERTIES COMPILE_FLAGS "-Wall -Werror -Wextra")
