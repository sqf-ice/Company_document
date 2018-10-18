COMPILERHOME = 
CC      = arm-none-eabi-gcc
CXX     = 
AS      = arm-none-eabi-as
LD      = arm-none-eabi-ld
AR      = arm-none-eabi-ar
CPP     = $(CC) -E
AR      = arm-none-eabi-ar
NM      = arm-none-eabi-nm
LDR     = arm-none-eabi-ldr
STRIP   = arm-none-eabi-strip
OBJCOPY = arm-none-eabi-objcopy
OBJDUMP = arm-none-eabi-objdump
RANLIB  = arm-none-eabi-RANLIB
SIZE    = arm-none-eabi-size -d
RM =		del
MODE = ME32
TYPE = pos
MULTIAPP = V2
OBJDIR = ../o/$(MODE)
ROMDIR = ../rom/$(MODE)/$(MULTIAPP)
LIBDIR = ../lib/$(MODE)
INCDIR = ../inc
SRCDIR = ../src/$(TYPE)
CFLAGS = -mthumb -Wall -Os -fno-common -mcpu=cortex-m3 -msoft-float 
CXXFLAGS = -Wall
LINKSCRIPT = appmod.ld
LIBS = $(LIBDIR)/libc_Cortex-M.a
ARCH_CFLAGS = -mthumb -Wall -Os -fno-common -mcpu=cortex-m3 -msoft-float
libgccdir =  "$(shell $(CC) $(ARCH_CFLAGS) -print-file-name=)"
LDFLAGS = -nostdlib -static -T$(LINKSCRIPT) --start-group -L$(libgccdir) -lgcc $(LIBS) --end-group
INCPATH = -I . -I $(INCDIR) -I $(INCDIR)/$(TYPE) -I $(INCDIR)/$(TYPE)/$(MODE)/$(MULTIAPP)
LIBPATH = -L $(LIBDIR)
TARGET_SUFFIX = 
TARGET = main$(TARGET_SUFFIX)
OBJS = $(LIBDIR)/NDK_sysapi.o $(OBJDIR)/main.o $(OBJDIR)/gui.o $(OBJDIR)/lib.o $(OBJDIR)/conf.o $(OBJDIR)/layer.o $(OBJDIR)/tools.o \
	$(OBJDIR)/_systest.o 
VPATH = $(INCDIR):$(INCDIR)/$(TYPE):$(INCDIR)/$(TYPE)/$(MODE)/$(MULTIAPP):$(LIBDIR):$(SRCDIR)/main:\
	$(SRCDIR)/systest:

ORIOBJS = $(OBJDIR)/main.o $(OBJDIR)/gui.o $(OBJDIR)/lib.o $(OBJDIR)/conf.o $(OBJDIR)/layer.o $(OBJDIR)/tools.o \
	$(OBJDIR)/systest.o $(OBJDIR)/systest5.o $(OBJDIR)/systest6.o $(OBJDIR)/systest7.o $(OBJDIR)/systest8.o $(OBJDIR)/systest10.o $(OBJDIR)/systest12.o $(OBJDIR)/systest13.o $(OBJDIR)/systest24.o $(OBJDIR)/systest25.o $(OBJDIR)/systest32.o $(OBJDIR)/systest34.o $(OBJDIR)/systest47.o $(OBJDIR)/systest48.o $(OBJDIR)/systest49.o $(OBJDIR)/systest50.o $(OBJDIR)/systest57.o $(OBJDIR)/systest58.o $(OBJDIR)/systest63.o $(OBJDIR)/systest64.o 

.PHONY: all check clean config

all:	config \
	check \
	$(ROMDIR)/$(TARGET) \
	clean

ECHO = @

sinclude $(subst .o,.deps,$(ORIOBJS))

$(ROMDIR)/$(TARGET): $(OBJS) $(LIBS) $(LINKSCRIPT)
	$(ECHO)$(LD) $(OBJS) $(LIBPATH) $(LDFLAGS) -Map $(ROMDIR)/$(Target).map -o $(ROMDIR)/$(TARGET).elf
	$(ECHO)$(OBJCOPY) -S -O binary $(ROMDIR)/$(TARGET).elf $@
	@echo 		--------------------------------------------------
	@echo 		-----------------构建目标代码成功-----------------
	@echo 		--------------------------------------------------

$(OBJDIR)/%.o: %.c
	$(ECHO)$(CC) $(CFLAGS) $(INCPATH) -c -o $@ $<

config: $(subst .o,.deps,$(ORIOBJS))

$(OBJDIR)/%.deps: %.c
#	-@if exist $(subst /,\,$@) attrib -H $(subst /,\,$@)
	$(ECHO)$(CC) -MM $(INCPATH) $<>$@
	$(ECHO)redeal_deps $@ $(MODE)
#	-@attrib +H $(subst /,\,$@)

check:
#	-@if exist $(subst /,\,$(ROMDIR))\$(TARGET) if exist $(subst /,\,$(ROMDIR))\$(MODE).ro_ $(RM) $(subst /,\,$(ROMDIR))\$(MODE).ro_
#	-@if exist $(subst /,\,$(ROMDIR))\$(TARGET) ren $(subst /,\,$(ROMDIR))\$(TARGET) $(MODE).ro_
#	-@if exist $(subst /,\,$(OBJDIR))\_*.o $(RM) $(subst /,\,$(OBJDIR))\_*.o

clean:
#	-@if exist $(ROMDIR)/$(TARGET) if exist $(ROMDIR)/$(MODE).err $(RM) $(ROMDIR)/$(MODE).err

$(OBJDIR)/_systest.o : $(OBJDIR)/systest.o $(OBJDIR)/systest5.o $(OBJDIR)/systest6.o $(OBJDIR)/systest7.o $(OBJDIR)/systest8.o $(OBJDIR)/systest10.o $(OBJDIR)/systest12.o $(OBJDIR)/systest13.o $(OBJDIR)/systest24.o $(OBJDIR)/systest25.o $(OBJDIR)/systest32.o $(OBJDIR)/systest34.o $(OBJDIR)/systest47.o $(OBJDIR)/systest48.o $(OBJDIR)/systest49.o $(OBJDIR)/systest50.o $(OBJDIR)/systest57.o $(OBJDIR)/systest58.o $(OBJDIR)/systest63.o $(OBJDIR)/systest64.o 
	$(ECHO)$(AR) rc $@ $^
	$(ECHO)$(RANLIB) $@

