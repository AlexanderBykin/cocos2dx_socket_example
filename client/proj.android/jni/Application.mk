APP_STL := gnustl_static

APP_CPPFLAGS := -frtti -DGOOGLE_PROTOBUF_NO_RTTI -DHAVE_PTHREAD=1 -std=c++11 -fsigned-char -Wno-extern-c-compat -DCC_ENABLE_CHIPMUNK_INTEGRATION=1
APP_LDFLAGS := -latomic
# APP_ABI := armeabi armeabi-v7a
APP_PLATFORM := android-16

ifeq ($(NDK_DEBUG),1)
  APP_CPPFLAGS += -DCOCOS2D_DEBUG=1
  APP_OPTIM := debug
else
  APP_CPPFLAGS += -DNDEBUG
  APP_OPTIM := release
endif
