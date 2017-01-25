#include "stdafx.h"  
using namespace std;

/**
�������������־
**/
void outLog(char* dataBuffer) {
	//����һ���ڴ�ռ�  
	char* buff = new char[1024];
	//��������  
	strcpy_s(buff, 1024, dataBuffer);

	//�س����з�  
	const char* enterLine = "\r\n";
	//�����ܳ���,����1Ϊ�ַ���������
	int len = (int)strlen(buff) + (int)strlen(enterLine) + 1;
	//�ϲ��ַ���,lenΪ�ϲ��ַ�������ܳ��ȣ��ᱨ��  
	strcat_s(buff, len, enterLine);


	HANDLE hAppend;
	DWORD dwBytesRead = (DWORD)strlen(buff);
	DWORD dwBytesWritten = 0;

	//��һ�����ڵ��ļ�������������򴴽���  
	hAppend = CreateFile(TEXT("LoadJVMDemoC.log"),
		FILE_APPEND_DATA,
		FILE_SHARE_READ,
		NULL,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	//�ж��Ƿ�ɹ���ȡ  
	if (hAppend == INVALID_HANDLE_VALUE) {
		cout << "could not open LoadJVMDemoC.log" << endl;
		return;
	}
	//д����־  
	WriteFile(hAppend, buff, dwBytesRead, &dwBytesWritten, NULL);
	//�ر���  
	CloseHandle(hAppend);
}

/**
����������������־
*/
void outLog(char* dataBuffer, DWORD lastError) {
	//����һ���ڴ�ռ�  
	char* buff = new char[1024];
	//��������  
	strcpy_s(buff, 1024, dataBuffer);

	//�ж��Ƿ��д�����  
	if (lastError != NULL) {
		//�����ʾ�ֶ�  
		strcat_s(buff, 1024, "---------error char:");
		//��ȡ�������ַ�  
		long lastErrorLong = lastError;
		char lastErrorChar[10];
		//�ܳ���  
		_ltoa_s(lastErrorLong, lastErrorChar, 10);
		int errorLen = (int)strlen(buff) + (int)strlen(lastErrorChar) + 1;
		strcat_s(buff, errorLen, lastErrorChar);
	}
	outLog(buff);
}