/**
* @file      CleanupObjectManager.h
* @brief     �йܵ�������Ĺ�����
*          
* @note      
*          
* @author    crack
* @date      2020-06-07
* @version   1.0.0.0
* @CopyRight IxLab
*/
#ifndef _CLEARNUP_OBJECT_MANAGER_H_
#define _CLEARNUP_OBJECT_MANAGER_H_

//#include <vector>
#include "CleanupObject.h"
#include "ThreadMutex.h"

/**
* @class CCleanupObjectManager
* @brief �йܵ�������Ĺ�����
* @note  
*/
class PATTERN_EXPORT CCleanupObjectManager
{
public:

	/**
	 * @fn       static CCleanupObjectManager* Instance()         
	 * @brief    ������������
	 * @return   CCleanupObjectManager*    ���ص���ָ��
	*/
	static CCleanupObjectManager* Instance();

	/**
	 * @fn       static void Release()         
	 * @brief    �����ͷź���
	 * @return   void       
	*/
	static void Release();

	/**
	 * @fn       void Cleanup(CCleanupObject* pExcept = NULL)         
	 * @brief    �����йܵĵ���
	 * @param    CCleanupObject * pExcept  ��������ĵ��������ΪNULL������������
	 * @return   void       
	*/
	void Cleanup(CCleanupObject* pExcept = NULL);

	/**
	 * @fn       void AddCleanupObject(CCleanupObject* pCleanupObject)         
	 * @brief    ������Ҫ���йܵĵ���
	 * @param    CCleanupObject * pCleanupObject  ���йܵĵ���
	 * @return   void       
	*/
	void AddCleanupObject(CCleanupObject* pCleanupObject);

private:
	CCleanupObjectManager();

	~CCleanupObjectManager();

private:


	//CleanupObjectVector m_vecCleanupObjectVector;	///<���汻�йܵ�����ָ��

	void* m_vecCleanupObjectVector;

private:
	static CCleanupObjectManager* s_pInstance;	///<����ָ��

	CCleanupObject* m_pExceptCleanupObject;		///<����Ҫ������ĵ���ָ��
	
	CThreadMutex m_cleanupObjectVectorLock;		///<���汻�йܵ���vector����
};

#endif
