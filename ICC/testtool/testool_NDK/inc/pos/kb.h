/*
 * FileName:       
 * Author:         zhangnw  Version: 3.0.0  Date: 2012-12-26
 * Description:  此文件最早是从应用部的lgui.h头文件中提取的,之后将根据具体机型来变化.统一在本文件中  
 * Version:        
 * Function List:  
 *                 1.
 * History:        
 *     <author>   <time>    <version>  <desc>
 */
#ifndef _KB_H_
#define _KB_H_

/*---------------constants/macro definition---------------------*/
#define KEY_0				'0'
#define KEY_1				'1'
#define KEY_2				'2'
#define KEY_3				'3'
#define KEY_4				'4'
#define KEY_5				'5'
#define KEY_6				'6'
#define KEY_7				'7'
#define KEY_8				'8'
#define KEY_9				'9'

#define KEY_POINT			'.'	//0x2e
#define KEY_ALPHA			0x1c

#define KEY_F1				0x01
#define KEY_F2				0x02
#define KEY_F3				0x03
#define KEY_F4				0x04
#define KEY_F5				0x05
#define KEY_F6				0x06
#define KEY_F7				0x07
#define KEY_F8				0x08
#define KEY_F9				0x09

#define KEY_BACK			0x0a		/**<退格键*/
#define KEY_ENTER			0x0d		/**<确认键*/
#define KEY_ESC				0x1b		/**<取消键*/

#define KEY_MENU			0x1d		/**<菜单键*/
#define KEY_ENTERPAPER  	0xb1		/**<进纸键*/
#define KEY_BACKPAPER   	0xb2		/**<退纸键*/

//以下差异化的地方,在新增机具时要注意维护!
#if defined GP720
#define KEY_UP				0x11
#define KEY_DOWN			0x12
#define KEY_LEFT			0x13
#define KEY_RIGHT			0x14
#elif defined SP60||defined SP50||defined SP80||defined ME31S||defined ME31SLS||defined SP33||defined SP600||defined NL8510GP||CPU5830X_ENABLE||OVERSEAS_ENABLE
#define KEY_UP				KEY_F1
#define KEY_DOWN			KEY_F2
#define KEY_LEFT			KEY_F1
#define KEY_RIGHT			KEY_F2
#else
#define KEY_UP				KEY_LEFT
#define KEY_DOWN			KEY_RIGHT
#define KEY_LEFT			KEY_F1
#define KEY_RIGHT			KEY_F2
#endif

//新增PS2键盘按键
#if defined ME50C
#define  PS2_KEY_CHAR1  0xE052
#define  PS2_KEY_BKSP   0xE066
#define  PS2_KEY_SPACE  0xE029
#define  PS2_KEY_TAB    0xE00D
#define  PS2_KEY_CAPS   0xE058
#define  PS2_KEY_L_SHFT 0xE012
#define  PS2_KEY_L_CTRL 0xE014
#define  PS2_KEY_L_GUI  0xE01F
#define  PS2_KEY_L_ALT  0xE011
#define  PS2_KEY_R_SHFT 0xE059
#define  PS2_KEY_R_CTRL 0xE014
#define  PS2_KEY_R_GUI  0xE027
#define  PS2_KEY_R_ALT  0xE011
#define  PS2_KEY_APPS   0xE02F
#define  PS2_KEY_ENTER  0xE05A
#define  PS2_KEY_ESC    0xE076
#define  PS2_KEY_F1     0xE005
#define  PS2_KEY_F2     0xE006
#define  PS2_KEY_F3     0xE004
#define  PS2_KEY_F4     0xE00C
#define  PS2_KEY_F5     0xE003
#define  PS2_KEY_F6     0xE00B
#define  PS2_KEY_F7     0xE083
#define  PS2_KEY_F8     0xE00A
#define  PS2_KEY_F9     0xE001
#define  PS2_KEY_F10    0xE009
#define  PS2_KEY_F11    0xE078
#define  PS2_KEY_F12    0xE007
#define  PS2_KEY_PRNT   0xE012
#define  PS2_KEY_SCRN   0xE07C
#define  PS2_KEY_SCROLL 0xE07E
#define  PS2_KEY_PAUSE  0xE0E1
#define  PS2_KEY_INSERT 0xE070
#define  PS2_KEY_HOME   0xE06C
#define  PS2_KEY_PG_UP  0xE07D
#define  PS2_KEY_DELETE 0xE071
#define  PS2_KEY_END    0xE069
#define  PS2_KEY_PG_DN  0xE07A
#define  PS2_KEY_U_ARROW 0xE075
#define  PS2_KEY_L_ARROW 0xE06B
#define  PS2_KEY_D_ARROW 0xE072
#define  PS2_KEY_R_ARROW 0xE074
#define  PS2_KEY_NUM     0xE077
#define  PS2_KEY_EN   0xE05A
#endif

#if defined ME50C||defined ME50H
//#define    K_DOT			 0x2E			/**小数点*/
#define	K_F00			 0xE1		 /**00键*/
#define	K_MENU			 0xE4 		 /**菜单键*/
#define	K_FUNC			 0xE3		/**功能键 */
#define    K_ADD             	 0x2b          /**<+键*/
#endif

#define KEY_NULL			0xfe		/**<键盘上无此按键，值定为0xfe*/

#endif

