LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := SDL2
LOCAL_SRC_FILES := /home/ehmcruz/Android/SDL/lib/arm64-v8a/libSDL2.so
#LOCAL_EXPORT_C_INCLUDES :=
include $(PREBUILT_SHARED_LIBRARY)

# ---------------------------------

include $(CLEAR_VARS)

LOCAL_MODULE := main

SDL_PATH := /home/ehmcruz/Android/SDL/SDL-release-2.28.5

LOCAL_C_INCLUDES := $(SDL_PATH)/include

# Add your application source files here...
LOCAL_SRC_FILES := $(LOCAL_PATH)/test.cpp

LOCAL_SHARED_LIBRARIES := SDL2

#LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -lOpenSLES -llog -landroid

include $(BUILD_SHARED_LIBRARY)
