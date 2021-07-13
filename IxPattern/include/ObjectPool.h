/**
* @file      ObjectPool.h
* @brief     对象池模板
*          
* @note      
*          
* @author    crack
* @date      2020-06-07
* @version   1.0.0.0
* @CopyRight IxLab
*/

#ifndef _OBJECT_POOL_H_
#define _OBJECT_POOL_H_

#include <list>
#include <algorithm>
#include "ThreadMutex.h"
#include "Guard.h"

/**
* @class CObjectPool
* @brief 对象池模板
* @note  
*/
template<typename T>
class CObjectPool
{
public:
	CObjectPool()
	{
		m_nAutoIncreaseNum = 1;
	}

	virtual ~CObjectPool()
	{
		Destroy();
	}

	/**
	 * @fn       void Create(int nNum)
	 * @brief    创建对象池
	 * @param    int nNum  创建对象的个数
	 * @return   void       
	*/
	void Create(int nNum)
	{
		GUARD(CThreadMutex,m_poolLock);

		for(int i = 0; i < nNum; i++)
		{
			T* pObject = OnCreate();

			if(NULL != pObject)
			{
				m_lstUnusedObjectList.push_back(pObject);
			}
		}
	}

	/**
	 * @fn       void Destroy()
	 * @brief    销毁对象池
	 * @return   void       
	*/
	void Destroy()
	{
		GUARD(CThreadMutex,m_poolLock);

		DestroyAll(m_lstUsedObjectList);
		DestroyAll(m_lstUnusedObjectList);
	}

	/**
	 * @fn       void SetAutoIncreaseNum(int nAutoIncreaseNum)
	 * @brief    设置在获取对象不够时，自动增长对象的个数
	 * @param    int nAutoIncreaseNum  自动增长对象的个数
	 * @return   void  
	*/
	void SetAutoIncreaseNum(int nAutoIncreaseNum)
	{
		if(nAutoIncreaseNum > 0)
		{
			m_nAutoIncreaseNum = nAutoIncreaseNum;
		}
	}

	/**
	 * @fn       T* Get(bool bAllocNew = false)
	 * @brief    从对象池中获取对象
	 * @param    bool bAllocNew  如果对象池里面没有可用的，那么该值如果为true，则创建一个对象，否则不创建
	 * @return   T*  获取的对象，如果没有获取成功，则返回NULL
	*/
	T* Get(bool bAllocNew = false)
	{
		GUARD_RETURN(CThreadMutex,m_poolLock,NULL);	

		//如果未被使用的列表的容量为0，则需要等待，直到有对象归还回来
		if(m_lstUnusedObjectList.size() == 0)
		{
			if(true == bAllocNew)
			{
				Create(m_nAutoIncreaseNum);
			}
			else
			{
				return NULL;
			}
		}

		T* pObject = m_lstUnusedObjectList.front();

		if(NULL == pObject)
		{
			return NULL;
		}

		//将对象从未实用队列中取出，然后放入使用队列中
		m_lstUnusedObjectList.pop_front();
		m_lstUsedObjectList.push_back(pObject);

		return pObject;
	}

	/**
	 * @fn       bool GiveBack(T* pObject)
	 * @brief    将对象归还给对象池
	 * @param    T * pObject  需要归还的对象
	 * @return   bool  成功返回true，失败返回false
	*/
	bool GiveBack(T* pObject)
	{
		GUARD_RETURN(CThreadMutex,m_poolLock,false);

		if(NULL == pObject)
		{
			return false;
		}

		ObjectListItr itr = std::find(
			m_lstUsedObjectList.begin(),
			m_lstUsedObjectList.end(),pObject);

		if(itr == m_lstUsedObjectList.end())
		{
			return false;
		}

		OnGiveBack(pObject);

		m_lstUsedObjectList.erase(itr);

		m_lstUnusedObjectList.push_back(pObject);

		return true;
	}

	/**
	 * @fn       int GetUnUsedCount()
	 * @brief    获取未使用对象的计数
	 * @return   int   未使用对象的计数
	*/
	int GetUnUsedCount()
	{
		GUARD_RETURN(CThreadMutex,m_poolLock,-1);

		return (int)m_lstUnusedObjectList.size();
	}

	/**
	 * @fn       int GetUsedCount()
	 * @brief    获取已经使用对象的计数
	 * @return   int   已经使用对象的计数
	*/
	int GetUsedCount()
	{
		GUARD_RETURN(CThreadMutex,m_poolLock,-1);

		return (int)m_lstUsedObjectList.size();
	}

protected:
	/**
	 * @fn       int OnCreate()
	 * @brief    当创建对象时调用，用于返回创建的对象，派生类可以重写该函数实现特殊的创建过程
	 * @return   T*   创建成功返回创建的对象，失败返回NULL
	*/
	virtual T* OnCreate()
	{
		return new T;
	}

	/**
	 * @fn       virtual void OnDestroy(T* pObject)
	 * @brief    销毁对象时调用，用于删除对象，派生类可以重写该函数实现特殊的删除过程
	 * @param    T * pObject  要销毁的对象
	 * @return   void       
	*/
	virtual void OnDestroy(T* pObject)
	{
		if(pObject != NULL)
		{
			delete pObject;
			pObject = NULL;
		}
	}

	/**
	 * @fn       virtual void OnGiveBack(T* pObject)
	 * @brief    归还对象前调用
	 * @param    T * pObject  要归还的对象
	 * @return   void       
	*/
	virtual void OnGiveBack(T* pObject)
	{

	}

protected:
	typedef std::list<T*> ObjectList;	///<对象列表
	typedef typename ObjectList::iterator ObjectListItr;	///<对象列表迭代器

	ObjectList m_lstUsedObjectList;		///<已使用的对象列表

	ObjectList m_lstUnusedObjectList;	///<未使用的对象列表

	CThreadMutex m_poolLock;	///<对象列表的锁

	int m_nAutoIncreaseNum;	///<在获取对象不够时，自动增长的个数

private:
	///销毁指定列表的所有对象
	void DestroyAll(ObjectList& rObjectList)
	{
		for(ObjectListItr itr = rObjectList.begin();itr != rObjectList.end();itr++)
		{
			T* pObject = *itr;
			if(NULL != pObject)
			{
				OnDestroy(pObject);
			}
		}

		rObjectList.clear();
	}
};
#endif
