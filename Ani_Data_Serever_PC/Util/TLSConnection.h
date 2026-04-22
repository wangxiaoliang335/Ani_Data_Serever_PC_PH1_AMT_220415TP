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
// Note: Not static — must be extern in .h so all TU's share the same thread-local slot.
// The actual definition (with __declspec(thread)) lives in TLSConnection.cpp.
extern __declspec(thread) SQLHDBC g_pTlsLightingConn;
extern __declspec(thread) SQLHENV g_pTlsLightingEnv;
extern __declspec(thread) BOOL g_bTlsLightingDBConnected;

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

    OutputDebugString(CStringSupport::FormatString(
        _T("[TLS] GetTlsLightingConnection ENTRY: Server=%s, DB=%s, User=%s, Pwd=%s\n"),
        strServer, strDBName, strUser,
        strPassword.IsEmpty() ? _T("(empty)") : _T("******")));

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
        //if (pLog) pLog->LOG_INFO(_T("GetTlsLightingConnection: Failed to allocate ODBC environment handle"));
        return FALSE;
    }

    // Set environment attributes
    ret = SQLSetEnvAttr(g_pTlsLightingEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
    if (!SQL_SUCCEEDED(ret))
    {
        //if (pLog) pLog->LOG_INFO(_T("GetTlsLightingConnection: Failed to set ODBC environment attribute"));
        SQLFreeHandle(SQL_HANDLE_ENV, g_pTlsLightingEnv);
        g_pTlsLightingEnv = SQL_NULL_HANDLE;
        return FALSE;
    }

    // Allocate connection handle
    ret = SQLAllocHandle(SQL_HANDLE_DBC, g_pTlsLightingEnv, &g_pTlsLightingConn);
    if (!SQL_SUCCEEDED(ret))
    {
        //if (pLog) pLog->LOG_INFO(_T("GetTlsLightingConnection: Failed to allocate ODBC connection handle"));
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

        OutputDebugString(CStringSupport::FormatString(
            _T("[TLS] Trying driver[%d]: %s\n"), i, CString(driverNames[i])));

        // Use ANSI version ODBC functions
        ret = SQLDriverConnectA(g_pTlsLightingConn, NULL, (SQLCHAR*)connStr, SQL_NTS,
            NULL, 0, NULL, SQL_DRIVER_NOPROMPT);

        if (SQL_SUCCEEDED(ret))
        {
            g_bTlsLightingDBConnected = TRUE;
            OutputDebugString(CStringSupport::FormatString(
                _T("[TLS] SQLDriverConnectA SUCCEEDED! g_pTlsLightingConn=0x%p, flag=%d\n"),
                (void*)g_pTlsLightingConn, g_bTlsLightingDBConnected));
            //if (pLog) pLog->LOG_INFO(CStringSupport::FormatString(
            //    _T("GetTlsLightingConnection: Connected successfully with driver: %s"), 
            //    CString(driverNames[i])));
            connected = TRUE;
            break;
        }
        else
        {
            SQLCHAR sqlState[6] = {0}, msg[256] = {0};
            SQLINTEGER nativeErr = 0;
            SQLSMALLINT msgLen = 0;
            SQLGetDiagRecA(SQL_HANDLE_DBC, g_pTlsLightingConn, 1, sqlState, &nativeErr, msg, sizeof(msg), &msgLen);
            OutputDebugString(CStringSupport::FormatString(
                _T("[TLS] Driver %s failed: [%s] %s\n"),
                CString(driverNames[i]), CString((char*)sqlState), CString((char*)msg)));
            //if (pLog) pLog->LOG_INFO(CStringSupport::FormatString(
            //    _T("GetTlsLightingConnection: Driver %s failed, trying next..."), 
            //    CString(driverNames[i])));
        }
    }
    
    if (!connected)
    {
        OutputDebugString(_T("[TLS] All drivers failed, cleanup and return FALSE\n"));
        //if (pLog) pLog->LOG_INFO(_T("GetTlsLightingConnection: Failed to connect with all drivers"));
        SQLFreeHandle(SQL_HANDLE_DBC, g_pTlsLightingConn);
        SQLFreeHandle(SQL_HANDLE_ENV, g_pTlsLightingEnv);
        g_pTlsLightingConn = SQL_NULL_HANDLE;
        g_pTlsLightingEnv = SQL_NULL_HANDLE;
        return FALSE;
    }
    
    OutputDebugString(CStringSupport::FormatString(
        _T("[TLS] GetTlsLightingConnection: SUCCESS, g_pTlsLightingConn=0x%p\n"),
        (void*)g_pTlsLightingConn));
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

// ==============================================
// Heartbeat: Ping MySQL server to check if connection is still alive
// Returns: TRUE if connection is alive, FALSE if connection is lost or error
// ==============================================
inline BOOL PingTlsLightingConnection(CLogger* pLog = nullptr)
{
    if (!IsTlsLightingDBConnected())
        return FALSE;

    SQLHSTMT hStmt = SQL_NULL_HANDLE;
    SQLRETURN ret;

    // Allocate statement handle
    ret = SQLAllocHandle(SQL_HANDLE_STMT, g_pTlsLightingConn, &hStmt);
    if (!SQL_SUCCEEDED(ret))
    {
        //if (pLog) pLog->LOG_INFO(_T("PingTlsLightingConnection: Failed to allocate statement handle"));
        return FALSE;
    }

    // Execute simple query to check connection
    ret = SQLExecDirectA(hStmt, (SQLCHAR*)"SELECT 1", SQL_NTS);

    if (SQL_SUCCEEDED(ret))
    {
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return TRUE;
    }

    // Connection is lost or error occurred
    //if (pLog) pLog->LOG_INFO(_T("PingTlsLightingConnection: Connection lost or error"));

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    return FALSE;
}

// ==============================================
// Check and reconnect if connection is lost
// Call this function before any database operation
// Returns: TRUE if connection is available (either old or newly connected)
// ==============================================
inline BOOL CheckAndReconnectTlsLighting(
    const CString& strServer,
    const CString& strDBName,
    const CString& strUser,
    const CString& strPassword,
    CLogger* pLog = nullptr
)
{
    // First check: is connection established at all?
    if (!IsTlsLightingDBConnected())
    {
        OutputDebugString(_T("[TLS] CheckAndReconnectTlsLighting: Not connected, calling GetTlsLightingConnection...\n"));
        //if (pLog) pLog->LOG_INFO(_T("CheckAndReconnectTlsLighting: No connection, creating new one..."));
        return GetTlsLightingConnection(strServer, strDBName, strUser, strPassword, pLog);
    }

    OutputDebugString(CStringSupport::FormatString(
        _T("[TLS] CheckAndReconnectTlsLighting: Already connected, ping check... connPtr=0x%p\n"),
        (void*)g_pTlsLightingConn));

    // Second check: is connection still alive? (heartbeat)
    if (!PingTlsLightingConnection(pLog))
    {
        OutputDebugString(_T("[TLS] CheckAndReconnectTlsLighting: Ping FAILED, reconnecting...\n"));
        //if (pLog) pLog->LOG_INFO(_T("CheckAndReconnectTlsLighting: Connection lost, reconnecting..."));

        // Close existing connection
        CloseTlsLightingConnection();

        // Reconnect
        return GetTlsLightingConnection(strServer, strDBName, strUser, strPassword, pLog);
    }

    OutputDebugString(_T("[TLS] CheckAndReconnectTlsLighting: Ping OK, return TRUE\n"));
    return TRUE;
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