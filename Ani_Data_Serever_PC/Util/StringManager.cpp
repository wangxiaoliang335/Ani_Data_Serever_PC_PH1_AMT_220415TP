#include "stdafx.h"
#include "StringManager.h"

#include "StringTable.h"
#include "StringSupport.h"



std::vector<CStringTable*> CStringManager::stringTableList;
std::vector<IStringChangeListener*> CStringManager::stringChangeListenerList;

void CStringManager::AddListener(IStringChangeListener* pListener)
{
	BOOL IsAdded = FALSE;

	if (stringChangeListenerList.size() > 0)
	{
		for (IStringChangeListener* pStringChangeListener : stringChangeListenerList)
			if (pStringChangeListener == pListener)
				IsAdded = TRUE;
	}

	if (IsAdded == FALSE)
		stringChangeListenerList.push_back(pListener);
}

CString CStringManager::GetString(LPCTSTR tableName, LPCTSTR searchKey)
{
	CString value;
	if (GetStringTable(tableName)->GetString(searchKey, value) == TRUE)
		return value;

	return searchKey;
}

CString CStringManager::GetString(LPCTSTR searchKey)
{
	CString value;

    for (CStringTable* pStringTable : stringTableList)
    {
        if (pStringTable->GetString(searchKey, value) == TRUE)
            return value;
    }
    return searchKey;
}

CStringTable* CStringManager::GetStringTable(LPCTSTR tableName)
{
	for (CStringTable* pStringTable : stringTableList)
	{
		if (pStringTable->GetName() == tableName)
            return pStringTable;
    }

    return new CStringTable();
}

void CStringManager::AddStringTable(CStringTable* pStringTable)
{
    stringTableList.push_back(pStringTable);
}

//void CStringManager::UpdateStringTable()
//{
//	ClearStringTable();
//
//	CStringTable* pStringTable = new CStringTable();
//
//	CString stringTableFile = CStringSupport::FormatString(_T("%sgamma_%s.st"), SYSTEM_DATA_PATH, theApp.m_pDataSys->m_langExt);
//	
//	pStringTable->Load(stringTableFile);
//
//	stringTableList.push_back(pStringTable);
//
//	if (stringChangeListenerList.size() > 0)
//	{
//		for (IStringChangeListener* pStringChangeListener : stringChangeListenerList)
//			pStringChangeListener->StringChanged();
//	}
//}

void CStringManager::ClearStringTable()
{
	for(int i=0; i<stringTableList.size(); i++)
		delete stringTableList[i];

	stringTableList.clear();
}


