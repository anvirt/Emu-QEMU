android_add_library(
  TARGET anvirt-emu-main STATIC INSTALL . LICENSE Apache-2.0
  SRC # cmake-format: sortable
      main.cpp)

target_link_libraries(anvirt-emu-main PRIVATE anvirt-service)
