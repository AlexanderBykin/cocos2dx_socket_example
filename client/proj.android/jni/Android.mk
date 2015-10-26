LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

$(call import-add-path,$(LOCAL_PATH)/../../cocos2d)
$(call import-add-path,$(LOCAL_PATH)/../../cocos2d/external)
$(call import-add-path,$(LOCAL_PATH)/../../cocos2d/cocos)
$(call import-add-path, $(LOCAL_PATH))

LOCAL_MODULE := cocos2dcpp_shared

LOCAL_MODULE_FILENAME := libcocos2dcpp

CC_LITE_SRC_FILES := /opt/protobuf/src/google/protobuf/stubs/common.cc \
                            /opt/protobuf/src/google/protobuf/stubs/once.cc \
                            /opt/protobuf/src/google/protobuf/stubs/strutil.cc \
                            /opt/protobuf/src/google/protobuf/stubs/status.cc \
                            /opt/protobuf/src/google/protobuf/stubs/stringpiece.cc \
                            /opt/protobuf/src/google/protobuf/stubs/stringprintf.cc \
                            /opt/protobuf/src/google/protobuf/stubs/int128.cc \
                            /opt/protobuf/src/google/protobuf/stubs/structurally_valid.cc \
                            /opt/protobuf/src/google/protobuf/extension_set.cc \
                            /opt/protobuf/src/google/protobuf/generated_message_util.cc \
                            /opt/protobuf/src/google/protobuf/message_lite.cc \
                            /opt/protobuf/src/google/protobuf/repeated_field.cc \
                            /opt/protobuf/src/google/protobuf/arenastring.cc \
                            /opt/protobuf/src/google/protobuf/arena.cc \
                            /opt/protobuf/src/google/protobuf/wire_format_lite.cc \
                            /opt/protobuf/src/google/protobuf/io/coded_stream.cc \
                            /opt/protobuf/src/google/protobuf/io/zero_copy_stream.cc \
                            /opt/protobuf/src/google/protobuf/io/zero_copy_stream_impl_lite.cc

FILE_LIST :=  $(wildcard $(LOCAL_PATH)/../../Classes/*.c*)
FILE_LIST += $(wildcard $(LOCAL_PATH)/../../Classes/*/*.c*)
FILE_LIST += $(wildcard $(LOCAL_PATH)/../../Classes/*/*/*.c*)

LOCAL_SRC_FILES := brrcpp/main.cpp \
		     $(FILE_LIST:$(LOCAL_PATH)/%=%) \
                   $(CC_LITE_SRC_FILES)

# LOCAL_LDLIBS := -landroid -llog
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../Classes \
                    $(LOCAL_PATH)/../../Classes/Custom \
                    $(LOCAL_PATH)/../../Classes/MessageSerializer \
                    $(LOCAL_PATH)/../../Classes/Network \
                    $(LOCAL_PATH)/../../Classes/ProtoMessages \
                    /opt/protobuf/src

# _COCOS_HEADER_ANDROID_BEGIN
# _COCOS_HEADER_ANDROID_END

LOCAL_STATIC_LIBRARIES := cocos2dx_static

# _COCOS_LIB_ANDROID_BEGIN
# _COCOS_LIB_ANDROID_END

include $(BUILD_SHARED_LIBRARY)

$(call import-module,.)

# _COCOS_LIB_IMPORT_ANDROID_BEGIN
# _COCOS_LIB_IMPORT_ANDROID_END
