#file searching path

CROSS_COMPILE=arm-unknown-linux-gnu-
#CROSS_COMPILE=arm-linux-
CC=$(CROSS_COMPILE)gcc
LD=$(CROSS_COMPILE)ld
AR=$(CROSS_COMPILE)ar
RM=rm
CP=cp

#CFLAGS+= -Wall $(CROSS_CFLAGS) -O2 -fPIC $(INCLPATH)
CFLAGS+= -Wall -D_GNU_SOURCE $(CROSS_CFLAGS) -O2 -fPIC $(INCLPATH)

#CFLAGS += -Wall $(INCLPATH)

# 程序链接参数
LDFLAGS=       

#程序中用到的函数库
LIBS=-L../src -lndk -lpthread
#LIBS =-L../src -lndk -L../../nldhandle/src/ -lnld -L../../libdevmgr/src -ldevmgr -L ../../libpms -lpms -L ../../libwifi -lwifi -L ../../libss -lss -L../../../src/gui/core -lguicore -lpthread -L../../../src/gui/widget -lwidget  -L../../../lib/ -ldbus-1 -lpthread -lglib-2.0 -ldbus-glib-1 -lgio-2.0 -lgmodule-2.0 -lgobject-2.0 -lgthread-2.0 -lsqlite3 -lcrypto -lssl -lconfig -lz

# 生成文件的路径
BINDIR = .

# 生成的程序名
NAME = main

#文件搜索路径
VPATH =

#头文件搜索路径
INCLPATH = -I./ -I../../../inc

LIBPATH = 

# 程序中用到的模块
SRCS=$(wildcard *.c)
DIRS=$(notdir $(SRCS))
OBJS=$(patsubst %.c,%.o,$(DIRS))

#包含依赖文件
all: $(NAME)
	
#-include $(subst .o,.deps,$(OBJS))

$(NAME):config $(OBJS)
#	-$(RM) $(BINDIR)/$(NAME)
#	$(CC) -fPIC -shared -o $(BINDIR)/$(NAME) $(OBJS) $(LIBS)
	$(CC) -o $(BINDIR)/$(NAME) $(OBJS) $(LIBS) -lpthread 
			
%.o: %.c 
	$(CC) -c  $(CFLAGS) $(INCLPATH) $< -o $@
	
#自动生成依赖文件
config: $(subst .o,.deps,$(OBJS))

%.deps: %.c
	$(CC) -MM $(INCLPATH) $< > $@

.PHONY:clean
clean:
	-$(RM) $(OBJS)
	-$(RM) $(subst .o,.deps,$(OBJS))
	-$(RM) $(NAME)
