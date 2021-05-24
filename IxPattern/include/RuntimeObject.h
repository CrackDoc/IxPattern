/**
* @file      RuntimeObject.h
* @brief     支持反射机制的基础类（采用类似MFC的RuntimeClass机制）
*          
* @note      
*          
* @author    
* @date      2015-05-15
* @version   1.0.0.0
* @CopyRight IxLab
*/

#ifndef _RUNTIME_OBJECT_H_
#define _RUNTIME_OBJECT_H_

#include "IxPatternExport.h"
#include <string>

class CRuntimeObject;

struct PATTERN_EXPORT CSwtcRuntimeClass
{
	char* m_lpszClassName;
	char* m_lpszClassNameAliase;
	int m_nObjectSize;
	unsigned short m_wSchema;
	CRuntimeObject* (* m_pfnCreateObject)();
	CSwtcRuntimeClass* m_pBaseClass;
	CSwtcRuntimeClass* m_pUpLevelClass; //上层类型，用于限制设备类型之间关系

	static CSwtcRuntimeClass* pFirstClass;
	CSwtcRuntimeClass* m_pNextClass;


	CRuntimeObject* CreateObject();
	void     RegisterClassAliase(const char* szAliaseName);
	static CSwtcRuntimeClass* Load(const char* szClassName);
	bool IsDerivedFrom(CSwtcRuntimeClass* type);
	static 	std::string GetDebugString();
};

struct PATTERN_EXPORT SWTC_AFX_CLASSINIT
{
	SWTC_AFX_CLASSINIT(CSwtcRuntimeClass* pNewClass);
};

#define SWT_RUNTIME_CLASS(class_name) \
	(&class_name::class##class_name)

#define SWT_DECLARE_DYNAMIC(class_name) \
public: \
	static CSwtcRuntimeClass class##class_name; \
	virtual CSwtcRuntimeClass* GetRuntimeClass() const; 

#define SWT_DECLARE_DYNCREATE(class_name) \
	SWT_DECLARE_DYNAMIC(class_name) \
	static CRuntimeObject* CreateObject();
	

#define _SWT_IMPLEMENT_RUNTIMECLASS(class_name, base_class_name, aliase_name, wSchema, pfnNew) \
	static char _lpsz##class_name[64] = #class_name; \
	static char _lpsz##class_name##aliase[64] = #aliase_name; \
	CSwtcRuntimeClass class_name::class##class_name = { \
	_lpsz##class_name, _lpsz##class_name##aliase, sizeof(class_name), wSchema, pfnNew, \
	SWT_RUNTIME_CLASS(base_class_name), NULL, NULL}; \
	static SWTC_AFX_CLASSINIT _init_##class_name(&class_name::class##class_name); \
	CSwtcRuntimeClass* class_name::GetRuntimeClass() const \
	{ \
		return &class_name::class##class_name; \
	}

#define SWT_IMPLEMENT_DYNAMIC(class_name, base_class_name, aliase_name) \
	_SWT_IMPLEMENT_RUNTIMECLASS(class_name, base_class_name, aliase_name, 0xFFFF, NULL)

#define SWT_IMPLEMENT_DYNCREATE(class_name, base_class_name, aliase_name) \
	CRuntimeObject* class_name::CreateObject() \
	{ \
		return new class_name; \
	} \
	_SWT_IMPLEMENT_RUNTIMECLASS(class_name, base_class_name, aliase_name, 0xFFFF, \
	class_name::CreateObject)

//给类注册别名.利用静态对象实现自动注册某类型的别名和上层类型
//example:
//static RegisterClassObj regObj(SWT_RUNTIME_CLASS(MyClass), "MY_CLASS");
class PATTERN_EXPORT RegisterClassObj
{
public:
	RegisterClassObj(CSwtcRuntimeClass *ptype, const char* aliasename, CSwtcRuntimeClass *pUpLevelType);
	~RegisterClassObj();
};

//类似MFC的CObject
class PATTERN_EXPORT CRuntimeObject
{
public:
	CRuntimeObject();
	virtual ~CRuntimeObject();

	virtual CSwtcRuntimeClass* GetRuntimeClass() const;

	
	/**
	 * @fn       std::string GetObjectType()
	 * @brief    获得对象类型名称
	 * @return std::string
	*/
	virtual std::string GetObjectType() ;

	/**
	 * @fn       std::string GetObjectAliaseType()
	 * @brief    获得对象类型别名
	 * @return std::string
	*/
	virtual std::string GetObjectAliaseType() ;

	bool IsKindOf(const CSwtcRuntimeClass* pClass) const;

	bool IsClass(const std::string& strClassName) const;

	static CRuntimeObject* Create(const std::string& strClassName);

	template<typename T>
	static T* CreateEx(const std::string& strClassName)
	{
		CRuntimeObject* pRuntimeObject = Create(strClassName);
		if(pRuntimeObject==NULL)
		{
			return NULL;
		}

		T* pObject = dynamic_cast<T*>(pRuntimeObject);
		if(pObject==NULL)
		{
			return NULL;
		}

		return pObject;
	}

	template<typename T>
	bool IsType()
	{
		T* pObject = dynamic_cast<T*>(this);
		if(pObject == NULL)
		{
			return NULL;
		}

		return pObject;
	}

public:
	static CSwtcRuntimeClass classCRuntimeObject;
};

#define REGISTER_CLASS_TYPE(rc, name, parentrc) static RegisterClassObj reg##name##Obj(rc, #name, parentrc);

#endif
