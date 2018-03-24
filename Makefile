TARGET = kirk_brute
OBJS = main.o kirk.o scePower_driver.o

USE_PSPSDK_LIBC = 1

INCDIR = 
CFLAGS = -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR =
LDFLAGS = 

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = kirk brute forcer

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
