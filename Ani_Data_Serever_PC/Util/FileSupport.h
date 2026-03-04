#pragma once

class CFileSupport
{
public:
	static BOOL FindPath( HWND hOwner , LPCTSTR caption, LPTSTR path) ;
	static CString FindFilePath ( BOOL isOpen , LPCTSTR defaultExt , LPCTSTR filter );
	static CString FindFilePath ( BOOL isOpen , LPCTSTR defaultExt , UINT filterID );
	
	static BOOL FileCheck(LPCTSTR path, BOOL fOkAtBlank = FALSE );
	static BOOL DirectoryCheck(LPCTSTR path, BOOL fOkAtBlank = FALSE );
	static BOOL FileDate(LPCTSTR path, COleDateTime& fileDate);
	static BOOL RemoveReadonly(LPCTSTR path);

	// new interface
	static BOOL IsReadonly(LPCTSTR path);
	static BOOL IsFile(LPCTSTR path, BOOL fOkAtBlank = FALSE );
	static BOOL IsDirectory(LPCTSTR path, BOOL fOkAtBlank = FALSE );
	static BOOL IsEmpty(LPCTSTR lpPathName);

	static CString GetFileName(CString fullPath);
	static CString GetPathName(CString fullPath);
	static void GetPathAndFileName ( LPCTSTR fullPath , CString& path , CString& fileName );

	static BOOL CreateDirectory( LPCTSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes );
	static void DeleteDirectory(LPCTSTR lpPathName, BOOL fConfirm);
	static void DeleteAllFile(LPCTSTR lpPathName, BOOL fConfirm);
	static BOOL ValidateName(CString name);

	static BOOL SafeSave(LPCTSTR tempFileName, LPCTSTR bakFileName, LPCTSTR fileName);
	static void RevertFile(LPCTSTR bakFileName, LPCTSTR fileName);

	static BOOL Copy(LPCTSTR srcName, LPCTSTR destName, HWND hWnd = NULL);
	static BOOL CopyDirectory(LPCTSTR srcDirectory, LPCTSTR destDirectory, HWND hWnd = NULL, BOOL fSilent = FALSE);
	static BOOL Delete(LPCTSTR path, HWND hWnd = NULL);

	static CString Combine(CString path1, CString path2);

	static void GetAppDirectory(CString &appPath);
	static BOOL IsSamePath(CString path1, CString path2);

	static long GetFileCount(CString strFilePath);
};

#define PC_SUCCESS	1
#define PC_FAILED	0
#define PC_CANCELLED	-1
