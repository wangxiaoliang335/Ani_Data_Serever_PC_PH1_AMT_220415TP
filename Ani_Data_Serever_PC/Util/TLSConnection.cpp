#include "stdafx.h"
#include "TLSConnection.h"

// Thread-Local Storage (TLS) variables
// Each thread automatically gets an independent database connection slot at runtime.
// These are NOT static — the extern declarations in TLSConnection.h ensure all
// translation units share the same __declspec(thread) storage.
__declspec(thread) SQLHDBC g_pTlsLightingConn = SQL_NULL_HANDLE;
__declspec(thread) SQLHENV g_pTlsLightingEnv = SQL_NULL_HANDLE;
__declspec(thread) BOOL g_bTlsLightingDBConnected = FALSE;
