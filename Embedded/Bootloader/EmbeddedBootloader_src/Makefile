#------------------------------------------------------------------------------- 
#  \file Makefile
#
#  \brief Declares build options for the bootloader.
#
#  \author
#    Atmel Corporation: http://www.atmel.com \n
#    Support email: avr@atmel.com
#
#  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
#  Licensed under Atmel's Limited License Agreement (BitCloudTM).
#
#------------------------------------------------------------------------------- 
#-------------------------------------------------------------------------------
# Project name:
#-------------------------------------------------------------------------------
PROJNAME = bootloader

#-------------------------------------------------------------------------------
# Compiler type:
#-------------------------------------------------------------------------------
COMPILER_TYPE = GCC

#-------------------------------------------------------------------------------
# Microcontroller selection:
#-------------------------------------------------------------------------------
#CHIP = ATXMEGA256A3
#CHIP = ATXMEGA256D3
#CHIP = ATXMEGA128A1
CHIP = ATMEGA1281
#CHIP = ATMEGA2561
#CHIP = ATMEGA128RFA1

#-------------------------------------------------------------------------------
# Serial interface selection:
#-------------------------------------------------------------------------------
#SERIAL_INTERFACE += USART0
SERIAL_INTERFACE += USART1
#SERIAL_INTERFACE += USB_FIFO
#SERIAL_INTERFACE += USARTD0
#SERIAL_INTERFACE += USARTF0
#SERIAL_INTERFACE += SPIE

ifneq (, $(findstring $(CHIP), ATMEGA1281 ATMEGA2561 ATMEGA128RFA1))
  ifeq (, $(findstring USART0, $(SERIAL_INTERFACE)))
    ifeq (, $(findstring USART1, $(SERIAL_INTERFACE)))
      ifeq (, $(findstring USB_FIFO, $(SERIAL_INTERFACE)))
        $(error Unknown type of serial interface)
      endif
    endif
  endif
endif

ifneq (, $(findstring $(CHIP), ATXMEGA128A1 ATXMEGA256A3 ATXMEGA256D3))
  ifeq (, $(findstring USARTD0, $(SERIAL_INTERFACE)))
    ifeq (, $(findstring USARTF0, $(SERIAL_INTERFACE)))
      ifeq (, $(findstring SPIE, $(SERIAL_INTERFACE)))
        $(error Unknown type of serial interface)
      endif
    endif
  endif
endif

ifneq (, $(findstring $(CHIP), ATXMEGA256D3))
  ifneq (, $(findstring USARTF0, $(SERIAL_INTERFACE)))
    $(error There is no USARTF0 into ATXMEGA256D3)
  endif
  ifneq (, $(findstring SPIE, $(SERIAL_INTERFACE)))
      $(error There is no SPIE into ATXMEGA256D3)
  endif
endif

#-------------------------------------------------------------------------------
# Configuration options:
#-------------------------------------------------------------------------------
USE_EXTERNAL_MEMORY = NO
#USE_EXTERNAL_MEMORY = YES

#TYPE_EXTERNAL_MEMORY = AT25F2048
TYPE_EXTERNAL_MEMORY = AT45DB041

RCB_BB_RS232_CONTROLLER = NO
#RCB_BB_RS232_CONTROLLER = YES

KEY_REMOTE_RS232_CONTROLLER = NO
#KEY_REMOTE_RS232_CONTROLLER = YES

# rf4ce functions just have been implemented only for gcc
INCLUDE_RF4CE_FUNCTIONS = NO
#INCLUDE_RF4CE_FUNCTIONS = YES

BUILD_CONFIGURATION = RELEASE
#BUILD_CONFIGURATION = DEBUG

ifeq ($(USE_EXTERNAL_MEMORY), YES)  
  ifeq (, $(findstring $(TYPE_EXTERNAL_MEMORY), AT25F2048 AT45DB041))
    $(error $(TYPE_EXTERNAL_MEMORY) Unknown type of external memory)
  endif
endif

ifeq (, $(findstring $(KEY_REMOTE_RS232_CONTROLLER), NO))
  ifeq (, $(findstring $(CHIP), ATMEGA128RFA1 ATMEGA1281))
    $(error RS232 controller can't be enabled for $(CHIP))
  endif
endif

ifeq (, $(findstring $(RCB_BB_RS232_CONTROLLER), NO))
  ifeq (, $(findstring $(CHIP), ATMEGA128RFA1 ATMEGA1281 ATMEGA2561))
    $(error RS232 controller can't be enabled for $(CHIP))
  endif
endif

ifeq ($(INCLUDE_RF4CE_FUNCTIONS), YES)
  ifeq (, $(findstring $(CHIP), ATMEGA128RFA1 ATMEGA1281))
    $(error rf4ce functions have been implemented only for ATMEGA128RFA1 ATMEGA1281)
  endif
endif 

#-------------------------------------------------------------------------------
# CPU selection:
#-------------------------------------------------------------------------------
ifeq ($(CHIP), ATXMEGA256A3)
  CPU = atxmega256a3
endif
ifeq ($(CHIP), ATXMEGA256D3)
  CPU = atxmega256d3
endif
ifeq ($(CHIP), ATXMEGA128A1)
  CPU = atxmega128a1
endif
ifeq ($(CHIP), ATMEGA1281)
  CPU = atmega1281
endif
ifeq ($(CHIP), ATMEGA2561)
  CPU = atmega2561
endif
ifeq ($(CHIP), ATMEGA128RFA1)
  CPU = atmega128rfa1
endif
ifndef CHIP
  $(error $(CHIP) Unknown type of microcontroller) 
endif

#-------------------------------------------------------------------------------
# Cross-platform compiler type declaration:
#-------------------------------------------------------------------------------
ifndef COMPILER_TYPE
  $(error $(COMPILER_TYPE) - unknown type of compiler) 
endif
ifneq (, $(findstring $(CHIP), ATXMEGA128A1 ATXMEGA256A3 ATXMEGA256D3 ATMEGA1281 ATMEGA2561 ATMEGA128RFA1))
  CROSS_COMPILER = avr
endif
ifndef CROSS_COMPILER
  #$(error ERROR in file  Makerules: Cross-platform compiler type is not detected) 
endif

#-------------------------------------------------------------------------------
# Tools definitions:
#-------------------------------------------------------------------------------
AS      = $(CROSS_COMPILER)-as
LD      = $(CROSS_COMPILER)-ld
CC      = $(CROSS_COMPILER)-gcc
CPP     = $(CROSS_COMPILER)-g++
AR      = $(CROSS_COMPILER)-ar
NM      = $(CROSS_COMPILER)-nm
STRIP   = $(CROSS_COMPILER)-strip
OBJCOPY = $(CROSS_COMPILER)-objcopy
OBJDUMP = $(CROSS_COMPILER)-objdump
SIZE    = $(CROSS_COMPILER)-size
AR_KEYS = cr

#-------------------------------------------------------------------------------
# Compiler flags:
#-------------------------------------------------------------------------------
CFLAGS  = -Os
CFLAGS += -c
CFLAGS += -std=gnu99
ifeq ($(BUILD_CONFIGURATION), DEBUG)
  CFLAGS += -g
endif # DEBUG
CFLAGS += -Wall -W
CFLAGS += -ffunction-sections
ifeq ($(CROSS_COMPILER), avr) 
  CFLAGS += -mmcu=$(CPU)
  CFLAGS += -fshort-enums
endif
  
## Assembly specific flags
ASMFLAGS = -mmcu=$(CPU)
ASM_EXT = s

USE_USART = 0
USE_SPI = 0

CFLAGS += -D$(CHIP)
ifneq (, $(findstring USART0, $(SERIAL_INTERFACE)))
  CFLAGS += -D_USART0_
  USE_USART = 1
endif
ifneq (, $(findstring USART1, $(SERIAL_INTERFACE)))
  CFLAGS += -D_USART1_
  USE_USART = 1
endif
ifneq (, $(findstring USARTD0, $(SERIAL_INTERFACE)))
  CFLAGS += -D_USARTD0_
  USE_USART = 1
endif
ifneq (, $(findstring USARTF0, $(SERIAL_INTERFACE)))
  CFLAGS += -D_USARTF0_
  USE_USART = 1
endif
ifneq (, $(findstring SPIE, $(SERIAL_INTERFACE)))
  CFLAGS += -D_SPIE_
  USE_SPI = 1
endif
ifneq (, $(findstring USB_FIFO, $(SERIAL_INTERFACE)))
  CFLAGS += -D_USB_FIFO_
endif
ifeq ($(USE_EXTERNAL_MEMORY), YES)
  CFLAGS += -D_EXTERNAL_MEMORY_
  CFLAGS += -D$(TYPE_EXTERNAL_MEMORY)
endif

ifeq ($(RCB_BB_RS232_CONTROLLER), YES)
  CFLAGS += -D_RCB_BB_RS232_CONTROLLER_
endif
ifeq ($(KEY_REMOTE_RS232_CONTROLLER), YES)
  CFLAGS += -D_KEY_REMOTE_RS232_CONTROLLER_
endif
ifeq ($(INCLUDE_RF4CE_FUNCTIONS), YES)
  CFLAGS += -D_INCLUDE_RF4CE_FUNCTIONS_
endif

#==============================================================================
# Include paths flags
#==============================================================================
ifneq (, $(findstring $(CHIP), ATMEGA1281 ATMEGA2561 ATMEGA128RFA1))
  INCLUDEDIRS  =  -I./application/avr/include
  PPATHDIRS    =  ./application/avr
endif
ifneq (, $(findstring $(CHIP), ATXMEGA128A1 ATXMEGA256A3 ATXMEGA256D3))
  INCLUDEDIRS  =  -I./application/xmega/include
  PPATHDIRS    =  ./application/xmega
endif
INCLUDEDIRS += -I./application/common/include
CPATHDIRS    =  ./application/common

#==============================================================================
# Linking
#==============================================================================
# It's easier to use indirect ld call via gcc instead of direct ld call.
EXECUTABLE_FORMAT = elf
LD = $(CC)
LINKER_FLAGS = -Xlinker -Map=./$(PROJNAME).map -Wl,--gc-sections
ifeq ($(CHIP), ATXMEGA128A1)
  LINKER_FLAGS += $(filter -mmcu%,$(CFLAGS))
  LINKER_FLAGS += -Wl,--script=./linkerScript/avrxmega128A1.ld
endif
ifeq ($(CHIP), ATXMEGA256A3)
  LINKER_FLAGS += $(filter -mmcu%,$(CFLAGS))
  LINKER_FLAGS += -Wl,--script=./linkerScript/avrxmega256A3.ld
endif
ifeq ($(CHIP), ATXMEGA256D3)
  LINKER_FLAGS += $(filter -mmcu%,$(CFLAGS))
  LINKER_FLAGS += -Wl,--script=./linkerScript/avrxmega256A3.ld
endif
ifeq ($(CHIP), ATMEGA1281)
  LINKER_FLAGS += $(filter -mmcu%,$(CFLAGS))
  ifeq ($(USE_EXTERNAL_MEMORY), YES)
    LINKER_FLAGS += -Wl,--script=./linkerScript/atmega1281otau.ld
  else
    ifeq ($(INCLUDE_RF4CE_FUNCTIONS), YES)
      LINKER_FLAGS += -Wl,--script=./linkerScript/atmega1281otau.ld
    else
      LINKER_FLAGS += -Wl,--script=./linkerScript/atmega1281.ld
    endif
  endif
endif
ifeq ($(CHIP), ATMEGA2561)
  LINKER_FLAGS += $(filter -mmcu%,$(CFLAGS))
  LINKER_FLAGS += -Wl,--script=./linkerScript/atmega2561.ld
endif
ifeq ($(CHIP), ATMEGA128RFA1)
  LINKER_FLAGS += $(filter -mmcu%,$(CFLAGS))
  ifeq ($(USE_EXTERNAL_MEMORY), YES)
    LINKER_FLAGS += -Wl,--script=./linkerScript/atmega128rfa1otau.ld
  else
    ifeq ($(INCLUDE_RF4CE_FUNCTIONS), YES)
      LINKER_FLAGS += -Wl,--script=./linkerScript/atmega128rfa1otau.ld
    else
      LINKER_FLAGS += -Wl,--script=./linkerScript/atmega128rfa1.ld
    endif 
  endif
  LINKER_FLAGS += -Wl,--section-start=.data=0x800200
endif
ifeq ($(BUILD_CONFIGURATION), DEBUG)
  LINKER_FLAGS += -g
endif

  pc_file  =  eepromLoader
  pc_file +=  flashLoader
  pc_file +=  hardwareInit
ifeq ($(INCLUDE_RF4CE_FUNCTIONS), YES)
  pc_file +=  rf4ceFlash
endif  
  
ifeq ($(USE_USART), 1)
  pc_file +=  uartSerializer
endif

ifeq ($(USE_SPI), 1)
  pc_file +=  spiSerializer
endif

ifneq (, $(findstring USB_FIFO, $(SERIAL_INTERFACE)))
  pc_file +=  usbFifoSerializer
endif

ifeq ($(USE_EXTERNAL_MEMORY), YES)
  pc_file +=  spiMemInterface
endif

  cc_file  =  bootloader
  cc_file +=  srecParser
ifeq ($(USE_EXTERNAL_MEMORY), YES)
  cc_file +=  extMemReader
  ifeq ($(TYPE_EXTERNAL_MEMORY), AT25F2048)
    cc_file +=  memAt25f2048
  endif
  ifeq ($(TYPE_EXTERNAL_MEMORY), AT45DB041)
    cc_file +=  memAt45db041
  endif
endif
  cc_file +=  abstractSerializer

  asm_file  = sp_driver
ifneq (, $(findstring $(CHIP), ATMEGA1281 ATMEGA2561 ATMEGA128RFA1))
  asm_file += calibMeasurement
endif  

cc_objects = $(addsuffix .o,$(addprefix $(CPATHDIRS)/obj/,$(cc_file)))
cc_sources = $(addsuffix .c,$(addprefix $(CPATHDIRS)/src/,$(cc_file)))
pc_objects = $(addsuffix .o,$(addprefix $(PPATHDIRS)/obj/,$(pc_file)))
pc_sources = $(addsuffix .c,$(addprefix $(PPATHDIRS)/src/,$(pc_file)))
asm_objects = $(addsuffix .o,$(addprefix $(PPATHDIRS)/obj/,$(asm_file)))
asm_sources = $(addsuffix .$(ASM_EXT),$(addprefix $(PPATHDIRS)/src/,$(asm_file)))
  
OBJS  =  $(cc_objects)
OBJS +=  $(pc_objects)
OBJS +=  $(asm_objects) 

################ common c part ##############################
$(CPATHDIRS)/obj/%.o: $(CPATHDIRS)/src/%.c
	$(CC) $(CFLAGS) $(INCLUDEDIRS) $^ -o $@
################ c part ##############################

################ personal c part ##############################
$(PPATHDIRS)/obj/%.o: $(PPATHDIRS)/src/%.c
	$(CC) $(CFLAGS) $(INCLUDEDIRS) $^ -o $@
################ c part ##############################

################ assembler part ######################
$(PPATHDIRS)/obj/%.o: $(PPATHDIRS)/src/%.$(ASM_EXT)
	$(AS) $(ASMFLAGS) $(INCLUDEDIRS) -o $@ $^
################ assembler part ######################

#-------------------------------------------------------------------------------
# Target definitions:
#-------------------------------------------------------------------------------
all: $(PROJNAME).elf $(PROJNAME).hex size

$(PROJNAME).elf: $(OBJS)
	@echo 
	@echo -----------------Linking---------------------------
	$(LD) $(LINKER_FLAGS) $(OBJS) -o $@
	$(OBJCOPY) -I elf32-avr -O elf32-avr -R .vectors $(PROJNAME).elf

%.hex:  $(PROJNAME).elf
	$(OBJCOPY) -O ihex -R .eeprom $^ $@

size: $(PROJNAME).elf
	@echo 
	@$(SIZE) -td $(PROJNAME).elf

clean:
	@echo 
	@echo -------Application cleaning------------------------
	-rm -rf $(OBJS) $(PROJNAME).elf $(PROJNAME).hex $(PROJNAME).map
