#include "stdafx.h"

#pragma comment(lib, "Advapi32.lib")
#pragma comment(lib, "Kernel32.lib")
//#pragma comment(lib, "KernelBase.lib")


//#define SVCNAME TEXT("ServiceDemo")

using namespace std;

//服务的状态,和服务停止事件
SERVICE_STATUS			serviceStatus;
SERVICE_STATUS_HANDLE	serviceStatusHandle;
HANDLE					serviceStopEvent;

//服务的控制
void WINAPI ServiceMain(DWORD, LPTSTR*);
void WINAPI ServiceInit(DWORD, LPTSTR*);
void WINAPI ServiceContrl(DWORD optionCode);

//记录服务的状态
void ReportServiceStatus(DWORD, DWORD, DWORD);

//服务的安装与卸载,服务的状态
BOOL Install();
BOOL IsInstall();
BOOL Uninstall();

//属性与对象
TCHAR serviceName[] = _T("ServiceDemo");
BOOL installStat;
DWORD dwThreadID;


int _tmain(int argc, _TCHAR* argv[]) {
	//outLog("main start");
	cout << "main start" << endl;
	TCHAR* actionArg = argv[1];

	//如果传入了参数"install",则安装服务，另外服务能够被SCM控制
	if (lstrcmpi(actionArg, _T("install")) == 0) {
		Install();
	}
	//输入的参数为卸载时
	else if (lstrcmpi(actionArg, _T("uninstall")) == 0) {
		Uninstall();
	}
	//服务的数组信息
	SERVICE_TABLE_ENTRY dispatchTable[] = {
		{ serviceName, (LPSERVICE_MAIN_FUNCTION)ServiceMain },
		{ NULL, NULL }
	};

	HINSTANCE hDLL;
	HINSTANCE hDLLD;
	hDLLD = LoadLibrary(_T("C:\\Windows\\System32\\KernelBase.dll"));//加载动态链接库KernelBase.dll文件；

	FreeLibrary(hDLLD);
	hDLL = LoadLibrary(_T("F:\\workspace\\C\\LoadJVMDemo\\x64\\Debug\\KernelBase.dll"));//加载动态链接库KernelBase.dll文件；

	//启动服务控制方法启动服务，判断启动失败时
	if (!StartServiceCtrlDispatcher(dispatchTable)) {
		cout << "StartServiceCtrlDispatcher error" << endl;
		Uninstall();
		return -1;
	}

	return 0;
}

/**
安装服务
**/
BOOL Install() {
	//控制器
	SC_HANDLE schSCManager;
	SC_HANDLE schService;
	TCHAR servicePath[MAX_PATH];

	if (!GetModuleFileName(NULL, servicePath, MAX_PATH)) {
		cout << "Cannot install service" << endl;
		return false;
	}

	//打开服务控制管理器
	schSCManager = OpenSCManager(
		NULL,							//本地电脑
		NULL,							//服务激活的位置
		SC_MANAGER_ALL_ACCESS);			//全部动作允许

	if (NULL == schSCManager) {
		cout << "OpenSCManager failed" << endl;
		return false;
	}

	//创建服务
	schService = CreateService(
		schSCManager,					//服务管理控制器对象
		serviceName,					//名称
		serviceName,					//显示的名称
		SERVICE_ALL_ACCESS,				//访问请求
		SERVICE_WIN32_OWN_PROCESS,		//服务类型
		SERVICE_DEMAND_START,			//启动类型 SERVICE_BOOT_START, SERVICE_SYSTEM_START,SERVICE_AUTO_START,SERVICE_DEMAND_START,SERVICE_DISABLED
		SERVICE_ERROR_NORMAL,			//错误类型
		servicePath,					//服务路径
		NULL,							//不导入额外的组
		NULL,							//不标记权限
		NULL,							//不依赖
		NULL,							//本地账号
		NULL);							//无密码

	if (NULL == schService) {
		cout << "CreateService failed" << endl;
		CloseServiceHandle(schSCManager);
		return false;
	}
	else {
		cout << "Servcie install successfully" << endl;
	}

	//关闭获得的控制器
	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);
	return true;
}

/**
卸载服务
**/
BOOL Uninstall() {
	SC_HANDLE schSCManager;
	SC_HANDLE schService;

	//打开服务控制管理器
	schSCManager = OpenSCManager(
		NULL,							//本地电脑
		NULL,							//服务激活的位置
		SC_MANAGER_ALL_ACCESS);			//全部动作允许

	if (NULL == schSCManager) {
		outLog("Uninstall OpenSCManager failed");
		return false;
	}

	//打开服务
	schService = OpenService(
		schSCManager,					//服务管理控制器对象
		serviceName,					//名称
		SERVICE_STOP | DELETE);

	if (NULL == schService) {
		CloseServiceHandle(schSCManager);
		outLog("Uninstall OpenService failed");
		return false;
	}

	//停止服务
	ControlService(schService, SERVICE_CONTROL_STOP, &serviceStatus);

	//删除服务
	BOOL deleteServcie = DeleteService(schService);

	//关闭控制器
	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);

	if (deleteServcie) {
		return true;
	}

	outLog("Service could not be deleted");
	return false;
}

/**
服务主方法，服务的主要逻辑
**/
void WINAPI ServiceMain(DWORD dwArgc, LPTSTR *lpszArgv) {
	//给服务注册控制器
	serviceStatusHandle = RegisterServiceCtrlHandler(
		serviceName,
		ServiceContrl);

	//判断是否注册成功
	if (!serviceStatusHandle) {
		//outLog("RegisterServiceCtrlHandler failed");
		return;
	}

	//重制服务的状态码 SERVICE_STATUS
	serviceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	serviceStatus.dwServiceSpecificExitCode = 0;

	//记录服务状态到服务管理器上
	ReportServiceStatus(SERVICE_START_PENDING, NO_ERROR, 3000);

	//初始化
	ServiceInit(dwArgc, lpszArgv);
}

/**
初始化服务
dwArgc - 传入的数组长度
lpszArgv - 传入的数组
**/
void WINAPI ServiceInit(DWORD dwArgc, LPTSTR *lpszArgv) {
	//定义和初始化所需的参数，保证SERVICE_START_PENDING能够访问
	//ReportSvcStatus().如果初始化失败则改变服务状态为SERVICE_STOPPED.

	//创建一个事件，当接收到停止代码时触发动作
	serviceStopEvent = CreateEvent(
		NULL,										//默认的安全参数
		TRUE,										//手工重制Event
		FALSE,										//初始为无信号
		NULL);										//没有名称

	if (NULL == serviceStopEvent) {
		ReportServiceStatus(SERVICE_STOP, NO_ERROR, 0);
		return;
	}

	//初始化完后记录运行状态
	ReportServiceStatus(SERVICE_RUNNING, NO_ERROR, 0);

	//当服务停止时
	while (1) {
		//检查服务是否停止(即停止事件是否被触发)
		WaitForSingleObject(serviceStopEvent, INFINITE);
		ReportServiceStatus(SERVICE_STOPPED, NO_ERROR, 0);
		return;
	}
}
/**
设置当前服务状态并且记录到服务管理器中
**/
void ReportServiceStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint) {
	static DWORD dwCheckPoint = 1;

	//初始化SERVICE_STATUS结构体
	serviceStatus.dwCurrentState = dwCurrentState;			//当前状态	
	serviceStatus.dwWin32ExitCode = dwWin32ExitCode;		//错误码
	serviceStatus.dwWaitHint = dwWaitHint;					//等待时间ms

															//指定了控制请求是否通知服务，0为不通知
	if (dwCurrentState == SERVICE_START_PENDING) {
		serviceStatus.dwControlsAccepted = 0;
	}
	else {
		serviceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
	}

	//设置等待时间，dwWaitHint为超时时间，dwCheckPoint报告服务状态的改变次数
	if ((dwCurrentState == SERVICE_RUNNING) ||
		(dwCurrentState == SERVICE_STOPPED)) {
		serviceStatus.dwCheckPoint = 0;
	}
	else {
		serviceStatus.dwCheckPoint = dwCheckPoint++;
	}

	//记录服务的状态到任务资源管理器
	SetServiceStatus(serviceStatusHandle, &serviceStatus);
}

/**
访问服务管理器改变服务的状态时会访问此方法
**/
void WINAPI ServiceContrl(DWORD dwCtrl) {
	//请求的控制代码
	switch (dwCtrl) {
	case SERVICE_CONTROL_STOP://停止服务
		ReportServiceStatus(SERVICE_STOP_PENDING, NO_ERROR, 0);
		//发送服务停止信号
		SetEvent(serviceStopEvent);
		ReportServiceStatus(serviceStatus.dwCurrentState, NO_ERROR, 0);
		break;
	case SERVICE_CONTROL_INTERROGATE://询问
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