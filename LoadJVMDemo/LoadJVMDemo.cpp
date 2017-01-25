// LoadJVMDemo.cpp : �������̨Ӧ�ó������ڵ㡣  

#include "stdafx.h"  
#include "jni.h"  

using namespace std;

//����JVM����java����  
bool startJVM();
typedef jint(JNICALL *JNICREATEPROC)(JavaVM **, void **, void *);


//int _tmain(int argc, _TCHAR* argv[]) {
//	outLog("main start");
//	cout << "main start" << endl;
//	startJVM();
//	return 0;
//}


//����java�����  

bool startJVM() {
	//��ȡjvm��̬���·��  
	//const char jvmPath[] = "E:\eclipse-jee-kepler-SR2-win32\Java\jre\bin\client\jvm.dll";  
	TCHAR* jvmPath = _T("E://eclipse-jee-kepler-SR2-win32//Java//jre//bin//client//jvm.dll");

	//java���������ʱ���յĲ�����ÿ����������һ��  
	const int nOptionCount = 3;
	JavaVMOption vmOption[nOptionCount];
	//����jvm�������տ���̨�Ŀ���handler  
	vmOption[0].optionString = "-Xrs";
	//����JVM����������Ķ��ڴ棬�������  
	vmOption[1].optionString = "-Xmx256M";
	//����classpath  
	vmOption[2].optionString = "-Djava.class.path=./HelloWorld.jar";

	JavaVMInitArgs vmInitArgs;
	vmInitArgs.version = JNI_VERSION_1_6;
	vmInitArgs.options = vmOption;
	vmInitArgs.nOptions = nOptionCount;
	//�����޷�ʶ��jvm�����  
	vmInitArgs.ignoreUnrecognized = JNI_TRUE;

	//���������࣬ע��ָ���Ϊ"/"  
	const char startClass[] = "test/HelloWorld";
	//����������һ����main��������Ȼ�������ó���������  
	const char startMethod[] = "main";

	//��־����ļ�  
	const char *logoutFile = "LoadJVMDemoJ.log";

	//��������������  
	//int nParamcount = 2;  
	//const char * params[2] = {"aa","bb"};  

	//����JVM,ע����Ҫ������ַ���ΪLPCWSTR,ָ��һ������Unicode�ַ�����32λָ�룬�൱��const wchar_t*  
	outLog("��ʼ����VM");
	HINSTANCE jvmDLL = LoadLibrary(jvmPath);
	if (jvmDLL == NULL) {
		outLog("����JVM��̬�����", GetLastError());
		return false;
	}

	//��ʼ��jvm�����ַ  
	JNICREATEPROC jvmProcAddress = (JNICREATEPROC)GetProcAddress(jvmDLL, "JNI_CreateJavaVM");
	if (jvmDLL == NULL) {
		FreeLibrary(jvmDLL);
		outLog("��ʼ��JVM�����ַʧ��", GetLastError());
		return false;
	}

	//����JVM  
	JNIEnv *env;
	JavaVM *jvm;
	jint jvmProc = (jvmProcAddress)(&jvm, (void **)&env, &vmInitArgs);
	if (jvmProc < 0 || jvm == NULL || env == NULL) {
		FreeLibrary(jvmDLL);
		outLog("����JVM����", GetLastError());
		return false;
	}

	//����������  
	jclass mainclass = env->FindClass(startClass);
	if (env->ExceptionCheck() == JNI_TRUE || mainclass == NULL) {
		env->ExceptionDescribe();
		env->ExceptionClear();
		FreeLibrary(jvmDLL);
		outLog("����������ʧ��", GetLastError());
		return false;
	}

	//������������  
	jmethodID methedID = env->GetStaticMethodID(mainclass, startMethod, "([Ljava/lang/String;)V");
	if (env->ExceptionCheck() == JNI_TRUE || methedID == NULL) {
		env->ExceptionDescribe();
		env->ExceptionClear();
		FreeLibrary(jvmDLL);
		outLog("������������ʧ��", GetLastError());
		return false;
	}

	outLog("��ʼִ��");
	env->CallStaticVoidMethod(mainclass, methedID, NULL);
	outLog("ִ�н���");

	//jvm�ͷ�  
	jvm->DestroyJavaVM();
	return true;
}


/**
windowsƽ̨�ļ��ط���
windowsƽ̨��jdk��java�������̬��Ϊjvm.dll��λ�ڣ�
%JAVA_HOME%/jre/bin/client/jvm.dll
%JAVA_HOME%/jre/bin/server/jvm.dll
Jvm��̬����client��server�����汾���ֱ��������Ӧ�úͷ�����Ӧ��������Ӧ���Ż���client�汾�����ٶȽϿ죬server�汾�����ٶȽ��������������Ͽ졣
��������ؿɰ������²�����У�
1��   װ��jvm��̬��
2��   ����JNI_CreateJavaVM�ӿ�
3��   ����JVM��������������JNI_CreateJavaVM�ӿ����������
4��   ���������࣬���������в��������ô�������ص����ļ�
5��   �����������������������java����
6��   ҪֹͣJava��������ʱ������java���ֹͣ��������


http://www.cnblogs.com/zhangs1986/p/3502026.html
http://www.cnblogs.com/freall/articles/3416209.html
**/