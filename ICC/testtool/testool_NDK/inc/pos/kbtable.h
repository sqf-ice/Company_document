/*
 * FileName:       
 * Author:         zhangnw  Version: 3.0.0  Date: 2012-12-26
 * Description:  
 * Version:        
 * Function List:  
 *                 1.
 * History:        
 *     <author>   <time>    <version>  <desc>
 */
//#ifndef _KB_H_
//#define _KB_H_

/*---------------constants/macro definition---------------------*/

/*------------global variables definition-----------------------*/
	//建一张键号与键码的表(使用产品宏,条件编译)
	KeyTable table[] =
	{
#if	defined GP730||defined GP730CP||defined GP730TOUCH||defined SPW70
		{"向上箭头", KEY_F1}, {"菜单", KEY_MENU}, {"向下箭头", KEY_F2}, 
		{"退格", BACKSPACE}, {"确认", ENTER }, {"取消", ESC}, {"小数点", DOT}, {"字母", KEY_ALPHA}, 
#elif defined GP720
		{"F1", KEY_F1}, {"F2", KEY_F2}, {"F3", KEY_F3}, {"F4", KEY_F4},
		{"向上箭头", KEY_UP}, {"向下箭头", KEY_DOWN}, {"向左箭头", LEFT}, {"向右箭头", RIGHT},	
		{"退格", BACKSPACE}, {"确认", ENTER }, {"取消", ESC}, {"小数点", DOT}, {"字母", KEY_ALPHA}, 
#elif defined ME31||defined SP10
		{"退格", BACKSPACE}, {"确认", ENTER }, {"取消", ESC}, {"F1/向上箭头", KEY_F1}, {"F2/向下箭头", KEY_F2}, 
#elif defined ME30||defined ME30S||defined ME30THM||defined ME30MH||defined ME30GX||defined ME32||defined ME32GX||defined ME32THM//14键的产品
	 	{"退格", BACKSPACE}, {"确认", ENTER }, {"取消", ESC}, {"功能键",KEY_F1}, 
#elif defined ME50S//15键的产品
		{"退格", BACKSPACE}, {"确认", ENTER }, {"取消", ESC}, {"#键", KEY_ALPHA}, {"*键", DOT},
#elif defined ME50//15键的产品
		{"退格", BACKSPACE}, {"确认", ENTER }, {"取消", ESC}, {"#键/向下箭头", KEY_F2}, {"*键/向上箭头", KEY_F1},
#elif defined ME50C||defined ME50H//19键的产品
		{"退格", BACKSPACE}, {"确认", ENTER }, {"取消", ESC},  {"功能键",KEY_F3/*K_FUNC*/}, {"菜单键", K_MENU},{"关机键",KEY_F5}, {"#键/向下箭头", K_F2}, {"*键/向上箭头", K_F1}, {"+键", K_ADD},		
#elif defined ME50N||defined ME50NX   //||defined ME68//16键的产品  ME68没有功能键 20180416 sull modify
		{"退格", BACKSPACE}, {"确认", ENTER }, {"取消", ESC}, {"功能键",KEY_F3},  {"#键/向下箭头", KEY_F2}, {"*键/向上箭头", KEY_F1},
#elif defined ME68
		{"退格", BACKSPACE}, {"确认", ENTER }, {"取消", ESC},  {"#键/向下箭头", KEY_F2}, {"*键/向上箭头", KEY_F1},
#elif ANDIROD_ENABLE//高端产品//defined IM81||defined N900||
		{"退格", BACKSPACE}, {"确认", ENTER }, {"取消", ESC}, 
#elif defined SP80||defined SP80G||defined NL8510GP||defined SP930G||defined SP930PG||defined SP830PG||CPU5830X_ENABLE||defined SP610//18键的产品
		{"退格", BACKSPACE}, {"确认", ENTER }, {"取消", ESC}, {"F1/向上箭头", KEY_F1}, {"F2/向下箭头", KEY_F2}, {"菜单/F3", KEY_F3},  {"小数点", DOT}, {"字母", KEY_ALPHA}, 
#elif defined SP630 || defined SP630PG
		{"退格", BACKSPACE}, {"确认", ENTER }, {"取消", ESC}, {"F1", KEY_F1}, {"F2", KEY_F2}, {"F3", KEY_F3}, {"F4", KEY_F4},  {"电源", KEY_MENU}, {"小数点", DOT}, {"字母", KEY_ALPHA}, 
#elif defined ME62 //20键电源键无键值
		{"退格", BACKSPACE}, {"确认", ENTER }, {"取消", ESC}, {"小数点", DOT}, {"#键", KEY_ALPHA},
		{"F1", KEY_F1}, {"F2", KEY_F2}, {"F3", KEY_F3}, {"F4", KEY_F4},
#else	
		//默认的产品是19键包括9个功能键和10个数字键,例如SP60,SP50,ME31S,SP33,SP600,以及对应的海外产品
		{"退格", BACKSPACE}, {"确认", ENTER }, {"取消", ESC}, {"小数点", DOT}, {"字母", KEY_ALPHA},
		{"F1/向上箭头", KEY_F1}, {"F2/向下箭头", KEY_F2}, {"菜单/F3", KEY_F3}, {"电源", KEY_F4},
#endif
		//作为POS产品,不论具体是什么产品,0-9是必定都具备的
		{"数字0", '0'}, {"数字1", '1'}, {"数字2", '2'}, {"数字3", '3'}, {"数字4", '4'}, 
		{"数字5", '5'}, {"数字6", '6'}, {"数字7", '7'}, {"数字8", '8'}, {"数字9", '9'}
	};

#if defined ME50C  //ME50C新增PS2键盘，单独建表  20180523  sull  add
	//键盘左边对应的数字表
	KeyTable ps2_num [] = 
	{
		{"数字0", '0'}, {"数字1", '1'}, {"数字2", '2'}, {"数字3", '3'}, {"数字4", '4'}, 
		{"数字5", '5'}, {"数字6", '6'}, {"数字7", '7'}, {"数字8", '8'}, {"数字9", '9'}
	};
	//键盘左边对应的大写字母表
	KeyTable ps2_big_letter [] = 
	{
		{"大写字母A", 'A'}, {"大写字母B", 'B'}, {"大写字母C", 'C'}, {"大写字母D", 'D'}, {"大写字母E", 'E'}, 
		{"大写字母F", 'F'}, {"大写字母G", 'G'}, {"大写字母H", 'H'}, {"大写字母I", 'I'}, {"大写字母J", 'J'},
		{"大写字母K", 'K'}, {"大写字母L", 'L'}, {"大写字母M", 'M'}, {"大写字母N", 'N'}, {"大写字母O", 'O'}, 
		{"大写字母P", 'P'}, {"大写字母Q", 'Q'}, {"大写字母R", 'R'}, {"大写字母S", 'S'}, {"大写字母T", 'T'},
		{"大写字母U", 'U'}, {"大写字母V", 'V'}, {"大写字母W", 'W'}, {"大写字母X", 'X'}, {"大写字母Y", 'Y'}, 
		{"大写字母Z", 'Z'}
	};
	//键盘左边对应的小写字母表
	KeyTable ps2_small_letter [] = 
	{
		{"小写字母a", 'a'}, {"小写字母b", 'b'}, {"小写字母c", 'c'}, {"小写字母d", 'd'}, {"小写字母e", 'e'}, 
		{"小写字母f", 'f'}, {"小写字母g", 'g'}, {"小写字母h", 'h'}, {"小写字母i", 'i'}, {"小写字母j", 'j'}, 
		{"小写字母k", 'k'}, {"小写字母l", 'l'}, {"小写字母m", 'm'}, {"小写字母n", 'n'}, {"小写字母o", 'o'}, 
		{"小写字母p", 'p'}, {"小写字母q", 'q'}, {"小写字母r", 'r'}, {"小写字母s", 's'}, {"小写字母t", 't'}, 
		{"小写字母u", 'u'}, {"小写字母v", 'v'}, {"小写字母w", 'w'}, {"小写字母x", 'x'}, {"小写字母y", 'y'}, 
		{"小写字母z", 'z'}
	};
	//键盘左边对应的符号表
	KeyTable ps2_char []  = 
	{
		{"符号)", ')'}, {"符号!", '!'}, {"符号@", '@'}, {"符号#", '#'}, {"符号$", '$'}, 
		{"符号%", '%'}, {"符号^", '^'}, {"符号&", '&'}, {"符号*", '*'}, {"符号(", '('},
		{"符号~", '~'}, {"符号_", '_'}, {"符号+", '+'}, {"符号|", '|'}, {"空格键", ' '}, 
		{"符号{", '{'}, {"符号}", '}'}, {"符号:", ':'}, {"双引号", '"'}, {"符号<", '<'},
		{"符号>", '>'}, {"符号?", '?'}, {"符号.", '.'}, {"符号`", '`'}, {"符号-", '-'},
		{"符号=", '='}, {"符号[", '['}, {"符号]", ']'}, {"符号;", ';'}, {"单斜杠", PS2_KEY_CHAR1}, 
		{"符号,", ','}, {"符号/", '/'}
	};
	//键盘对应的F 按键
	KeyTable ps2_f [] = 
	{
		{"F1", PS2_KEY_F1}, {"F2", PS2_KEY_F2}, {"F3", PS2_KEY_F3}, {"F4", PS2_KEY_F4}, 
		{"F5", PS2_KEY_F5}, {"F6", PS2_KEY_F6}, {"F7", PS2_KEY_F7}, {"F8", PS2_KEY_F8}, 
		{"F9", PS2_KEY_F9}, {"F10", PS2_KEY_F10}, {"F11", PS2_KEY_F11}, {"F12", PS2_KEY_F12} 
	};
	//键盘右边对应的表
	KeyTable ps2_right [] = 
	{
		{"数字0", '0'}, {"数字1", '1'}, {"数字2", '2'}, {"数字3", '3'}, {"数字4", '4'}, 
		{"数字5", '5'}, {"数字6", '6'}, {"数字7", '7'}, {"数字8", '8'}, {"数字9", '9'},
		{"符号+", '+'}, {"符号-", '-'}, {"符号*", '*'}, {"符号/", '/'}, {"Enter", PS2_KEY_EN},
		{"符号.", '.'}, {"Num Lock", PS2_KEY_NUM}	
	};	
	//其它特殊按键
	KeyTable ps2_other [] = 
	{
		{"Backspace", PS2_KEY_BKSP}, {"空格", PS2_KEY_SPACE}, {"TAB", PS2_KEY_TAB},  
		{"Caps Lock", PS2_KEY_CAPS}, {"左Ctrl", PS2_KEY_L_CTRL}, 
		{"左GUI", PS2_KEY_L_GUI}, {"左A/t", PS2_KEY_L_ALT},  
		{"右Ctrl", PS2_KEY_R_CTRL}, {"右GUI", PS2_KEY_R_GUI}, {"右A/t", PS2_KEY_R_ALT}, 
		{"APPS", PS2_KEY_APPS}, {"Enter", PS2_KEY_ENTER}, {"Esc", PS2_KEY_ESC}, 
		{"Print Screen", PS2_KEY_SCRN}, {"Scroll Lock", PS2_KEY_SCROLL}, {"Pause", PS2_KEY_PAUSE}, 
		{"Insert", PS2_KEY_INSERT}, {"Home", PS2_KEY_HOME}, {"Page Up", PS2_KEY_PG_UP},
		{"Delete", PS2_KEY_DELETE}, {"End", PS2_KEY_END}, {"Page Down", PS2_KEY_PG_DN},
		{"方向(向上)", PS2_KEY_U_ARROW}, {"方向(向左)", PS2_KEY_L_ARROW}, {"方向(向下)", PS2_KEY_D_ARROW},
		{"方向(向右)", PS2_KEY_R_ARROW}
	};				
#endif

//#endif

