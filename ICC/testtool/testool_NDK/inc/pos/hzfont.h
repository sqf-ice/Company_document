/*
 * FileName:       
 * Author:         zhangnw  Version: 1.6.15  Date: 2010-12-07
 * Description:    
 * Version:        
 * Function List:  
 *                 1.
 * History:        
 *     <author>   <time>    <version>  <desc>
 */
//#ifndef HZFONT_H
//#define HZFONT_H

/*---------------constants/macro definition---------------------*/

/*------------global variables definition-----------------------*/
	EM_PRN_HZ_FONT hzfont[] = {PRN_HZ_FONT_24x24 ,
								PRN_HZ_FONT_16x32 ,
								PRN_HZ_FONT_32x32 ,
								PRN_HZ_FONT_32x16 ,
								PRN_HZ_FONT_24x32 ,
								PRN_HZ_FONT_16x16 ,
								PRN_HZ_FONT_12x16 ,
								PRN_HZ_FONT_16x8 ,
								PRN_HZ_FONT_24x24A ,			/**<自有宋体24x24点阵*/
								PRN_HZ_FONT_24x24B ,			/**<仿宋24x24点阵*/
								PRN_HZ_FONT_24x24C ,			/**<楷体24x24点阵*/
								PRN_HZ_FONT_24x24USER ,
#if !K21_ENABLE
								PRN_HZ_FONT_12x12A,
#endif								
#if ANDIROD_ENABLE
								PRN_HZ_FONT_16x24,
								PRN_HZ_FONT_16x16BL,
								PRN_HZ_FONT_24x24BL,
#endif								
#if !K21_ENABLE
								PRN_HZ_FONT_48x24A,
								PRN_HZ_FONT_48x24B,
								PRN_HZ_FONT_48x24C,
								PRN_HZ_FONT_24x48A,
								PRN_HZ_FONT_24x48B,
								PRN_HZ_FONT_24x48C,
								PRN_HZ_FONT_48x48A,
								PRN_HZ_FONT_48x48B,
								PRN_HZ_FONT_48x48C,
#endif
							}; 
	char *hzfontstr[] = {"HZ24x24","HZ16x32","HZ32x32","HZ32x16","HZ24x32","HZ16x16","HZ12x16","HZ16x8",
						"HZ24x24A","HZ24x24B","HZ24x24C","HZ24x24USER",
#if !K21_ENABLE
						"HZ12x12A",
#endif						
#if ANDIROD_ENABLE
						"HZ16x24","HZ16x16BL","HZ24x24BL",
#endif						
#if !K21_ENABLE
						"HZ48x24A","HZ48x24B","HZ48x24C","HZ24x48A","HZ24x48B","HZ24x48C","HZ48x48A","HZ48x48B","HZ48x48C",
#endif
					}; 

//#endif

