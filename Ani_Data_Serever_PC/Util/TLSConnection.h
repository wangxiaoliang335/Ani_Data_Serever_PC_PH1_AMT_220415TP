#pragma once

#ifndef _TLS_DB_CONNECTION_H_
#define _TLS_DB_CONNECTION_H_

#include <windows.h>
#include <sql.h>
#include <sqlext.h>
#include "stdafx.h"
#include "Logger.h"
#include "StringSupport.h"

// Thread Local Storage (TLS) Database Connection Manager
// Each thread automatically has an independent database connection, no need to pass manually

//========================================
// Lighting Thread TLS Connection
//========================================
// TLS variable declaration - Each thread has an independent instance
static __declspec(thread) SQLHDBC g_pTlsLightingConn = SQL_NULL_HANDLE;
static __declspec(thread) SQLHENV g_pTlsLightingEnv = SQL_NULL_HANDLE;
static __declspec(thread) BOOL g_bTlsLightingDBConnected = FALSE;

// ==============================================
// Function: Convert Unicode string to multibyte string (ANSI/GBK)
// Parameter: unicode string (const wchar_t*)
// Return: multibyte string (std::string can be converted to const char*)
// ==============================================
inline std::string UnicodeToMultiByte1(const wchar_t* unicodeStr)
{
    // Check for null or empty string
    if (unicodeStr == nullptr || wcslen(unicodeStr) == 0)
    {
        return "";
    }

    // First call to calculate the required buffer size
    int bufferSize = WideCharToMultiByte(
        CP_ACP,         // Use system default code page (usually GBK)
        0,
        unicodeStr,     // Input Unicode string
        -1,             // Auto-calculate length
        nullptr,        // Pass nullptr to only get size
        0,
        nullptr,
        nullptr
    );

    if (bufferSize <= 0)
    {
        return "";
    }

    // Allocate memory and perform conversion
    std::vector<char> buffer(bufferSize);
    WideCharToMultiByte(
        CP_ACP,
        0,
        unicodeStr,
        -1,
        buffer.data(),  // Output buffer
        bufferSize,
        nullptr,
        nullptr
    );

    // Return std::string that can be used directly as const char*
    return std::string(buffer.data());
}

// Get TLS database connection (lazy loading)
inline BOOL GetTlsLightingConnection(
    const CString& strServer,
    const CString& strDBName,
    const CString& strUser,
    const CString& strPassword,
    CLogger* pLog = nullptr
)
{
    if (g_bTlsLightingDBConnected && g_pTlsLightingConn != SQL_NULL_HANDLE)
        return TRUE;

    // Close old connection (if exists)
    if (g_pTlsLightingConn != SQL_NULL_HANDLE)
    {
        SQLDisconnect(g_pTlsLightingConn);
        SQLFreeHandle(SQL_HANDLE_DBC, g_pTlsLightingConn);
        g_pTlsLightingConn = SQL_NULL_HANDLE;
    }

    if (g_pTlsLightingEnv != SQL_NULL_HANDLE)
    {
        SQLFreeHandle(SQL_HANDLE_ENV, g_pTlsLightingEnv);
        g_pTlsLightingEnv = SQL_NULL_HANDLE;
    }

    SQLRETURN ret;

    // Allocate environment handle
    ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &g_pTlsLightingEnv);
    if (!SQL_SUCCEEDED(ret))
    {
        if (pLog) pLog->LOG_INFO(_T("GetTlsLightingConnection: Failed to allocate ODBC environment handle"));
        return FALSE;
    }

    // Set environment attributes
    ret = SQLSetEnvAttr(g_pTlsLightingEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
    if (!SQL_SUCCEEDED(ret))
    {
        if (pLog) pLog->LOG_INFO(_T("GetTlsLightingConnection: Failed to set ODBC environment attribute"));
        SQLFreeHandle(SQL_HANDLE_ENV, g_pTlsLightingEnv);
        g_pTlsLightingEnv = SQL_NULL_HANDLE;
        return FALSE;
    }

    // Allocate connection handle
    ret = SQLAllocHandle(SQL_HANDLE_DBC, g_pTlsLightingEnv, &g_pTlsLightingConn);
    if (!SQL_SUCCEEDED(ret))
    {
        if (pLog) pLog->LOG_INFO(_T("GetTlsLightingConnection: Failed to allocate ODBC connection handle"));
        SQLFreeHandle(SQL_HANDLE_ENV, g_pTlsLightingEnv);
        g_pTlsLightingEnv = SQL_NULL_HANDLE;
        return FALSE;
    }

    // Build connection string
    //const char* server = CT2A(strServer);
    //const char* dbname = CT2A(strDBName);
    //const char* user = CT2A(strUser);
    //const char* password = CT2A(strPassword);

    string server = UnicodeToMultiByte1(strServer.GetString()).c_str();
    string dbname = UnicodeToMultiByte1(strDBName.GetString()).c_str();
    string user = UnicodeToMultiByte1(strUser.GetString()).c_str();
    string password = UnicodeToMultiByte1(strPassword.GetString()).c_str();
    
    // Try multiple drivers
    const char* driverNames[] = {
        "MySQL ODBC 5.3 ANSI Driver",
        "MySQL ODBC 5.3 Unicode Driver",
        "MySQL ODBC 5.3 Driver"
    };
    
    char connStr[512];
    BOOL connected = FALSE;
    
    for (int i = 0; i < sizeof(driverNames)/sizeof(driverNames[0]); i++) {
        sprintf_s(connStr, sizeof(connStr), 
            "DRIVER={%s};SERVER=%s;PORT=3306;DATABASE=%s;UID=%s;PWD=%s;OPTION=3;",
            driverNames[i], server.c_str(), dbname.c_str(), user.c_str(), password.c_str());

        if (pLog) pLog->LOG_INFO(CStringSupport::FormatString(
            _T("GetTlsLightingConnection: Connecting with driver: %s, Server: %s, DB: %s"), 
            CString(driverNames[i]), strServer, strDBName));

        // Use ANSI version ODBC functions
        ret = SQLDriverConnectA(g_pTlsLightingConn, NULL, (SQLCHAR*)connStr, SQL_NTS,
            NULL, 0, NULL, SQL_DRIVER_NOPROMPT);

        if (SQL_SUCCEEDED(ret))
        {
            g_bTlsLightingDBConnected = TRUE;
            if (pLog) pLog->LOG_INFO(CStringSupport::FormatString(
                _T("GetTlsLightingConnection: Connected successfully with driver: %s"), 
                CString(driverNames[i])));
            connected = TRUE;
            break;
        }
        else
        {
            if (pLog) pLog->LOG_INFO(CStringSupport::FormatString(
                _T("GetTlsLightingConnection: Driver %s failed, trying next..."), 
                CString(driverNames[i])));
        }
    }
    
    if (!connected)
    {
        if (pLog) pLog->LOG_INFO(_T("GetTlsLightingConnection: Failed to connect with all drivers"));
        SQLFreeHandle(SQL_HANDLE_DBC, g_pTlsLightingConn);
        SQLFreeHandle(SQL_HANDLE_ENV, g_pTlsLightingEnv);
        g_pTlsLightingConn = SQL_NULL_HANDLE;
        g_pTlsLightingEnv = SQL_NULL_HANDLE;
        return FALSE;
    }
    
    return TRUE;
}

// Close TLS database connection
inline void CloseTlsLightingConnection()
{
    if (g_pTlsLightingConn != SQL_NULL_HANDLE)
    {
        SQLDisconnect(g_pTlsLightingConn);
        SQLFreeHandle(SQL_HANDLE_DBC, g_pTlsLightingConn);
        g_pTlsLightingConn = SQL_NULL_HANDLE;
    }

    if (g_pTlsLightingEnv != SQL_NULL_HANDLE)
    {
        SQLFreeHandle(SQL_HANDLE_ENV, g_pTlsLightingEnv);
        g_pTlsLightingEnv = SQL_NULL_HANDLE;
    }

    g_bTlsLightingDBConnected = FALSE;
}

// Get TLS connection pointer (without creating connection)
inline SQLHDBC GetTlsLightingConnPtr()
{
    return g_pTlsLightingConn;
}

// Check if TLS connection is established
inline BOOL IsTlsLightingDBConnected()
{
    return g_bTlsLightingDBConnected && g_pTlsLightingConn != SQL_NULL_HANDLE;
}

//========================================
// DFS Thread TLS Connection (reuse the same TLS variable)
// Note: Lighting thread and DFS thread share the same TLS connection variable
//========================================
inline BOOL GetTlsDfsConnection(
    const CString& strServer,
    const CString& strDBName,
    const CString& strUser,
    const CString& strPassword,
    CLogger* pLog = nullptr
)
{
    // DFS thread reuses Lighting's TLS connection
    // Because they won't run simultaneously and use the same database
    return GetTlsLightingConnection(strServer, strDBName, strUser, strPassword, pLog);
}

inline SQLHDBC GetTlsDfsConnPtr()
{
    return GetTlsLightingConnPtr();
}

#endif // _TLS_DB_CONNECTION_H_