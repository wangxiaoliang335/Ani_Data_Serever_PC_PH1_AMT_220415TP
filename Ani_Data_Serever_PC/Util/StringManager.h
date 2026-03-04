#pragma once

#include <vector>
class CStringTable;

interface IStringChangeListener
{
	virtual void StringChanged() = 0;
};

class CStringManager
{
	static std::vector<CStringTable*> stringTableList;
	static std::vector<IStringChangeListener*> stringChangeListenerList;

public:
	static void AddListener(IStringChangeListener* pListener);
	static CString GetString(LPCTSTR tableName, LPCTSTR searchKey);
	static CString GetString(LPCTSTR searchKey);
	static CStringTable* GetStringTable(LPCTSTR tableName);
	static void AddStringTable(CStringTable* stringTable);
	static void ClearStringTable();
//	static void UpdateStringTable();
};

#define _STR(x) CStringManager::GetString(_T(x))