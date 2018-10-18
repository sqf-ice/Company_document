TOPDIR	:= ../../..
#file searching path

CROSS_COMPILE=arm-unknown-linux-gnu-
#CROSS_COMPILE=arm-linux-
CC=$(CROSS_COMPILE)gcc
LD=$(CROSS_COMPILE)ld
AR=$(CROSS_COMPILE)ar
RM=rm
CP=cp

#CFLAGS += -Wall $(INCPATH) -DBSP_OS_IS_LINUX -DCONFIG_NLGP730_L1
CFLAGS += -Wall $(INCPATH) -DCONFIG_NLGP730_L1

# 程序链接参数
LDFLAGS=       

#程序中用到的函数库
#LIBS= -L../../../../core -lguicore -L../../../../dbus_widget -lwidget -L../../../../posapi/src -lappapi_c -L../../../../guifs/lib -ldbus-1 
LIBS= -L ../lib -lndk
# 生成文件的路径
BINDIR = ..

# 生成的程序名
NAME = main

#文件搜索路径
VPATH =../src


#头文件搜索路径
INCLPATH = -I../inc -I$(TOPDIR)/gui/inc -I$(TOPDIR)/../sdk/ndk/inc 

LIBPATH = 

# 程序中用到的模块
SRCS=$(wildcard $(VPATH)/*.c)
OBJS=$(patsubst $(VPATH)/%.c,%.o,$(SRCS))

#包含依赖文件
all: $(NAME)

#-include $(subst .o,.deps,$(OBJS))

$(NAME):config $(OBJS)
	-$(RM) $(BINDIR)/$(NAME)
#	$(CC) -fPIC -shared -o $(BINDIR)/$(NAME) $(OBJS) $(LIBS)
	$(CC) -o $(BINDIR)/$(NAME) $(OBJS) $(LIBS)
		
%.o: %.c 
	$(CC) -c  $(CFLAGS) $(INCLPATH) $< -o $@
	
#自动生成依赖文件
config: $(subst .o,.deps,$(OBJS))

%.deps: %.c
	$(CC) -MM $(INCLPATH) $< > $@


.PHONY:install
install:
	cp ../download ../../../../install/guiapp/bin/download/

.PHONY:clean
clean:
	-$(RM) $(OBJS)
	-$(RM) $(subst .o,.deps,$(OBJS))
	-$(RM) ../$(NAME)
