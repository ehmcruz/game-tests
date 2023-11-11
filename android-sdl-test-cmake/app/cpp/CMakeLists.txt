#cmake_minimum_required(VERSION 3.6)
cmake_minimum_required(VERSION 3.22.1)

set(CMAKE_VERBOSE_MAKEFILE ON)

#project(app_cruz)

# armeabi-v7a requires cpufeatures library
# include(AndroidNdkModules)
# android_ndk_import_module_cpufeatures()

set(SDL2_INCLUDE_DIR /home/ehmcruz/Android/SDL/SDL-release-2.28.5/include)
set(SDL2_LINK_DIR /home/ehmcruz/Android/SDL/lib/arm64-v8a)

add_library( SDL2 SHARED IMPORTED )

set_target_properties(SDL2 PROPERTIES IMPORTED_LOCATION ${SDL2_LINK_DIR}/libSDL2.so)
#set_target_properties(SDL2 PROPERTIES IMPORTED_LOCATION /home/ehmcruz/Android/SDL/lib/arm64-v8a/libSDL2.so)

#include_directories(${SDL2_INCLUDE_DIR})

add_library(main SHARED hello.cpp)

# SDL sources are in a subfolder named "SDL"
#add_subdirectory(SDL)

# Compilation of companion libraries
#add_subdirectory(SDL_image)
#add_subdirectory(SDL_mixer)
#add_subdirectory(SDL_ttf)

# Your game and its CMakeLists.txt are in a subfolder named "src"
#add_subdirectory(src)

#target_sources(main PRIVATE hello.cpp)

target_include_directories(main PRIVATE ${SDL2_INCLUDE_DIR})

target_link_libraries(main SDL2)
