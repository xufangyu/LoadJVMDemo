#include "stdafx.h"
#include "jni.h"
#include <iostream>

using namespace std;

bool setStream(JNIEnv *env, const char* fileName, const char* method) {
	//创建接收字符数组
	char* buffer = new char[1024];

	//输出文件名转化成字符串
	jstring fileNameStr = env->NewStringUTF(fileName);
	if (env->ExceptionCheck() == JNI_TRUE || fileNameStr == NULL) {
		env->ExceptionDescribe();
		env->ExceptionClear();
		cout << "转化路径失败" << endl;
		return false;
	}

	//搜索FileOutputStream类
	jclass fileOutputStreamClass = env->FindClass("java/io/FileOutputStream");
	if (env->ExceptionCheck() == JNI_TRUE || fileOutputStreamClass == NULL) {
		env->ExceptionDescribe();
		env->ExceptionClear();
		cout << "找不到FileOutputSteam类" << endl;
		return false;
	}

	//获得构造方法的id
	jmethodID fileOutputStreamConstructor = env->GetMethodID(fileOutputStreamClass, "<init>", "(Ljava/lang/String;)V");
	if (env->ExceptionCheck() == JNI_TRUE || fileOutputStreamConstructor == NULL) {
		env->ExceptionDescribe();
		env->ExceptionClear();
		cout << "找不到FileOutputSteam的构造方法" << endl;
		return false;
	}

	//创建FileOutputStream对象
	jobject fileOutputSteam = env->NewObject(fileOutputStreamClass, fileOutputStreamConstructor, fileNameStr);
	if (env->ExceptionCheck() == JNI_TRUE || fileOutputSteam == NULL) {
		env->ExceptionDescribe();
		env->ExceptionClear();
		cout << "创建FileOutputStream对象失败" << endl;
		return false;
	}

	//搜索PrintStream类
	jclass printStreamClass = env->FindClass("java/io/PrintStream");
	if (env->ExceptionCheck() == JNI_TRUE || printStreamClass == NULL) {
		env->ExceptionDescribe();
		env->ExceptionClear();
		cout << "找不到FileOutputSteam类" << endl;
		return false;
	}

	//获得构造方法的id
	jmethodID printStreamConstructor = env->GetMethodID(printStreamClass, "<init>", "(Ljava/io/OutputStream;)V");
	if (env->ExceptionCheck() == JNI_TRUE || printStreamConstructor == NULL) {
		env->ExceptionDescribe();
		env->ExceptionClear();
		cout << "找不到PrintStreamConstructor的构造方法" << endl;
		return false;
	}

	//创建PrintStream对象
	jobject printStream = env->NewObject(printStreamClass, printStreamConstructor, fileOutputSteam);
	if (env->ExceptionCheck() == JNI_TRUE || printStream == NULL) {
		env->ExceptionDescribe();
		env->ExceptionClear();
		cout << "创建PrintStream对象失败" << endl;
		return false;
	}

	//搜索System类
	jclass systemClass = env->FindClass("java/lang/System");
	if (env->ExceptionCheck() == JNI_TRUE || systemClass == NULL) {
		env->ExceptionDescribe();
		env->ExceptionClear();
		cout << "找不到System类" << endl;
		return false;
	}

	//获得System类的指定方法
	jmethodID setSystemMethod = env->GetStaticMethodID(systemClass, method, "(Ljava/io/PrintStream;)V");
	if (env->ExceptionCheck() == JNI_TRUE || setSystemMethod == NULL) {
		env->ExceptionDescribe();
		env->ExceptionClear();
		cout << "获得System类的指定方法失败" << endl;
		return false;
	}

	//调用System输出方法，输出日志
	env->CallStaticVoidMethod(systemClass, setSystemMethod, printStream);
	if (env->ExceptionCheck() == JNI_TRUE) {
		env->ExceptionDescribe();
		env->ExceptionClear();
		cout << "调用System输出方法，输出日志失败" << endl;
		return false;
	}
	return true;
}
