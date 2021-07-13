#include "ReflectObject.h"
#include <string>
#include <algorithm>
#include <stdio.h>

CClassRep::CClassRep( const char* szClassName,const char* szRealClassName )
{
	memset(m_strClassName, '\0', sizeof(char) * 256);

	memset(m_strRealClassName, '\0', sizeof(char) * 256);

	strcat_s(m_strClassName, szClassName);

	strcat_s(m_strRealClassName, szRealClassName);
}

CClassRep::~CClassRep()
{

}

CReflectObject* CClassRep::Create( const char* szClassName,bool bCaseSensitive /*= true*/ )
{
	return CReflectObject::Create(szClassName,bCaseSensitive);
}

const char* CClassRep::GetClassName()
{
	const char *pClassName  = std::strstr(m_strClassName, " ");
	if (pClassName)
	{
		size_t prev_len = (size_t)(pClassName - m_strClassName);
		if (memcmp(m_strClassName, "class", prev_len) == 0 ||
			memcmp(m_strClassName, "struct", prev_len) == 0 ||
			memcmp(m_strClassName, "enum", prev_len) == 0 ||
			memcmp(m_strClassName, "union", prev_len) == 0)
		{
			pClassName += 1;
			memset(m_strClassName, '\0', sizeof(char) * 256);
			strcat_s(m_strClassName, pClassName);
		}
	}
	return m_strClassName;
}

const char* CClassRep::GetRealClassName()
{
	const char* pClassName = std::strstr(m_strRealClassName, " ");

	if (pClassName)
	{
		size_t prev_len = (size_t)(pClassName - m_strRealClassName);
		if (memcmp(m_strRealClassName, "class", prev_len) == 0 ||
			memcmp(m_strRealClassName, "struct", prev_len) == 0 ||
			memcmp(m_strRealClassName, "enum", prev_len) == 0 ||
			memcmp(m_strRealClassName, "union", prev_len) == 0)
		{
			pClassName += 1;
			memset(m_strRealClassName, '\0', sizeof(char) * 256);
			strcat_s(m_strRealClassName, pClassName);
		}
	}
	return m_strRealClassName;
}

void CClassRep::SetDisplayName( const char* strClassName )
{
	memset(m_strDisplayName, '\0', sizeof(char) * 256);
	strcat_s(m_strDisplayName, strClassName);
}

const char* CClassRep::GetDisplayName()
{
	return m_strDisplayName;
}

CClassRep* CClassRepManager::Get( const char* szClassName,bool bCaseSensitive /*= true*/ )
{
	CClassRep *rpCClassRep = NULL;

	m_classRepListLock.Acquire();
	if(!bCaseSensitive)
	{
		std::list<CClassRep*>& lstClassRepList = *static_cast<std::list<CClassRep*>*>(m_lstClassRepList);

		std::list<CClassRep*>::iterator it0 = lstClassRepList.begin();

		std::string strClassUpperName = szClassName;
		std::transform(strClassUpperName.begin(), strClassUpperName.end(), strClassUpperName.begin(), ::toupper);
		for(;it0 != lstClassRepList.end();++it0)
		{
			CClassRep *pCClassRep = *it0;
			std::string realName = pCClassRep->GetRealClassName();
			std::transform(realName.begin(), realName.end(), realName.begin(), ::toupper);
			if(std::strcmp(realName.c_str(),strClassUpperName.c_str()) == 0)
			{
				rpCClassRep = pCClassRep;
				break;
			}
		}
	}
	else
	{
		std::list<CClassRep*>& lstClassRepList = *static_cast<std::list<CClassRep*>*>(m_lstClassRepList);

		std::list<CClassRep*>::iterator it0 = lstClassRepList.begin();

		std::string strClassUpperName = szClassName;
		for(;it0 != lstClassRepList.end();++it0)
		{
			CClassRep *pCClassRep = *it0;
			std::string realName = pCClassRep->GetRealClassName();
			if(std::strcmp(realName.c_str(),strClassUpperName.c_str()) == 0)
			{
				rpCClassRep = pCClassRep;
				break;
			}
		}
	}
	m_classRepListLock.Release();
	return rpCClassRep;
}

bool CClassRepManager::Add( CClassRep* pClassRep )
{
	m_classRepListLock.Acquire();

	std::list<CClassRep*>& lstClassRepList = *static_cast<std::list<CClassRep*>*>(m_lstClassRepList);
	lstClassRepList.push_back(pClassRep);
	m_classRepListLock.Release();
	return true; 
}
bool CClassRepManager::Remove( CClassRep* pClassRep )
{
	m_classRepListLock.Acquire();
	std::list<CClassRep*>& lstClassRepList = *static_cast<std::list<CClassRep*>*>(m_lstClassRepList);

	std::list<CClassRep*>::iterator it0 = std::find(lstClassRepList.begin(), lstClassRepList.end(), pClassRep);


	if(it0 == lstClassRepList.end())
	{
		m_classRepListLock.Release();
		return false;
	}
	CClassRep *rpClassRep = *it0;
	delete rpClassRep;
	rpClassRep = NULL;
	lstClassRepList.remove(*it0);
	m_classRepListLock.Release();
	return true; 
}

int CClassRepManager::GetSize()
{
	int nSize = 0;
	m_classRepListLock.Acquire();
	std::list<CClassRep*>& lstClassRepList = *static_cast<std::list<CClassRep*>*>(m_lstClassRepList);
	nSize = lstClassRepList.size();
	m_classRepListLock.Release();
	return nSize;
}

CClassRepManager::CClassRepManager()
	:m_lstClassRepList(new std::list<CClassRep*>())
{

}

CClassRepManager::~CClassRepManager()
{

}
IMPLEMENT_UNMANGED_SINGLETON(CClassRepManager);

CReflectObject::CReflectObject()
{

}
CReflectObject::~CReflectObject()
{

}
CReflectObject* CReflectObject::Create( const char* szClassName,bool bCaseSensitive /*= true*/ )
{
	//如果不存在就增加
	CClassRep *pCClassRep = CClassRepManager::Instance()->Get(szClassName,bCaseSensitive);
	return pCClassRep->OnCreate();
}

bool CReflectObject::IsClass( const char* strClassName,bool bCaseSensitive /*= true*/ )
{
	std::string strClassUpperName;
	std::string strCurrentName;
	if(!bCaseSensitive)
	{
		strClassUpperName = strClassName;
		std::transform(strClassUpperName.begin(), strClassUpperName.end(), strClassUpperName.begin(), ::toupper);
		strCurrentName = GetClassName();
		std::transform(strCurrentName.begin(), strCurrentName.end(), strCurrentName.begin(), ::toupper);
		if(std::strcmp(strCurrentName.c_str(),strClassUpperName.c_str()) == 0)
		{
			return true;
		}
	}
	else
	{
		strClassUpperName = strClassName;
		strCurrentName = GetClassName();
	}
	if(std::strcmp(strCurrentName.c_str(),strClassUpperName.c_str()) == 0)
	{
		return true;
	}
	return false;
}

void CReflectObject::RegisterManualCreateReflectObjectCallback( ManualCreateReflectObjectCallback pCallback )
{
	//s_pManualCreateReflectObjectCallback = pCallback;
}
