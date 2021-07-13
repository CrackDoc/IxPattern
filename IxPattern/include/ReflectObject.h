/**
* @file      ReflectObject.h
* @brief     C++创建型反射机制模式
*          
* @note      
*          
* @author    crack
* @date      2020-06-07
* @version   1.0.0.0
* @CopyRight IxLab
*/

#ifndef _REFLECT_OBJECT_H_
#define _REFLECT_OBJECT_H_

#include "IxPatternExport.h"
#include "ThreadMutex.h"
#include "Condition.h"

#include "Singleton.h"
#include <stdio.h>
#include <string>
#include <list>

class PATTERN_EXPORT CReflectObject;

//////////////////////////////////////////////////////////////////////////

/**
* @class CClassRep
* @brief 类描述基类
* @note  
*/
class PATTERN_EXPORT CClassRep
{
	friend class CReflectObject;
public:
	///构造函数
	CClassRep(const char* szClassName,const char* szRealClassName);

	///析构函数
	virtual ~CClassRep();

public:
	/**
	 * @fn       virtual CReflectObject* Create(const char* szClassName,bool bCaseSensitive)   
	 * @brief    根据类名创建反射对象
	 * @param    const char * szClassName    类名
	 * @param    bool bCaseSensitive   类名是否大小写敏感
	 * @return   CReflectObject*     反射对象指针
	*/
	static CReflectObject* Create(const char* szClassName,bool bCaseSensitive = true);

	/**
	 * @fn       std::string GetClassName()   
	 * @brief    获取类名
	 * @return   std::string   类名
	*/
	const char* GetClassName();

	/**
	 * @fn       std::string GetRealClassName()
	 * @brief    获取真实的类名
	 * @return   std::string  真实的类名
	*/
	const char* GetRealClassName();

	/**
	 * @fn       void SetDisplayName(const std::string& strClassName)   
	 * @brief    设置显示的名字
	 * @param    const std::string& strClassName   显示的名字
	 * @return   void       
	*/
	void SetDisplayName(const char* strClassName);

	/**
	 * @fn       std::string GetDisplayName()   
	 * @brief    获取显示的名字
	 * @return   std::string    显示的名字
	*/
	const char* GetDisplayName();

protected:
	/**
	 * @fn       virtual CReflectObject* OnCreate()   
	 * @brief    创建反射对象接口
	 * @return   CReflectObject*    反射对象指针
	*/
	virtual CReflectObject* OnCreate() = 0;

private:
	//std::string m_strClassName;		///<类名字，可能不是真实的类名，是类别名
	char m_strClassName[256];
	//std::string m_strRealClassName;	///<类真实的名字
	char m_strRealClassName[256];
	//std::string m_strDisplayName;	///<显示的名字
	char m_strDisplayName[256];
};

/**
* @class CClassRepManager
* @brief 类描述对象管理类
* @note  
*/
class PATTERN_EXPORT CClassRepManager
{
	DECLARE_UNMANGED_SINGLETON(CClassRepManager)
public:
	/**
	 * @fn       CClassRep* Get(const char* szClassName,bool bCaseSensitive)   
	 * @brief    根据类名获取类描述对象指针
	 * @param    const char * szClassName     类名
	 * @param    bool bCaseSensitive    类名字是否大小写敏感
	 * @return   CClassRep*    类描述对象指针
	*/
	CClassRep* Get(const char* szClassName,bool bCaseSensitive = true);

	/**
	 * @fn       bool Add(CClassRep* pClassRep)   
	 * @brief    增加一个类描述对象指针，如果已经存在一个类名，那么添加会失败
	 * @param    CClassRep * pClassRep    添加的类描述指针
	 * @return   bool   成功返回true，失败返回false
	*/
	bool Add(CClassRep* pClassRep);

	/**
	 * @fn       bool Remove(CClassRep* pClassRep)   
	 * @brief    删除类描述对象
	 * @param    CClassRep * pClassRep    类描述对象指针
	 * @return   bool    成功返回true，失败返回false
	*/
	bool Remove(CClassRep* pClassRep);

	/**
	 * @fn       int GetSize()   
	 * @brief    获取当前对象的个数
	 * @return   int       
	*/
	int GetSize();

private:
	CClassRepManager();
	~CClassRepManager();

private:
	//typedef std::list<CClassRep*> ClassRepList;
	//typedef ClassRepList::iterator ClassRepListItr;
	//ClassRepList m_lstClassRepList;
	void* m_lstClassRepList;
	CThreadMutex m_classRepListLock;
};

//////////////////////////////////////////////////////////////////////////

///反射对象注册回调函数
typedef void (*ReflectObjectRegisterCallback)(const char* szClassName, void* pInfo);

///当创建反射对象失败时，给用户的一个手动创建对象的接口
typedef CReflectObject* (*ManualCreateReflectObjectCallback)(const char* szClassName, bool bCaseSensitive);

/**
* @class CClassRepInstance
* @brief 类描述模板类
* @note  
*/
template <class T>
class CClassRepInstance : public CClassRep
{
public:
	/**
	 * @fn       CClassRepInstance(const char* szClassName, const char* szRealClassName, ReflectObjectRegisterCallback pRegisterCallback = NULL, void* pInfo = NULL)   
	 * @brief    构造函数   
	 * @param    const char * szClassName    类名
	 * @param    const char * szRealClassName    真实的类名
	 * @param    ReflectObjectRegisterCallback pRegisterCallback    注册回调函数
	 * @param    void * pInfo    注册回调函数传入的参数
	*/
	CClassRepInstance(const char* szClassName, const char* szRealClassName,
		ReflectObjectRegisterCallback pRegisterCallback = NULL, void* pInfo = NULL)
		:CClassRep(szClassName,szRealClassName), 
		m_pRegisterCallback(pRegisterCallback), 
		m_pInfo(pInfo)
	{
		
		if(true == CClassRepManager::Instance()->Add(this))
		{
			if(NULL != m_pRegisterCallback)
			{
				m_pRegisterCallback(szClassName, m_pInfo);
			}
		}
		else
		{
			printf("%s 名字有重复\n",szClassName);
		}
	}

	///析构函数
	virtual ~CClassRepInstance()
	{
		CClassRepManager::Instance()->Remove(this);

		if(CClassRepManager::Instance()->GetSize() == 0)
		{
			CClassRepManager::Release();
		}
	}

protected:
	/**
	 * @fn       virtual CReflectObject* OnCreate()   
	 * @brief    创建一个对象
	 * @return   CReflectObject*     创建的对象
	*/
	virtual CReflectObject* OnCreate()
	{
		return new T;
	}

private:
	ReflectObjectRegisterCallback m_pRegisterCallback;	///<创建对象时注册的回调函数
	void* m_pInfo;	///<创建对象时需要给回调函数传递的值
};

//////////////////////////////////////////////////////////////////////////
/**
* @class CReflectObject
* @brief 具有创建型反射机制的类的基类
* @note 使用方法
	- 首先公有继承该类
	- 在需要动态创建的类头h中申明DECLARE_REFLECT_CLASS(class LIB_EXPORTName)
	- 在需要动态创建的类体cpp中申明IMPLEMENT_REFLECT_CLASS(class LIB_EXPORTName)
	- 需要动态创建的时候使用CReflectObject::Create("...");
* @code
	//Person.h
	class Person:public CReflectObject
	{
		DECLARE_REFLECT_CLASS(Person)
	};
	//Person.cpp
	IMPLEMENT_REFLECT_CLASS(Person)
	//main.cpp
	Person* p=(Person*)CReflectObject::Create("Person");
	//或者
	Person* p=CReflectObject::CreateEx<Person>("Person");
* @endcode
*/
class PATTERN_EXPORT CReflectObject
{
public:
	///构造函数
	CReflectObject();

	///析构函数
	virtual ~CReflectObject();

	/**
	 * @fn       static CReflectObject* Create(const std::string& strClassName,bool bCaseSensitive)   
	 * @brief    根据字符串创建对象,创建的对象需要由使用者来删除
	 * @param    const std::string & strClassName    类名
	 * @param    bool bCaseSensitive    类名字是否大小写敏感
	 * @return   CReflectObject*  CReflectObject类型的对象指针
	*/
	static CReflectObject* Create(const char* strClassName,bool bCaseSensitive = true);

	/**
	 * @fn       static T* CreateEx(const std::string& strClassName,bool bCaseSensitive)   
	 * @brief    获取全局对象，不需要使用者来删除，该对象一直存在，主要用于不想频繁new，只是想获取对象的动态信息
	 * @param    const std::string& strClassName  类的名字
	 * @param    bool bCaseSensitive    类名字是否大小写敏感
	 * @return   T*  返回T类型的全局对象指针
	*/
	template<typename T>
	static T* CreateEx(const std::string& strClassName,bool bCaseSensitive = true)
	{
		CReflectObject* pReflectObject = Create(strClassName);
		if(NULL == pReflectObject)
		{
			return NULL;
		}

		T* pObject = dynamic_cast<T*>(pReflectObject);
		if(NULL == pObject)
		{
			delete pReflectObject;
			return NULL;
		}

		return pObject;
	}

	/**
	 * @fn         bool IsType()   
	 * @brief      判断该对象是否是T类型
	 * @return   bool   如果是T类型，则返回true，否则返回false
	*/
	template<typename T>
	bool IsType()
	{
		if(NULL == dynamic_cast<T*>(this))
		{
			return false;
		}

		return true;
	}

	/**
	 * @fn         T* ToType()   
	 * @brief      将该对象转换为T类型指针
	 * @return   T*  T类型指针
	*/
	template<typename T>
	T* ToType()
	{
		return dynamic_cast<T*>(this);
	}

	/**
	 * @fn       bool IsClass(const std::string& strClassName,bool bCaseSensitive)   
	 * @brief    判断该对象是否是strClassName的子类型
	 * @param    const std::string& strClassName
	 * @param    bool bCaseSensitive    类名字是否大小写敏感
	 * @return   bool   如果是则返回true，否则返回false
	*/
	bool IsClass(const char* strClassName,bool bCaseSensitive = true);

	/**
	 * @fn       bool IsClass()   
	 * @brief    判断是否为某个类型
	 * @return   bool   是某个类型则返回true，否则返回false
	*/
	template<typename T>
	bool IsClass()
	{
		if(T::GetClassNameStatic() == GetClassName())
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	/**
	 * @fn       virtual CClassRep* GetClassRep()   
	 * @brief    获取CClassRep*指针，在宏里面实现
	 * @return   CClassRep*   CClassRep*指针
	*/
	virtual CClassRep* GetClassRep() const = 0;

	/**
	 * @fn       virtual std::string GetClassName()   
	 * @brief    获取该对象所对应的类的名字，通过宏来实现，返回的可能是类的别名，真实类名通过GetRealClassName返回
	 * @return   std::string  类名的字符串，可能是类的别名
	*/
	virtual std::string GetClassName() = 0;

	/**
	 * @fn       std::string GetRealClassName()
	 * @brief    获取真实的类名字
	 * @return   std::string  真实的类名字
	*/
	virtual std::string GetRealClassName() = 0;

	/**
	 * @fn       void RegisterManualCreateReflectObjectCallback(ManualCreateReflectObjectCallback pCallback)
	 * @brief    注册创建反射对象失败时的回调函数，该回调函数中，用户可以返回一个CReflectObject对象指针
	 * @param    ManualCreateReflectObjectCallback  pCallback  回调函数
	 * @return   void  
	*/
	static void RegisterManualCreateReflectObjectCallback(ManualCreateReflectObjectCallback pCallback);

private:
	static ManualCreateReflectObjectCallback s_pManualCreateReflectObjectCallback;	///<当创建反射对象失败时的回调函数
};

/**
 * @fn      DECLARE_REFLECT_CLASS(className)
 * @brief   具有创建型反射机制类的申明，需要在头文件的内中任意位置申明
 * @param	className  类的名字
*/
#define DECLARE_REFLECT_CLASS(className) \
public:\
	friend class CClassRepInstance< className >;\
	static CClassRepInstance< className > s_DynamicClassRep;\
	virtual CClassRep* GetClassRep() const\
	{\
		return &className::s_DynamicClassRep;\
	}\
	static std::string GetClassNameStatic()\
	{\
		return s_DynamicClassRep.GetClassName();\
	}\
	virtual std::string GetClassName()\
	{\
		return s_DynamicClassRep.GetClassName();\
	}\
	virtual std::string GetRealClassName()\
	{\
		return s_DynamicClassRep.GetRealClassName();\
	}\
	virtual std::string GetDisplayName()	\
	{\
		return s_DynamicClassRep.GetDisplayName();	\
	}\
	virtual void SetDisplayName(const std::string& strClassName)\
	{\
		s_DynamicClassRep.SetDisplayName(strClassName);\
	}

/**
 * @fn      IMPLEMENT_REFLECT_CLASS(className)
 * @brief   具有创建型反射机制类的实现，需要在cpp文件中申明
 * @param	className  类的名字
*/
#define IMPLEMENT_REFLECT_CLASS(className) \
	CClassRepInstance< className > className::s_DynamicClassRep(#className, #className);

/**
 * @fn      IMPLEMENT_REFLECT_CLASS_WITH_REGISTER_CALLBACK_1(className, callbackName, info)
 * @brief   具有创建型反射机制类的实现，需要在cpp文件中申明，并传入了创建的回调函数和参数
 * @param	className  类的名字
 * @param   callbackName  回调函数
 * @param   info  传入的参数，回调函数的第二个参数
*/
#define IMPLEMENT_REFLECT_CLASS_WITH_REGISTER_CALLBACK_1(className, callbackName, info) \
	CClassRepInstance< className > className::s_DynamicClassRep(#className, #className, callbackName, (void*)&info);

/**
 * @fn      IMPLEMENT_REFLECT_CLASS_WITH_REGISTER_CALLBACK_2(className, callbackName, info)
 * @brief   具有创建型反射机制类的实现，需要在cpp文件中申明，并传入了创建的回调函数和参数
 * @param	className  类的名字
 * @param   callbackName  回调函数
 * @param   info  传入的参数，回调函数的第二个参数
*/
#define IMPLEMENT_REFLECT_CLASS_WITH_REGISTER_CALLBACK_2(className, callbackName, info) \
	CClassRepInstance< className > className::s_DynamicClassRep(#className, #className, callbackName, (void*)info);

/**
 * @fn      IMPLEMENT_REFLECT_CLASS_SPECIFY_NAME(className, name)
 * @brief   具有创建型反射机制类的实现，指定类名的同时，指定类的别名，需要在cpp文件中申明
 * @param	className  类的名字
 * @param   name   类的别名
*/
#define IMPLEMENT_REFLECT_CLASS_SPECIFY_NAME(className, name)\
	CClassRepInstance< className > className::s_DynamicClassRep(#name, #className);

/**
 * @fn      IMPLEMENT_REFLECT_CLASS_SPECIFY_NAME_EX(className, name)
 * @brief   具有创建型反射机制类的实现，指定类名的同时，指定类的别名，需要在cpp文件中申明
 * @param	className  类的名字
 * @param   name   类的别名，传入字符串变量
*/
#define IMPLEMENT_REFLECT_CLASS_SPECIFY_NAME_EX(className, name)\
	CClassRepInstance< className > className::s_DynamicClassRep(name, #className);

/**
 * @fn      IMPLEMENT_REFLECT_CLASS_SPECIFY_NAME_WITH_REGISTER_CALLBACK_1(className, callbackName, info)
 * @brief   具有创建型反射机制类的实现，需要在cpp文件中申明，并传入了创建的回调函数和参数
 * @param	className  类的名字
 * @param   name   类的别名
 * @param   callbackName  回调函数
 * @param   info  传入的参数，回调函数的第二个参数
*/
#define IMPLEMENT_REFLECT_CLASS_SPECIFY_NAME_WITH_REGISTER_CALLBACK_1(className, name, callbackName, info)\
	CClassRepInstance< className > className::s_DynamicClassRep(#name, #className, callbackName, (void*)&info);

/**
 * @fn      IMPLEMENT_REFLECT_CLASS_SPECIFY_NAME_WITH_REGISTER_CALLBACK_2(className, callbackName, info)
 * @brief   具有创建型反射机制类的实现，需要在cpp文件中申明，并传入了创建的回调函数和参数
 * @param	className  类的名字
 * @param   name   类的别名
 * @param   callbackName  回调函数
 * @param   info  传入的参数，回调函数的第二个参数
*/
#define IMPLEMENT_REFLECT_CLASS_SPECIFY_NAME_WITH_REGISTER_CALLBACK_2(className, name, callbackName, info)\
	CClassRepInstance< className > className::s_DynamicClassRep(#name, #className, callbackName, (void*)info);

/**
 * @fn      IMPLEMENT_REFLECT_TEMPLATE_CLASS(className, type)
 * @brief   具有创建型反射机制类的实现，模板类型的反射，需要在cpp文件中申明
 * @param	className  类的名字
 * @param   type   模板类型
*/
#define IMPLEMENT_REFLECT_TEMPLATE_CLASS(className, type)\
	CClassRepInstance< className< type > > className< type >::s_DynamicClassRep( std::string(std::string(#className)+"<"+std::string(#type)+">").c_str(), std::string(std::string(#className)+"<"+std::string(#type)+">").c_str() );

#endif
