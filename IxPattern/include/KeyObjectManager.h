/**
* @file      KeyObjectManager.h
* @brief     值、对象映射的管理类
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
* @brief 需要有Key来标示的对象模板
* @note 使用方法是，继承该类，并提供关键字的类型，需要和CKeyObjectManager一起使用，比如
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
	 * @brief    设置关键字
	 * @param    KeyType & key  关键字的值
	 * @return   void       
	*/
	void SetKey(KeyType& key)
	{
		m_Key = key;
	}

	/**
	 * @fn       KeyType& GetKey()         
	 * @brief    获取关键字的值
	 * @return   KeyType&  关键字的值
	*/
	KeyType& GetKey()
	{
		return m_Key;
	}

	virtual void VisitKeyObject(){}

	/**
	 * @fn       bool operator==( CKeyObject<KeyType>& param )      
	 * @brief    比较关键字是否相等
	 * @param    CKeyObject<KeyType> & param  比较对象
	 * @return   bool  相等返回true，否则返回false
	*/
	bool operator==( CKeyObject<KeyType>& param )
	{
		return param.GetKey() == m_Key;
	}

	/**
	 * @fn       bool IsDeleteByContainer()
	 * @brief    该对象是否需要被管理类删除
	 * @return   bool  该对象是否需要被管理类删除
	*/
	bool IsDeleteByContainer()
	{
		return m_bDeleteByContainer;
	}

	/**
	 * @fn       void SetDeleteByContainer(bool bDeleteByContainer)
	 * @brief    设置被是否需要被管理类删除
	 * @param    bool bDeleteByContainer     
	 * @return   void       
	*/
	void SetDeleteByContainer(bool bDeleteByContainer)
	{
		m_bDeleteByContainer = bDeleteByContainer;
	}

private:
	KeyType m_Key;	///<关键字的值
	bool m_bDeleteByContainer;	///<是否需要被管理类删除
};

//////////////////////////////////////////////////////////////////////////
/**
* @class CKeyObjectManager
* @brief 以KeyType类型关键字来管理ClassType类型的CKeyObject，内部使用map类型来实现
* @note 使用方法是，继承该类，需要和CKeyObject一起使用，比如
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
	test_manager.Insert(1,test_obj_1);//该对象的关键字为1
	test_manager.Insert(2,test_obj_2);//该对象的关键字为2
	test_manager.Insert(3,test_obj_3);//该对象的关键字为3

	MyKeyObject* p2=test_manager.Get(2);//获取关键字为2的对象
	MyKeyObject* p3=test_manager.Remove(3);//erase关键字为3的对象，并不真正删除
	delete p3;//执行删除操作
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
	 * @brief    构造函数
	 * @param    bool bAutoDelete   该参数表示是否进行自动删除，当为true的时候，必须传入new出来的堆上的对象指针，为false的时候必须传入栈上的对象指针
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
	 * @brief    根据关键字从map中获取对象指针
	 * @param    KeyType key  KeyType类型的关键字
	 * @return   ClassType*  ClassType的对象指针，如果存在该对象，则返回非NULL，否则返回NULL
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
	 * @brief    添加一个对象指针到map中，并将该对象的关键字一起加入
	 * @param    KeyType key   KeyType类型的关键字
	 * @param    ClassType * pObject  ClassType的对象指针
	 * @return   bool  成功返回true，失败返回false
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
	 * @brief    根据关键字key从map中移出一个对象指针
	 * @param    KeyType key   KeyType类型的关键字
	 * @return   ClassType*   返回NULL表示操作失败，返回非NULL表示操作成功
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
	 * @brief    根据关键字key从map中移出一个对象指针，并将其删除
	 * @param    KeyType key   KeyType类型的关键字
	 * @return   bool  返回false表示操作失败，返回true表示操作成功
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
	 * @brief    清除map中的所有对象指针，仅在m_bAutoClear为true的时候有效
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
	 * @brief    获取map中存放的对象指针的个数
	 * @return   int   对象指针的个数
	*/
	virtual int Size()
	{
		GUARD_RETURN(CThreadMutex, m_objectMapLock, 0);

		return (int) m_mapObjectMap.size();
	}

	/**
	 * @fn       void Visit()         
	 * @brief    访问map中的各个CKeyObject
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
	 * @brief    获取存放关键字和对象指针的map
	 * @return   ObjectMap&   存放关键字和对象指针的map
	*/
	ObjectMap& GetObjectMap()
	{
		return m_mapObjectMap;
	}

	/**
	 * @fn       void SetAutoDelete(bool bAutoDelete)         
	 * @brief    设置是否自动删除容器中的对象
	 * @param    bool bAutoDelete   是否自动删除
	 * @return   void       
	*/
	void SetAutoDelete(bool bAutoDelete)
	{
		m_bAutoDelete = bAutoDelete;
	}

	/**
	 * @fn       virtual void OnBeforeDelete(ClassType* pObject)         
	 * @brief    在删除某个对象前调用
	 * @param    ClassType * pObject  需要删除的对象
	 * @return   void       
	*/
	virtual void OnBeforeDelete(ClassType* pObject)
	{

	}

	/**
	 * @fn       virtual void OnBeforeInsert(KeyType key, ClassType* pObject)         
	 * @brief    在插入某个对象之前调用
	 * @param    KeyType key   插入对象的关键字
	 * @param    ClassType * pObject  对象指针
	 * @return   void       
	*/
	virtual void OnBeforeInsert(KeyType key, ClassType* pObject)
	{

	}

	/**
	 * @fn       virtual void OnAfterInsert(KeyType key, ClassType* pObject)         
	 * @brief    插入某个对象之后调用
	 * @param    KeyType key  插入对象关键字
	 * @param    ClassType * pObject  插入对象的指针
	 * @return   void       
	*/
	virtual void OnAfterInsert(KeyType key, ClassType* pObject)
	{

	}

protected:
	bool m_bAutoDelete;		///<是否需要自动删除容器中的对象

	ObjectMap m_mapObjectMap;	///<保存关键字和对象的map容器

	CThreadMutex m_objectMapLock;	///<保存关键字和对象的map容器的所
};

#endif
