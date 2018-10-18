1、先了解下接触式IC卡的背景知识以及使用
2、深入学习EMV2000规范，详细资料见：
	./EMV2000规范资料/EMV2000+第一册.pdf	
	./EMV2000规范资料/EMV_v4.3_Book_1_ICC_to_Terminal_Interface_2012060705394541.pdf
	./EMV2000规范资料/iso7816-3_v3.pdf
	
	注：《EMV2000+第一册.pdf》为中文版，且历史悠久，存在一定问题，
		需参考《EMV_v4.3_Book_1_ICC_to_Terminal_Interface_2012060705394541.pdf》和《iso7816-3_v3.pdf》
		
	学习EMV2000规范需熟悉以下内容：
	a. 上电时序（冷复位、热复位）
	b. ATR解析规则
	c. T0和T1卡的通信规则
	d. 下电时序
		
3、查阅TDA8035芯片资料，详细资料见：
	./tda8035芯片资料/AN10997---tda8035.pdf
	./tda8035芯片资料/TDA8035.pdf

   完成以下目标：
	a. 明确芯片功能
	b. 芯片使用注意事项（上电时序、下电时序、休眠设置、卡片插入检测有效电平、测试模式等）
	
4、完成以上学习的基础下，查阅代码，梳理流程；主要包含以下部分：
	a. 上电时序
	b. ATR解析流程
	c. T0卡协议部分
	d. T1卡协议部分
	e. 下电时序
	
5、认证相关资料了解：
	./EMV Contact Level1认证/V2.1/T1_TC_V21.pdf
	./EMV Contact Level1认证/V4.3a/IFM_L1_Loopback_v43a_20151015.pdf
	./EMV Contact Level1认证/V4.3a/IFM_L1_Protocol_Test_Cases_v43a_20151103.pdf
	./EMV Contact Level1认证/V4.3a/IFM_L1_Mechanical_and_Electrical_Test_Cases_v43a_20151015.pdf
	
	a. loopback流程
	b. 公司内部协议测试设备操作方法
	c. 了解各测试case的测试点