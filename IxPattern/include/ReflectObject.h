/**
* @file      ReflectObject.h
* @brief     C++�����ͷ������ģʽ
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
* @brief ����������
* @note  
*/
class PATTERN_EXPORT CClassRep
{
	friend class CReflectObject;
public:
	///���캯��
	CClassRep(const char* szClassName,const char* szRealClassName);

	///��������
	virtual ~CClassRep();

public:
	/**
	 * @fn       virtual CReflectObject* Create(const char* szClassName,bool bCaseSensitive)   
	 * @brief    �������������������
	 * @param    const char * szClassName    ����
	 * @param    bool bCaseSensitive   �����Ƿ��Сд����
	 * @return   CReflectObject*     �������ָ��
	*/
	static CReflectObject* Create(const char* szClassName,bool bCaseSensitive = true);

	/**
	 * @fn       std::string GetClassName()   
	 * @brief    ��ȡ����
	 * @return   std::string   ����
	*/
	const char* GetClassName();

	/**
	 * @fn       std::string GetRealClassName()
	 * @brief    ��ȡ��ʵ������
	 * @return   std::string  ��ʵ������
	*/
	const char* GetRealClassName();

	/**
	 * @fn       void SetDisplayName(const std::string& strClassName)   
	 * @brief    ������ʾ������
	 * @param    const std::string& strClassName   ��ʾ������
	 * @return   void       
	*/
	void SetDisplayName(const char* strClassName);

	/**
	 * @fn       std::string GetDisplayName()   
	 * @brief    ��ȡ��ʾ������
	 * @return   std::string    ��ʾ������
	*/
	const char* GetDisplayName();

protected:
	/**
	 * @fn       virtual CReflectObject* OnCreate()   
	 * @brief    �����������ӿ�
	 * @return   CReflectObject*    �������ָ��
	*/
	virtual CReflectObject* OnCreate() = 0;

private:
	//std::string m_strClassName;		///<�����֣����ܲ�����ʵ���������������
	char m_strClassName[256];
	//std::string m_strRealClassName;	///<����ʵ������
	char m_strRealClassName[256];
	//std::string m_strDisplayName;	///<��ʾ������
	char m_strDisplayName[256];
};

/**
* @class CClassRepManager
* @brief ���������������
* @note  
*/
class PATTERN_EXPORT CClassRepManager
{
	DECLARE_UNMANGED_SINGLETON(CClassRepManager)
public:
	/**
	 * @fn       CClassRep* Get(const char* szClassName,bool bCaseSensitive)   
	 * @brief    ����������ȡ����������ָ��
	 * @param    const char * szClassName     ����
	 * @param    bool bCaseSensitive    �������Ƿ��Сд����
	 * @return   CClassRep*    ����������ָ��
	*/
	CClassRep* Get(const char* szClassName,bool bCaseSensitive = true);

	/**
	 * @fn       bool Add(CClassRep* pClassRep)   
	 * @brief    ����һ������������ָ�룬����Ѿ�����һ����������ô��ӻ�ʧ��
	 * @param    CClassRep * pClassRep    ��ӵ�������ָ��
	 * @return   bool   �ɹ�����true��ʧ�ܷ���false
	*/
	bool Add(CClassRep* pClassRep);

	/**
	 * @fn       bool Remove(CClassRep* pClassRep)   
	 * @brief    ɾ������������
	 * @param    CClassRep * pClassRep    ����������ָ��
	 * @return   bool    �ɹ�����true��ʧ�ܷ���false
	*/
	bool Remove(CClassRep* pClassRep);

	/**
	 * @fn       int GetSize()   
	 * @brief    ��ȡ��ǰ����ĸ���
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

///�������ע��ص�����
typedef void (*ReflectObjectRegisterCallback)(const char* szClassName, void* pInfo);

///�������������ʧ��ʱ�����û���һ���ֶ���������Ľӿ�
typedef CReflectObject* (*ManualCreateReflectObjectCallback)(const char* szClassName, bool bCaseSensitive);

/**
* @class CClassRepInstance
* @brief ������ģ����
* @note  
*/
template <class T>
class CClassRepInstance : public CClassRep
{
public:
	/**
	 * @fn       CClassRepInstance(const char* szClassName, const char* szRealClassName, ReflectObjectRegisterCallback pRegisterCallback = NULL, void* pInfo = NULL)   
	 * @brief    ���캯��   
	 * @param    const char * szClassName    ����
	 * @param    const char * szRealClassName    ��ʵ������
	 * @param    ReflectObjectRegisterCallback pRegisterCallback    ע��ص�����
	 * @param    void * pInfo    ע��ص���������Ĳ���
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
			printf("%s �������ظ�\n",szClassName);
		}
	}

	///��������
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
	 * @brief    ����һ������
	 * @return   CReflectObject*     �����Ķ���
	*/
	virtual CReflectObject* OnCreate()
	{
		return new T;
	}

private:
	ReflectObjectRegisterCallback m_pRegisterCallback;	///<��������ʱע��Ļص�����
	void* m_pInfo;	///<��������ʱ��Ҫ���ص��������ݵ�ֵ
};

//////////////////////////////////////////////////////////////////////////
/**
* @class CReflectObject
* @brief ���д����ͷ�����Ƶ���Ļ���
* @note ʹ�÷���
	- ���ȹ��м̳и���
	- ����Ҫ��̬��������ͷh������DECLARE_REFLECT_CLASS(class LIB_EXPORTName)
	- ����Ҫ��̬����������cpp������IMPLEMENT_REFLECT_CLASS(class LIB_EXPORTName)
	- ��Ҫ��̬������ʱ��ʹ��CReflectObject::Create("...");
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
	//����
	Person* p=CReflectObject::CreateEx<Person>("Person");
* @endcode
*/
class PATTERN_EXPORT CReflectObject
{
public:
	///���캯��
	CReflectObject();

	///��������
	virtual ~CReflectObject();

	/**
	 * @fn       static CReflectObject* Create(const std::string& strClassName,bool bCaseSensitive)   
	 * @brief    �����ַ�����������,�����Ķ�����Ҫ��ʹ������ɾ��
	 * @param    const std::string & strClassName    ����
	 * @param    bool bCaseSensitive    �������Ƿ��Сд����
	 * @return   CReflectObject*  CReflectObject���͵Ķ���ָ��
	*/
	static CReflectObject* Create(const char* strClassName,bool bCaseSensitive = true);

	/**
	 * @fn       static T* CreateEx(const std::string& strClassName,bool bCaseSensitive)   
	 * @brief    ��ȡȫ�ֶ��󣬲���Ҫʹ������ɾ�����ö���һֱ���ڣ���Ҫ���ڲ���Ƶ��new��ֻ�����ȡ����Ķ�̬��Ϣ
	 * @param    const std::string& strClassName  �������
	 * @param    bool bCaseSensitive    �������Ƿ��Сд����
	 * @return   T*  ����T���͵�ȫ�ֶ���ָ��
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
	 * @brief      �жϸö����Ƿ���T����
	 * @return   bool   �����T���ͣ��򷵻�true�����򷵻�false
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
	 * @brief      ���ö���ת��ΪT����ָ��
	 * @return   T*  T����ָ��
	*/
	template<typename T>
	T* ToType()
	{
		return dynamic_cast<T*>(this);
	}

	/**
	 * @fn       bool IsClass(const std::string& strClassName,bool bCaseSensitive)   
	 * @brief    �жϸö����Ƿ���strClassName��������
	 * @param    const std::string& strClassName
	 * @param    bool bCaseSensitive    �������Ƿ��Сд����
	 * @return   bool   ������򷵻�true�����򷵻�false
	*/
	bool IsClass(const char* strClassName,bool bCaseSensitive = true);

	/**
	 * @fn       bool IsClass()   
	 * @brief    �ж��Ƿ�Ϊĳ������
	 * @return   bool   ��ĳ�������򷵻�true�����򷵻�false
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
	 * @brief    ��ȡCClassRep*ָ�룬�ں�����ʵ��
	 * @return   CClassRep*   CClassRep*ָ��
	*/
	virtual CClassRep* GetClassRep() const = 0;

	/**
	 * @fn       virtual std::string GetClassName()   
	 * @brief    ��ȡ�ö�������Ӧ��������֣�ͨ������ʵ�֣����صĿ�������ı�������ʵ����ͨ��GetRealClassName����
	 * @return   std::string  �������ַ�������������ı���
	*/
	virtual std::string GetClassName() = 0;

	/**
	 * @fn       std::string GetRealClassName()
	 * @brief    ��ȡ��ʵ��������
	 * @return   std::string  ��ʵ��������
	*/
	virtual std::string GetRealClassName() = 0;

	/**
	 * @fn       void RegisterManualCreateReflectObjectCallback(ManualCreateReflectObjectCallback pCallback)
	 * @brief    ע�ᴴ���������ʧ��ʱ�Ļص��������ûص������У��û����Է���һ��CReflectObject����ָ��
	 * @param    ManualCreateReflectObjectCallback  pCallback  �ص�����
	 * @return   void  
	*/
	static void RegisterManualCreateReflectObjectCallback(ManualCreateReflectObjectCallback pCallback);

private:
	static ManualCreateReflectObjectCallback s_pManualCreateReflectObjectCallback;	///<�������������ʧ��ʱ�Ļص�����
};

/**
 * @fn      DECLARE_REFLECT_CLASS(className)
 * @brief   ���д����ͷ�����������������Ҫ��ͷ�ļ�����������λ������
 * @param	className  �������
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
 * @brief   ���д����ͷ���������ʵ�֣���Ҫ��cpp�ļ�������
 * @param	className  �������
*/
#define IMPLEMENT_REFLECT_CLASS(className) \
	CClassRepInstance< className > className::s_DynamicClassRep(#className, #className);

/**
 * @fn      IMPLEMENT_REFLECT_CLASS_WITH_REGISTER_CALLBACK_1(className, callbackName, info)
 * @brief   ���д����ͷ���������ʵ�֣���Ҫ��cpp�ļ����������������˴����Ļص������Ͳ���
 * @param	className  �������
 * @param   callbackName  �ص�����
 * @param   info  ����Ĳ������ص������ĵڶ�������
*/
#define IMPLEMENT_REFLECT_CLASS_WITH_REGISTER_CALLBACK_1(className, callbackName, info) \
	CClassRepInstance< className > className::s_DynamicClassRep(#className, #className, callbackName, (void*)&info);

/**
 * @fn      IMPLEMENT_REFLECT_CLASS_WITH_REGISTER_CALLBACK_2(className, callbackName, info)
 * @brief   ���д����ͷ���������ʵ�֣���Ҫ��cpp�ļ����������������˴����Ļص������Ͳ���
 * @param	className  �������
 * @param   callbackName  �ص�����
 * @param   info  ����Ĳ������ص������ĵڶ�������
*/
#define IMPLEMENT_REFLECT_CLASS_WITH_REGISTER_CALLBACK_2(className, callbackName, info) \
	CClassRepInstance< className > className::s_DynamicClassRep(#className, #className, callbackName, (void*)info);

/**
 * @fn      IMPLEMENT_REFLECT_CLASS_SPECIFY_NAME(className, name)
 * @brief   ���д����ͷ���������ʵ�֣�ָ��������ͬʱ��ָ����ı�������Ҫ��cpp�ļ�������
 * @param	className  �������
 * @param   name   ��ı���
*/
#define IMPLEMENT_REFLECT_CLASS_SPECIFY_NAME(className, name)\
	CClassRepInstance< className > className::s_DynamicClassRep(#name, #className);

/**
 * @fn      IMPLEMENT_REFLECT_CLASS_SPECIFY_NAME_EX(className, name)
 * @brief   ���д����ͷ���������ʵ�֣�ָ��������ͬʱ��ָ����ı�������Ҫ��cpp�ļ�������
 * @param	className  �������
 * @param   name   ��ı����������ַ�������
*/
#define IMPLEMENT_REFLECT_CLASS_SPECIFY_NAME_EX(className, name)\
	CClassRepInstance< className > className::s_DynamicClassRep(name, #className);

/**
 * @fn      IMPLEMENT_REFLECT_CLASS_SPECIFY_NAME_WITH_REGISTER_CALLBACK_1(className, callbackName, info)
 * @brief   ���д����ͷ���������ʵ�֣���Ҫ��cpp�ļ����������������˴����Ļص������Ͳ���
 * @param	className  �������
 * @param   name   ��ı���
 * @param   callbackName  �ص�����
 * @param   info  ����Ĳ������ص������ĵڶ�������
*/
#define IMPLEMENT_REFLECT_CLASS_SPECIFY_NAME_WITH_REGISTER_CALLBACK_1(className, name, callbackName, info)\
	CClassRepInstance< className > className::s_DynamicClassRep(#name, #className, callbackName, (void*)&info);

/**
 * @fn      IMPLEMENT_REFLECT_CLASS_SPECIFY_NAME_WITH_REGISTER_CALLBACK_2(className, callbackName, info)
 * @brief   ���д����ͷ���������ʵ�֣���Ҫ��cpp�ļ����������������˴����Ļص������Ͳ���
 * @param	className  �������
 * @param   name   ��ı���
 * @param   callbackName  �ص�����
 * @param   info  ����Ĳ������ص������ĵڶ�������
*/
#define IMPLEMENT_REFLECT_CLASS_SPECIFY_NAME_WITH_REGISTER_CALLBACK_2(className, name, callbackName, info)\
	CClassRepInstance< className > className::s_DynamicClassRep(#name, #className, callbackName, (void*)info);

/**
 * @fn      IMPLEMENT_REFLECT_TEMPLATE_CLASS(className, type)
 * @brief   ���д����ͷ���������ʵ�֣�ģ�����͵ķ��䣬��Ҫ��cpp�ļ�������
 * @param	className  �������
 * @param   type   ģ������
*/
#define IMPLEMENT_REFLECT_TEMPLATE_CLASS(className, type)\
	CClassRepInstance< className< type > > className< type >::s_DynamicClassRep( std::string(std::string(#className)+"<"+std::string(#type)+">").c_str(), std::string(std::string(#className)+"<"+std::string(#type)+">").c_str() );

#endif
