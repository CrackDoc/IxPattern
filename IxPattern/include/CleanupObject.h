/**
* @file      CleanupObject.h
* @brief     可自动释放的单件基类
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
* @brief 可自动释放的单件基类
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
	 * @brief    托管类的单件的清理删除接口
	 * @return   void       
	*/
	virtual void Cleanup() = 0;
};
#endif
