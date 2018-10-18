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
//#ifndef ICSAM_H
//#define ICSAM_H

/*---------------constants/macro definition---------------------*/

/*------------global variables definition-----------------------*/
	EM_ICTYPE CardNo[] = {
#if IC1_ENABLE	 //默认有修改为IC1_ENABLE 20180510 N510不支持IC卡
					ICTYPE_IC, 
#endif
#if SAM1_ENABLE
					ICTYPE_SAM1, 
#endif
#if SAM2_ENABLE
					ICTYPE_SAM2, 
#endif
#if SAM3_ENABLE
					ICTYPE_SAM3, 
#endif
#if SAM4_ENABLE
					ICTYPE_SAM4, 
#endif
					0xff};

	char CardNoStr[][8] = {
#if IC1_ENABLE	 //默认有修改为IC1_ENABLE 20180510 N510不支持IC卡
						"IC1", 
#endif
#if SAM1_ENABLE
						"SAM1", 
#endif
#if SAM2_ENABLE
						"SAM2", 
#endif
#if SAM3_ENABLE
						"SAM3", 
#endif
#if SAM4_ENABLE
						"SAM4", 
#endif
						"unknown"};

//#endif

