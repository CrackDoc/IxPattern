/**
* @file      LoopQueue.h
* @brief     ѭ������ģ��
*          
* @note      ��������ʵ�ֵ�ѭ�����壬��ʼ��ʱ����û�������С�����Ϳ���ģ�����ָ��(�߳�ͬ������������ʵ��)������ʵ������������
* @code

	//ѭ�����������Ķ������
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

	//�̳е�ʹ�÷���
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

	//�Ǽ̳�ʱ
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
* @brief ѭ������
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
	 * @brief    ����ѭ������
	 * @param    int nMaxSize  ѭ�����е��������
	 * @return   bool  �ɹ�����true��ʧ�ܷ���false
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
	 * @brief    ����ѭ������
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
	 * @brief    ��Ӳ���
	 * @param    T & rObject   ��ӵĶ��󣬸ö�������Ӧ����Ӧ��ʵ��operator=����
	 * @param    int nTimeout  ��������������ȴ�nTimeout�����ʱ�䣬���Ϊ-1��ʾ��Զ�ȴ�
	 * @param    bool bBlock   ��ʾ������������������ȴ���Ϣ���г��ֿ�λ��Ϊfalseʱ��ʾ��������������Զ�����ǰ��δ�������Ϣ
	 * @return   bool  �ɹ�����true��ʧ�ܷ���false
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
			//������ģʽ��ֱ�Ӹ���ǰ���
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
	 * @brief    ���Ӳ���
	 * @param    T & rObject    ���ӵĶ��󣬸ö�������Ӧ����Ӧ��ʵ��operator=����
	 * @param    int nTimeout   ������пգ����ȴ�nTimeout�����ʱ�䣬���Ϊ-1��ʾ��Զ�ȴ�
	 * @return   bool   �ɹ�����true��ʧ�ܷ���false
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
	 * @brief    ��ն���
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
	 * @brief    ��ȡ��ǰ���е�Ԫ�ظ���
	 * @return   int   ��ǰ���е�Ԫ�ظ���
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
	 * @brief    ��ȡ���е�ͷԪ��
	 * @return   T*   ���е�ͷԪ��
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
	 * @brief    ��ȡ���е�βԪ��
	 * @return   T*  ���е�βԪ��
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
	 * @brief    ����������麯�����û�������д�ú�����ʵ�ֲ�ͬ�Ĵ�����ʽ
	 * @return   T*   ���������ָ��
	*/
	virtual T* OnCreate()
	{
		return new T;
	}

	/**
	 * @fn       virtual void OnDestroy(T* pObject)
	 * @brief    ���ٶ�����麯�����û�������д�ú�����ʵ�ֲ�ͬ�����ٷ�ʽ
	 * @param    T * pObject  ��Ҫ���ٵĶ���
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
	int m_nMaxSize;		///<��������

	int m_nFront;		///<�����׵�����

	int m_nRear;		///<����β������

	bool m_bDestroy;	///<�����Ƿ��Ѿ�����

	int m_nTotalObjects;	///<��ǰ���е�Ԫ�ظ���

	typedef std::vector<T*> LoopQueue;

	LoopQueue m_vecLoopQueue;		///<��Ŷ��е�vector����

	CCondition m_enQueueCond;	///<��ӵ���������

	CCondition m_deQueueCond;	///<���ӵ���������

	CThreadMutex m_loopQueueLock;	///<���е���
};
#endif
