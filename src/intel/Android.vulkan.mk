# Copyright © 2016 Intel Corporation
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
# THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
# DEALINGS IN THE SOFTWARE.

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
include $(LOCAL_PATH)/vulkan/Makefile.sources

VULKAN_FILES := $(addprefix vulkan/, $(VULKAN_FILES))
VULKAN_GEM_FILES := $(addprefix vulkan/, $(VULKAN_GEM_FILES))
VULKAN_GENERATED_FILES := vulkan/anv_entrypoints.h vulkan/anv_timestamp.h

GEN7_FILES := $(addprefix vulkan/, $(GEN7_FILES))
GEN75_FILES := $(addprefix vulkan/, $(GEN75_FILES))
GEN8_FILES := $(addprefix vulkan/, $(GEN8_FILES))
GEN9_FILES := $(addprefix vulkan/, $(GEN9_FILES))

VULKAN_SOURCES := \
	$(VULKAN_GENERATED_FILES) \
	$(VULKAN_FILES)

VULKAN_HEADERS := \
	$(MESA_TOP)/include/vulkan/vk_platform.h \
	$(MESA_TOP)/include/vulkan/vulkan.h \
	$(MESA_TOP)/include/vulkan/vulkan_intel.h

VULKAN_COMMON_INCLUDES := \
	$(VULKAN_HEADERS) \
	$(MESA_TOP)/src/mapi \
	$(MESA_TOP)/src/gallium/auxiliary \
	$(MESA_TOP)/src/gallium/include \
	$(MESA_TOP)/src/mesa \
	$(MESA_TOP)/src/mesa/drivers/dri/common \
	$(MESA_TOP)/src/mesa/drivers/dri/i965 \
	$(MESA_TOP)/src/vulkan/wsi \
	$(MESA_TOP)/src/intel/vulkan

#
# libmesa_anv_entrypoints with header and dummy.c
#

include $(CLEAR_VARS)
LOCAL_MODULE := libmesa_anv_entrypoints
LOCAL_MODULE_CLASS := STATIC_LIBRARIES

intermediates := $(call local-generated-sources-dir)

LOCAL_C_INCLUDES := \
	$(VULKAN_COMMON_INCLUDES) \
	$(intermediates)/vulkan

LOCAL_GENERATED_SOURCES += $(addprefix $(intermediates)/, $(VULKAN_GENERATED_FILES))
LOCAL_GENERATED_SOURCES += $(intermediates)/vulkan/dummy.c

$(intermediates)/vulkan/dummy.c:
	@mkdir -p $(dir $@)
	@echo "Gen Dummy: $(PRIVATE_MODULE) <= $(notdir $(@))"
	$(hide) touch $@

define generator
        @mkdir -p $(dir $@)
        $(hide) cat $(VULKAN_HEADERS) | $(PRIVATE_SCRIPT) $(PRIVATE_ARGS) > $@
endef

$(intermediates)/vulkan/anv_entrypoints.h: $(intermediates)/vulkan/dummy.c
$(intermediates)/vulkan/anv_entrypoints.h: PRIVATE_SCRIPT := $(MESA_PYTHON2) $(LOCAL_PATH)/vulkan/anv_entrypoints_gen.py
$(intermediates)/vulkan/anv_entrypoints.h: PRIVATE_ARGS := header
$(intermediates)/vulkan/anv_entrypoints.h:
	$(call generator)

$(intermediates)/vulkan/anv_timestamp.h:
	echo "#define ANV_TIMESTAMP \"$(shell date +%s)\"" > $@

LOCAL_EXPORT_C_INCLUDE_DIRS := \
        $(intermediates)

LOCAL_SHARED_LIBRARIES := libdrm_intel

include $(MESA_COMMON_MK)
include $(BUILD_STATIC_LIBRARY)

ANV_INCLUDES := \
	$(VULKAN_COMMON_INCLUDES) \
	$(call generated-sources-dir-for,STATIC_LIBRARIES,libmesa_anv_entrypoints,,)/vulkan \
	$(call generated-sources-dir-for,STATIC_LIBRARIES,libmesa_nir,,)/nir

#
# libanv for gen7
#

include $(CLEAR_VARS)
LOCAL_MODULE := libmesa_anv_gen7
LOCAL_MODULE_CLASS := STATIC_LIBRARIES

LOCAL_SRC_FILES := $(GEN7_FILES)
LOCAL_CFLAGS := -DGEN_VERSIONx10=70

LOCAL_C_INCLUDES := $(ANV_INCLUDES)

LOCAL_WHOLE_STATIC_LIBRARIES := libmesa_anv_entrypoints libmesa_genxml

LOCAL_SHARED_LIBRARIES := libdrm_intel

include $(MESA_COMMON_MK)
include $(BUILD_STATIC_LIBRARY)

#
# libanv for gen75
#

include $(CLEAR_VARS)
LOCAL_MODULE := libmesa_anv_gen75
LOCAL_MODULE_CLASS := STATIC_LIBRARIES

LOCAL_SRC_FILES := $(GEN75_FILES)
LOCAL_CFLAGS := -DGEN_VERSIONx10=75

LOCAL_C_INCLUDES := $(ANV_INCLUDES)

LOCAL_WHOLE_STATIC_LIBRARIES := libmesa_anv_entrypoints libmesa_genxml

LOCAL_SHARED_LIBRARIES := libdrm_intel

include $(MESA_COMMON_MK)
include $(BUILD_STATIC_LIBRARY)

#
# libanv for gen8
#

include $(CLEAR_VARS)
LOCAL_MODULE := libmesa_anv_gen8
LOCAL_MODULE_CLASS := STATIC_LIBRARIES

LOCAL_SRC_FILES := $(GEN8_FILES)
LOCAL_CFLAGS := -DGEN_VERSIONx10=80

LOCAL_C_INCLUDES := $(ANV_INCLUDES)

LOCAL_WHOLE_STATIC_LIBRARIES := libmesa_anv_entrypoints libmesa_genxml

LOCAL_SHARED_LIBRARIES := libdrm_intel

include $(MESA_COMMON_MK)
include $(BUILD_STATIC_LIBRARY)

#
# libanv for gen9
#

include $(CLEAR_VARS)
LOCAL_MODULE := libmesa_anv_gen9
LOCAL_MODULE_CLASS := STATIC_LIBRARIES

LOCAL_SRC_FILES := $(GEN9_FILES)
LOCAL_CFLAGS := -DGEN_VERSIONx10=90

LOCAL_C_INCLUDES := $(ANV_INCLUDES)

LOCAL_WHOLE_STATIC_LIBRARIES := libmesa_anv_entrypoints libmesa_genxml

LOCAL_SHARED_LIBRARIES := libdrm_intel

include $(MESA_COMMON_MK)
include $(BUILD_STATIC_LIBRARY)

#
# libmesa_vulkan_common
#

include $(CLEAR_VARS)
LOCAL_MODULE := libmesa_vulkan_common
LOCAL_MODULE_CLASS := STATIC_LIBRARIES

intermediates := $(call local-generated-sources-dir)

LOCAL_SRC_FILES := $(VULKAN_FILES)

LOCAL_C_INCLUDES := \
	$(ANV_INCLUDES) \
	$(MESA_TOP)/src/compiler

LOCAL_WHOLE_STATIC_LIBRARIES := libmesa_anv_entrypoints libmesa_genxml

LOCAL_GENERATED_SOURCES += $(intermediates)/vulkan/anv_entrypoints.c
$(intermediates)/vulkan/anv_entrypoints.c: PRIVATE_SCRIPT := $(MESA_PYTHON2) $(LOCAL_PATH)/vulkan/anv_entrypoints_gen.py
$(intermediates)/vulkan/anv_entrypoints.c: PRIVATE_ARGS := code
$(intermediates)/vulkan/anv_entrypoints.c:
	$(call generator)

LOCAL_SHARED_LIBRARIES := libdrm_intel

include $(MESA_COMMON_MK)
include $(BUILD_STATIC_LIBRARY)


#
# libmesa_vulkan_intel
#

include $(CLEAR_VARS)

LOCAL_MODULE := vulkan.mesa_intel
LOCAL_MODULE_CLASS := SHARED_LIBRARIES

LOCAL_SRC_FILES := \
	$(VULKAN_GEM_FILES)

LOCAL_C_INCLUDES := \
	$(VULKAN_COMMON_INCLUDES) \
	$(call generated-sources-dir-for,STATIC_LIBRARIES,libmesa_vulkan_intel,,)/vulkan \
	$(call generated-sources-dir-for,STATIC_LIBRARIES,libmesa_anv_entrypoints,,)/vulkan

LOCAL_EXPORT_C_INCLUDE_DIRS := $(MESA_TOP)/src/intel/vulkan

LOCAL_WHOLE_STATIC_LIBRARIES := \
	libmesa_nir \
	libmesa_isl \
	libmesa_glsl \
	libmesa_util \
	libmesa_blorp \
	libmesa_compiler \
	libmesa_dricore \
	libmesa_dri_common \
	libmesa_intel_common \
	libmesa_vulkan_common \
	libmesa_anv_gen7 \
	libmesa_anv_gen75 \
	libmesa_anv_gen8 \
	libmesa_anv_gen9 \
	libmesa_i965_compiler \
	libmesa_anv_entrypoints

LOCAL_SHARED_LIBRARIES := \
	libdrm_intel

LOCAL_MODULE_RELATIVE_PATH := hw

include $(MESA_COMMON_MK)
include $(BUILD_SHARED_LIBRARY)
