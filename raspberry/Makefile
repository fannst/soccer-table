# Copyright Luke A.C.A. Rieff 2020 - Robot Dog Project
# /

#
# Global definitions.
#

# The folder with libraries
LIBRARY_DIR 	:= ../shared/lib

# Defines the tools from the toolchain.
GCC 			:= gcc
GPP 			:= g++
AS 				:= as
LINKER 			:= g++
SIZE 			:= size
OBJCOPY 		:= objcopy

#
# Global Arguments.
#

GCC_ARGS 		+= -Os
GCC_ARGS 		+= -Wall
GCC_ARGS 		+= -I./inc
GCC_ARGS 		+= -fdata-sections
GCC_ARGS 		+= -ffunction-sections
GCC_ARGS		+= -I$(LIBRARY_DIR)/spfp/inc
GCC_ARGS		+= -I../shared/inc

LINKER_ARGS 	+= $(GCC_ARGS)
LINKER_ARGS 	+= -Wl,--gc-sections

#
# Gets the source files.
#

# Gets the default sources
CPP_SOURCES 	:= $(shell find src -name "*.cpp")

# Adds the SPFP Sources (Compile as C++ to make it compatable with our code)
FAKE_CC_SOURCES += $(LIBRARY_DIR)/spfp/src/spfp.c
FAKE_CC_SOURCES += $(LIBRARY_DIR)/spfp/src/helpers/linux.c

# Translates them to object files
OBJECTS 		+= $(CPP_SOURCES:.cpp=.x86.o)
OBJECTS			+= $(FAKE_CC_SOURCES:.c=.x86.lib.o)

#
# Make Rules.
#

%.x86.lib.o: %.c
	$(GPP)  $(GCC_ARGS) -c $< -o $@

%.x86.o: %.cpp
	$(GPP)  $(GCC_ARGS) -c $< -o $@

all: $(OBJECTS)
	$(LINKER) $(LINKER_ARGS) $(OBJECTS) -o main.elf
size:
	$(SIZE) --format=gnu main.elf
clean:
	rm -rf $(OBJECTS)
