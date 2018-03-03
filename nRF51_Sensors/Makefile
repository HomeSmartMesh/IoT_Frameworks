PROJECT_NAME     := mesh_sensors_node
TARGETS          := nrf51822_xxaa
OUTPUT_DIRECTORY := _build

SDK_ROOT := ./nRF5_SDK_12.3.0_prf
PROJ_DIR := .

$(OUTPUT_DIRECTORY)/nrf51822_xxaa.out: \
  LINKER_SCRIPT  := esb_low_power_ptx_gcc_nrf51_aa.ld

# Source files common to all targets
SRC_FILES += \
  $(PROJ_DIR)/main.c \
  $(PROJ_DIR)/mpu6050_app/mpu6050_app.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_backend_serial.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_frontend.c \
  $(SDK_ROOT)/components/libraries/util/app_error.c \
  $(SDK_ROOT)/components/libraries/util/app_error_weak.c \
  $(SDK_ROOT)/components/libraries/util/app_util_platform.c \
  $(SDK_ROOT)/components/libraries/util/nrf_assert.c \
  $(SDK_ROOT)/components/libraries/util/sdk_errors.c \
  $(SDK_ROOT)/components/boards/boards.c \
  $(SDK_ROOT)/components/drivers_nrf/common/nrf_drv_common.c \
  $(SDK_ROOT)/components/drivers_nrf/uart/nrf_drv_uart.c \
  $(SDK_ROOT)/external/segger_rtt/RTT_Syscalls_GCC.c \
  $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT.c \
  $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT_printf.c \
  $(SDK_ROOT)/components/proprietary_rf/esb/nrf_esb.c \
  $(SDK_ROOT)/components/toolchain/gcc/gcc_startup_nrf51.S \
  $(SDK_ROOT)/components/toolchain/system_nrf51.c \
  $(SDK_ROOT)/components/drivers_ext/mpu6050/mpu6050.c \
  $(SDK_ROOT)/components/drivers_nrf/twi_master/deprecated/twi_hw_master.c \

# Include folders common to all targets
INC_FOLDERS += \
  $(PROJ_DIR) \
  $(PROJ_DIR)/mpu6050_app \
  $(SDK_ROOT)/components \
  $(SDK_ROOT)/components/libraries/util \
  $(SDK_ROOT)/components/toolchain/gcc \
  $(SDK_ROOT)/components/drivers_nrf/uart \
  $(SDK_ROOT)/components/drivers_nrf/common \
  $(SDK_ROOT)/components/proprietary_rf/esb \
  $(SDK_ROOT)/external/segger_rtt \
  $(SDK_ROOT)/components/libraries/bsp \
  $(SDK_ROOT)/components/drivers_nrf/nrf_soc_nosd \
  $(SDK_ROOT)/components/toolchain \
  $(SDK_ROOT)/components/device \
  $(SDK_ROOT)/components/libraries/log \
  $(SDK_ROOT)/components/boards \
  $(SDK_ROOT)/components/drivers_nrf/delay \
  $(SDK_ROOT)/components/toolchain/cmsis/include \
  $(SDK_ROOT)/components/drivers_nrf/hal \
  $(SDK_ROOT)/components/libraries/log/src \
  $(SDK_ROOT)/components/drivers_ext/mpu6050 \
  $(SDK_ROOT)/components/drivers_nrf/twi_master/deprecated \
  

# Libraries common to all targets
LIB_FILES += \

# C flags common to all targets
CFLAGS += -DNRF51
CFLAGS += -DBOARD_SENSOR_TAG
CFLAGS += -DESB_PRESENT
CFLAGS += -DNRF51822
CFLAGS += -DBSP_DEFINES_ONLY
CFLAGS += -mcpu=cortex-m0
CFLAGS += -mthumb -mabi=aapcs
CFLAGS +=  -Wall -Werror -O3 -g3
CFLAGS += -mfloat-abi=soft
# keep every function in separate section, this allows linker to discard unused ones
CFLAGS += -ffunction-sections -fdata-sections -fno-strict-aliasing
CFLAGS += -fno-builtin --short-enums 

CFLAGS += -DNRF_LOG_USES_RTT=1

# C++ flags common to all targets
CXXFLAGS += \

# Assembler flags common to all targets
ASMFLAGS += -x assembler-with-cpp
ASMFLAGS += -DNRF51
ASMFLAGS += -DBOARD_SENSOR_TAG
ASMFLAGS += -DESB_PRESENT
ASMFLAGS += -DNRF51822
ASMFLAGS += -DBSP_DEFINES_ONLY

# Linker flags
LDFLAGS += -mthumb -mabi=aapcs -L $(TEMPLATE_PATH) -T$(LINKER_SCRIPT)
LDFLAGS += -mcpu=cortex-m0
# let linker to dump unused sections
LDFLAGS += -Wl,--gc-sections
# use newlib in nano version
LDFLAGS += --specs=nano.specs -lc -lnosys


.PHONY: $(TARGETS) default all clean help flash 

# Default target - first one defined
default: nrf51822_xxaa

# Print all targets that can be built
help:
	@echo following targets are available:
	@echo 	nrf51822_xxaa

TEMPLATE_PATH := $(SDK_ROOT)/components/toolchain/gcc

include $(TEMPLATE_PATH)/Makefile.common

$(foreach target, $(TARGETS), $(call define_target, $(target)))

# Flash the program
flash: $(OUTPUT_DIRECTORY)/nrf51822_xxaa.hex
	@echo Flashing: $<
	nrfjprog --program $< -f nrf51 --sectorerase
	nrfjprog --reset -f nrf51

erase:
	nrfjprog --eraseall -f nrf51
