#include <linux/module.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/ioport.h>
#include <asm/io.h>
#include <asm/ioctl.h>
#include <asm/uaccess.h>	/* for copy_from_user */
#include <mach/platform.h>
#include <mach/reg_gpio.h>
#include <mach/irqs.h>
#include <linux/semaphore.h>
#include <linux/wait.h>
#include <linux/poll.h>
#include <linux/jiffies.h>
#include <linux/signal.h>
#include <linux/hardirq.h>
#include <linux/interrupt.h>
#include <linux/spinlock.h>
#include <linux/delay.h>
#include <linux/sched.h>



#include "board.h"
#include "sc_linux.h"
#include "iccard.h"
#include "icc_define.h"
#include "sam.h"
#include "memory.h"
#include "ic_memory.h"

#define SC_DEBUG
#undef SC_DEBUG
#ifdef SC_DEBUG
#define nl_printk(fmt, arg...)  printk(fmt, ##arg)
#else
#define nl_printk(fmt, arg...)    
#endif

ioctl_CMDarg CMDarg;
IC_TYPE Flag_ic_type;
IC_PARAM ic1_param;
static icc_standard_t icc_standard = iso7816;
struct semaphore sem;
wait_queue_head_t outq;
spin_lock sc_lock ={SPIN_LOCK_UNLOCKED,0};
static volatile  int g_mem_stat = 0;
static int sc_fops_open(struct inode *inod,struct file *filp)
{
	nonseekable_open(inod, filp);
	nl_printk("open sc0 file\n");
	return 0;
}
static int sc_fops_release(struct inode *inod,struct file *filp)
{
	nl_printk("close sc0 file \n");
	return 0;
}
static int sc_fops_ioctl(struct inode *inod, struct file *filp,unsigned int cmd,unsigned long arg)
{
	int ret = 0;
	int cardnum =0;
	unsigned long tmp =0;
	unsigned char buf[300]={0,};
	
	switch(cmd)
	{
		case IC_IOCG_WORKSTATUS:/*add sam card work status by guoc 2013-12-02*/
			if(get_sc_state() == SC_State_Ready || mem_powon_flag == 1 ||\
				((ret = get_sc_state_c()) & 0x70) ){
				ret = 1;
			}
			else ret =0;
		break;
		case ioctl_getjiffies:
			tmp =jiffies;
			if(copy_to_user((ioctl_CMDarg __user *)arg, &tmp, sizeof(tmp))){
				nl_printk("getjiff copy to err");
				ret = -EFAULT;
			}
		break;
		case ioctl_gpiotest:
			tmp = 0;
			if(copy_from_user(&tmp, (unsigned long __user *)arg, sizeof(tmp))){
				nl_printk("gpiotest copy to err");
				ret = -EFAULT;
			}
		break;
		case ioctl_gpiotest_end:
			ret = 0;
		break;
		case N_POWERON:
			spin_lock_irqsave(&sc_lock.lock, sc_lock.flag);
			ret = icc_powerup(buf,40,icc_standard);
			spin_unlock_irqrestore(&sc_lock.lock, sc_lock.flag);
			if(ret > 0){
				CMDarg.rapdu_len = ret ;
				memcpy(CMDarg.buf_rapdu,buf,ret);
				ret = 0;
			}
			if(copy_to_user((int __user *)arg,&ret,sizeof(ret))){
				nl_printk("poweron copy to err \n");
				return -EFAULT;
			}
		break;
		case N_POWERDOWN:
			if(copy_from_user(&CMDarg,(ioctl_CMDarg __user *)arg,sizeof(CMDarg))){
				nl_printk("powerdown copy from err");
				ret = -EFAULT;
			}
			cardnum = CMDarg.cmdflag;
			spin_lock_irqsave(&sc_lock.lock, sc_lock.flag);
			if(cardnum == SC_IC1){
				ic_power_down();
			}
			else{
				sam_power_down(cardnum);
			}
			spin_unlock_irqrestore(&sc_lock.lock, sc_lock.flag);
			ret = 0;
			CMDarg.retflag = 0;
			CMDarg.rapdu_len = 0;
			if(copy_to_user((ioctl_CMDarg __user *)arg,&CMDarg,sizeof(CMDarg))){
				nl_printk("powerdown copy to err");
				ret = -EFAULT;
			}
		break;
		case N_GetState:
			ret = get_sc_state();
			CMDarg.retflag = ret ;
		break;
		case N_GetState_c:
			ret = get_sc_state_c();
			CMDarg.capdu_len = 0;
			CMDarg.rapdu_len = 0;
			CMDarg.retflag = ret;
			if(copy_to_user((ioctl_CMDarg __user *)arg,&CMDarg,sizeof(CMDarg))){
				nl_printk("getstste_c copy to err");
				ret = -EFAULT;
			}
			ret =0;
		break;
		case N_GetPowerSta:
			ret = check_iccard_vccon();/*powe down =0*/
			ret = ret ? 0 : 1;/*ret=1 power down*/
			
		break;
		case ioctl_SC_PowerOn:
			if(copy_from_user(&CMDarg,(ioctl_CMDarg __user *)arg,sizeof(CMDarg))){
				nl_printk("sc_poweron copy from err");
				ret = -EFAULT;
			}
			cardnum = CMDarg.cmdflag;
			spin_lock_irqsave(&sc_lock.lock, sc_lock.flag);
			if(cardnum == SC_IC1){
				ret= icc_powerup(buf,40,icc_standard);
				mdelay(1);
			}
			else{
				ret = sam_powerup(buf,40,cardnum,icc_standard);
			}
			spin_unlock_irqrestore(&sc_lock.lock, sc_lock.flag);
			nl_printk("on-1 ret %d \n",ret);
			CMDarg.rapdu_len = 0;
			if(ret == -3){
				CMDarg.retflag = 0xffffffc0;
				ret = -1;
			}
			if(ret >0){
				CMDarg.retflag = 0;
				CMDarg.rapdu_len = ret;
				memcpy(CMDarg.buf_rapdu,buf,ret);
				ret = 0;
			}
			if(g_atr.Vcc == 0)
				CMDarg.cmdflag = 3;
			else
				CMDarg.cmdflag = 0;
			if(copy_to_user((ioctl_CMDarg __user *)arg,&CMDarg,sizeof(CMDarg))){
				nl_printk("getstste_c copy to err");
				ret = -EFAULT;
			}
			//printk("on-2 ret %d \n",ret);
		break;
		case ioctl_SC_Read:
			if(CMDarg.rapdu_len > 0){
				if(copy_to_user((ioctl_CMDarg __user *)arg,&CMDarg,sizeof(CMDarg))){
					nl_printk("getstste_c copy to err");
					ret = -1;
				}
				else{
					CMDarg.rapdu_len = 0;
					ret = 0;
				}
			}
			else{
				ret = -1;
			}
		break;
		case ioctl_SC_APDU:
			if(copy_from_user(&CMDarg,(ioctl_CMDarg __user *)arg,sizeof(CMDarg))){
				nl_printk("sc_poweron copy from err");
				ret = -EFAULT;
			}
			cardnum = CMDarg.cmdflag;
		#if 0
			CMDarg.rapdu_len = 0;
			for(CMDarg.rapdu_len=0;CMDarg.rapdu_len<CMDarg.capdu_len;CMDarg.rapdu_len++){
				printk("%02x ",CMDarg.buf_capdu[CMDarg.rapdu_len]);
			}
			printk("ioctl,apdu\n");
		#endif
			CMDarg.rapdu_len = 0;
			if(cardnum == SC_IC1){	
				if(!check_iccard_pres()){
					nl_printk("no card \n");
					CMDarg.retflag = -20;/*iccard out*/
					ret = -1;
					goto adpu_end;
				}
				else{
					if(!check_iccard_vccon()){
						nl_printk("no pow \n");
						CMDarg.retflag = -7;
						ret = -1;
						goto adpu_end;
					}
				}
				if((CMDarg.capdu_len < 4)||(CMDarg.capdu_len > MaxBufLen_CAPDU)||\
				((CMDarg.capdu_len > 5)&&(CMDarg.buf_capdu[4] != (CMDarg.capdu_len -5)) &&\
				(CMDarg.buf_capdu[4] != (CMDarg.capdu_len -6)))){
					//printk("len %d buf[4] %d len-5 %d",CMDarg.capdu_len,CMDarg.buf_capdu[4],(CMDarg.capdu_len -5));
					nl_printk("cmd err \n");
					CMDarg.retflag = -224;
					ret = -1;
					goto adpu_end;
				}
				spin_lock_irqsave(&sc_lock.lock, sc_lock.flag);
				ret = icc_send_apdu(CMDarg.buf_capdu,CMDarg.capdu_len,buf);
				spin_unlock_irqrestore(&sc_lock.lock, sc_lock.flag);
				nl_printk("ret is %d \n",ret);
				if(ret > 0){
					memcpy(CMDarg.buf_rapdu,buf,ret);
					CMDarg.rapdu_len = ret ;
					ret = 0;
					CMDarg.retflag = 0;
				}
			adpu_end:
				if((CMDarg.retflag < 0) ||(ret < 0)){
					nl_printk("adpu err %d \n",ret);
					CMDarg.rapdu_len = 0;
					CMDarg.buf_rapdu[0] = 0;
				}
				if(copy_to_user((ioctl_CMDarg __user *)arg,&CMDarg,sizeof(CMDarg))){
					//nl_printk("sc_adpu copy to err");
					nl_printk("sc_adpu copy to err");
					ret = -EFAULT;
				}
			}
			else if ((cardnum == SC_SAM1) || (cardnum == SC_SAM2) || (cardnum == SC_SAM3)){
				if(!sam_stat[cardnum].power_stat){
					CMDarg.retflag = -7;
					ret = -1;
					goto sam_apdu_end;
				}
				if((CMDarg.capdu_len < 4)||(CMDarg.capdu_len > MaxBufLen_CAPDU)){
					CMDarg.retflag = -224;
					ret = -1;
					goto sam_apdu_end;
				}
				spin_lock_irqsave(&sc_lock.lock, sc_lock.flag);
				ret = sam_send_apdu(CMDarg.buf_capdu,CMDarg.capdu_len,buf,cardnum);
				spin_unlock_irqrestore(&sc_lock.lock, sc_lock.flag);
				if(ret > 0){
					memcpy(CMDarg.buf_rapdu,buf,ret);
					CMDarg.rapdu_len = ret ;
					ret = 0;
				}
			sam_apdu_end:
				if((CMDarg.retflag < 0) ||(ret < 0)){
					CMDarg.rapdu_len = 0;
					CMDarg.buf_rapdu[0] = 0;
				}
				if(copy_to_user((ioctl_CMDarg __user *)arg,&CMDarg,sizeof(CMDarg))){
					nl_printk("sam_adpu copy to err");
					ret = -EFAULT;
				}
			}
		break;
		case ioctl_SetICType:
			if(copy_from_user(&CMDarg,(ioctl_CMDarg __user *)arg,sizeof(CMDarg))){
				nl_printk("sc_poweron copy from err");
				ret = -EFAULT;
			}
			Flag_ic_type.ic1 = CMDarg.buf_capdu[0];
			Flag_ic_type.ic2 = CMDarg.buf_capdu[1];
			if((Flag_ic_type.ic1 == 0x78) && (Flag_ic_type.ic2 == 0x16))
				icc_standard = iso7816;
			else icc_standard = emv;
			ret = 0;
		break;
		case ioctl_GetICType:
			CMDarg.buf_capdu[0] = Flag_ic_type.ic1;
			CMDarg.buf_capdu[1] = Flag_ic_type.ic2;
			ret = 0;
			if(copy_to_user((ioctl_CMDarg __user *)arg,&CMDarg,sizeof(CMDarg))){
				nl_printk("getic copy to err");
				ret = -EFAULT;
			}
		break;
		case ioctl_ClearICInit:
			ret = 0;
		break;
		case ioctl_ResetSC:
			ret = 0;
		break;
		case ioctl_mem_init_card:
			spin_lock_irqsave(&sc_lock.lock, sc_lock.flag);
			tda8035_mem_gpio_init();
			spin_unlock_irqrestore(&sc_lock.lock, sc_lock.flag);
			ret = 0;
		break;
		case ioctl_mem_card_powerup:
			buf[0] = 0;
			if(copy_from_user(&CMDarg,(ioctl_CMDarg __user *)arg,sizeof(CMDarg))){
				nl_printk("mem_poweron copy from err");
				ret = -EFAULT;
			} 
			CMDarg.rapdu_len = 0;
			spin_lock_irqsave(&sc_lock.lock, sc_lock.flag);
			ret = memory_poweron(CMDarg.cmdflag,buf);
			spin_unlock_irqrestore(&sc_lock.lock, sc_lock.flag);
			if((ret == 0) && (buf[0] > 0)){
				CMDarg.rapdu_len = buf[0];
				memcpy(CMDarg.buf_rapdu,&buf[1],CMDarg.rapdu_len);
				g_mem_stat = 1;
				ret = 0;
			}
			else{
				tda8035_mem_powdown();
				g_mem_stat = 0;
				ret = -1;
			}
			if(copy_to_user((ioctl_CMDarg __user *)arg,&CMDarg,sizeof(CMDarg))){
				nl_printk("mem_powon copy to err");
				ret = -EFAULT;
			}
		break;
		case ioctl_mem_card_powerdown:
			spin_lock_irqsave(&sc_lock.lock, sc_lock.flag);
			tda8035_mem_powdown();
			spin_unlock_irqrestore(&sc_lock.lock, sc_lock.flag);
			g_mem_stat = 0;
			ret = check_mem_vcc_on();
			if(ret == 1)
				ret = -1;
			else
				ret = 0;
			return ret;
		break;
		case ioctl_mem_card_getsta:
			spin_lock_irqsave(&sc_lock.lock, sc_lock.flag);
			CMDarg.retflag = 0;
			if(check_mem_insert())
				CMDarg.retflag |= 0x01;
			if(g_mem_stat)
				CMDarg.retflag |= 0x02;
			spin_unlock_irqrestore(&sc_lock.lock, sc_lock.flag);
			if(copy_to_user((ioctl_CMDarg __user *)arg,&CMDarg,sizeof(CMDarg))){
				nl_printk("mem_get_stat copy to err");
				ret = -EFAULT;
			}
			ret = 0;
		break;
		case ioctl_mem_card_rw:
			if(copy_from_user(&CMDarg,(ioctl_CMDarg __user *)arg,sizeof(CMDarg))){
				nl_printk("mem_rw copy from err");
				ret = -EFAULT;
			}
			spin_lock_irqsave(&sc_lock.lock, sc_lock.flag);
			ret = memory_rw(CMDarg.cmdflag,CMDarg.buf_capdu,(unsigned char *)(&(CMDarg.rapdu_len)),CMDarg.buf_rapdu);
			spin_unlock_irqrestore(&sc_lock.lock, sc_lock.flag);
			if(copy_to_user((ioctl_CMDarg __user *)arg,&CMDarg,sizeof(CMDarg))){
				nl_printk("mem_rw copy to err");
				ret = -EFAULT;
			}
		break;
		case ioctl_getvision:
			CMDarg.rapdu_len = sizeof(ModuleVersion) -1;
			memcpy(CMDarg.buf_rapdu, ModuleVersion, CMDarg.rapdu_len);
			CMDarg.retflag = 0;
			if(copy_to_user((ioctl_CMDarg __user *)arg,&CMDarg,sizeof(CMDarg))){
				nl_printk("getic copy to err");
				ret = -EFAULT;
			}
		break;
		case ioctl_ic_insert_times:
			if(copy_to_user((unsigned long __user *)arg, &g_iccard_insert_cnt, sizeof(g_iccard_insert_cnt))){
				nl_printk("insert times copy to err");
				ret = -EFAULT;
			}
		break;
		case ioctl_sc_suspend:
		{
			int suspend = 0;
			if(copy_from_user(&suspend,(ioctl_CMDarg __user *)arg,sizeof(suspend))){
				nl_printk("suspend copy from err");
				ret = -EFAULT;
			}
			ic_sam_suspend_ctrl(suspend);
		}
		default:
		ret = -ENOTTY;
		break;
		
	}
	return ret;
}
static ssize_t sc_fops_write(struct file *filp,const char __user *buf,size_t count,loff_t *f_ops)
{
	int ret = 0;
	if(count <= 0)
		return -1;
	ret = count;
	if (down_interruptible(&sem)){
		return -ERESTARTSYS;
	}
	if(((ret = check_iccard_vccon())== 0)||count > MaxBufLen_CAPDU){
		ret = -1;
		goto wrt_end;
	}
	if(copy_from_user(CMDarg.buf_capdu,buf,count)){
		ret = -1;
		goto wrt_end;
	}
	CMDarg.capdu_len = count;
	spin_lock_irqsave(&sc_lock.lock, sc_lock.flag);
	CMDarg.rapdu_len = icc_send_apdu(CMDarg.buf_capdu,CMDarg.capdu_len,CMDarg.buf_rapdu);
	spin_unlock_irqrestore(&sc_lock.lock, sc_lock.flag);
	if(ic1_param.flag_pollselect){
		ic1_param.flag_pollselect = 0;
		wake_up_interruptible(&outq);
	}
wrt_end:
	up(&sem);
	return count;
	
}
static ssize_t sc_fops_read(struct file *filp,char __user *buf,size_t count,loff_t *f_ops)
{
	int ret =0,rval =0;
	if (down_interruptible(&sem)){
		return - ERESTARTSYS;
	}
	if((ret =check_iccard_vccon())== 0){
		ret = -1;
		goto read_end;
	}
	rval =CMDarg.rapdu_len;
	if(rval){
		if(copy_to_user(buf,CMDarg.buf_rapdu,rval)){
			ret = -EFAULT;
			goto read_end;
		}
		else{
			CMDarg.rapdu_len = 0;
			ret = rval;
		}
	}
read_end:
	up(&sem);
	return ret;
}
static unsigned int sc_fops_poll(struct file *filp,poll_table *wait)
{
	unsigned int mask = 0;
	if (down_interruptible(&sem)){
		return - ERESTARTSYS;
	}
	ic1_param.flag_pollselect= 1;
	poll_wait(filp, &outq,wait);
	if(CMDarg.rapdu_len > 0){/*read*/
		mask |= POLLIN | POLLRDNORM ;
	}
	if(g_atr.ATROK){/*write*/
		mask |= POLLOUT | POLLWRNORM;
	}
	up(&sem);
	return mask;
}
static struct file_operations sc_fops =
{
	.owner=		THIS_MODULE,
	.open=		sc_fops_open,
	.release=	sc_fops_release,
	.read=		sc_fops_read,
	.write=		sc_fops_write,
	.ioctl=		sc_fops_ioctl,
	.poll=		sc_fops_poll,
	.llseek=	no_llseek
};
static struct miscdevice sc_dev =
{
	.minor = sc_dev_minor,
	.name = sc_dev_name,
	.fops = &sc_fops,
};
static int sc_init(void)
{
	int ret = 0;
	if(misc_register(&sc_dev)){
		printk("\nSmartIC Insmod Err ?\n");
		misc_deregister(&sc_dev);
		return -ENODEV;
	}
	printk("-------------SmartIC Insmod OK!-------------\n");	
	printk("   Ver %s ! \n", ModuleVersion);	
	printk("TOTAL SAM CARD NUM = %d \n",g_sam_card_num);
	ic_sam_gpio_init();
	ret = request_irq(IRQ_OSMARTCARD0, icc_isr,IRQF_TIMER, "icc", NULL);
	if (ret != 0) {
		printk("---request irq err---\n");
		return ret;
	}
	printk(" -------------------------------------------\n");
	init_MUTEX(&sem);
	init_waitqueue_head(&outq);
	ic1_param.flag_pollselect= 0;
	g_atr.ATROK = 0;
	return 0;
	
}
static void sc_exit(void)
{
	
	//powdown_sc_module();
	//sam_power_down();
	ic_sam_gpio_exit();
	free_irq(IRQ_OSMARTCARD0, NULL);
	misc_deregister(&sc_dev);
	printk(" -------SmartIC[TDA8035] Driver Exit ! -----\n");	
}

module_init(sc_init);
module_exit(sc_exit);
MODULE_LICENSE("GPL");




