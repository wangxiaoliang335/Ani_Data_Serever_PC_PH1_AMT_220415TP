#include "stdafx.h"
#include "StringTable.h"

#include "FileSupport.h"
#include <msxml6.h>
#include "StringSupport.h"

CStringTable::CStringTable()
{
}

CStringTable::~CStringTable()
{
	m_stringMap.clear();
}

CStringTable::CStringTable(LPCTSTR name)
{
	m_name = name;
}

CString CStringTable::GetName()
{
    return m_name;
}

int CStringTable::GetCount()
{
    return (int)m_stringMap.size();
}

void CStringTable::Load(LPCTSTR fileName)
{
	if (CFileSupport::IsFile(fileName) == FALSE)
		return;

	FILE *fStream;
	errno_t e = _tfopen_s(&fStream, fileName, _T("rt,ccs=UTF-8"));
	if (e != 0)
		return;

	CStdioFile file(fStream);  // open the file from this stream

	CString oneLine;
	while(file.ReadString(oneLine))
	{
		CStringArray tokenArray;
		CStringSupport::GetTokenArray(oneLine, _T('='), tokenArray);

		if (tokenArray.GetCount() == 2)
			m_stringMap.insert(std::pair<CString, CString>(tokenArray[0], tokenArray[1]));
    }
}

BOOL CStringTable::GetString(LPCTSTR searchKey, CString& value)
{
	std::map<CString, CString>::iterator itrMap = m_stringMap.find(searchKey);
    if (itrMap != m_stringMap.end())
    {
		value = (*itrMap).second;
        return TRUE;
    }

    return FALSE;
}
