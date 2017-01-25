// LoadJVMDemo.cpp : 定义控制台应用程序的入口点。  

#include "stdafx.h"  
#include "jni.h"  

using namespace std;

//加载JVM启动java程序  
bool startJVM();
typedef jint(JNICALL *JNICREATEPROC)(JavaVM **, void **, void *);


//int _tmain(int argc, _TCHAR* argv[]) {
//	outLog("main start");
//	cout << "main start" << endl;
//	startJVM();
//	return 0;
//}


//启动java虚拟机  

bool startJVM() {
	//获取jvm动态库的路径  
	//const char jvmPath[] = "E:\eclipse-jee-kepler-SR2-win32\Java\jre\bin\client\jvm.dll";  
	TCHAR* jvmPath = _T("E://eclipse-jee-kepler-SR2-win32//Java//jre//bin//client//jvm.dll");

	//java虚拟机启动时接收的参数，每个参数单独一项  
	const int nOptionCount = 3;
	JavaVMOption vmOption[nOptionCount];
	//设置jvm将不接收控制台的控制handler  
	vmOption[0].optionString = "-Xrs";
	//设置JVM最大允许分配的堆内存，按需分配  
	vmOption[1].optionString = "-Xmx256M";
	//设置classpath  
	vmOption[2].optionString = "-Djava.class.path=./HelloWorld.jar";

	JavaVMInitArgs vmInitArgs;
	vmInitArgs.version = JNI_VERSION_1_6;
	vmInitArgs.options = vmOption;
	vmInitArgs.nOptions = nOptionCount;
	//忽略无法识别jvm的情况  
	vmInitArgs.ignoreUnrecognized = JNI_TRUE;

	//设置启动类，注意分隔符为"/"  
	const char startClass[] = "test/HelloWorld";
	//启动方法，一般是main函数，当然可以设置成其他函数  
	const char startMethod[] = "main";

	//日志输出文件  
	const char *logoutFile = "LoadJVMDemoJ.log";

	//传入参数，如果有  
	//int nParamcount = 2;  
	//const char * params[2] = {"aa","bb"};  

	//加载JVM,注意需要传入的字符串为LPCWSTR,指向一个常量Unicode字符串的32位指针，相当于const wchar_t*  
	outLog("开始加载VM");
	HINSTANCE jvmDLL = LoadLibrary(jvmPath);
	if (jvmDLL == NULL) {
		outLog("加载JVM动态库错误", GetLastError());
		return false;
	}

	//初始化jvm物理地址  
	JNICREATEPROC jvmProcAddress = (JNICREATEPROC)GetProcAddress(jvmDLL, "JNI_CreateJavaVM");
	if (jvmDLL == NULL) {
		FreeLibrary(jvmDLL);
		outLog("初始化JVM物理地址失败", GetLastError());
		return false;
	}

	//创建JVM  
	JNIEnv *env;
	JavaVM *jvm;
	jint jvmProc = (jvmProcAddress)(&jvm, (void **)&env, &vmInitArgs);
	if (jvmProc < 0 || jvm == NULL || env == NULL) {
		FreeLibrary(jvmDLL);
		outLog("创建JVM错误", GetLastError());
		return false;
	}

	//加载启动类  
	jclass mainclass = env->FindClass(startClass);
	if (env->ExceptionCheck() == JNI_TRUE || mainclass == NULL) {
		env->ExceptionDescribe();
		env->ExceptionClear();
		FreeLibrary(jvmDLL);
		outLog("加载启动类失败", GetLastError());
		return false;
	}

	//加载启动方法  
	jmethodID methedID = env->GetStaticMethodID(mainclass, startMethod, "([Ljava/lang/String;)V");
	if (env->ExceptionCheck() == JNI_TRUE || methedID == NULL) {
		env->ExceptionDescribe();
		env->ExceptionClear();
		FreeLibrary(jvmDLL);
		outLog("加载启动方法失败", GetLastError());
		return false;
	}

	outLog("开始执行");
	env->CallStaticVoidMethod(mainclass, methedID, NULL);
	outLog("执行结束");

	//jvm释放  
	jvm->DestroyJavaVM();
	return true;
}


/**
windows平台的加载方法
windows平台下jdk的java虚拟机动态库为jvm.dll，位于：
%JAVA_HOME%/jre/bin/client/jvm.dll
%JAVA_HOME%/jre/bin/server/jvm.dll
Jvm动态库有client和server两个版本，分别针对桌面应用和服务器应用做了相应的优化，client版本加载速度较快，server版本加载速度较慢但运行起来较快。
虚拟机加载可按照以下步骤进行：
1）   装载jvm动态库
2）   查找JNI_CreateJavaVM接口
3）   设置JVM启动参数，调用JNI_CreateJavaVM接口启动虚拟机
4）   查找启动类，设置命令行参数，设置窗口输出重导向文件
5）   调用启动类的启动方法启动java程序
6）   要停止Java程序运行时，调用java类的停止方法即可


http://www.cnblogs.com/zhangs1986/p/3502026.html
http://www.cnblogs.com/freall/articles/3416209.html
**/