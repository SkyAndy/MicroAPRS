#
# Wizard autogenerated makefile.
# DO NOT EDIT, use the Modem_user.mk file instead.
#

# Constants automatically defined by the selected modules
Modem_DEBUG = 1

# Our target application
TRG += Modem

# Change 20131216 to you version of installed CrossPack
# you can found it for OSX on http://www.obdev.at/products/crosspack/download.html

Modem_PREFIX = "/usr/local/CrossPack-AVR-20131216/bin/avr-"

Modem_SUFFIX = ""

Modem_SRC_PATH = Modem

Modem_HW_PATH = Modem

# Files automatically generated by the wizard. DO NOT EDIT, USE Modem_USER_CSRC INSTEAD!
Modem_WIZARD_CSRC = \
	bertos/cpu/avr/drv/ser_avr.c \
	bertos/cpu/avr/drv/ser_mega.c \
	bertos/cpu/avr/drv/timer_avr.c \
	bertos/cpu/avr/drv/timer_mega.c \
	bertos/drv/ser.c \
	bertos/drv/timer.c \
	bertos/io/kfile.c \
	bertos/mware/event.c \
	bertos/mware/formatwr.c \
	bertos/mware/hex.c \
	bertos/struct/heap.c \
	bertos/net/ax25.c \
	bertos/algo/crc_ccitt.c \
	#

# Files automatically generated by the wizard. DO NOT EDIT, USE Modem_USER_PCSRC INSTEAD!
Modem_WIZARD_PCSRC = \
	bertos/mware/formatwr.c \
	#

# Files automatically generated by the wizard. DO NOT EDIT, USE Modem_USER_CPPASRC INSTEAD!
Modem_WIZARD_CPPASRC = \
	 \
	#

# Files automatically generated by the wizard. DO NOT EDIT, USE Modem_USER_CXXSRC INSTEAD!
Modem_WIZARD_CXXSRC = \
	 \
	#

# Files automatically generated by the wizard. DO NOT EDIT, USE Modem_USER_ASRC INSTEAD!
Modem_WIZARD_ASRC = \
	 \
	#

Modem_CPPFLAGS = -D'CPU_FREQ=(16000000UL)' -D'ARCH=(ARCH_DEFAULT)' -D'WIZ_AUTOGEN' -I$(Modem_HW_PATH) -I$(Modem_SRC_PATH) $(Modem_CPU_CPPFLAGS) $(Modem_USER_CPPFLAGS)

# Automatically generated by the wizard. PLEASE DO NOT EDIT!
Modem_LDFLAGS = $(Modem_CPU_LDFLAGS) $(Modem_WIZARD_LDFLAGS) $(Modem_USER_LDFLAGS)

# Automatically generated by the wizard. PLEASE DO NOT EDIT!
Modem_CPPAFLAGS = $(Modem_CPU_CPPAFLAGS) $(Modem_WIZARD_CPPAFLAGS) $(Modem_USER_CPPAFLAGS)

# Automatically generated by the wizard. PLEASE DO NOT EDIT!
Modem_CSRC = $(Modem_CPU_CSRC) $(Modem_WIZARD_CSRC) $(Modem_USER_CSRC)

# Automatically generated by the wizard. PLEASE DO NOT EDIT!
Modem_PCSRC = $(Modem_CPU_PCSRC) $(Modem_WIZARD_PCSRC) $(Modem_USER_PCSRC)

# Automatically generated by the wizard. PLEASE DO NOT EDIT!
Modem_CPPASRC = $(Modem_CPU_CPPASRC) $(Modem_WIZARD_CPPASRC) $(Modem_USER_CPPASRC)

# Automatically generated by the wizard. PLEASE DO NOT EDIT!
Modem_CXXSRC = $(Modem_CPU_CXXSRC) $(Modem_WIZARD_CXXSRC) $(Modem_USER_CXXSRC)

# Automatically generated by the wizard. PLEASE DO NOT EDIT!
Modem_ASRC = $(Modem_CPU_ASRC) $(Modem_WIZARD_ASRC) $(Modem_USER_ASRC)

# CPU specific flags and options, defined in the CPU definition files.
# Automatically generated by the wizard. PLEASE DO NOT EDIT!
Modem_MCU = atmega328p
Modem_CPU_CPPFLAGS = -Os -Ibertos/cpu/avr/
Modem_PROGRAMMER_CPU = atmega328p
Modem_STOPFLASH_SCRIPT = bertos/prg_scripts/avr/stopflash.sh
Modem_STOPDEBUG_SCRIPT = bertos/prg_scripts/none.sh
Modem_DEBUG_SCRIPT = bertos/prg_scripts/nodebug.sh
Modem_FLASH_SCRIPT = bertos/prg_scripts/avr/flash.sh

include $(Modem_SRC_PATH)/Modem_user.mk
