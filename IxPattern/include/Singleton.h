/**
* @file      Singleton.h
* @brief     ������ĺ�
*          
* @note      
*          
* @author    crack
* @date      2015-05-15
* @version   1.0.0.0
* @CopyRight IxLab
*/
#ifndef _SINGLETON_H__
#define _SINGLETON_H__

#include "CleanupObject.h"
#include "CleanupObjectManager.h"


/**
 * @fn       DECLARE_SINGLETON(classType)
 * @brief    �й��൥���������꣬��ͷ�ļ�������ʹ�ã��й��൥������Ҫ�ֶ��ͷţ����������̳�CCleanupObject
 * @param    classType ����  
*/
#define DECLARE_SINGLETON(classType)\
public:\
	static classType* Instance();\
	virtual void Cleanup();\
	static classType* s_pInstance;\
	static CThreadMutex* s_pSingletonLock;

/**
 * @fn       IMPLEMENT_SINGLETON(classType)
 * @brief    �й��൥����ʵ�ֺ꣬��cpp�ļ���ʹ��
 * @param    classType ����  
*/
#define IMPLEMENT_SINGLETON(classType)\
	classType* classType::s_pInstance = NULL;\
	CThreadMutex* classType::s_pSingletonLock = NULL;\
	classType* classType::Instance()\
	{\
		if(s_pInstance == NULL)\
		{\
			if(s_pSingletonLock == NULL)\
			{\
				s_pSingletonLock = new CThreadMutex;\
			}\
			s_pSingletonLock->Acquire();\
			if(s_pInstance == NULL)\
			{\
				s_pInstance = new classType;\
				CCleanupObjectManager::Instance()->AddCleanupObject(s_pInstance);\
			}\
			s_pSingletonLock->Release();\
		}\
		return s_pInstance;\
	}\
	void classType::Cleanup()\
	{\
		if(s_pSingletonLock != NULL)\
		{\
			delete s_pSingletonLock;\
			s_pSingletonLock = NULL;\
		}\
		if(s_pInstance != NULL)\
		{\
			delete s_pInstance;\
			s_pInstance = NULL;\
		}\
	}

/**
 * @fn       IMPLEMENT_SINGLETON(classType)
 * @brief    ��ͨ�����������꣬��ͷ�ļ�������ʹ�ã���Ҫ����Release�����ͷ�
 * @param    classType ����  
*/
#define DECLARE_UNMANGED_SINGLETON(classType)\
public:\
	static classType* Instance();\
	static void Release();\
	static classType* s_pInstance;\
	static CThreadMutex* s_pSingletonLock;

/**
 * @fn       IMPLEMENT_SINGLETON(classType)
 * @brief    ��ͨ������ʵ�ֺ꣬��cpp�ļ���ʹ��
 * @param    classType ����  
*/
#define IMPLEMENT_UNMANGED_SINGLETON(classType)\
	classType* classType::s_pInstance = NULL;\
	CThreadMutex* classType::s_pSingletonLock = NULL;\
	classType* classType::Instance()\
	{\
		if(s_pInstance == NULL)\
		{\
			if(s_pSingletonLock == NULL)\
			{\
				s_pSingletonLock = new CThreadMutex;\
			}\
			s_pSingletonLock->Acquire();\
			if(s_pInstance == NULL)\
			{\
				s_pInstance = new classType;\
			}\
			s_pSingletonLock->Release();\
		}\
		return s_pInstance;\
	}\
	void classType::Release()\
	{\
		if(s_pSingletonLock != NULL)\
		{\
			delete s_pSingletonLock;\
			s_pSingletonLock = NULL;\
		}\
		if(s_pInstance != NULL)\
		{\
			delete s_pInstance;\
			s_pInstance = NULL;\
		}\
	}
#endif
