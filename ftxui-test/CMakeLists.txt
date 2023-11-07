cmake_minimum_required (VERSION 3.11)
 
# --- Fetch FTXUI --------------------------------------------------------------
include(FetchContent)
 
set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)
FetchContent_Declare(ftxui
  # GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
  GIT_REPOSITORY /home/ehmcruz/git-others/FTXUI
  # Important: Specify a GIT_TAG XXXXX here.
  GIT_TAG "main"
)
 
FetchContent_GetProperties(ftxui)
if(NOT ftxui_POPULATED)
  FetchContent_Populate(ftxui)
  add_subdirectory(${ftxui_SOURCE_DIR} ${ftxui_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()
 
# ------------------------------------------------------------------------------
 
project(ftxui-test
  LANGUAGES CXX
  VERSION 1.0.0
)

add_executable(ftxui-test main.cpp)
#target_include_directories(ftxui PRIVATE src)
 
target_link_libraries(ftxui-test
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component # Not needed for this example.
)

project(ftxui-test2
  LANGUAGES CXX
  VERSION 1.0.0
)

add_executable(ftxui-test2 main2.cpp)
#target_include_directories(ftxui PRIVATE src)
 
target_link_libraries(ftxui-test2
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component # Not needed for this example.
)

project(ftxui-test3
  LANGUAGES CXX
  VERSION 1.0.0
)

add_executable(ftxui-test3 main3.cpp)
#target_include_directories(ftxui PRIVATE src)
 
target_link_libraries(ftxui-test3
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component # Not needed for this example.
)