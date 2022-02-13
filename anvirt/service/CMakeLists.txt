
set(LIB_SERVICE anvirt-service)

set(LIB_SERVICE_SRC
    service.c
    api.c
)

if (WIN32)
# TODO: win32 impl
elseif (APPLE)
# TODO: darwin impl
endif ()

set(SRC 
    ${LIB_SERVICE_SRC}
)

android_add_library(
  TARGET ${LIB_SERVICE} STATIC INSTALL . LICENSE Apache-2.0
  SRC ${SRC}
)

target_link_libraries(${LIB_SERVICE} PUBLIC anvirt-log-if anvirt-ipc)

set_target_properties(${LIB_SERVICE} PROPERTIES COMPILE_FLAGS "-Wall -Werror -Wextra")


################################################################################
# agent
################################################################################

set(LIB_AGENT anvirt-service-agent)

android_add_library(
  TARGET ${LIB_AGENT} STATIC INSTALL . LICENSE Apache-2.0
  SRC agent.c
)

set_target_properties(${LIB_AGENT} PROPERTIES COMPILE_FLAGS "-Wall -Werror -Wextra")
