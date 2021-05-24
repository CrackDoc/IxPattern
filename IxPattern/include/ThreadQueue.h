/**
 * @file      ThreadQueue.h
 * @brief     �̰߳�ȫ�Ķ���
 *          
 * @note      �Ƚ��ȳ����У�ͨ�����������ͻ��Ᵽ��������ʵ��һ���̷߳����ݣ���һ���߳�ȡ���ݵ�����������
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
	 * @brief    ���ö��е�����
	 * @param    const char * strName  ���е�����
	 * @return   void  
	*/
	void SetName(const char* strName)
	{
		m_strName = strName;
	}

	/**
	 * @fn       void SetName(const std::string& strName)
	 * @brief    ���ö��е�����
	 * @param    const std::string & strName  ���е�����
	 * @return   void  
	*/
	void SetName(const std::string& strName)
	{
		m_strName = strName;
	}

	/**
	 * @fn       std::string GetName()
	 * @brief    ��ȡ���е�����
	 * @return   std::string  ���е�����
	*/
	std::string GetName()
	{
		return m_strName;
	}

	/**
	 * @fn       void PutItem(T item)
	 * @brief    �������з�����
	 * @param    T item  ����
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
	 * @brief    �������з�����
	 * @param    const T & rItem  ����
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
	 * @brief    �������з�����,�������
	 * @param    T item  ����
	 * @return   void  
	*/
	void PutItemNoLock(T item)
	{
		m_queue.push_back(item);
	}

	/**
	 * @fn       void PutItemRefNoLock(const T& rItem)
	 * @brief    �������з�����,�������
	 * @param    const T & rItem  ����
	 * @return   void  
	*/
	void PutItemRefNoLock(const T& rItem)
	{
		m_queue.push_back(rItem);
	}
  
	/**
	 * @fn       bool GetItem(T& rItem, int nWaitTime)
	 * @brief    �Ӷ����л�ȡ����
	 * @param    T & rItem  ��ȡ������
	 * @param    int nWaitTime  �ȴ���ʱ�䣬С��0��ʾһֱ�ȴ�
	 * @return   bool  �ɹ�����true��ʧ�ܷ���false
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
	 * @brief    ��ȡ��������
	 * @param    std::vector<T> & rItems  �������������
	 * @param    int nWaitTime  �ȴ���ʱ�䣬С��0��ʾһֱ�ȴ�
	 * @return   bool  �ɹ�����true��ʧ�ܷ���false
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
	 * @brief    ��ȡ���еĴ�С
	 * @return   int  ���еĴ�С
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
	 * @brief    ��ն���
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
	 * @brief    ���ٶ����е�Item�Ļص�
	 * @param    T & rItem  �����ٵ�Item
	 * @return   void  
	*/
	virtual void OnDestroyQueueItem(T& rItem)
	{

	}
protected:
	std::string m_strName;		///<���е�����
	std::list<T> m_queue;		///<���е����ݽṹ
	CThreadMutex m_lock;		///<���е���
	CCondition m_waitCondition;	///<���е���������
};

#endif
