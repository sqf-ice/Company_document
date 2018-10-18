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
//#ifndef ZMFONT_H
//#define ZMFONT_H

/*---------------constants/macro definition---------------------*/

/*------------global variables definition-----------------------*/
	EM_PRN_ZM_FONT dotfont[] = {
//#if !ANDIROD_ENABLE
								PRN_ZM_FONT_8x16 ,
//#endif					
								PRN_ZM_FONT_16x16,
								PRN_ZM_FONT_16x32,
								PRN_ZM_FONT_24x32,
								PRN_ZM_FONT_6x8,
								PRN_ZM_FONT_8x8,
								PRN_ZM_FONT_5x7,
								PRN_ZM_FONT_5x16,
								PRN_ZM_FONT_10x16,
								PRN_ZM_FONT_10x8,
								PRN_ZM_FONT_12x16A,
								PRN_ZM_FONT_12x24A,
								PRN_ZM_FONT_16x32A,
								PRN_ZM_FONT_12x16B,
								PRN_ZM_FONT_12x24B,
								PRN_ZM_FONT_16x32B,
								PRN_ZM_FONT_12x16C,
								PRN_ZM_FONT_12x24C,
								PRN_ZM_FONT_16x32C,
								PRN_ZM_FONT_24x24A,
								PRN_ZM_FONT_32x32A,
								PRN_ZM_FONT_24x24B,
								PRN_ZM_FONT_32x32B,
								PRN_ZM_FONT_24x24C,
								PRN_ZM_FONT_32x32C,
								PRN_ZM_FONT_12x12 ,
#if ANDIROD_ENABLE
								PRN_ZM_FONT_12x12A,
							    PRN_ZM_FONT_12x12B,
							    PRN_ZM_FONT_12x12C,
							    PRN_ZM_FONT_8x12,
							    PRN_ZM_FONT_8x24,
#endif
						    	PRN_ZM_FONT_8x32,
#if ANDIROD_ENABLE
							    PRN_ZM_FONT_12x32A,
							    PRN_ZM_FONT_12x32B,
							    PRN_ZM_FONT_12x32C,
							    PRN_ZM_FONT_8x16BL,
							    //PRN_ZM_FONT_16x16BL,
							    PRN_ZM_FONT_12x24BL,
#endif
							};
	char *dotfontstr[] = {
//#if !ANDIROD_ENABLE
						"ZM8x16",
//#endif
						"ZM16x16","ZM16x32","ZM24x32","ZM6x8","ZM8x8","ZM5x7","ZM5x16",
						"ZM10x16","ZM10x8","ZM12x16A","ZM12x24A","ZM16x32A","ZM12x16B",
						"ZM12x24B","ZM16x32B","ZM12x16C","ZM12x24C","ZM16x32C",
						"ZM24x24A","ZM32x32A","ZM24x24B","ZM32x32B","ZM24x24C","ZM32x32C","ZM12x12",
#if ANDIROD_ENABLE
						"ZM12x12A","ZM12x12B","ZM12x12C","ZM8x12","ZM8x24",
#endif					
						"ZM8x32",
#if ANDIROD_ENABLE
						"ZM12x32A","ZM12x32B","ZM12x32C","ZM8x16BL",/*"ZM16x16BL",*/"ZM12x24BL",
#endif	
					};
//#endif

