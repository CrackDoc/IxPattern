/**
 * @file      ThreadQueue.h
 * @brief     线程安全的队列
 *          
 * @note      先进先出队列，通过条件变量和互斥保护。用于实现一个线程放数据，另一个线程取数据的生产消费者
 *       
 * @author    
 * @date      2014/05/17
 * @version   1.0.0.0
 * @CopyRight IxLab
 */
#ifndef _THREAD_QUEUE_H_
#define _THREAD_QUEUE_H_

#include <list>
#include <string>
#include <vector>
#include "ThreadMutex.h"
#include "Condition.h"

template<typename T> 
class CThreadQueue
{
public:
	CThreadQueue(const char* szName = "UnNamedQueue")
		:m_strName(szName)
	{

	}

	CThreadQueue(const std::string& strName)
		:m_strName(strName)
	{

	}

	virtual ~CThreadQueue()
	{
		Clear();
	}

	/**
	 * @fn       void SetName(const char* strName)
	 * @brief    设置队列的名字
	 * @param    const char * strName  队列的名字
	 * @return   void  
	*/
	void SetName(const char* strName)
	{
		m_strName = strName;
	}

	/**
	 * @fn       void SetName(const std::string& strName)
	 * @brief    设置队列的名字
	 * @param    const std::string & strName  队列的名字
	 * @return   void  
	*/
	void SetName(const std::string& strName)
	{
		m_strName = strName;
	}

	/**
	 * @fn       std::string GetName()
	 * @brief    获取队列的名字
	 * @return   std::string  队列的名字
	*/
	std::string GetName()
	{
		return m_strName;
	}

	/**
	 * @fn       void PutItem(T item)
	 * @brief    往队列中放数据
	 * @param    T item  数据
	 * @return   void  
	*/
	void PutItem(T item)
	{
		m_lock.Acquire();
		m_queue.push_back(item);
		m_lock.Release();

		m_waitCondition.Signal();
	}

	/**
	 * @fn       void PutItemRef(const T& rItem)
	 * @brief    往队列中放数据
	 * @param    const T & rItem  数据
	 * @return   void  
	*/
	void PutItemRef(const T& rItem)
	{
		m_lock.Acquire();
		m_queue.push_back(rItem);
		m_lock.Release();

		m_waitCondition.Signal();
	}

	/**
	 * @fn       void PutItemNoLock(T item)
	 * @brief    往队列中放数据,不会加锁
	 * @param    T item  数据
	 * @return   void  
	*/
	void PutItemNoLock(T item)
	{
		m_queue.push_back(item);
	}

	/**
	 * @fn       void PutItemRefNoLock(const T& rItem)
	 * @brief    往队列中放数据,不会加锁
	 * @param    const T & rItem  数据
	 * @return   void  
	*/
	void PutItemRefNoLock(const T& rItem)
	{
		m_queue.push_back(rItem);
	}
  
	/**
	 * @fn       bool GetItem(T& rItem, int nWaitTime)
	 * @brief    从队列中获取数据
	 * @param    T & rItem  获取的数据
	 * @param    int nWaitTime  等待的时间，小于0表示一直等待
	 * @return   bool  成功返回true，失败返回false
	*/
	bool GetItem(T& rItem, int nWaitTime = -1)
	{
		bool bRet = true;
		m_lock.Acquire();	

		while(m_queue.empty() == true)
		{
			bRet = m_waitCondition.Wait(m_lock, nWaitTime);
			if(bRet == false)
			{
				m_lock.Release();
				return bRet;
			}
		}

		rItem = m_queue.front();
		m_queue.pop_front();
		m_lock.Release();
		return bRet;
	}

	/**
	 * @fn       bool GetAllItems(std::vector<T>& rItems, int nWaitTime)
	 * @brief    获取所有数据
	 * @param    std::vector<T> & rItems  输出的所有数据
	 * @param    int nWaitTime  等待的时间，小于0表示一直等待
	 * @return   bool  成功返回true，失败返回false
	*/
	bool GetAllItems(std::vector<T>& rItems, int nWaitTime = -1)
	{
		bool bRet = true;
		m_lock.Acquire();
		while(m_queue.empty() == true)
		{
			bRet = m_waitCondition.Wait(m_lock, nWaitTime);
			if(bRet == false)
			{
				m_lock.Release();
				return bRet;
			}
		}

		for(typename std::list<T>::iterator itr = m_queue.begin(); itr != m_queue.end(); itr++)
		{
			T& rItem = *itr;
			rItems.push_back(rItem);
		}

		m_queue.clear();

		m_lock.Release();

		return bRet;
	}

	/**
	 * @fn       int GetSize()
	 * @brief    获取队列的大小
	 * @return   int  队列的大小
	*/
	int GetSize()
	{
		m_lock.Acquire();
		int nSize = (int)m_queue.size();
		m_lock.Release();
		return nSize;
	}
 
	/**
	 * @fn       void Clear()
	 * @brief    清空队列
	 * @return   void  
	*/
	void Clear()
	{
		m_lock.Acquire();
		for(typename std::list<T>::iterator itr = m_queue.begin(); itr != m_queue.end(); itr++)
		{
			OnDestroyQueueItem(*itr);
		}
		m_queue.clear();
		m_lock.Release();
	}

	/**
	 * @fn       void OnDestroyQueueItem(T& rItem)
	 * @brief    销毁队列中的Item的回调
	 * @param    T & rItem  被销毁的Item
	 * @return   void  
	*/
	virtual void OnDestroyQueueItem(T& rItem)
	{

	}
protected:
	std::string m_strName;		///<队列的名字
	std::list<T> m_queue;		///<队列的数据结构
	CThreadMutex m_lock;		///<队列的锁
	CCondition m_waitCondition;	///<队列的条件变量
};

#endif
