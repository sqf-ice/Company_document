
#file searching path

ifeq ($(CONFIG_BCM5830X_PHOENIX_PLUS),y)
CROSS_COMPILE=arm-linux-gnueabihf-
LIBS= -L./lib/BCM5830X_phoenixplus -lndk
#EXTRA_CFLAGS += -DPHOENIX_PLUS
CFLAGS += -DPHOENIX_PLUS
BINDIR = ./build/BCM5830X_phoenixplus
OBJDIR = ./build/BCM5830X_phoenixplus/obj
endif

ifeq ($(CONFIG_BCM5830X),y)
CROSS_COMPILE=arm-none-eabi-
LIBS= -L./lib/BCM5830X  -lndk
BINDIR = ./build/BCM5830X
OBJDIR = ./build/BCM5830X/obj
endif

ifeq ($(CONFIG_BCM5892),y)
CROSS_COMPILE=arm-unknown-linux-gnu-
LIBS= -L./lib/BCM5892  -ldevmgr -lndk -ldbus-1 -lpthread -lconfig -lglib-2.0 -ldbus-glib-1 -lgobject-2.0 -lgthread-2.0 -lgio-2.0 -lgmodule-2.0 -lz -lm
BINDIR = ./build/BCM5892
OBJDIR = ./build/BCM5892/obj
endif

CC=$(CROSS_COMPILE)gcc
LD=$(CROSS_COMPILE)ld
AR=$(CROSS_COMPILE)ar
RM=rm
CP=cp

#CFLAGS+= -Wall -DNDEBUG -D CONFIG_NLGP730_L1 $(CROSS_CFLAGS) -O2 -fPIC $(INCLPATH)
CFLAGS+= -Wall -DNDEBUG  $(CROSS_CFLAGS) -O2 -fPIC $(INCLPATH)

# 程序链接参数
LDFLAGS=

#程序中用到的函数库


# 生成文件的路径
#BINDIR = ./build

# 生成的程序名
NAME = EMV_L1_LoopBack

#文件搜索路径
VPATH =.

#头文件搜索路径
INCLPATH = -I.

LIBPATH =

# 程序中用到的模块
SRCS=$(wildcard $(VPATH)/*.c)
#SRCS = selftest.c
OBJS=$(patsubst $(VPATH)/%.c,%.o,$(SRCS))
#OBJS=main.o

#包含依赖文件
all: $(NAME)

#-include $(subst .o,.deps,$(OBJS))

$(NAME):config $(OBJS)
#	-$(RM) $(BINDIR)/$(NAME)
#	$(CC) -fPIC -shared -o $(BINDIR)/$(NAME) $(OBJS) $(LIBS)
	@echo "	###############################################################"
	@echo "	Please select the platform"
	@echo "	>> [BCM5830X_phoenixplus]   make CONFIG_BCM5830X_PHOENIX_PLUS=y"
	@echo "	>> [BCM5892]                make CONFIG_BCM5892=y"
	@echo "	###############################################################"
	@mkdir -p $(BINDIR)
	$(CC) -o $(BINDIR)/$(NAME) $(OBJDIR)/$(OBJS) $(LIBS)

%.o: %.c
#	$(CC) -c  $(CFLAGS) $(INCLPATH) $< -o $@
#	@echo "$@, $(SRCDIR)"
	@mkdir -p $(OBJDIR)
	$(CC) $(INCLPATH) $(CFLAGS) -o $(OBJDIR)/$@ -c $<

#自动生成依赖文件
config: $(subst .o,.deps,$(OBJS))

%.deps: %.c
	$(CC) -MM $(INCLPATH) $< > $(OBJDIR)/$@

.PHONY:clean
clean:
#	find . -name "$(NAME)"  | xargs rm -f	
	-$(RM) $(BINDIR)/$(NAME)
	-$(RM) $(OBJDIR)/*.*
