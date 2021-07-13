/**
* @file      ObjectPool.h
* @brief     �����ģ��
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
* @brief �����ģ��
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
	 * @brief    ���������
	 * @param    int nNum  ��������ĸ���
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
	 * @brief    ���ٶ����
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
	 * @brief    �����ڻ�ȡ���󲻹�ʱ���Զ���������ĸ���
	 * @param    int nAutoIncreaseNum  �Զ���������ĸ���
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
	 * @brief    �Ӷ�����л�ȡ����
	 * @param    bool bAllocNew  ������������û�п��õģ���ô��ֵ���Ϊtrue���򴴽�һ�����󣬷��򲻴���
	 * @return   T*  ��ȡ�Ķ������û�л�ȡ�ɹ����򷵻�NULL
	*/
	T* Get(bool bAllocNew = false)
	{
		GUARD_RETURN(CThreadMutex,m_poolLock,NULL);	

		//���δ��ʹ�õ��б������Ϊ0������Ҫ�ȴ���ֱ���ж���黹����
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

		//�������δʵ�ö�����ȡ����Ȼ�����ʹ�ö�����
		m_lstUnusedObjectList.pop_front();
		m_lstUsedObjectList.push_back(pObject);

		return pObject;
	}

	/**
	 * @fn       bool GiveBack(T* pObject)
	 * @brief    ������黹�������
	 * @param    T * pObject  ��Ҫ�黹�Ķ���
	 * @return   bool  �ɹ�����true��ʧ�ܷ���false
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
	 * @brief    ��ȡδʹ�ö���ļ���
	 * @return   int   δʹ�ö���ļ���
	*/
	int GetUnUsedCount()
	{
		GUARD_RETURN(CThreadMutex,m_poolLock,-1);

		return (int)m_lstUnusedObjectList.size();
	}

	/**
	 * @fn       int GetUsedCount()
	 * @brief    ��ȡ�Ѿ�ʹ�ö���ļ���
	 * @return   int   �Ѿ�ʹ�ö���ļ���
	*/
	int GetUsedCount()
	{
		GUARD_RETURN(CThreadMutex,m_poolLock,-1);

		return (int)m_lstUsedObjectList.size();
	}

protected:
	/**
	 * @fn       int OnCreate()
	 * @brief    ����������ʱ���ã����ڷ��ش����Ķ��������������д�ú���ʵ������Ĵ�������
	 * @return   T*   �����ɹ����ش����Ķ���ʧ�ܷ���NULL
	*/
	virtual T* OnCreate()
	{
		return new T;
	}

	/**
	 * @fn       virtual void OnDestroy(T* pObject)
	 * @brief    ���ٶ���ʱ���ã�����ɾ�����������������д�ú���ʵ�������ɾ������
	 * @param    T * pObject  Ҫ���ٵĶ���
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
	 * @brief    �黹����ǰ����
	 * @param    T * pObject  Ҫ�黹�Ķ���
	 * @return   void       
	*/
	virtual void OnGiveBack(T* pObject)
	{

	}

protected:
	typedef std::list<T*> ObjectList;	///<�����б�
	typedef typename ObjectList::iterator ObjectListItr;	///<�����б������

	ObjectList m_lstUsedObjectList;		///<��ʹ�õĶ����б�

	ObjectList m_lstUnusedObjectList;	///<δʹ�õĶ����б�

	CThreadMutex m_poolLock;	///<�����б����

	int m_nAutoIncreaseNum;	///<�ڻ�ȡ���󲻹�ʱ���Զ������ĸ���

private:
	///����ָ���б�����ж���
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
