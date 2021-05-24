/**
* @file      KeyObjectManager.h
* @brief     ֵ������ӳ��Ĺ�����
*          
* @note      
*          
* @author    crack
* @date      2020-06-07
* @version   1.0.0.0
* @CopyRight IxLab
*/
#ifndef _KEY_OBJECT_MANAGER_H_
#define _KEY_OBJECT_MANAGER_H_

#include "IxPatternExport.h"
#include "ThreadMutex.h"

#include <map>
#include <string>

/**
* @class CKeyObject
* @brief ��Ҫ��Key����ʾ�Ķ���ģ��
* @note ʹ�÷����ǣ��̳и��࣬���ṩ�ؼ��ֵ����ͣ���Ҫ��CKeyObjectManagerһ��ʹ�ã�����
* @code
	class MyKeyObject:public CKeyObject<int>
	{
	}
* @endcode
*/
template< typename KeyType >
class CKeyObject
{
public:
	CKeyObject(void)
		:m_bDeleteByContainer(true)
	{
	}
	virtual ~CKeyObject(void)
	{
	}

	/**
	 * @fn       void SetKey(KeyType& key)         
	 * @brief    ���ùؼ���
	 * @param    KeyType & key  �ؼ��ֵ�ֵ
	 * @return   void       
	*/
	void SetKey(KeyType& key)
	{
		m_Key = key;
	}

	/**
	 * @fn       KeyType& GetKey()         
	 * @brief    ��ȡ�ؼ��ֵ�ֵ
	 * @return   KeyType&  �ؼ��ֵ�ֵ
	*/
	KeyType& GetKey()
	{
		return m_Key;
	}

	virtual void VisitKeyObject(){}

	/**
	 * @fn       bool operator==( CKeyObject<KeyType>& param )      
	 * @brief    �ȽϹؼ����Ƿ����
	 * @param    CKeyObject<KeyType> & param  �Ƚ϶���
	 * @return   bool  ��ȷ���true�����򷵻�false
	*/
	bool operator==( CKeyObject<KeyType>& param )
	{
		return param.GetKey() == m_Key;
	}

	/**
	 * @fn       bool IsDeleteByContainer()
	 * @brief    �ö����Ƿ���Ҫ��������ɾ��
	 * @return   bool  �ö����Ƿ���Ҫ��������ɾ��
	*/
	bool IsDeleteByContainer()
	{
		return m_bDeleteByContainer;
	}

	/**
	 * @fn       void SetDeleteByContainer(bool bDeleteByContainer)
	 * @brief    ���ñ��Ƿ���Ҫ��������ɾ��
	 * @param    bool bDeleteByContainer     
	 * @return   void       
	*/
	void SetDeleteByContainer(bool bDeleteByContainer)
	{
		m_bDeleteByContainer = bDeleteByContainer;
	}

private:
	KeyType m_Key;	///<�ؼ��ֵ�ֵ
	bool m_bDeleteByContainer;	///<�Ƿ���Ҫ��������ɾ��
};

//////////////////////////////////////////////////////////////////////////
/**
* @class CKeyObjectManager
* @brief ��KeyType���͹ؼ���������ClassType���͵�CKeyObject���ڲ�ʹ��map������ʵ��
* @note ʹ�÷����ǣ��̳и��࣬��Ҫ��CKeyObjectһ��ʹ�ã�����
* @code
	class MyKeyObject:public CKeyObject<int>
	{
	}
	class MyKeyObjectManager:public CKeyObjectManager<int,MyKeyObject>
	{
	}
	MyKeyObjectManager test_manager;
	MyKeyObject* test_obj_1=new MyKeyObject;
	MyKeyObject* test_obj_2=new MyKeyObject;
	MyKeyObject* test_obj_3=new MyKeyObject;
	test_manager.Insert(1,test_obj_1);//�ö���Ĺؼ���Ϊ1
	test_manager.Insert(2,test_obj_2);//�ö���Ĺؼ���Ϊ2
	test_manager.Insert(3,test_obj_3);//�ö���Ĺؼ���Ϊ3

	MyKeyObject* p2=test_manager.Get(2);//��ȡ�ؼ���Ϊ2�Ķ���
	MyKeyObject* p3=test_manager.Remove(3);//erase�ؼ���Ϊ3�Ķ��󣬲�������ɾ��
	delete p3;//ִ��ɾ������
* @endcode
*/
template< typename KeyType, typename ClassType >
class CKeyObjectManager
{
public:
	typedef std::map< KeyType,ClassType* >  ObjectMap;
	typedef typename std::map< KeyType, ClassType* >::iterator ObjectMapItr;

	/**
	 * @fn       CKeyObjectManager(bool bAutoDelete = true)         
	 * @brief    ���캯��
	 * @param    bool bAutoDelete   �ò�����ʾ�Ƿ�����Զ�ɾ������Ϊtrue��ʱ�򣬱��봫��new�����Ķ��ϵĶ���ָ�룬Ϊfalse��ʱ����봫��ջ�ϵĶ���ָ��
	*/
	CKeyObjectManager(bool bAutoDelete = true)
		:m_bAutoDelete(bAutoDelete)
	{
	}

	virtual ~CKeyObjectManager()
	{
		ClearMap();
	}

	/**
	 * @fn       virtual ClassType* Get(KeyType key)         
	 * @brief    ���ݹؼ��ִ�map�л�ȡ����ָ��
	 * @param    KeyType key  KeyType���͵Ĺؼ���
	 * @return   ClassType*  ClassType�Ķ���ָ�룬������ڸö����򷵻ط�NULL�����򷵻�NULL
	*/
	virtual ClassType* Get(KeyType key)
	{
		GUARD_RETURN(CThreadMutex, m_objectMapLock, NULL);

		ObjectMapItr itr = m_mapObjectMap.find(key);
		if(itr != m_mapObjectMap.end())
		{
			return itr->second;
		}
		else
		{
			return NULL;
		}
	}

	/**
	 * @fn       virtual bool Insert(KeyType key, ClassType* pObject)         
	 * @brief    ���һ������ָ�뵽map�У������ö���Ĺؼ���һ�����
	 * @param    KeyType key   KeyType���͵Ĺؼ���
	 * @param    ClassType * pObject  ClassType�Ķ���ָ��
	 * @return   bool  �ɹ�����true��ʧ�ܷ���false
	*/
	virtual bool Insert(KeyType key, ClassType* pObject)
	{
		GUARD_RETURN(CThreadMutex, m_objectMapLock, false);

		if(pObject == NULL)
		{
			return false;
		}

		ObjectMapItr itr = m_mapObjectMap.find(key);
		if(itr != m_mapObjectMap.end())
		{
			return false;
		}

		pObject->SetKey(key);

		OnBeforeInsert(key, pObject);

		m_mapObjectMap.insert(std::make_pair(key, pObject));

		OnAfterInsert(key, pObject);

		return true;
	}

	/**
	 * @fn       virtual ClassType* Remove(KeyType key)         
	 * @brief    ���ݹؼ���key��map���Ƴ�һ������ָ��
	 * @param    KeyType key   KeyType���͵Ĺؼ���
	 * @return   ClassType*   ����NULL��ʾ����ʧ�ܣ����ط�NULL��ʾ�����ɹ�
	*/
	virtual ClassType* Remove(KeyType key)
	{
		GUARD_RETURN(CThreadMutex, m_objectMapLock, NULL);

		ObjectMapItr itr = m_mapObjectMap.find(key);
		if(itr != m_mapObjectMap.end())
		{
			ClassType* pObject = itr->second;

			m_mapObjectMap.erase(itr);

			return pObject;
		}
		else
		{
			return NULL;
		}
	}

	/**
	 * @fn       virtual bool Delete(KeyType key)         
	 * @brief    ���ݹؼ���key��map���Ƴ�һ������ָ�룬������ɾ��
	 * @param    KeyType key   KeyType���͵Ĺؼ���
	 * @return   bool  ����false��ʾ����ʧ�ܣ�����true��ʾ�����ɹ�
	*/
	virtual bool Delete(KeyType key)
	{
		GUARD_RETURN(CThreadMutex, m_objectMapLock, false);

		try
		{
			ClassType* pObject = Remove(key);

			if(pObject == NULL)
			{
				return false;
			}

			OnBeforeDelete(pObject);

			if(pObject->IsDeleteByContainer() == true)
			{
				delete pObject;
				pObject = NULL;
			}

			return true;
		}
		catch(...)
		{
			return false;
		}
	}

	/**
	 * @fn       virtual void ClearMap()         
	 * @brief    ���map�е����ж���ָ�룬����m_bAutoClearΪtrue��ʱ����Ч
	 * @return   void       
	*/
	virtual void ClearMap()
	{
		GUARD(CThreadMutex, m_objectMapLock);

		if(m_bAutoDelete == true)
		{
			for(ObjectMapItr itr = m_mapObjectMap.begin(); itr != m_mapObjectMap.end(); itr++)
			{
				ClassType* pObject = itr->second;
				if(pObject == NULL)
				{
					continue;	
				}

				OnBeforeDelete(pObject);

				if(pObject->IsDeleteByContainer() == true)
				{
					delete pObject;
					pObject = NULL;
				}
			}
		}
		m_mapObjectMap.clear();
	}

	/**
	 * @fn       virtual int Size()         
	 * @brief    ��ȡmap�д�ŵĶ���ָ��ĸ���
	 * @return   int   ����ָ��ĸ���
	*/
	virtual int Size()
	{
		GUARD_RETURN(CThreadMutex, m_objectMapLock, 0);

		return (int) m_mapObjectMap.size();
	}

	/**
	 * @fn       void Visit()         
	 * @brief    ����map�еĸ���CKeyObject
	 * @return   void       
	*/
	void Visit()
	{
		GUARD(CThreadMutex, m_objectMapLock);

		for(ObjectMapItr itr = m_mapObjectMap.begin(); itr != m_mapObjectMap.end(); itr++)
		{
			ClassType* pObject = itr->second;
			if(pObject)
			{
				pObject->VisitKeyObject();
			}
		}
	}

	/**
	 * @fn       ObjectMap& GetObjectMap()         
	 * @brief    ��ȡ��Źؼ��ֺͶ���ָ���map
	 * @return   ObjectMap&   ��Źؼ��ֺͶ���ָ���map
	*/
	ObjectMap& GetObjectMap()
	{
		return m_mapObjectMap;
	}

	/**
	 * @fn       void SetAutoDelete(bool bAutoDelete)         
	 * @brief    �����Ƿ��Զ�ɾ�������еĶ���
	 * @param    bool bAutoDelete   �Ƿ��Զ�ɾ��
	 * @return   void       
	*/
	void SetAutoDelete(bool bAutoDelete)
	{
		m_bAutoDelete = bAutoDelete;
	}

	/**
	 * @fn       virtual void OnBeforeDelete(ClassType* pObject)         
	 * @brief    ��ɾ��ĳ������ǰ����
	 * @param    ClassType * pObject  ��Ҫɾ���Ķ���
	 * @return   void       
	*/
	virtual void OnBeforeDelete(ClassType* pObject)
	{

	}

	/**
	 * @fn       virtual void OnBeforeInsert(KeyType key, ClassType* pObject)         
	 * @brief    �ڲ���ĳ������֮ǰ����
	 * @param    KeyType key   �������Ĺؼ���
	 * @param    ClassType * pObject  ����ָ��
	 * @return   void       
	*/
	virtual void OnBeforeInsert(KeyType key, ClassType* pObject)
	{

	}

	/**
	 * @fn       virtual void OnAfterInsert(KeyType key, ClassType* pObject)         
	 * @brief    ����ĳ������֮�����
	 * @param    KeyType key  �������ؼ���
	 * @param    ClassType * pObject  ��������ָ��
	 * @return   void       
	*/
	virtual void OnAfterInsert(KeyType key, ClassType* pObject)
	{

	}

protected:
	bool m_bAutoDelete;		///<�Ƿ���Ҫ�Զ�ɾ�������еĶ���

	ObjectMap m_mapObjectMap;	///<����ؼ��ֺͶ����map����

	CThreadMutex m_objectMapLock;	///<����ؼ��ֺͶ����map��������
};

#endif
