#include "stdafx.h"

#pragma comment(lib, "Advapi32.lib")
#pragma comment(lib, "Kernel32.lib")
//#pragma comment(lib, "KernelBase.lib")


//#define SVCNAME TEXT("ServiceDemo")

using namespace std;

//�����״̬,�ͷ���ֹͣ�¼�
SERVICE_STATUS			serviceStatus;
SERVICE_STATUS_HANDLE	serviceStatusHandle;
HANDLE					serviceStopEvent;

//����Ŀ���
void WINAPI ServiceMain(DWORD, LPTSTR*);
void WINAPI ServiceInit(DWORD, LPTSTR*);
void WINAPI ServiceContrl(DWORD optionCode);

//��¼�����״̬
void ReportServiceStatus(DWORD, DWORD, DWORD);

//����İ�װ��ж��,�����״̬
BOOL Install();
BOOL IsInstall();
BOOL Uninstall();

//���������
TCHAR serviceName[] = _T("ServiceDemo");
BOOL installStat;
DWORD dwThreadID;


int _tmain(int argc, _TCHAR* argv[]) {
	//outLog("main start");
	cout << "main start" << endl;
	TCHAR* actionArg = argv[1];

	//��������˲���"install",��װ������������ܹ���SCM����
	if (lstrcmpi(actionArg, _T("install")) == 0) {
		Install();
	}
	//����Ĳ���Ϊж��ʱ
	else if (lstrcmpi(actionArg, _T("uninstall")) == 0) {
		Uninstall();
	}
	//�����������Ϣ
	SERVICE_TABLE_ENTRY dispatchTable[] = {
		{ serviceName, (LPSERVICE_MAIN_FUNCTION)ServiceMain },
		{ NULL, NULL }
	};

	HINSTANCE hDLL;
	HINSTANCE hDLLD;
	hDLLD = LoadLibrary(_T("C:\\Windows\\System32\\KernelBase.dll"));//���ض�̬���ӿ�KernelBase.dll�ļ���

	FreeLibrary(hDLLD);
	hDLL = LoadLibrary(_T("F:\\workspace\\C\\LoadJVMDemo\\x64\\Debug\\KernelBase.dll"));//���ض�̬���ӿ�KernelBase.dll�ļ���

	//����������Ʒ������������ж�����ʧ��ʱ
	if (!StartServiceCtrlDispatcher(dispatchTable)) {
		cout << "StartServiceCtrlDispatcher error" << endl;
		Uninstall();
		return -1;
	}

	return 0;
}

/**
��װ����
**/
BOOL Install() {
	//������
	SC_HANDLE schSCManager;
	SC_HANDLE schService;
	TCHAR servicePath[MAX_PATH];

	if (!GetModuleFileName(NULL, servicePath, MAX_PATH)) {
		cout << "Cannot install service" << endl;
		return false;
	}

	//�򿪷�����ƹ�����
	schSCManager = OpenSCManager(
		NULL,							//���ص���
		NULL,							//���񼤻��λ��
		SC_MANAGER_ALL_ACCESS);			//ȫ����������

	if (NULL == schSCManager) {
		cout << "OpenSCManager failed" << endl;
		return false;
	}

	//��������
	schService = CreateService(
		schSCManager,					//����������������
		serviceName,					//����
		serviceName,					//��ʾ������
		SERVICE_ALL_ACCESS,				//��������
		SERVICE_WIN32_OWN_PROCESS,		//��������
		SERVICE_DEMAND_START,			//�������� SERVICE_BOOT_START, SERVICE_SYSTEM_START,SERVICE_AUTO_START,SERVICE_DEMAND_START,SERVICE_DISABLED
		SERVICE_ERROR_NORMAL,			//��������
		servicePath,					//����·��
		NULL,							//������������
		NULL,							//�����Ȩ��
		NULL,							//������
		NULL,							//�����˺�
		NULL);							//������

	if (NULL == schService) {
		cout << "CreateService failed" << endl;
		CloseServiceHandle(schSCManager);
		return false;
	}
	else {
		cout << "Servcie install successfully" << endl;
	}

	//�رջ�õĿ�����
	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);
	return true;
}

/**
ж�ط���
**/
BOOL Uninstall() {
	SC_HANDLE schSCManager;
	SC_HANDLE schService;

	//�򿪷�����ƹ�����
	schSCManager = OpenSCManager(
		NULL,							//���ص���
		NULL,							//���񼤻��λ��
		SC_MANAGER_ALL_ACCESS);			//ȫ����������

	if (NULL == schSCManager) {
		outLog("Uninstall OpenSCManager failed");
		return false;
	}

	//�򿪷���
	schService = OpenService(
		schSCManager,					//����������������
		serviceName,					//����
		SERVICE_STOP | DELETE);

	if (NULL == schService) {
		CloseServiceHandle(schSCManager);
		outLog("Uninstall OpenService failed");
		return false;
	}

	//ֹͣ����
	ControlService(schService, SERVICE_CONTROL_STOP, &serviceStatus);

	//ɾ������
	BOOL deleteServcie = DeleteService(schService);

	//�رտ�����
	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);

	if (deleteServcie) {
		return true;
	}

	outLog("Service could not be deleted");
	return false;
}

/**
�������������������Ҫ�߼�
**/
void WINAPI ServiceMain(DWORD dwArgc, LPTSTR *lpszArgv) {
	//������ע�������
	serviceStatusHandle = RegisterServiceCtrlHandler(
		serviceName,
		ServiceContrl);

	//�ж��Ƿ�ע��ɹ�
	if (!serviceStatusHandle) {
		//outLog("RegisterServiceCtrlHandler failed");
		return;
	}

	//���Ʒ����״̬�� SERVICE_STATUS
	serviceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	serviceStatus.dwServiceSpecificExitCode = 0;

	//��¼����״̬�������������
	ReportServiceStatus(SERVICE_START_PENDING, NO_ERROR, 3000);

	//��ʼ��
	ServiceInit(dwArgc, lpszArgv);
}

/**
��ʼ������
dwArgc - ��������鳤��
lpszArgv - ���������
**/
void WINAPI ServiceInit(DWORD dwArgc, LPTSTR *lpszArgv) {
	//����ͳ�ʼ������Ĳ�������֤SERVICE_START_PENDING�ܹ�����
	//ReportSvcStatus().�����ʼ��ʧ����ı����״̬ΪSERVICE_STOPPED.

	//����һ���¼��������յ�ֹͣ����ʱ��������
	serviceStopEvent = CreateEvent(
		NULL,										//Ĭ�ϵİ�ȫ����
		TRUE,										//�ֹ�����Event
		FALSE,										//��ʼΪ���ź�
		NULL);										//û������

	if (NULL == serviceStopEvent) {
		ReportServiceStatus(SERVICE_STOP, NO_ERROR, 0);
		return;
	}

	//��ʼ������¼����״̬
	ReportServiceStatus(SERVICE_RUNNING, NO_ERROR, 0);

	//������ֹͣʱ
	while (1) {
		//�������Ƿ�ֹͣ(��ֹͣ�¼��Ƿ񱻴���)
		WaitForSingleObject(serviceStopEvent, INFINITE);
		ReportServiceStatus(SERVICE_STOPPED, NO_ERROR, 0);
		return;
	}
}
/**
���õ�ǰ����״̬���Ҽ�¼�������������
**/
void ReportServiceStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint) {
	static DWORD dwCheckPoint = 1;

	//��ʼ��SERVICE_STATUS�ṹ��
	serviceStatus.dwCurrentState = dwCurrentState;			//��ǰ״̬	
	serviceStatus.dwWin32ExitCode = dwWin32ExitCode;		//������
	serviceStatus.dwWaitHint = dwWaitHint;					//�ȴ�ʱ��ms

															//ָ���˿��������Ƿ�֪ͨ����0Ϊ��֪ͨ
	if (dwCurrentState == SERVICE_START_PENDING) {
		serviceStatus.dwControlsAccepted = 0;
	}
	else {
		serviceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
	}

	//���õȴ�ʱ�䣬dwWaitHintΪ��ʱʱ�䣬dwCheckPoint�������״̬�ĸı����
	if ((dwCurrentState == SERVICE_RUNNING) ||
		(dwCurrentState == SERVICE_STOPPED)) {
		serviceStatus.dwCheckPoint = 0;
	}
	else {
		serviceStatus.dwCheckPoint = dwCheckPoint++;
	}

	//��¼�����״̬��������Դ������
	SetServiceStatus(serviceStatusHandle, &serviceStatus);
}

/**
���ʷ���������ı�����״̬ʱ����ʴ˷���
**/
void WINAPI ServiceContrl(DWORD dwCtrl) {
	//����Ŀ��ƴ���
	switch (dwCtrl) {
	case SERVICE_CONTROL_STOP://ֹͣ����
		ReportServiceStatus(SERVICE_STOP_PENDING, NO_ERROR, 0);
		//���ͷ���ֹͣ�ź�
		SetEvent(serviceStopEvent);
		ReportServiceStatus(serviceStatus.dwCurrentState, NO_ERROR, 0);
		break;
	case SERVICE_CONTROL_INTERROGATE://ѯ��
		break;
	case SERVICE_CONTROL_PAUSE:
		break;
	case SERVICE_CONTROL_CONTINUE:
		break;
	case SERVICE_CONTROL_SHUTDOWN:
		break;
	default:
		outLog("error control code", dwCtrl);
		break;
	}
}