#pragma once

#ifndef _TLS_DB_CONNECTION_H_
#define _TLS_DB_CONNECTION_H_

#include "jdbc/mysql_connection.h"
#include <jdbc/mysql_driver.h>
#include <memory>

// 线程局部存储（TLS）数据库连接管理器
// 每个线程自动拥有独立的数据库连接，无需手动传递

//========================================
// Lighting 线程 TLS 连接
//========================================
// TLS 变量声明 - 每个线程有独立的实例
static __declspec(thread) sql::Connection* g_pTlsLightingConn = nullptr;
static __declspec(thread) BOOL g_bTlsLightingDBConnected = FALSE;

// 获取 TLS 数据库连接（懒加载）
inline BOOL GetTlsLightingConnection(
    const CString& strServer,
    const CString& strDBName,
    const CString& strUser,
    const CString& strPassword,
    CLogger* pLog = nullptr
)
{
    if (g_bTlsLightingDBConnected && g_pTlsLightingConn != nullptr)
        return TRUE;

    // 关闭旧连接（如果有）
    if (g_pTlsLightingConn != nullptr)
    {
        try {
            delete g_pTlsLightingConn;
        }
        catch (...) {
        }
        g_pTlsLightingConn = nullptr;
        g_bTlsLightingDBConnected = FALSE;
    }

    try {
        sql::Driver* driver = get_driver_instance();
        if (!driver) {
            if (pLog) pLog->LOG_INFO(_T("GetTlsLightingConnection: Failed to get MySQL driver"));
            return FALSE;
        }

        CString strUrl;
        strUrl.Format(_T("tcp://%s:3306"), strServer);

        sql::SQLString sql_str1((std::string)CT2A(strUrl));
        sql::SQLString sql_user((std::string)CT2A(strUser));
        sql::SQLString sql_password((std::string)CT2A(strPassword));

        if (pLog) pLog->LOG_INFO(CStringSupport::FormatString(
            _T("GetTlsLightingConnection: Connecting to %s, User: %s"), strUrl, strUser));

        g_pTlsLightingConn = driver->connect(sql_str1, sql_user, sql_password);
        if (!g_pTlsLightingConn) {
            if (pLog) pLog->LOG_INFO(_T("GetTlsLightingConnection: Failed to connect"));
            return FALSE;
        }

        // 设置连接选项
        g_pTlsLightingConn->setClientOption("optReadTimeout", "30");
        g_pTlsLightingConn->setClientOption("optWriteTimeout", "30");
        g_pTlsLightingConn->setClientOption("optConnectTimeout", "10");
        g_pTlsLightingConn->setClientOption("characterSetResults", "utf8mb4");

        g_pTlsLightingConn->setSchema((std::string)CT2A(strDBName));

        g_bTlsLightingDBConnected = TRUE;
        if (pLog) pLog->LOG_INFO(_T("GetTlsLightingConnection: Connected successfully"));
        return TRUE;
    }
    catch (sql::SQLException& e) {
        if (pLog) pLog->LOG_INFO(CStringSupport::FormatString(
            _T("GetTlsLightingConnection: SQLException: %s, errCode=%d"),
            CString(e.what()), e.getErrorCode()));
        return FALSE;
    }
}

// 关闭 TLS 数据库连接
inline void CloseTlsLightingConnection()
{
    if (g_pTlsLightingConn != nullptr)
    {
        try {
            g_pTlsLightingConn->close();
        }
        catch (...) {
        }
        try {
            delete g_pTlsLightingConn;
        }
        catch (...) {
        }
        g_pTlsLightingConn = nullptr;
        g_bTlsLightingDBConnected = FALSE;
    }
}

// 获取 TLS 连接指针（不创建连接）
inline sql::Connection* GetTlsLightingConnPtr()
{
    return g_pTlsLightingConn;
}

// 检查 TLS 连接是否已建立
inline BOOL IsTlsLightingDBConnected()
{
    return g_bTlsLightingDBConnected && g_pTlsLightingConn != nullptr;
}

//========================================
// DFS 线程 TLS 连接（复用同一个 TLS 变量）
// 注意：Lighting 线程和 DFS 线程共用同一个 TLS 连接变量
//========================================
inline BOOL GetTlsDfsConnection(
    const CString& strServer,
    const CString& strDBName,
    const CString& strUser,
    const CString& strPassword,
    CLogger* pLog = nullptr
)
{
    // DFS 线程复用 Lighting 的 TLS 连接
    // 因为它们不会同时运行，且使用相同的数据库
    return GetTlsLightingConnection(strServer, strDBName, strUser, strPassword, pLog);
}

inline sql::Connection* GetTlsDfsConnPtr()
{
    return GetTlsLightingConnPtr();
}

#endif // _TLS_DB_CONNECTION_H_
