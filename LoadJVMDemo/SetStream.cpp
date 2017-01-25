#include "stdafx.h"
#include "jni.h"
#include <iostream>

using namespace std;

bool setStream(JNIEnv *env, const char* fileName, const char* method) {
	//���������ַ�����
	char* buffer = new char[1024];

	//����ļ���ת�����ַ���
	jstring fileNameStr = env->NewStringUTF(fileName);
	if (env->ExceptionCheck() == JNI_TRUE || fileNameStr == NULL) {
		env->ExceptionDescribe();
		env->ExceptionClear();
		cout << "ת��·��ʧ��" << endl;
		return false;
	}

	//����FileOutputStream��
	jclass fileOutputStreamClass = env->FindClass("java/io/FileOutputStream");
	if (env->ExceptionCheck() == JNI_TRUE || fileOutputStreamClass == NULL) {
		env->ExceptionDescribe();
		env->ExceptionClear();
		cout << "�Ҳ���FileOutputSteam��" << endl;
		return false;
	}

	//��ù��췽����id
	jmethodID fileOutputStreamConstructor = env->GetMethodID(fileOutputStreamClass, "<init>", "(Ljava/lang/String;)V");
	if (env->ExceptionCheck() == JNI_TRUE || fileOutputStreamConstructor == NULL) {
		env->ExceptionDescribe();
		env->ExceptionClear();
		cout << "�Ҳ���FileOutputSteam�Ĺ��췽��" << endl;
		return false;
	}

	//����FileOutputStream����
	jobject fileOutputSteam = env->NewObject(fileOutputStreamClass, fileOutputStreamConstructor, fileNameStr);
	if (env->ExceptionCheck() == JNI_TRUE || fileOutputSteam == NULL) {
		env->ExceptionDescribe();
		env->ExceptionClear();
		cout << "����FileOutputStream����ʧ��" << endl;
		return false;
	}

	//����PrintStream��
	jclass printStreamClass = env->FindClass("java/io/PrintStream");
	if (env->ExceptionCheck() == JNI_TRUE || printStreamClass == NULL) {
		env->ExceptionDescribe();
		env->ExceptionClear();
		cout << "�Ҳ���FileOutputSteam��" << endl;
		return false;
	}

	//��ù��췽����id
	jmethodID printStreamConstructor = env->GetMethodID(printStreamClass, "<init>", "(Ljava/io/OutputStream;)V");
	if (env->ExceptionCheck() == JNI_TRUE || printStreamConstructor == NULL) {
		env->ExceptionDescribe();
		env->ExceptionClear();
		cout << "�Ҳ���PrintStreamConstructor�Ĺ��췽��" << endl;
		return false;
	}

	//����PrintStream����
	jobject printStream = env->NewObject(printStreamClass, printStreamConstructor, fileOutputSteam);
	if (env->ExceptionCheck() == JNI_TRUE || printStream == NULL) {
		env->ExceptionDescribe();
		env->ExceptionClear();
		cout << "����PrintStream����ʧ��" << endl;
		return false;
	}

	//����System��
	jclass systemClass = env->FindClass("java/lang/System");
	if (env->ExceptionCheck() == JNI_TRUE || systemClass == NULL) {
		env->ExceptionDescribe();
		env->ExceptionClear();
		cout << "�Ҳ���System��" << endl;
		return false;
	}

	//���System���ָ������
	jmethodID setSystemMethod = env->GetStaticMethodID(systemClass, method, "(Ljava/io/PrintStream;)V");
	if (env->ExceptionCheck() == JNI_TRUE || setSystemMethod == NULL) {
		env->ExceptionDescribe();
		env->ExceptionClear();
		cout << "���System���ָ������ʧ��" << endl;
		return false;
	}

	//����System��������������־
	env->CallStaticVoidMethod(systemClass, setSystemMethod, printStream);
	if (env->ExceptionCheck() == JNI_TRUE) {
		env->ExceptionDescribe();
		env->ExceptionClear();
		cout << "����System��������������־ʧ��" << endl;
		return false;
	}
	return true;
}
