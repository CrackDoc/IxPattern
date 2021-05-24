/**
* @file      LoopQueue.h
* @brief     循环队列模板
*          
* @note      条件变量实现的循环缓冲，初始化时分配好缓冲区大小，类型可由模板参数指定(线程同步用条件变量实现)。用于实现生产消费者
* @code

	//循环队列里面存的对象的类
	class CObject
	{
	public:
		CObject& operator = (const CObject& rObject)
		{
			if(this == &rObject)
			{
				return *this;
			}

			m_nIndex = rObject.m_nIndex;
			return *this;
		}
	public:
		int m_nIndex;
	};

	//继承的使用方法
	class CLoopQueueTest:public CTask,public CLoopQueue<CObject>
	{
	typedef CLoopQueue<CObject> ObjectLoopQueue;
	public:
		CLoopQueueTest()
		{
			ObjectLoopQueue::Create(10);
		}

		void Start()
		{
			CTask::Activate();
			for(int i = 0;i < 1000;i++)
			{
				CObject object;
				object.m_nIndex = i;
				if(ObjectLoopQueue::EnQueue(object,1000) == false)
				{
					//cout << "enqueue error" << endl;
				}
			}
		}

		void Stop()
		{
			ObjectLoopQueue::Destroy();

			CTask::Wait();
		}

		virtual void OnRunTask()
		{
			while(true)
			{
				if(true == CTask::IsQuit())
				{
					break;
				}
				CObject object;
				if(ObjectLoopQueue::DeQueue(object,1000) == false)
				{
					//cout<<"dequeue error"<<endl;
				}
				else
				{
					object.Print();
				}
				CTimeUtility::Sleep(1000);
			}
		}
	};

	//非继承时
	CLoopQueue<CObject> objectLoopQueue;
	CObject obj;
	objectLoopQueue.EnQueue(obj,1000);

	CObject getObj;
	objectLoopQueue.DeQueue(getObj,1000);
* @endcode
* @author    
* @date      2020-06-07
* @version   1.0.0.0
* @CopyRight IxLab
*/
#ifndef _LOOP_QUEUE_H_
#define _LOOP_QUEUE_H_
#include "ThreadMutex.h"
#include "Condition.h"
#include <vector>

/**
* @class CLoopQueue
* @brief 循环队列
* @note  
*/
template<typename T>
class CLoopQueue
{
public:
	CLoopQueue()
		:m_nMaxSize(0),
		m_nFront(0),
		m_nRear(0),
		m_bDestroy(true),
		m_nTotalObjects(0)
	{
		
	}

	virtual ~CLoopQueue()
	{
		Destroy();
	}

	/**
	 * @fn       bool Create(int nMaxSize)
	 * @brief    创建循环队列
	 * @param    int nMaxSize  循环队列的最大容量
	 * @return   bool  成功返回true，失败返回false
	*/
	bool Create(int nMaxSize)
	{
		GUARD_RETURN(CThreadMutex,m_loopQueueLock,false);

		if(nMaxSize <= 0)
		{
			return false;
		}

		if(m_bDestroy == false)
		{
			return true;
		}

		for(int i = 0; i < nMaxSize; i++)
		{
			T* pObject = OnCreate();
			if(pObject != NULL)
			{
				m_vecLoopQueue.push_back(pObject);
			}
		}

		m_nMaxSize = nMaxSize;

		Clear();

		m_bDestroy = false;

		return true;
	}

	/**
	 * @fn       void Destroy()
	 * @brief    销毁循环队列
	 * @return   void       
	*/
	void Destroy()
	{
		GUARD(CThreadMutex,m_loopQueueLock);

		if(m_bDestroy == true)
		{
			return;
		}

		for(int i = 0; i < m_nMaxSize; i++)
		{			
			OnDestroy(m_vecLoopQueue[i]);
		}

		Clear();

		m_vecLoopQueue.clear();		

		m_nMaxSize = 0;

		m_bDestroy = true;
	}

	/**
	 * @fn       bool EnQueue(T& rObject,int nTimeout = -1,bool bBlock = true)
	 * @brief    入队操作
	 * @param    T & rObject   入队的对象，该对象所对应的类应该实现operator=操作
	 * @param    int nTimeout  如果队列满，将等待nTimeout毫秒的时间，如果为-1表示永远等待
	 * @param    bool bBlock   表示如果队列已满将阻塞等待消息队列出现空位，为false时表示如果队列已满将自动覆盖前面未处理的消息
	 * @return   bool  成功返回true，失败返回false
	*/
	bool EnQueue(T& rObject,int nTimeout = -1,bool bBlock = true)
	{
		if(m_bDestroy == true)
		{
			return false;
		}

		bool bRet = true;
		m_loopQueueLock.Acquire();

		if(bBlock == true)
		{
			while((m_nTotalObjects >= m_nMaxSize) && (bRet == true))
			{
				bRet = m_enQueueCond.Wait(m_loopQueueLock,nTimeout);
			}
		}
		else
		{
			//非阻塞模式下直接覆盖前面的
			if((m_nTotalObjects >= m_nMaxSize) && (bRet == true))
			{
				m_nFront = (m_nFront+1)%m_nMaxSize;
				m_nTotalObjects--;
			}
		}
		m_loopQueueLock.Release();

		m_loopQueueLock.Acquire();
		if(bRet == true)
		{
			if(m_nRear >= (int)m_vecLoopQueue.size())
			{
				m_loopQueueLock.Release();

				return false;
			}

			T* pObject  =m_vecLoopQueue[m_nRear];
			if(pObject == NULL)
			{
				m_loopQueueLock.Release();

				return false;
			}

			*pObject = rObject;

			m_nRear = (m_nRear+1)%m_nMaxSize;

			m_nTotalObjects++;

			m_deQueueCond.Signal();
		}

		m_loopQueueLock.Release();

		return bRet;
	}

	/**
	 * @fn       bool DeQueue(T& rObject,int nTimeout = -1)
	 * @brief    出队操作
	 * @param    T & rObject    出队的对象，该对象所对应的类应该实现operator=操作
	 * @param    int nTimeout   如果队列空，将等待nTimeout毫秒的时间，如果为-1表示永远等待
	 * @return   bool   成功返回true，失败返回false
	*/
	bool DeQueue(T& rObject,int nTimeout = -1)
	{
		if(m_bDestroy == true)
		{
			return false;
		}

		bool bRet = true;

		m_loopQueueLock.Acquire();

		while((m_nTotalObjects <= 0) && (bRet == true))
		{
			bRet = m_deQueueCond.Wait(m_loopQueueLock,nTimeout);
		}

		m_loopQueueLock.Release();

		m_loopQueueLock.Acquire();

		if(bRet == true)
		{
			if(m_nFront >= (int)m_vecLoopQueue.size())
			{
				m_loopQueueLock.Release();

				return false;
			}

			T* pObject = m_vecLoopQueue[m_nFront];
			if(pObject == NULL)
			{
				m_loopQueueLock.Release();

				return false;
			}

			rObject = *pObject;

			m_nFront = (m_nFront+1)%m_nMaxSize;

			m_nTotalObjects--;

			m_enQueueCond.Signal();
		}

		m_loopQueueLock.Release();

		return bRet;
	}

	/**
	 * @fn       void Clear()         
	 * @brief    清空队列
	 * @return   void       
	*/
	void Clear()
	{
		GUARD(CThreadMutex,m_loopQueueLock);

		m_nRear = 0;
		m_nFront = 0;
		m_nTotalObjects = 0;
	}

	/**
	 * @fn       int GetSize()         
	 * @brief    获取当前队列的元素个数
	 * @return   int   当前队列的元素个数
	*/
	int GetSize()
	{
		GUARD_RETURN(CThreadMutex,m_loopQueueLock,-1);

		if(m_nMaxSize == 0)
		{
			return 0;
		}

		return m_nTotalObjects;
	}

	/**
	 * @fn       T* Front()         
	 * @brief    获取队列的头元素
	 * @return   T*   队列的头元素
	*/
	T* Front()
	{
		GUARD_RETURN(CThreadMutex,m_loopQueueLock,NULL);

		if(GetSize() == 0)
		{
			return NULL;
		}

		if(m_nFront >= (int)m_vecLoopQueue.size())
		{
			return NULL;
		}

		return m_vecLoopQueue[m_nFront];
	}

	/**
	 * @fn       T* Rear()         
	 * @brief    获取队列的尾元素
	 * @return   T*  队列的尾元素
	*/
	T* Rear()
	{
		GUARD_RETURN(CThreadMutex,m_loopQueueLock,NULL);

		if(GetSize() == 0)
		{
			return NULL;
		}

		if(m_nRear >= (int)m_vecLoopQueue.size())
		{
			return NULL;
		}

		return m_vecLoopQueue[m_nRear];
	}

protected:
	/**
	 * @fn       virtual T* OnCreate()
	 * @brief    创建对象的虚函数，用户可以重写该函数，实现不同的创建方式
	 * @return   T*   创建对象的指针
	*/
	virtual T* OnCreate()
	{
		return new T;
	}

	/**
	 * @fn       virtual void OnDestroy(T* pObject)
	 * @brief    销毁对象的虚函数，用户可以重写该函数，实现不同的销毁方式
	 * @param    T * pObject  需要销毁的对象
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

protected:
	int m_nMaxSize;		///<队列容量

	int m_nFront;		///<队列首的索引

	int m_nRear;		///<队列尾的索引

	bool m_bDestroy;	///<队列是否已经销毁

	int m_nTotalObjects;	///<当前队列的元素个数

	typedef std::vector<T*> LoopQueue;

	LoopQueue m_vecLoopQueue;		///<存放队列的vector容器

	CCondition m_enQueueCond;	///<入队的条件变量

	CCondition m_deQueueCond;	///<出队的条件变量

	CThreadMutex m_loopQueueLock;	///<队列的锁
};
#endif
