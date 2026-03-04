#pragma once

#include <map>

class CStringTable
{
    CString m_name;
	std::map<CString, CString> m_stringMap;

public:
	CStringTable();
	CStringTable(LPCTSTR name);
	~CStringTable();

	CString GetName();
	int GetCount();

	void Load(LPCTSTR fileName);
	BOOL GetString(LPCTSTR searchKey, CString& value);
};

