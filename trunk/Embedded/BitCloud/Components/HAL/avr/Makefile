HAL_PATH = ..
include $(HAL_PATH)/Makerules

######
LIBDIR = $(STACK_LIB_PATH)
LIB = $(LIBDIR)/lib$(HAL_LIB).a

ifeq ($(HAL), ATMEGA1281)
  BUILDDIR = $(HAL_PATH)/avr/atmega1281
endif

ifeq ($(HAL), ATMEGA2561)
  BUILDDIR = $(HAL_PATH)/avr/atmega1281
endif

ifeq ($(HAL), ATMEGA1284)
  BUILDDIR = $(HAL_PATH)/avr/atmega1284
endif

ifeq ($(HAL), AT90USB1287)
  BUILDDIR = $(HAL_PATH)/avr/at90usb1287
endif

ifeq ($(HAL), ATMEGA128RFA1)
  BUILDDIR = $(HAL_PATH)/avr/atmega128rfa1
endif

#### DEFINES FLAGS #######
CFLAGS += -DHAL_USE_ADC
CFLAGS += -DHAL_USE_EE_READY
CFLAGS += -DHAL_USE_TWI
CFLAGS += -DHAL_USE_TIMER4_COMPA
ifneq ($(HAL), AT90USB1287)
  CFLAGS += -DHAL_USE_SLEEP
  CFLAGS += -DHAL_USE_TIMER2_COMPA
  CFLAGS += -DHAL_USE_WAKEUP
endif
ifneq ($(HAL), ATMEGA1284)
  CFLAGS += -DHAL_USE_PWM
endif #ATMEGA1284
ifeq ($(HAL), AT90USB1287)
  CFLAGS += -DHAL_USE_USB
endif
ifeq ($(HAL), ATMEGA128RFA1)
  CFLAGS += -D_HAL_HW_AES_
  CFLAGS += -DHAL_USE_SPI
endif
CFLAGS += -DSLEEP_PRESCALER_1024
ifeq ($(HAL), ATMEGA1281)
  ifeq ($(HAL_TINY_UID), TRUE)
    CFLAGS += -D_HAL_TINY_UID_
  endif    
endif


##### DECLARES USART CHANNELS TO BE USED ##########
CFLAGS += -DHAL_USE_USART
ifneq ($(HAL), AT90USB1287)
  CFLAGS += -DNUM_USART_CHANNELS=2
else
  CFLAGS += -DNUM_USART_CHANNELS=1
endif # AT90USB1287

ifneq ($(HAL), AT90USB1287)
  CFLAGS += -DHAL_USE_USART_CHANNEL_0
endif # AT90USB1287

CFLAGS += -DHAL_USE_USART_CHANNEL_1

# Defines which USART uses hardware control pins - only one port at 
# the same time can be hardware controlled. If the macros is not declared - 
# hw control is not used.
ifeq (, $(findstring $(HAL), ATMEGA1284 AT90USB1287 ATMEGA128RFA1))
  #CFLAGS += -DHARDWARE_CONTROL_PINS_PORT_ASSIGNMENT=USART_CHANNEL_0
  CFLAGS += -DHW_CONTROL_PINS_PORT_ASSIGNMENT=USART_CHANNEL_1
endif

ifeq ($(SYSTEM_TIME_ON), SLEEP_TIMER)
  CFLAGS += -D_SYSTEM_TIME_ON_SLEEP_TIMER_
endif

##### PATHS FLAGS OF INCLUDES #########
CFLAGS += -I$(HAL_HWD_COMMON_PATH)/include
CFLAGS += -I$(HAL_MAC_API_PATH)/include  
CFLAGS += -I$(BUILDDIR)/../../include
CFLAGS += -I$(SE_PATH)/include

#### ASM DEFINES FLAGS #######
ASMFLAGS  = -I$(HAL_HWD_COMMON_PATH)/include
ifeq ($(COMPILER_TYPE), GCC)
  ASMFLAGS += -mmcu=$(CPU)
  ASM_FILE_EXT = s
endif
ifeq ($(COMPILER_TYPE), IAR)  
  ASMFLAGS += -s+ # Enables case sensitivity.
  ASMFLAGS += -w+ # Enables warnings.
  ASMFLAGS += -D$(HAL_FREQUENCY)
  ASMFLAGS += -M'<>'# Change asm.macro argument quote chars.
  ASMFLAGS += -L$(HAL_PATH)/avr/list  # Generate a list on: <path> /<source> <.lst>
  ASMFLAGS += -t8 # Set tab spacing.
  ASMFLAGS += -u_enhancedCore
  ASMFLAGS += -D__HAS_ENHANCED_CORE__=1
  ASMFLAGS += -D__HAS_MUL__=1
  ASMFLAGS += -D__MEMORY_MODEL__=2
  ifneq (, $(findstring $(HAL), ATMEGA1281 ATMEGA1284 AT90USB1287 ATMEGA128RFA1)) 
    ASMFLAGS += -v3 # Select processor option: Relative jumps do not wrap.
    ifeq ($(HAL), ATMEGA1281)
      ASMFLAGS += -D__ATmega1281__=1
    endif  
  endif
  ifneq (, $(findstring $(HAL), ATMEGA2561))
    ASMFLAGS += -v5 # Select processor option: Relative jumps do not wrap.
    ASMFLAGS += -D__ATmega2561__=1
  endif
  ASMFLAGS += -D__HAS_ELPM__=1 
  ASMFLAGS += -S 
  ASM_FILE_EXT = s90
endif

# Main clock of CPU in Hz.
ifeq ($(HAL_FREQUENCY), HAL_4MHz)
  F_CPU=4000000
endif
ifeq ($(HAL_FREQUENCY), HAL_8MHz)
  F_CPU=8000000
endif

###### LIB ##########
LIBS = $(LIB) $(LIBDIR)/$(WDT_INIT_OBJ)

ifneq ($(HAL), ATMEGA1284)
  common_hwd += halPwm
endif
ifneq ($(HAL), AT90USB1287)  
  common_hwd += halSleepTimerClock
endif
ifeq ($(HAL), AT90USB1287)  
  common_hwd += halUsb
endif
ifeq ($(HAL), ATMEGA128RFA1)
  common_hwd += halSecurityModule
  common_hwd += halDelay
endif
common_hwd +=  halAppClock
common_hwd +=  halClkCtrl
common_hwd +=  halUsart
common_hwd +=  halIrq
common_hwd +=  i2c
common_hwd +=  halEeprom
common_hwd +=  wdt
common_hwd +=  halSleep
common_hwd +=  halAdc
common_hwd +=  halSpi
common_hwd +=  calibration
common_hwd +=  halInit

ifneq ($(HAL), ATMEGA128RFA1)
  personal_hwd += halRfSpi
endif

personal_hwd += halRfCtrl
personal_hwd += halUid
personal_hwd += halMacIsr

ifeq ($(HAL), ATMEGA1281)
  ifeq ($(HAL_TINY_UID), TRUE)
    personal_hwd += halAtmelUid
  endif    
endif

asm_hwd += halW1
asm_hwd += halCalibration
ifeq ($(COMPILER_TYPE), IAR)
  asm_hwd += cstartup
endif

ifneq ($(HAL), ATMEGA1284)
  hwi += pwm
endif
ifneq ($(HAL), AT90USB1287)
  hwi += sleepTimer
endif
ifeq ($(HAL), AT90USB1287)
  hwi += usb
endif
hwi += appTimer
hwi += halTaskManager
hwi += usart
hwi += w1
hwi += irq
hwi += resetReason
hwi += i2cPacket
hwi += eeprom
hwi += timer
hwi += adc
hwi += spi
hwi += sleep

objects_hwd = $(addsuffix .o,$(addprefix $(HAL_HWD_COMMON_PATH)/objs/,$(common_hwd)))
objects_hwd += $(addsuffix .o,$(addprefix $(HAL_MAC_API_PATH)/objs/,$(personal_hwd)))
sources_hwd = $(addsuffix .c,$(addprefix $(HAL_HWD_COMMON_PATH)/src/,$(common_hwd)))
sources_hwd += $(addsuffix .c,$(addprefix $(HAL_MAC_API_PATH)/src/,$(personal_hwd)))
objects_asm_hwd = $(addsuffix .o,$(addprefix $(HAL_HWD_COMMON_PATH)/objs/,$(asm_hwd)))
sources_asm_hwd = $(addsuffix .$(ASM_FILE_EXT),$(addprefix $(HAL_HWD_COMMON_PATH)/src/,$(asm_hwd)))
objects_hwi = $(addsuffix .o,$(addprefix $(BUILDDIR)/../common/objs/,$(hwi)))
sources_hwi = $(addsuffix .c,$(addprefix $(BUILDDIR)/../common/src/,$(hwi)))

###### TARGETS ################
all: component_label WdtInit $(LIB)

component_label:
	@echo
	@echo ----------------------------------------------------
	@echo HAL library creation.
	@echo ----------------------------------------------------

################ common part ##############################
$(HAL_HWD_COMMON_PATH)/objs/%.o: $(HAL_HWD_COMMON_PATH)/src/%.c
	$(CC_MSG)
	$(Q)$(CC) $(CFLAGS) $^ -o $@
################ common part ##############################

################ personal part ##############################
$(HAL_MAC_API_PATH)/objs/%.o: $(HAL_MAC_API_PATH)/src/%.c
	$(CC_MSG)
	$(Q)$(CC) $(CFLAGS) $^ -o $@
################ personal part ##############################

################ common assembler part ######################
$(HAL_HWD_COMMON_PATH)/objs/%.o: $(HAL_HWD_COMMON_PATH)/src/%.$(ASM_FILE_EXT)
	$(AS_MSG)
	$(Q)$(AS) $(ASMFLAGS) -o $@ $^

ifeq ($(COMPILER_TYPE), IAR)
gen_fcpu_header:
	@echo "FCPU EQU $(F_CPU)" > $(HAL_HWD_COMMON_PATH)/include/halIarD.h
endif

ifeq ($(COMPILER_TYPE), GCC)
gen_fcpu_header:
	@echo ".equ FCPU, $(F_CPU)" > $(HAL_HWD_COMMON_PATH)/include/halGccD.h
endif
################ common assembler part ######################

################ hwi part ###################################
$(BUILDDIR)/../common/objs/%.o: $(BUILDDIR)/../common/src/%.c
	$(CC_MSG)
	$(Q)$(CC) $(CFLAGS) $^ -o $@
################ hwi part ###################################

################
WdtInit: $(HAL_HWD_COMMON_PATH)/src/halWdtInit.c
	$(CC_MSG)
	$(Q)$(CC) $(CFLAGS) $(HAL_HWD_COMMON_PATH)/src/halWdtInit.c -o $(LIBDIR)/$(WDT_INIT_OBJ)
################
$(LIB): $(objects_hwd) gen_fcpu_header $(objects_asm_hwd) $(objects_hwi)
	$(AR_MSG)
	$(Q)$(AR) $(AR_KEYS) $(LIB) $(objects_hwd) $(objects_asm_hwd) $(objects_hwi)
	$(SIZE_MSG)
	$(Q)$(SHOW_SIZE) -td $(LIBDIR)/$(WDT_INIT_OBJ)
	$(SIZE_MSG)
	$(Q)$(SHOW_SIZE) -td $(LIB)

#	$(foreach lib_iter,$(wildcard $(LIBDIR)/lib*$(STACK_LIB)*.a), $(shell $(AR) $(AR_KEYS) $(lib_iter) $(objects_hwd) $(objects_asm_hwd) $(objects_hwi)))
################
clean:
	@echo 
	@echo ----------------------------------------------------
	@echo HAL component cleaning.
	$(Q)rm -f $(objects_hwd) $(objects_hwi) $(LIBS) $(objects_asm_hwd)
	$(Q)rm -f $(HAL_PATH)/lib/*.a $(HAL_PATH)/lib/*.o $(HAL_PATH)/avr/list/*.*
	@echo HAL cleaning done!
	@echo ----------------------------------------------------
