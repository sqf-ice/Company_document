############################
#define obj
############################
MAINOBJS = BOARD/bcm5892_ic.o IC/iccard.o SAM/sam.o MEMORY/memory.o MEMORY/at24xx.o MEMORY/sc102_1604.o MEMORY/sle4428.o \
           MEMORY/sle4442.o MEMORY/at88sc153.o MEMORY/at88sc1608.o MEMORY/ic_memory.o SC/sc.o
SP60OBJS = BOARD/sp60.o
SP50OBJS = BOARD/sp50.o
SP80V1OBJS = BOARD/sp80v1.o
SP80V2OBJS = BOARD/sp80v2.o
IM90OBJS = BOARD/IM90.o
ME31OBJS = BOARD/ME31.o
GP720OBJS = BOARD/gp720.o
SPU90OBJS = BOARD/spu90.o
SP33OBJS = BOARD/sp33.o
SP600OBJS = BOARD/sp600.o
#############################

EXTRA_CFLAGS +=  -I $(PWD)/BOARD -I $(PWD)/IC -I $(PWD)/MEMORY -I $(PWD)/SAM  -I $(PWD)/SC
#########SP60###############
ifeq ($(CONFIG_SP60),y)
obj-m += sc_tda8035.o
sc_tda8035-objs := $(SP60OBJS) $(MAINOBJS) 
##########SP50###############
else ifeq ($(CONFIG_SP50),y) 
obj-m += sc_tda8035.o
sc_tda8035-objs := $(SP50OBJS) $(MAINOBJS) 
##########SP80V1###############
else ifeq ($(CONFIG_SP80V1),y) 
obj-m += sc_tda8035.o
sc_tda8035-objs := $(SP80V1OBJS) $(MAINOBJS) 
##########SP80V2###############
else ifeq ($(CONFIG_SP80V2),y) 
obj-m += sc2_tda8035.o
sc2_tda8035-objs := $(SP80V2OBJS) $(MAINOBJS) 
##########IM90###############
else ifeq ($(CONFIG_IM90),y) 
obj-m += sc_tda8035.o
sc_tda8035-objs := $(IM90OBJS) $(MAINOBJS)
##########ME31############### 
else ifeq ($(CONFIG_ME31),y) 
obj-m += sc_tda8035.o
sc_tda8035-objs := $(ME31OBJS) $(MAINOBJS) 
##########GP720###############
else ifeq ($(CONFIG_GP720),y) 
obj-m += sc_tda8035.o
sc_tda8035-objs := $(GP720OBJS) $(MAINOBJS) 
##########SPU90###############
else ifeq ($(CONFIG_SPU90),y) 
EXTRA_CFLAGS += -DCONFIG_SPU90
obj-m += sc_tda8035.o
sc_tda8035-objs := $(SPU90OBJS) $(MAINOBJS) 
##########SPU90###############
else ifeq ($(CONFIG_SP33),y) 
obj-m += sc_tda8035.o
sc_tda8035-objs := $(SP33OBJS) $(MAINOBJS)
else ifeq ($(CONFIG_SP600),y) 
obj-m += sc_tda8035.o
sc_tda8035-objs := $(SP600OBJS) $(MAINOBJS)
endif

USERNAME := $(shell logname)
PWD := $(shell pwd)
#KDIR := /usr/src/linux-2.6.32.9
KDIR := /usr/src/BCM5892_linux-2.6.32-env
default:
	@echo "	###############################################################"
	@echo "	Please make sure the following setting:"
	@echo "	1.sp60 make CONFIG_SP60=y "  
	@echo "	2.sp50 make CONFIG_SP50=y "  
	@echo "	3.sp80v1 make CONFIG_SP80V1=y "  
	@echo "	4.sp80v2 make CONFIG_SP80V2=y "  
	@echo "	5.im90 make CONFIG_IM90=y " 
	@echo "	6.me31 make CONFIG_ME31=y " 
	@echo "	7.gp720 make CONFIG_GP720=y " 
	@echo "	8.spu90 make CONFIG_SPU90=y " 
	@echo "	9.sp33 make CONFIG_SP33=y " 
	@echo "	10.sp600 make CONFIG_SP600=y " 
	@echo "	###############################################################"
	$(MAKE) -C $(KDIR) M=$(PWD) modules

install:
	cp -f sc_tda8035.ko $(INSTALLDIR)
	
clean:
	rm -f *.ko *.o *.mod.c .*.cmd BOARD/*.o  BOARD/*.mod.c BOARD/.*.cmd IC/*.o IC/*.mod.c IC/.*.o MEMORY/*.o MEMORY/*.mod.c\
	MEMORY/.*.cmd SAM/*.o SAM/*.mod.c SAM/.*.cmd SC/*.o SC/*.mod.c SC/.*.cmd Modules.symvers .tmp_versions/*
	rm -rf .tmp_versions
