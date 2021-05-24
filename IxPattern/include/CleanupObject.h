/**
* @file      CleanupObject.h
* @brief     ���Զ��ͷŵĵ�������
*          
* @note      
*          
* @author    crack
* @date      2020-06-07
* @version   1.0.0.0
* @CopyRight IxLab
*/
#ifndef _CLEANUP_OBJECT_H_
#define _CLEANUP_OBJECT_H_

#include "IxPatternExport.h"

/**
* @class CCleanupObject
* @brief ���Զ��ͷŵĵ�������
* @note  
*/
class PATTERN_EXPORT CCleanupObjectManager;

class PATTERN_EXPORT CCleanupObject
{
	friend class CCleanupObjectManager;
protected:
	CCleanupObject();
	virtual ~CCleanupObject();

	/**
	 * @fn       virtual void Cleanup() = 0         
	 * @brief    �й���ĵ���������ɾ���ӿ�
	 * @return   void       
	*/
	virtual void Cleanup() = 0;
};
#endif
