#include "CleanupObjectManager.h"
#include <algorithm>
#include <vector>

CCleanupObjectManager* CCleanupObjectManager::s_pInstance = NULL;


CCleanupObjectManager* CCleanupObjectManager::Instance()
{
	if(s_pInstance == NULL)
	{
		s_pInstance = new CCleanupObjectManager;
	}
	return s_pInstance;
}

void CCleanupObjectManager::Release()
{
	if(s_pInstance != NULL)
	{
		delete s_pInstance;
		s_pInstance = NULL;
	}
}

void CCleanupObjectManager::Cleanup( CCleanupObject* pExcept /*= NULL*/ )
{
	m_cleanupObjectVectorLock.Acquire();

	std::vector<CCleanupObject*> &pCleanupObjectVector = *static_cast<std::vector<CCleanupObject*>*>(m_vecCleanupObjectVector);

	std::vector<CCleanupObject*>::iterator it0 = std::find(pCleanupObjectVector.begin(), pCleanupObjectVector.end(),pExcept);
	if(it0 != pCleanupObjectVector.end())
	{
		CCleanupObject *rpExcept = *it0;
		rpExcept->Cleanup();
		pCleanupObjectVector.erase(it0);
	}
	m_cleanupObjectVectorLock.Release();

}

void CCleanupObjectManager::AddCleanupObject( CCleanupObject* pCleanupObject )
{
	m_cleanupObjectVectorLock.Acquire();
	std::vector<CCleanupObject*>& pCleanupObjectVector = *static_cast<std::vector<CCleanupObject*>*>(m_vecCleanupObjectVector);

	pCleanupObjectVector.push_back(pCleanupObject);

	m_cleanupObjectVectorLock.Release();
}

CCleanupObjectManager::CCleanupObjectManager():
	m_vecCleanupObjectVector(new std::vector<CCleanupObject*>())
	,m_pExceptCleanupObject(nullptr)
{

}

CCleanupObjectManager::~CCleanupObjectManager()
{
	if (!m_vecCleanupObjectVector)
	{
		delete m_vecCleanupObjectVector;
		m_vecCleanupObjectVector = nullptr;
	}

}



