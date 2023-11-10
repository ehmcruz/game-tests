#cmake_minimum_required(VERSION 3.6)
cmake_minimum_required(VERSION 3.22.1)

project(app_cruz)

# armeabi-v7a requires cpufeatures library
# include(AndroidNdkModules)
# android_ndk_import_module_cpufeatures()

add_library(main SHARED)

# SDL sources are in a subfolder named "SDL"
#add_subdirectory(SDL)

# Compilation of companion libraries
#add_subdirectory(SDL_image)
#add_subdirectory(SDL_mixer)
#add_subdirectory(SDL_ttf)

# Your game and its CMakeLists.txt are in a subfolder named "src"
#add_subdirectory(src)

target_sources(main PRIVATE hello.cpp)

target_link_libraries(main)
