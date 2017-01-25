#include "stdafx.h"  
using namespace std;

/**
输出服务启动日志
**/
void outLog(char* dataBuffer) {
	//分配一块内存空间  
	char* buff = new char[1024];
	//复制内容  
	strcpy_s(buff, 1024, dataBuffer);

	//回车换行符  
	const char* enterLine = "\r\n";
	//计算总长度,最后的1为字符串结束符
	int len = (int)strlen(buff) + (int)strlen(enterLine) + 1;
	//合并字符串,len为合并字符串后的总长度，会报错  
	strcat_s(buff, len, enterLine);


	HANDLE hAppend;
	DWORD dwBytesRead = (DWORD)strlen(buff);
	DWORD dwBytesWritten = 0;

	//打开一个存在的文件，如果不存在则创建它  
	hAppend = CreateFile(TEXT("LoadJVMDemoC.log"),
		FILE_APPEND_DATA,
		FILE_SHARE_READ,
		NULL,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	//判断是否成功读取  
	if (hAppend == INVALID_HANDLE_VALUE) {
		cout << "could not open LoadJVMDemoC.log" << endl;
		return;
	}
	//写入日志  
	WriteFile(hAppend, buff, dwBytesRead, &dwBytesWritten, NULL);
	//关闭流  
	CloseHandle(hAppend);
}

/**
输出包含错误码的日志
*/
void outLog(char* dataBuffer, DWORD lastError) {
	//分配一块内存空间  
	char* buff = new char[1024];
	//复制内容  
	strcpy_s(buff, 1024, dataBuffer);

	//判断是否有错误编号  
	if (lastError != NULL) {
		//添加提示字段  
		strcat_s(buff, 1024, "---------error char:");
		//获取错误结果字符  
		long lastErrorLong = lastError;
		char lastErrorChar[10];
		//总长度  
		_ltoa_s(lastErrorLong, lastErrorChar, 10);
		int errorLen = (int)strlen(buff) + (int)strlen(lastErrorChar) + 1;
		strcat_s(buff, errorLen, lastErrorChar);
	}
	outLog(buff);
}