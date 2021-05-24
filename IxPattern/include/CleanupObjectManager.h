/**
* @file      CleanupObjectManager.h
* @brief     托管单件对象的管理类
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
* @brief 托管单件对象的管理类
* @note  
*/
class PATTERN_EXPORT CCleanupObjectManager
{
public:

	/**
	 * @fn       static CCleanupObjectManager* Instance()         
	 * @brief    单件创建函数
	 * @return   CCleanupObjectManager*    返回单件指针
	*/
	static CCleanupObjectManager* Instance();

	/**
	 * @fn       static void Release()         
	 * @brief    单件释放函数
	 * @return   void       
	*/
	static void Release();

	/**
	 * @fn       void Cleanup(CCleanupObject* pExcept = NULL)         
	 * @brief    清理被托管的单件
	 * @param    CCleanupObject * pExcept  不被清理的单件，如果为NULL，则清理所有
	 * @return   void       
	*/
	void Cleanup(CCleanupObject* pExcept = NULL);

	/**
	 * @fn       void AddCleanupObject(CCleanupObject* pCleanupObject)         
	 * @brief    增加需要被托管的单件
	 * @param    CCleanupObject * pCleanupObject  被托管的单件
	 * @return   void       
	*/
	void AddCleanupObject(CCleanupObject* pCleanupObject);

private:
	CCleanupObjectManager();

	~CCleanupObjectManager();

private:


	//CleanupObjectVector m_vecCleanupObjectVector;	///<保存被托管单件的指针

	void* m_vecCleanupObjectVector;

private:
	static CCleanupObjectManager* s_pInstance;	///<单件指针

	CCleanupObject* m_pExceptCleanupObject;		///<不需要被清除的单件指针
	
	CThreadMutex m_cleanupObjectVectorLock;		///<保存被托管单件vector的锁
};

#endif
