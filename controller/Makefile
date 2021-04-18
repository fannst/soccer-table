# Copyright Luke A.C.A. Rieff 2020 - Robot Dog Project
# /

#
# Global definitions.
#

# The folder with libraries
LIBRARY_DIR 	:= ../shared/lib

# Microcontroller Definitions.
MCPU 			:= cortex-m4

# Defines the toolchain.
TOOLCHAIN 		?= arm-none-eabi

# Defines the tools from the toolchain.
GCC 			:= $(TOOLCHAIN)-gcc
GPP 			:= $(TOOLCHAIN)-g++
AS 				:= $(TOOLCHAIN)-as
LINKER 			:= $(TOOLCHAIN)-g++
SIZE 			:= $(TOOLCHAIN)-size
OBJCOPY 		:= $(TOOLCHAIN)-objcopy

# Other tools
FLASH 			?= st-flash

#
# Global Arguments.
#

LINKER_SCRIPT 	?= linker/linker.ld

GCC_ARGS 		+= -Os
GCC_ARGS 		+= -mthumb
GCC_ARGS 		+= -mcpu=$(MCPU)
GCC_ARGS 		+= -Wall
GCC_ARGS 		+= -I./inc
GCC_ARGS 		+= -I./vendor
GCC_ARGS 		+= -fdata-sections
GCC_ARGS 		+= -ffunction-sections
GCC_ARGS 		+= -mfloat-abi=soft
GCC_ARGS 		+= -D__DEBUG
GCC_ARGS		+= -I$(LIBRARY_DIR)/spfp/inc
GCC_ARGS		+= -I../shared/inc
GCC_ARGS		+= -D__EMBED

GPP_ARGS		+= $(GCC_ARGS)
GPP_ARGS		+= -D__USE_CXX

LINKER_ARGS 	+= $(GCC_ARGS)
LINKER_ARGS 	+= -T $(LINKER_SCRIPT)
LINKER_ARGS 	+= -fdata-sections
LINKER_ARGS 	+= -ffunction-sections
LINKER_ARGS 	+= -Wl,--gc-sections

FLASH_ARGS 		+= write
FLASH_ARGS 		+= main.bin
FLASH_ARGS 		+= 0x8000000

#
# Gets the source files.
#

# Gets the default sources
CPP_SOURCES 	:= $(shell find src -name "*.cpp")
CPP_SOURCES 	+= $(shell find ../shared/src/ -name "*.cpp")
C_SOURCES 		:= $(shell find src -name "*.c")

# Translates them to object files
OBJECTS 		+= $(CPP_SOURCES:.cpp=.arm.o)
OBJECTS 		+= $(C_SOURCES:.c=.arm.o)

#
# Make Rules.
#

%.arm.o: %.c
	$(GCC)  $(GCC_ARGS) -c $< -o $@

%.arm.lib.o: %.c
	$(GPP)  $(GCC_ARGS) -c $< -o $@

%.arm.o: %.cpp
	$(GPP)  $(GPP_ARGS) -c $< -o $@

%.arm.o: %.s
	$(GCC) $(GCC_ARGS) -c $< -o $@

all: $(OBJECTS) startup/startup.arm.o
	$(LINKER) $(LINKER_ARGS) $(OBJECTS) startup/startup.arm.o -o main.elf
bin:
	$(OBJCOPY) main.elf -O binary main.bin
size:
	$(SIZE) --format=gnu main.elf
flash:
	$(FLASH) $(FLASH_ARGS)
clean:
	rm -rf $(OBJECTS) startup.arm.o main.bin
