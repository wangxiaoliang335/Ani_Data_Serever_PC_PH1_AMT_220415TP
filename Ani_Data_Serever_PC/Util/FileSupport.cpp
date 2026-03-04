#include "stdafx.h"

#include "resource.h"
#include "FileSupport.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int CALLBACK BrowseCallbackProc(HWND hwnd,UINT uMsg,LPARAM lp, LPARAM pData) 
{
	TCHAR szDir[MAX_PATH];

	switch(uMsg) {
    case BFFM_INITIALIZED: 
		_tcscpy_s ( szDir , MAX_PATH, (LPTSTR)pData );
		if ( _tcscmp ( szDir , _T("") ) == 0 ) {
			GetCurrentDirectory(sizeof(szDir)/sizeof(TCHAR), szDir);
		}
		// WParam is TRUE since you are passing a path.
		// It would be FALSE if you were passing a pidl.
		SendMessage(hwnd,BFFM_SETSELECTION,TRUE,(LPARAM)szDir);

		break;
	case BFFM_SELCHANGED: 
		// Set the status window to the currently selected path.
		if (SHGetPathFromIDList((LPITEMIDLIST) lp ,szDir)) 
			SendMessage(hwnd,BFFM_SETSTATUSTEXT,0,(LPARAM)szDir);
		break;
	default:
		break;
	}
	return 0;
}

BOOL CFileSupport::FindPath( HWND hOwner , LPCTSTR caption, LPTSTR path) 
{
	BOOL fResult = FALSE;
    BROWSEINFO bi;
    TCHAR szDir[MAX_PATH];
    LPITEMIDLIST pidl;
    LPMALLOC pMalloc;

    if (SUCCEEDED(SHGetMalloc(&pMalloc)))
	{
		ZeroMemory(&bi,sizeof(bi));
        bi.hwndOwner = hOwner;
        bi.pszDisplayName = 0;
		bi.lpszTitle = caption;
        bi.pidlRoot = 0;
        bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT;
        bi.lpfn = BrowseCallbackProc;
		bi.lParam = (LPARAM)path;

        pidl = SHBrowseForFolder(&bi);
        if (pidl) {
			if (SHGetPathFromIDList(pidl,szDir)) {
				if ( _tcscmp ( path ,  szDir ) != 0 ) {
					_tcscpy_s( path , MAX_PATH, szDir );
					fResult = TRUE;
				} 
			}
	        pMalloc->Free(pidl);
        }
		pMalloc->Release();
    }
	return fResult;
}

CString CFileSupport::FindFilePath ( BOOL isOpen , LPCTSTR defaultExt , UINT filterID )
{
	CString filter;
	filter.LoadString ( filterID );
	return FindFilePath(isOpen,defaultExt,filter);
}

CString CFileSupport::FindFilePath ( BOOL isOpen , LPCTSTR defaultExt , LPCTSTR filter )
{
	TCHAR szCurDir[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, szCurDir);
	CFileDialog dlg(isOpen, defaultExt , NULL, OFN_HIDEREADONLY , filter );
	dlg.m_ofn.lpstrInitialDir = szCurDir;
	if ( dlg.DoModal() == IDOK ) {
		return dlg.GetPathName();
	}
	return _T("");
}

BOOL CFileSupport::CreateDirectory( LPCTSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes ) 
{
	CString temp = lpPathName;
	temp.Replace(_T("\\"), _T("/"));
	CString parentFolder = temp.Left(temp.ReverseFind(_T('/')));
	if (parentFolder == _T(""))
		return FALSE;

	DWORD dwResult;

	BOOL fResult = TRUE;
	if ( (dwResult = GetFileAttributes(parentFolder)) == -1 ) {
		fResult = CFileSupport::CreateDirectory(parentFolder , lpSecurityAttributes);
	}

	if ( fResult && ( dwResult & FILE_ATTRIBUTE_DIRECTORY ) ) {
		if ( (dwResult = GetFileAttributes(lpPathName)) == -1 ) {
			return ::CreateDirectory ( lpPathName , lpSecurityAttributes ); 
		} 
	} 
	
	return FALSE;
}

BOOL CFileSupport::CopyDirectory(LPCTSTR srcDirectory, LPCTSTR destDirectory, HWND hWnd, BOOL fSilent)
{
	if (!DirectoryCheck(destDirectory))
		CreateDirectory(destDirectory, NULL);

	TCHAR        pszTo[1024] = {0};
	TCHAR        pszFrom[1024] = {0};

	_tcscpy_s(pszTo, 1024, destDirectory);
	_tcscpy_s(pszFrom , 1024, srcDirectory);

	SHFILEOPSTRUCT FileOp= {0};
	FileOp.hwnd = hWnd;
	FileOp.wFunc = FO_COPY;
	FileOp.pFrom = pszFrom;    // 원본의 위치
	FileOp.pTo = pszTo;   // 복사할 대상의 위치..
	
	//FileOp.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI |FOF_MULTIDESTFILES;

	if (fSilent)
		FileOp.fFlags = FOF_NOCONFIRMMKDIR|FOF_NOCONFIRMATION|FOF_SILENT;
	else
		FileOp.fFlags = FOF_NOCONFIRMMKDIR|FOF_NOCONFIRMATION;

	FileOp.fAnyOperationsAborted = false;
	FileOp.hNameMappings = NULL;
	FileOp.lpszProgressTitle = NULL;

	long result = SHFileOperation(&FileOp);
	return result == 0;
}

BOOL CFileSupport::Delete(LPCTSTR path, HWND hWnd)
{
	TCHAR        pszFrom[1024] = {0};

	_tcscpy_s(pszFrom, 1024, path);

	pszFrom[_tcslen(path)+1] = _T('\0');

	SHFILEOPSTRUCT fileop= {0};
	fileop.hwnd = hWnd;
	fileop.wFunc = FO_DELETE;
	fileop.pFrom = pszFrom;    // 원본의 위치
	fileop.fFlags = FOF_NOCONFIRMMKDIR|FOF_NOCONFIRMATION;
	
	fileop.fAnyOperationsAborted = false;
	fileop.hNameMappings = NULL;
	fileop.lpszProgressTitle = NULL;

	long result = SHFileOperation(&fileop);
	return result == 0;
}

BOOL CFileSupport::IsFile(LPCTSTR path, BOOL fOkAtBlank )
{
	return FileCheck(path, fOkAtBlank);
}

BOOL CFileSupport::FileCheck(LPCTSTR path, BOOL fOkAtBlank )
{
	if ( fOkAtBlank && _tcscmp(path, _T("")) == 0 ) {
		return TRUE;
	}

	DWORD result;
	if (((result=GetFileAttributes(path)) == -1 ) || (result&FILE_ATTRIBUTE_DIRECTORY) )
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CFileSupport::IsReadonly(LPCTSTR path)
{
	ASSERT(_tcscmp(path, _T("")) != 0);

	DWORD result;
	if (((result=GetFileAttributes(path)) != -1 ) && (result&FILE_ATTRIBUTE_READONLY) ) {
		return TRUE;
	}

	return FALSE;
}

BOOL CFileSupport::RemoveReadonly(LPCTSTR path)
{
	DWORD result;
	if (((result=GetFileAttributes(path)) != -1 ) && (result&FILE_ATTRIBUTE_READONLY) ) {
		result &= ~FILE_ATTRIBUTE_READONLY;
		return SetFileAttributes(path, result);
	}
	return TRUE;
}

BOOL CFileSupport::FileDate(LPCTSTR path, COleDateTime& fileDate)
{
	if (CFileSupport::FileCheck(path)) {
		HANDLE hFile = CreateFile(path, GENERIC_WRITE|GENERIC_READ , 
			FILE_SHARE_READ , NULL , OPEN_EXISTING , FILE_ATTRIBUTE_NORMAL, NULL );

		FILETIME lastWrite;
		GetFileTime ( hFile , NULL, NULL, &lastWrite);

		SYSTEMTIME stLastWrite;
		FileTimeToSystemTime(&lastWrite, &stLastWrite);

		fileDate.SetDateTime(stLastWrite.wYear, stLastWrite.wMonth, stLastWrite.wDay, 
			stLastWrite.wHour, stLastWrite.wMinute, stLastWrite.wSecond);

		CloseHandle(hFile);
	}
	return FALSE;
}


BOOL CFileSupport::IsDirectory(LPCTSTR path, BOOL fOkAtBlank )
{
	return DirectoryCheck(path, fOkAtBlank);
}

BOOL CFileSupport::DirectoryCheck(LPCTSTR path, BOOL fOkAtBlank )
{
	if ( fOkAtBlank && _tcscmp(path, _T("")) == 0 ) {
		return TRUE;
	}

	DWORD result;
	if (((result=GetFileAttributes(path)) == -1 ) || !(result&FILE_ATTRIBUTE_DIRECTORY) ) {
		return FALSE;
	}

	return TRUE;
}

CString CFileSupport::GetFileName(CString fullPath)
{
	fullPath.Replace(_T("\\"), _T("/"));
	return fullPath.Mid(fullPath.ReverseFind(_T('/'))+1);
}

CString CFileSupport::GetPathName(CString fullPath)
{
	fullPath.Replace(_T("\\"), _T("/"));
	return fullPath.Left(fullPath.ReverseFind(_T('/'))+1);
}

// srcName과 destName 뒤에 \0 이 두개 있어야 한다.
BOOL CFileSupport::Copy(LPCTSTR srcName, LPCTSTR destName, HWND hWnd)
{
	SHFILEOPSTRUCT fileop;
	fileop.hwnd = hWnd;
	fileop.wFunc = FO_COPY;
	fileop.fFlags = FOF_NOCONFIRMMKDIR|FOF_NOCONFIRMATION/*|FOF_SILENT*/;
	fileop.pFrom = srcName;
	fileop.pTo = destName;

	long result = SHFileOperation(&fileop);
	return result == 0;
}

BOOL CFileSupport::IsEmpty(LPCTSTR lpPathName)
{
	CFileFind finder;
	CString strModelname;

	if (!CFileSupport::IsDirectory(lpPathName, 0))
		return FALSE;

	long count = 0;
	CString searchPath = CFileSupport::Combine(lpPathName, _T("*.*"));
	BOOL bWorking = finder.FindFile(searchPath);
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		
		if(finder.GetFileName() != _T(".") && finder.GetFileName() != _T(".."))
		{
			count++;
		}
	}
	finder.Close();

	return count == 0;
}

void CFileSupport::DeleteAllFile(LPCTSTR lpPathName, BOOL fConfirm)
{
	CFileFind finder;
	CString strModelname;

	if (lpPathName == _T(""))
		return ;	
	
	CString strFullPath = lpPathName;
	strFullPath.TrimRight(_T("\\/ "));

	try {
		CFileSupport::DirectoryCheck(strFullPath, 0);
	} catch ( UINT ) {
//		TRACE0("The path is not directory or does not exist.");
		return;
	}

	SetFileAttributes( lpPathName, FILE_ATTRIBUTE_READONLY );	//	RD 폴더 삭제 중 리머트디버깅 적용 방지위해서...

	CString msg;
	msg.Format(_T("Would you like to delete the directory [ %s ]"), lpPathName);
	if (fConfirm && AfxMessageBox(msg, MB_YESNO) == IDCANCEL)
		return;

	CString searchPath;
	searchPath.Format(_T("%s/*.*"), lpPathName);
	BOOL bWorking = finder.FindFile(searchPath);
	while (bWorking){
		bWorking = finder.FindNextFile();
		strModelname = finder.GetFileName();
		strFullPath.Format(_T("%s/%s"), lpPathName, strModelname);
		
		if(strModelname.Right(1) != _T(".")) {
			if (finder.IsDirectory()) {
				DeleteDirectory(strFullPath, fConfirm);
			} else {
				SetFileAttributes( strFullPath, FILE_ATTRIBUTE_NORMAL );
				DeleteFile( strFullPath );
			}
		}
	}
	finder.Close();
	SetFileAttributes( lpPathName, FILE_ATTRIBUTE_NORMAL );
}
void CFileSupport::DeleteDirectory(LPCTSTR lpPathName, BOOL fConfirm)
{
	CFileFind finder;
	CString strModelname;

	if (lpPathName == _T(""))
		return ;	
	
	CString strFullPath = lpPathName;
	strFullPath.TrimRight(_T("\\/ "));

	try {
		CFileSupport::DirectoryCheck(strFullPath, 0);
	} catch ( UINT ) {
		return;
	}

	SetFileAttributes( lpPathName, FILE_ATTRIBUTE_READONLY );	//	RD 폴더 삭제 중 리머트디버깅 적용 방지위해서...

	CString msg;
	msg.Format(_T("Would you like to delete the directory [ %s ]"), lpPathName);
	if (fConfirm && AfxMessageBox(msg, MB_YESNO) == IDCANCEL)
		return;

	CString searchPath;
	searchPath.Format(_T("%s/*.*"), lpPathName);
	BOOL bWorking = finder.FindFile(searchPath);
	while (bWorking){
		bWorking = finder.FindNextFile();
		strModelname = finder.GetFileName();
		strFullPath.Format(_T("%s/%s"), lpPathName, strModelname);
		
		if(strModelname.Right(1) != _T(".")) {
			if (finder.IsDirectory()) {
				DeleteDirectory(strFullPath, fConfirm);
			} else {
				SetFileAttributes( strFullPath, FILE_ATTRIBUTE_NORMAL );
				DeleteFile( strFullPath );
			}
		}
	}
	finder.Close();
	SetFileAttributes( lpPathName, FILE_ATTRIBUTE_NORMAL );
	if (!::RemoveDirectory(lpPathName)) {
		DWORD result = GetLastError();
	}
}

void CFileSupport::GetPathAndFileName ( LPCTSTR fullPath , CString& path , CString& fileName )
{ 
	CString fullPathStr = fullPath;
	fullPathStr.Replace(_T("\\"), _T("/"));
	int pos = fullPathStr.ReverseFind(_T('/'));
	path = fullPathStr.Left ( pos );
	fileName = fullPathStr.Mid ( pos + 1 );
}

BOOL CFileSupport::ValidateName(CString name)
{
	if (name.Find(_T('\n')) != -1)
		return FALSE;
	if (name.GetLength() > 255)
		return FALSE;
	if (name.FindOneOf(_T("\\/:*?\"<>|,")) > -1)
		return FALSE;
	return TRUE;
}

BOOL CFileSupport::SafeSave(LPCTSTR tempFileName, LPCTSTR bakFileName, LPCTSTR fileName)
{
	try {
		if (!IsFile(tempFileName))
			throw 1;

		if (IsFile(bakFileName) && !::DeleteFile(bakFileName))
			throw 1;

		if (IsFile(fileName) && !::MoveFile(fileName, bakFileName))
			throw 1;

		if (!::MoveFile(tempFileName, fileName)) {
			RevertFile(bakFileName, fileName);
			throw 1;
		}
	} catch (int ) {
		return FALSE;
	}

	return TRUE;
}

void CFileSupport::RevertFile(LPCTSTR bakFileName, LPCTSTR fileName)
{
	if (IsFile(bakFileName))
		::MoveFile(bakFileName, fileName);
}

CString CFileSupport::Combine(CString path1, CString path2)
{
	path1.TrimLeft();
	path1.TrimRight();
	path1.TrimRight(_T("\\/"));

	path2.TrimLeft();
	path2.TrimRight();
	path2.TrimLeft(_T("\\/"));

	return path1 + _T("\\") + path2;
}

void CFileSupport::GetAppDirectory(CString &appPath)
{
	TCHAR szBuff[_MAX_PATH];
	CWinApp *pApp = AfxGetApp();
	VERIFY(::GetModuleFileName(pApp->m_hInstance, szBuff, _MAX_PATH));
	appPath = szBuff;
	int nPos = appPath.ReverseFind(_T('\\'));
	appPath = appPath.Left(nPos+1);
}

BOOL CFileSupport::IsSamePath(CString path1, CString path2)
{
	path1.TrimLeft();
	path1.TrimRight();
	path1.Replace(_T('/'), _T('\\'));
	
	path2.TrimLeft();
	path2.TrimRight();
	path2.Replace(_T('/'), _T('\\'));
	
	return path1 == path2;
}

long CFileSupport::GetFileCount(CString strFilePath)
{
	CFileFind fileFind;
	long fileCount = 0;

	CString searchStr;
	searchStr.Format(_T("%s\\*.*"), strFilePath);

	BOOL fFind = fileFind.FindFile(searchStr);

	while(fFind)
	{
		fFind = fileFind.FindNextFile();

		if( fileFind.IsDots() || fileFind.IsDirectory() )
			continue;

		fileCount++;
	}

	return fileCount;
}