#include "RuntimeObject.h"

RegisterClassObj::RegisterClassObj( CSwtcRuntimeClass *ptype, const char* aliasename, CSwtcRuntimeClass *pUpLevelType )
{

}

RegisterClassObj::~RegisterClassObj()
{

}

CRuntimeObject::CRuntimeObject()
{

}

CRuntimeObject::~CRuntimeObject()
{

}

CSwtcRuntimeClass* CRuntimeObject::GetRuntimeClass() const
{
	return NULL;
}

std::string CRuntimeObject::GetObjectType()
{
	return "";
}

std::string CRuntimeObject::GetObjectAliaseType()
{
	return "";
}

bool CRuntimeObject::IsKindOf( const CSwtcRuntimeClass* pClass ) const
{
	return false;
}

bool CRuntimeObject::IsClass( const std::string& strClassName ) const
{
	return false;
}

CRuntimeObject* CRuntimeObject::Create( const std::string& strClassName )
{
	return false;
}
