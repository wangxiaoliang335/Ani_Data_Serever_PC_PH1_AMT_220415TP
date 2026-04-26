// LightingSimulator.cpp : Lighting Test Server Simulator
//

#include <iostream>
#include <string>
#include <thread>
#include <atomic>
#include <vector>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <memory>
#include <mutex>
#include <map>
#include <algorithm>
#include <random>
#include <csignal>
#include <cstdarg>
#include <fstream>

#define CPP_CONN_STATIC

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    typedef SOCKET SocketType;
    #define CLOSE_SOCKET closesocket
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    typedef int SocketType;
    #define CLOSE_SOCKET close
    #define SOCKET_ERROR (-1)
    #define INVALID_SOCKET (-1)
#endif

// Use ODBC for MySQL 5.1 compatibility
#ifdef USE_ODBC
#include <windows.h>
#include <sql.h>
#include <sqlext.h>

// ODBC handle types
SQLHENV g_odbcEnv = SQL_NULL_HANDLE;
SQLHDBC g_odbcConn = SQL_NULL_HANDLE;
std::mutex g_odbcMutex;
#else
// MySQL include (Connector/C++ 9.6.0)
#include <mysql_connection.h>
#include <mysql_driver.h>
#include <cppconn/statement.h>
#include <cppconn/resultset.h>
#include <cppconn/exception.h>

using namespace sql;
using namespace sql::mysql;
#endif

// Forward declarations
void Log(const char* fmt, ...);
#ifdef USE_ODBC
void PrintOdbcError(SQLHANDLE handle, SQLSMALLINT type);
#endif

// Global running flag - declared early for console handler
std::atomic<bool> g_running(false);

// Console close handler (defined above Log function)
#ifdef _WIN32
    BOOL WINAPI ConsoleCtrlHandler(DWORD dwCtrlType);
#endif

// Configuration
namespace Config {
    std::string SERVER_IP = "0.0.0.0";
    int SERVER_PORT = 6501;
    const int BUFFER_SIZE = 4096;
    const int TIMEOUT_SEC = 1;
    const int TOTAL_FIXTURES = 4;

    // Database
    std::string DB_HOST = "localhost";
    int DB_PORT = 3306;
    std::string DB_USER = "root";
    std::string DB_PASSWORD = "";
    std::string DB_NAME = "ivs_lcd";
    std::string DB_CHARSET = "utf8mb4";

    // Test timing
    int MIN_IMAGE_DELAY = 2000;
    int MAX_IMAGE_DELAY = 5000;
    int MIN_DETECT_DELAY = 1000;
    int MAX_DETECT_DELAY = 3000;

    // Result probability
    int NG_PROBABILITY = 30;
    int BRIGHT_DOT_PROB = 20;
    int BLACK_DOT_PROB = 20;
    int LINE_PROB = 10;
    int MURA_PROB = 15;
    int BLOCK_PROB = 10;
    int BM_PROB = 5;
    int OTHER_PROB = 5;
}

// Logging function
void Log(const char* fmt, ...) {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::tm tm;
#ifdef _WIN32
    localtime_s(&tm, &time_t);
#else
    localtime_r(&time_t, &tm);
#endif
    
    char buffer[32];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm);
    
    printf("[%s.%03lld] ", buffer, (long long)ms.count());
    
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    
    printf("\n");
    fflush(stdout);
}

// Console close handler implementation
#ifdef _WIN32
BOOL WINAPI ConsoleCtrlHandler(DWORD dwCtrlType) {
    if (dwCtrlType == CTRL_CLOSE_EVENT || dwCtrlType == CTRL_SHUTDOWN_EVENT) {
        g_running = false;
        Log("Console close requested, shutting down...");
        // Give some time for cleanup
        Sleep(500);
        return TRUE;
    }
    return FALSE;
}
#endif

// Load config from ini file
bool LoadConfig(const std::string& configPath) {
    std::ifstream file(configPath);
    if (!file.is_open()) {
        Log("Config file not found: %s, using defaults", configPath.c_str());
        return false;
    }

    std::string line;
    std::string currentSection;
    while (std::getline(file, line)) {
        // Trim whitespace
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);

        if (line.empty() || line[0] == '#' || line[0] == ';')
            continue;

        if (line[0] == '[') {
            currentSection = line.substr(1, line.find(']') - 1);
            continue;
        }

        size_t eqPos = line.find('=');
        if (eqPos == std::string::npos) continue;

        std::string key = line.substr(0, eqPos);
        std::string value = line.substr(eqPos + 1);
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);

        if (currentSection == "Database") {
            if (key == "Host") Config::DB_HOST = value;
            else if (key == "Port") Config::DB_PORT = std::stoi(value);
            else if (key == "User") Config::DB_USER = value;
            else if (key == "Password") Config::DB_PASSWORD = value;
            else if (key == "Database") Config::DB_NAME = value;
            else if (key == "Charset") Config::DB_CHARSET = value;
        }
        else if (currentSection == "Server") {
            if (key == "Port") Config::SERVER_PORT = std::stoi(value);
            else if (key == "ListenIP") Config::SERVER_IP = value;
        }
        else if (currentSection == "Test") {
            if (key == "MinImageDelay") Config::MIN_IMAGE_DELAY = std::stoi(value);
            else if (key == "MaxImageDelay") Config::MAX_IMAGE_DELAY = std::stoi(value);
            else if (key == "MinDetectDelay") Config::MIN_DETECT_DELAY = std::stoi(value);
            else if (key == "MaxDetectDelay") Config::MAX_DETECT_DELAY = std::stoi(value);
            else if (key == "NGProbability") Config::NG_PROBABILITY = std::stoi(value);
        }
    }

    Log("Config loaded from: %s", configPath.c_str());
    return true;
}

// Client info
struct ClientInfo {
    SocketType socket;
    std::string address;
    int port;
    std::thread* thread;
    std::atomic<bool> runningFlag;
    
    ClientInfo() : socket(INVALID_SOCKET), thread(nullptr), runningFlag(true) {}
};

// Global variables
std::vector<std::shared_ptr<ClientInfo>> g_clients;
std::mutex g_clients_mutex;

// Current test state
struct TestState {
    std::string currentFixtures;
    std::string maxFixtures;
    bool isTesting;
    
    TestState() : isTesting(false) {}
};

TestState g_testState;

// Store UniqueID for each fixture (indexed by fixture number 1-4)
std::map<int, std::string> g_fixtureUniqueID;

#ifndef USE_ODBC
// MySQL connection
std::unique_ptr<sql::Connection> g_dbConnection;
std::mutex g_dbMutex;
#endif

// Defect types
enum DefectType {
    DEFECT_OK = 0,
    DEFECT_BRIGHT_DOT,
    DEFECT_BLACK_DOT,
    DEFECT_LINE,
    DEFECT_MURA,
    DEFECT_BLOCK,
    DEFECT_BM,
    DEFECT_OTHER
};

const char* DefectTypeToString(DefectType type) {
    switch (type) {
        case DEFECT_OK: return "OK";
        case DEFECT_BRIGHT_DOT: return "BrightDot";
        case DEFECT_BLACK_DOT: return "BlackDot";
        case DEFECT_LINE: return "Line";
        case DEFECT_MURA: return "Mura";
        case DEFECT_BLOCK: return "Block";
        case DEFECT_BM: return "BM";
        default: return "Other";
    }
}

// Generate random defect based on probability
DefectType GenerateRandomDefect() {
    int randVal = rand() % 100;
    
    if (randVal >= Config::NG_PROBABILITY) {
        return DEFECT_OK;
    }
    
    int ngRand = rand() % 100;
    int cumulative = 0;
    
    cumulative += Config::BRIGHT_DOT_PROB;
    if (ngRand < cumulative) return DEFECT_BRIGHT_DOT;
    
    cumulative += Config::BLACK_DOT_PROB;
    if (ngRand < cumulative) return DEFECT_BLACK_DOT;
    
    cumulative += Config::LINE_PROB;
    if (ngRand < cumulative) return DEFECT_LINE;
    
    cumulative += Config::MURA_PROB;
    if (ngRand < cumulative) return DEFECT_MURA;
    
    cumulative += Config::BLOCK_PROB;
    if (ngRand < cumulative) return DEFECT_BLOCK;
    
    cumulative += Config::BM_PROB;
    if (ngRand < cumulative) return DEFECT_BM;
    
    return DEFECT_OTHER;
}

// Generate test result code
int GenerateResultCode() {
    DefectType defect = GenerateRandomDefect();
    return static_cast<int>(defect);
}

// Get current timestamp string
std::string GetCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::tm tm;
#ifdef _WIN32
    localtime_s(&tm, &time_t);
#else
    localtime_r(&time_t, &tm);
#endif
    
    char buffer[64];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm);
    char fullBuffer[128];
    sprintf(fullBuffer, "%s.%03lld", buffer, (long long)ms.count());
    return std::string(fullBuffer);
}

// Generate UUID
std::string GenerateUUID() {
    std::ostringstream ss;
    ss << std::hex << std::uppercase;
    ss << "{" << rand() % 0xFFFF << "-" << rand() % 0xFFFF << "-" << rand() % 0xFFFF << "-";
    ss << rand() % 0xFFFF << "-" << rand() % 0xFFFFFFFF << "}";
    return ss.str();
}

// Generate UniqueID
std::string GenerateUniqueID() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::ostringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y%m%d%H%M%S");
    ss << std::setfill('0') << std::setw(3) << ms.count();
    ss << "_" << (rand() % 100);
    return ss.str();
}

// Create tables if they don't exist
void CreateTablesIfNotExist() {
#ifdef USE_ODBC
    std::lock_guard<std::mutex> lock(g_odbcMutex);
    
    try {
        SQLHSTMT stmt = SQL_NULL_HANDLE;
        SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_STMT, g_odbcConn, &stmt);
        if (!SQL_SUCCEEDED(ret)) {
            PrintOdbcError(g_odbcConn, SQL_HANDLE_DBC);
            return;
        }
        
        // Create ivs_lcd_idmap if not exists (ID映射表)
        std::string createIdmap = 
            "CREATE TABLE IF NOT EXISTS ivs_lcd_idmap ("
            "  MarkID VARCHAR(15) NOT NULL,"
            "  PosID INT NULL,"
            "  UniqueID VARCHAR(38) NULL,"
            "  TableSuffix VARCHAR(10) NULL,"
            "  Barcode VARCHAR(128) NULL,"
            "  MainAoiFixID VARCHAR(10) NULL,"
            "  Fix_IDCode VARCHAR(128) NULL,"
            "  PRIMARY KEY (MarkID)"
            ") ENGINE=InnoDB DEFAULT CHARSET=utf8";
        
        // Use ANSI version of SQLExecDirect
        ret = SQLExecDirectA(stmt, (SQLCHAR*)createIdmap.c_str(), SQL_NTS);
        if (!SQL_SUCCEEDED(ret)) {
            PrintOdbcError(stmt, SQL_HANDLE_STMT);
        } else {
            Log("[DB] Table ivs_lcd_idmap ensured");
        }
        
        // Create IVS_LCD_InspectionResult if not exists (屏表 - 完整版)
        std::string createResult = 
            "CREATE TABLE IF NOT EXISTS IVS_LCD_InspectionResult ("
            "  SysID INT AUTO_INCREMENT PRIMARY KEY,"
            "  GUID VARCHAR(38),"
            "  ScreenID VARCHAR(128) NOT NULL,"
            "  DeviceID VARCHAR(15),"
            "  PlatformID INT NOT NULL,"
            "  ShiftID VARCHAR(15),"
            "  LotID VARCHAR(50),"
            "  ModelName VARCHAR(50),"
            "  StartTime VARCHAR(50),"
            "  StopTime VARCHAR(50),"
            "  Status VARCHAR(30) NOT NULL,"
            "  AOIResult VARCHAR(15) NOT NULL,"
            "  ReviewResult_Worker VARCHAR(15),"
            "  ReviewResult_Machine VARCHAR(15),"
            "  AllPerspectiveResult VARCHAR(15),"
            "  MarkID VARCHAR(15),"
            "  ProcessType VARCHAR(15),"
            "  LineID VARCHAR(15),"
            "  UniqueID VARCHAR(38),"
            "  MainAoiFixID VARCHAR(10),"
            "  ReviewFixID_Worker VARCHAR(10),"
            "  ReviewFixID_Machine VARCHAR(10),"
            "  AllPerspectiveFixID VARCHAR(10),"
            "  LocateShiftX FLOAT,"
            "  LocateShiftY FLOAT,"
            "  LocateAngle FLOAT,"
            "  XMLInfo VARCHAR(5000),"
            "  RawImageXLen INT,"
            "  RawImageYLen INT,"
            "  GridImageXLen INT,"
            "  GridImageYLen INT,"
            "  PanelPhysicalXLen FLOAT,"
            "  PanelPhysicalYLen FLOAT,"
            "  LocalIP VARCHAR(100),"
            "  CIMMode VARCHAR(10),"
            "  RuncardID VARCHAR(50),"
            "  CassetteID VARCHAR(50),"
            "  SlotID VARCHAR(50),"
            "  OperatorID VARCHAR(50),"
            "  ProductID VARCHAR(50),"
            "  L255_Grayscale INT,"
            "  L0_Grayscale INT,"
            "  DevUnitID VARCHAR(50),"
            "  Code_AOI VARCHAR(50),"
            "  Grade_AOI VARCHAR(50),"
            "  Level_AOI VARCHAR(50),"
            "  Code_AllView VARCHAR(50),"
            "  Grade_AllView VARCHAR(50),"
            "  Level_AllView VARCHAR(50),"
            "  Code_AutoReview VARCHAR(50),"
            "  Grade_AutoReview VARCHAR(50),"
            "  Level_AutoReview VARCHAR(50),"
            "  Code_ManualReview VARCHAR(50),"
            "  Grade_ManualReview VARCHAR(50),"
            "  Level_ManualReview VARCHAR(50),"
            "  Code_Final VARCHAR(50),"
            "  Grade_Final VARCHAR(50),"
            "  Level_Final VARCHAR(50),"
            "  Station_AllView VARCHAR(50),"
            "  Station_AutoReview VARCHAR(50),"
            "  Station_ManualReview VARCHAR(50),"
            "  Operator_ManualReview VARCHAR(50),"
            "  StartTime_AllView VARCHAR(50),"
            "  StopTime_AllView VARCHAR(50),"
            "  StartTime_AutoReview VARCHAR(50),"
            "  StopTime_AutoReview VARCHAR(50),"
            "  StartTime_ManualReview VARCHAR(50),"
            "  StopTime_ManualReview VARCHAR(50),"
            "  DefClass_AOI VARCHAR(100),"
            "  DefName_AOI VARCHAR(100),"
            "  DefClass_AllView VARCHAR(100),"
            "  DefName_AllView VARCHAR(100),"
            "  Pats_AOI VARCHAR(1000),"
            "  DefClass_AutoReview VARCHAR(100),"
            "  DefName_AutoReview VARCHAR(100),"
            "  ElecRes VARCHAR(30),"
            "  DustCount INT,"
            "  UNIQUE KEY idx_GUID (GUID),"
            "  KEY idx_ScreenID (ScreenID),"
            "  KEY idx_UniqueID (UniqueID),"
            "  KEY idx_MainAoiFixID (MainAoiFixID)"
            ") ENGINE=InnoDB DEFAULT CHARSET=utf8";
        
        ret = SQLExecDirectA(stmt, (SQLCHAR*)createResult.c_str(), SQL_NTS);
        if (!SQL_SUCCEEDED(ret)) {
            PrintOdbcError(stmt, SQL_HANDLE_STMT);
        } else {
            Log("[DB] Table IVS_LCD_InspectionResult ensured");
        }
        
        // Create IVS_LCD_AOIResult if not exists (缺陷表 - 完整版)
        std::string createAOIDefect = 
            "CREATE TABLE IF NOT EXISTS ivs_lcd_aoiresult ("
            "  SysID INT NOT NULL AUTO_INCREMENT,"
            "  GUID_IVS_LCD_InspectionResult VARCHAR(100) NULL DEFAULT NULL,"
            "  DefectIndex INT(11) NULL DEFAULT NULL,"
            "  Type VARCHAR(50) NULL DEFAULT NULL,"
            "  PatternID INT(11) NULL DEFAULT NULL,"
            "  PatternName VARCHAR(100) NULL DEFAULT NULL,"
            "  Pos_x INT(11) NULL DEFAULT NULL,"
            "  Pos_y INT(11) NULL DEFAULT NULL,"
            "  Pos_width INT(11) NULL DEFAULT NULL,"
            "  Pos_height INT(11) NULL DEFAULT NULL,"
            "  TrueSize FLOAT NULL DEFAULT NULL,"
            "  TrueDiameter FLOAT NULL DEFAULT NULL,"
            "  TrueLongSize FLOAT NULL DEFAULT NULL,"
            "  TrueShortSize FLOAT NULL DEFAULT NULL,"
            "  GrayScale INT(11) NULL DEFAULT NULL,"
            "  GrayScale_BK INT(11) NULL DEFAULT NULL,"
            "  GrayScaleDiff INT(11) NULL DEFAULT NULL,"
            "  ReviewResult_Worker VARCHAR(50) NULL DEFAULT NULL,"
            "  ReviewResult_Machine VARCHAR(50) NULL DEFAULT NULL,"
            "  MachineReviewDefectName VARCHAR(100) NULL DEFAULT NULL,"
            "  InspType VARCHAR(50) NULL DEFAULT NULL,"
            "  Layer VARCHAR(50) NULL DEFAULT NULL,"
            "  Area INT(11) NULL DEFAULT NULL,"
            "  Roundness FLOAT NULL DEFAULT NULL,"
            "  GrayscaleMean INT(11) NULL DEFAULT NULL,"
            "  GrayscaleMin INT(11) NULL DEFAULT NULL,"
            "  GrayscaleMax INT(11) NULL DEFAULT NULL,"
            "  JND FLOAT NULL DEFAULT NULL,"
            "  MajorAxisAngle FLOAT NULL DEFAULT NULL,"
            "  Code_AOI VARCHAR(50) NULL DEFAULT NULL,"
            "  Grade_AOI VARCHAR(50) NULL DEFAULT NULL,"
            "  Level_AOI VARCHAR(50) NULL DEFAULT NULL,"
            "  Code_AutoReview VARCHAR(50) NULL DEFAULT NULL,"
            "  Grade_AutoReview VARCHAR(50) NULL DEFAULT NULL,"
            "  Level_AutoReview VARCHAR(50) NULL DEFAULT NULL,"
            "  Code_ManualReview VARCHAR(50) NULL DEFAULT NULL,"
            "  Grade_ManualReview VARCHAR(50) NULL DEFAULT NULL,"
            "  Level_ManualReview VARCHAR(50) NULL DEFAULT NULL,"
            "  ImagePath VARCHAR(500) NULL DEFAULT NULL,"
            "  XMLInfo TEXT NULL,"
            "  OriArea INT(11) NULL DEFAULT NULL,"
            "  OriLongSize INT(11) NULL DEFAULT NULL,"
            "  OriShortSize INT(11) NULL DEFAULT NULL,"
            "  DefClass_AOI VARCHAR(100) NULL DEFAULT NULL,"
            "  DefName_AOI VARCHAR(100) NULL DEFAULT NULL,"
            "  AlgName VARCHAR(100) NULL DEFAULT NULL,"
            "  AlgID INT(11) NULL DEFAULT NULL,"
            "  ReasonCode VARCHAR(200) NULL DEFAULT NULL,"
            "  FeatureName VARCHAR(100) NULL DEFAULT NULL,"
            "  FeatureMin VARCHAR(100) NULL DEFAULT NULL,"
            "  FeatureMax VARCHAR(100) NULL DEFAULT NULL,"
            "  FeatureUnit VARCHAR(100) NULL DEFAULT NULL,"
            "  FeatureValue VARCHAR(100) NULL DEFAULT NULL,"
            "  DefColor VARCHAR(50) NULL DEFAULT NULL,"
            "  DefColorValue FLOAT NULL DEFAULT NULL,"
            "  DefClass_AutoReview VARCHAR(100) NULL DEFAULT NULL,"
            "  DefName_AutoReview VARCHAR(100) NULL DEFAULT NULL,"
            "  PointType VARCHAR(50) NULL DEFAULT NULL,"
            "  PRIMARY KEY (SysID) USING BTREE,"
            "  INDEX idx_GUID (GUID_IVS_LCD_InspectionResult) USING BTREE"
            ") ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = Compact";
        
        ret = SQLExecDirectA(stmt, (SQLCHAR*)createAOIDefect.c_str(), SQL_NTS);
        if (!SQL_SUCCEEDED(ret)) {
            PrintOdbcError(stmt, SQL_HANDLE_STMT);
        } else {
            Log("[DB] Table IVS_LCD_AOIResult ensured");
        }
        
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        
    } catch (const std::exception& e) {
        Log("[DB] Table creation warning: %s", e.what());
    }
#else
    std::lock_guard<std::mutex> lock(g_dbMutex);
    
    try {
        std::unique_ptr<Statement> stmt(g_dbConnection->createStatement());
        
        // Create ivs_lcd_idmap if not exists (ID映射表)
        std::string createIdmap = 
            "CREATE TABLE IF NOT EXISTS ivs_lcd_idmap ("
            "  MarkID VARCHAR(15) NOT NULL,"
            "  PosID INT NULL,"
            "  UniqueID VARCHAR(38) NULL,"
            "  TableSuffix VARCHAR(10) NULL,"
            "  Barcode VARCHAR(128) NULL,"
            "  MainAoiFixID VARCHAR(10) NULL,"
            "  Fix_IDCode VARCHAR(128) NULL,"
            "  PRIMARY KEY (MarkID)"
            ") ENGINE=InnoDB DEFAULT CHARSET=utf8";
        stmt->execute(createIdmap);
        Log("[DB] Table ivs_lcd_idmap ensured");
        
        // Create IVS_LCD_InspectionResult if not exists (屏表 - 完整版)
        std::string createResult = 
            "CREATE TABLE IF NOT EXISTS IVS_LCD_InspectionResult ("
            "  SysID INT AUTO_INCREMENT PRIMARY KEY,"
            "  GUID VARCHAR(38),"
            "  ScreenID VARCHAR(128) NOT NULL,"
            "  DeviceID VARCHAR(15),"
            "  PlatformID INT NOT NULL,"
            "  ShiftID VARCHAR(15),"
            "  LotID VARCHAR(50),"
            "  ModelName VARCHAR(50),"
            "  StartTime VARCHAR(50),"
            "  StopTime VARCHAR(50),"
            "  Status VARCHAR(30) NOT NULL,"
            "  AOIResult VARCHAR(15) NOT NULL,"
            "  ReviewResult_Worker VARCHAR(15),"
            "  ReviewResult_Machine VARCHAR(15),"
            "  AllPerspectiveResult VARCHAR(15),"
            "  MarkID VARCHAR(15),"
            "  ProcessType VARCHAR(15),"
            "  LineID VARCHAR(15),"
            "  UniqueID VARCHAR(38),"
            "  MainAoiFixID VARCHAR(10),"
            "  ReviewFixID_Worker VARCHAR(10),"
            "  ReviewFixID_Machine VARCHAR(10),"
            "  AllPerspectiveFixID VARCHAR(10),"
            "  LocateShiftX FLOAT,"
            "  LocateShiftY FLOAT,"
            "  LocateAngle FLOAT,"
            "  XMLInfo VARCHAR(5000),"
            "  RawImageXLen INT,"
            "  RawImageYLen INT,"
            "  GridImageXLen INT,"
            "  GridImageYLen INT,"
            "  PanelPhysicalXLen FLOAT,"
            "  PanelPhysicalYLen FLOAT,"
            "  LocalIP VARCHAR(100),"
            "  CIMMode VARCHAR(10),"
            "  RuncardID VARCHAR(50),"
            "  CassetteID VARCHAR(50),"
            "  SlotID VARCHAR(50),"
            "  OperatorID VARCHAR(50),"
            "  ProductID VARCHAR(50),"
            "  L255_Grayscale INT,"
            "  L0_Grayscale INT,"
            "  DevUnitID VARCHAR(50),"
            "  Code_AOI VARCHAR(50),"
            "  Grade_AOI VARCHAR(50),"
            "  Level_AOI VARCHAR(50),"
            "  Code_AllView VARCHAR(50),"
            "  Grade_AllView VARCHAR(50),"
            "  Level_AllView VARCHAR(50),"
            "  Code_AutoReview VARCHAR(50),"
            "  Grade_AutoReview VARCHAR(50),"
            "  Level_AutoReview VARCHAR(50),"
            "  Code_ManualReview VARCHAR(50),"
            "  Grade_ManualReview VARCHAR(50),"
            "  Level_ManualReview VARCHAR(50),"
            "  Code_Final VARCHAR(50),"
            "  Grade_Final VARCHAR(50),"
            "  Level_Final VARCHAR(50),"
            "  Station_AllView VARCHAR(50),"
            "  Station_AutoReview VARCHAR(50),"
            "  Station_ManualReview VARCHAR(50),"
            "  Operator_ManualReview VARCHAR(50),"
            "  StartTime_AllView VARCHAR(50),"
            "  StopTime_AllView VARCHAR(50),"
            "  StartTime_AutoReview VARCHAR(50),"
            "  StopTime_AutoReview VARCHAR(50),"
            "  StartTime_ManualReview VARCHAR(50),"
            "  StopTime_ManualReview VARCHAR(50),"
            "  DefClass_AOI VARCHAR(100),"
            "  DefName_AOI VARCHAR(100),"
            "  DefClass_AllView VARCHAR(100),"
            "  DefName_AllView VARCHAR(100),"
            "  Pats_AOI VARCHAR(1000),"
            "  DefClass_AutoReview VARCHAR(100),"
            "  DefName_AutoReview VARCHAR(100),"
            "  ElecRes VARCHAR(30),"
            "  DustCount INT,"
            "  UNIQUE KEY idx_GUID (GUID),"
            "  KEY idx_ScreenID (ScreenID),"
            "  KEY idx_UniqueID (UniqueID),"
            "  KEY idx_MainAoiFixID (MainAoiFixID)"
            ") ENGINE=InnoDB DEFAULT CHARSET=utf8";
        stmt->execute(createResult);
        Log("[DB] Table IVS_LCD_InspectionResult ensured");
        
        // Create IVS_LCD_AOIResult if not exists (缺陷表 - 完整版)
        std::string createAOIDefect = 
            "CREATE TABLE IF NOT EXISTS ivs_lcd_aoiresult ("
            "  SysID INT NOT NULL AUTO_INCREMENT,"
            "  GUID_IVS_LCD_InspectionResult VARCHAR(100) NULL DEFAULT NULL,"
            "  DefectIndex INT(11) NULL DEFAULT NULL,"
            "  Type VARCHAR(50) NULL DEFAULT NULL,"
            "  PatternID INT(11) NULL DEFAULT NULL,"
            "  PatternName VARCHAR(100) NULL DEFAULT NULL,"
            "  Pos_x INT(11) NULL DEFAULT NULL,"
            "  Pos_y INT(11) NULL DEFAULT NULL,"
            "  Pos_width INT(11) NULL DEFAULT NULL,"
            "  Pos_height INT(11) NULL DEFAULT NULL,"
            "  TrueSize FLOAT NULL DEFAULT NULL,"
            "  TrueDiameter FLOAT NULL DEFAULT NULL,"
            "  TrueLongSize FLOAT NULL DEFAULT NULL,"
            "  TrueShortSize FLOAT NULL DEFAULT NULL,"
            "  GrayScale INT(11) NULL DEFAULT NULL,"
            "  GrayScale_BK INT(11) NULL DEFAULT NULL,"
            "  GrayScaleDiff INT(11) NULL DEFAULT NULL,"
            "  ReviewResult_Worker VARCHAR(50) NULL DEFAULT NULL,"
            "  ReviewResult_Machine VARCHAR(50) NULL DEFAULT NULL,"
            "  MachineReviewDefectName VARCHAR(100) NULL DEFAULT NULL,"
            "  InspType VARCHAR(50) NULL DEFAULT NULL,"
            "  Layer VARCHAR(50) NULL DEFAULT NULL,"
            "  Area INT(11) NULL DEFAULT NULL,"
            "  Roundness FLOAT NULL DEFAULT NULL,"
            "  GrayscaleMean INT(11) NULL DEFAULT NULL,"
            "  GrayscaleMin INT(11) NULL DEFAULT NULL,"
            "  GrayscaleMax INT(11) NULL DEFAULT NULL,"
            "  JND FLOAT NULL DEFAULT NULL,"
            "  MajorAxisAngle FLOAT NULL DEFAULT NULL,"
            "  Code_AOI VARCHAR(50) NULL DEFAULT NULL,"
            "  Grade_AOI VARCHAR(50) NULL DEFAULT NULL,"
            "  Level_AOI VARCHAR(50) NULL DEFAULT NULL,"
            "  Code_AutoReview VARCHAR(50) NULL DEFAULT NULL,"
            "  Grade_AutoReview VARCHAR(50) NULL DEFAULT NULL,"
            "  Level_AutoReview VARCHAR(50) NULL DEFAULT NULL,"
            "  Code_ManualReview VARCHAR(50) NULL DEFAULT NULL,"
            "  Grade_ManualReview VARCHAR(50) NULL DEFAULT NULL,"
            "  Level_ManualReview VARCHAR(50) NULL DEFAULT NULL,"
            "  ImagePath VARCHAR(500) NULL DEFAULT NULL,"
            "  XMLInfo TEXT NULL,"
            "  OriArea INT(11) NULL DEFAULT NULL,"
            "  OriLongSize INT(11) NULL DEFAULT NULL,"
            "  OriShortSize INT(11) NULL DEFAULT NULL,"
            "  DefClass_AOI VARCHAR(100) NULL DEFAULT NULL,"
            "  DefName_AOI VARCHAR(100) NULL DEFAULT NULL,"
            "  AlgName VARCHAR(100) NULL DEFAULT NULL,"
            "  AlgID INT(11) NULL DEFAULT NULL,"
            "  ReasonCode VARCHAR(200) NULL DEFAULT NULL,"
            "  FeatureName VARCHAR(100) NULL DEFAULT NULL,"
            "  FeatureMin VARCHAR(100) NULL DEFAULT NULL,"
            "  FeatureMax VARCHAR(100) NULL DEFAULT NULL,"
            "  FeatureUnit VARCHAR(100) NULL DEFAULT NULL,"
            "  FeatureValue VARCHAR(100) NULL DEFAULT NULL,"
            "  DefColor VARCHAR(50) NULL DEFAULT NULL,"
            "  DefColorValue FLOAT NULL DEFAULT NULL,"
            "  DefClass_AutoReview VARCHAR(100) NULL DEFAULT NULL,"
            "  DefName_AutoReview VARCHAR(100) NULL DEFAULT NULL,"
            "  PointType VARCHAR(50) NULL DEFAULT NULL,"
            "  PRIMARY KEY (SysID) USING BTREE,"
            "  INDEX idx_GUID (GUID_IVS_LCD_InspectionResult) USING BTREE"
            ") ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = Compact";
        stmt->execute(createAOIDefect);
        Log("[DB] Table IVS_LCD_AOIResult ensured");
        
        // Insert sample data into ivs_lcd_idmap for fixtures 1-4 if empty
        std::unique_ptr<ResultSet> res(stmt->executeQuery("SELECT COUNT(*) FROM ivs_lcd_idmap"));
        if (res->next() && res->getInt(1) == 0) {
            for (int i = 1; i <= 4; i++) {
                std::ostringstream insertSample;
                insertSample << "INSERT INTO ivs_lcd_idmap (MarkID, UniqueID, Barcode, MainAoiFixID) VALUES ('"
                           << std::setw(2) << std::setfill('0') << i << "', '" << GenerateUniqueID() << "', 'SIM_BARCODE_" << i << "', '"
                           << i << "')";
                stmt->execute(insertSample.str());
            }
            Log("[DB] Sample data inserted into ivs_lcd_idmap");
        }
        
        // Insert sample defect data into IVS_LCD_AOIResult if empty (only if idmap has data)
        std::unique_ptr<ResultSet> resDefect(stmt->executeQuery("SELECT COUNT(*) FROM IVS_LCD_AOIResult"));
        if (resDefect->next() && resDefect->getInt(1) == 0) {
            std::unique_ptr<ResultSet> resIdmap(stmt->executeQuery("SELECT UniqueID FROM ivs_lcd_idmap"));
            int defectIdx = 1;
            while (resIdmap->next()) {
                std::string uniqueID = resIdmap->getString("UniqueID");
                // Insert 2 sample defects for each UniqueID
                for (int d = 1; d <= 2; d++) {
                    std::ostringstream insertDefect;
                    insertDefect << "INSERT INTO IVS_LCD_AOIResult ("
                                << "GUID_IVS_LCD_InspectionResult, DefectIndex, Type, PatternID, PatternName, "
                                << "Pos_x, Pos_y, Pos_width, Pos_height, TrueSize, "
                                << "GrayScale, GrayScale_BK, GrayScaleDiff, Code_AOI, Grade_AOI, "
                                << "DefClass_AOI, DefName_AOI, ImagePath"
                                << ") VALUES ("
                                << "'" << uniqueID << "', "
                                << defectIdx << ", "
                                << "'BrightDot', "
                                << d << ", "
                                << "'White', "
                                << (100 + d * 10) << ", "
                                << (200 + d * 10) << ", "
                                << "5, "
                                << "5, "
                                << "2.5, "
                                << "128, "
                                << "64, "
                                << "64, "
                                << "'BR01', "
                                << "'B', "
                                << "'Dot', "
                                << "'BrightDot_Type" << d << "', "
                                << "'C:\\\\LightingSimulator\\\\Images\\\\sample_defect.jpg'"
                                << ")";
                    stmt->execute(insertDefect.str());
                    defectIdx++;
                }
            }
            Log("[DB] Sample defect data inserted into IVS_LCD_AOIResult");
        }
    }
    catch (sql::SQLException& e) {
        Log("[DB] Table creation warning: %s", e.what());
    }
#endif
}

#ifdef USE_ODBC
// Print ODBC error
void PrintOdbcError(SQLHANDLE handle, SQLSMALLINT type) {
    SQLCHAR sqlState[6];
    SQLCHAR message[SQL_MAX_MESSAGE_LENGTH];
    SQLINTEGER nativeError;
    SQLSMALLINT length;
    
    SQLRETURN ret = SQLGetDiagRecA(type, handle, 1, sqlState, &nativeError, message, sizeof(message), &length);
    if (SQL_SUCCEEDED(ret)) {
        Log("ODBC Error: %s (SQL State: %s, Native Error: %d)", message, sqlState, nativeError);
    } else {
        Log("ODBC Error: Failed to get error information");
    }
}
#endif

// Database connection
bool InitDatabase() {
#ifdef USE_ODBC
    try {
        // Allocate environment handle
        SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &g_odbcEnv);
        if (!SQL_SUCCEEDED(ret)) {
            Log("Failed to allocate ODBC environment");
            return false;
        }
        
        // Set ODBC version
        ret = SQLSetEnvAttr(g_odbcEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);
        if (!SQL_SUCCEEDED(ret)) {
            PrintOdbcError(g_odbcEnv, SQL_HANDLE_ENV);
            SQLFreeHandle(SQL_HANDLE_ENV, g_odbcEnv);
            g_odbcEnv = SQL_NULL_HANDLE;
            return false;
        }
        
        // Allocate connection handle
        ret = SQLAllocHandle(SQL_HANDLE_DBC, g_odbcEnv, &g_odbcConn);
        if (!SQL_SUCCEEDED(ret)) {
            PrintOdbcError(g_odbcEnv, SQL_HANDLE_ENV);
            SQLFreeHandle(SQL_HANDLE_ENV, g_odbcEnv);
            g_odbcEnv = SQL_NULL_HANDLE;
            return false;
        }
        
        // Use the installed MySQL ODBC 5.3 driver
        const char* driverNames[] = {
            "MySQL ODBC 5.3 ANSI Driver",
            "MySQL ODBC 5.3 Unicode Driver",
            "MySQL ODBC 5.3 Driver"
        };
        
        bool connected = false;
        
        for (int i = 0; i < sizeof(driverNames) / sizeof(driverNames[0]); i++) {
            // Free and reallocate connection handle for each attempt
            if (g_odbcConn != SQL_NULL_HANDLE) {
                SQLDisconnect(g_odbcConn);
                SQLFreeHandle(SQL_HANDLE_DBC, g_odbcConn);
            }
            
            ret = SQLAllocHandle(SQL_HANDLE_DBC, g_odbcEnv, &g_odbcConn);
            if (!SQL_SUCCEEDED(ret)) {
                Log("[DB] Failed to allocate ODBC connection handle");
                continue;
            }
            
            Log("[DB] Trying driver: %s", driverNames[i]);
            
            // Build connection string - add PORT parameter
            char connStr[512];
            sprintf_s(connStr, sizeof(connStr), 
                     "DRIVER={%s};SERVER=%s;PORT=%d;DATABASE=%s;UID=%s;PWD=%s;OPTION=3;",
                     driverNames[i], Config::DB_HOST.c_str(), Config::DB_PORT, Config::DB_NAME.c_str(),
                     Config::DB_USER.c_str(), Config::DB_PASSWORD.c_str());
            
            Log("[DB] Connection string: %s", connStr);
            
            // Use SQLDriverConnectA
            ret = SQLDriverConnectA(g_odbcConn, NULL, (SQLCHAR*)connStr, SQL_NTS,
                                   NULL, 0, NULL, SQL_DRIVER_NOPROMPT);
            
            if (SQL_SUCCEEDED(ret)) {
                Log("[DB] Successfully connected with driver: %s", driverNames[i]);
                connected = true;
                break;
            } else {
                Log("[DB] Driver %s failed, trying next...", driverNames[i]);
                PrintOdbcError(g_odbcConn, SQL_HANDLE_DBC);
            }
        }
        
        if (!connected) {
            if (g_odbcConn != SQL_NULL_HANDLE) {
                SQLFreeHandle(SQL_HANDLE_DBC, g_odbcConn);
                g_odbcConn = SQL_NULL_HANDLE;
            }
            SQLFreeHandle(SQL_HANDLE_ENV, g_odbcEnv);
            g_odbcEnv = SQL_NULL_HANDLE;
            return false;
        }
        
        Log("Database connected: %s:%d/%s", Config::DB_HOST.c_str(), Config::DB_PORT, Config::DB_NAME.c_str());
        CreateTablesIfNotExist();
        return true;
    } catch (const std::exception& e) {
        Log("Database connection failed: %s", e.what());
        if (g_odbcConn != SQL_NULL_HANDLE) {
            SQLFreeHandle(SQL_HANDLE_DBC, g_odbcConn);
            g_odbcConn = SQL_NULL_HANDLE;
        }
        if (g_odbcEnv != SQL_NULL_HANDLE) {
            SQLFreeHandle(SQL_HANDLE_ENV, g_odbcEnv);
            g_odbcEnv = SQL_NULL_HANDLE;
        }
        return false;
    }
#else
    try {
        //sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
        sql::Driver* driver = sql::mysql::get_driver_instance();   // ✅ 1.0.5 唯一正确写法
        std::string url = Config::DB_HOST + ":" + std::to_string(Config::DB_PORT);
        
        // For MySQL 5.1 compatibility, use the simple connect method
        g_dbConnection.reset(driver->connect(url, Config::DB_USER, Config::DB_PASSWORD));
        
        // Select database
        g_dbConnection->setSchema(Config::DB_NAME);
        
        Log("Database connected: %s:%d/%s", Config::DB_HOST.c_str(), Config::DB_PORT, Config::DB_NAME.c_str());
        
        // Ensure required tables exist
        CreateTablesIfNotExist();
        
        return true;
    }
    catch (sql::SQLException& e) {
        Log("Database connection failed: %s", e.what());
        return false;
    }
#endif
}

void CloseDatabase() {
#ifdef USE_ODBC
    if (g_odbcConn != SQL_NULL_HANDLE) {
        SQLDisconnect(g_odbcConn);
        SQLFreeHandle(SQL_HANDLE_DBC, g_odbcConn);
        g_odbcConn = SQL_NULL_HANDLE;
    }
    if (g_odbcEnv != SQL_NULL_HANDLE) {
        SQLFreeHandle(SQL_HANDLE_ENV, g_odbcEnv);
        g_odbcEnv = SQL_NULL_HANDLE;
    }
#else
    if (g_dbConnection) {
        g_dbConnection->close();
        g_dbConnection.reset();
    }
#endif
}

// Insert inspection result to database
bool InsertInspectionResult(const std::string& barcode, int fixtureNo, DefectType defect, const std::string& uniqueID) {
#ifdef USE_ODBC
    if (g_odbcConn == SQL_NULL_HANDLE) {
        Log("[DB] Database not connected, skipping insert");
        return false;
    }

    std::lock_guard<std::mutex> lock(g_odbcMutex);
    
    try {
        // 使用传入的 uniqueID，而不是自己生成
        // 如果传入的 uniqueID 为空，则生成一个新的（兼容旧逻辑）
        std::string finalUniqueID = uniqueID.empty() ? GenerateUniqueID() : uniqueID;
        
        std::string screenID = barcode.empty() ? ("BARCODE_" + std::to_string(fixtureNo)) : barcode;
        std::string markID = std::to_string(fixtureNo);
        
        SQLHSTMT stmt = SQL_NULL_HANDLE;
        SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_STMT, g_odbcConn, &stmt);
        if (!SQL_SUCCEEDED(ret)) {
            PrintOdbcError(g_odbcConn, SQL_HANDLE_DBC);
            return false;
        }
        
        // Check if ivs_lcd_idmap has this fixture
        {
            std::ostringstream queryIdmap;
            queryIdmap << "SELECT Barcode, MarkID FROM ivs_lcd_idmap WHERE MainAoiFixID = '" << fixtureNo << "' LIMIT 1";
            
            ret = SQLExecDirectA(stmt, (SQLCHAR*)queryIdmap.str().c_str(), SQL_NTS);
            if (SQL_SUCCEEDED(ret)) {
                SQLCHAR barcodeBuf[129];
                SQLCHAR markIDBuf[16];
                SQLLEN lenBarcode, lenMarkID;
                
                ret = SQLFetch(stmt);
                if (SQL_SUCCEEDED(ret)) {
                    ret = SQLGetData(stmt, 1, SQL_C_CHAR, barcodeBuf, sizeof(barcodeBuf), &lenBarcode);
                    if (SQL_SUCCEEDED(ret) && lenBarcode > 0) {
                        screenID = (char*)barcodeBuf;
                    }
                    
                    ret = SQLGetData(stmt, 2, SQL_C_CHAR, markIDBuf, sizeof(markIDBuf), &lenMarkID);
                    if (SQL_SUCCEEDED(ret) && lenMarkID > 0) {
                        markID = (char*)markIDBuf;
                    }
                }
            }
            
            // Release statement after SELECT to avoid cursor state issues
            SQLFreeHandle(SQL_HANDLE_STMT, stmt);
            stmt = SQL_NULL_HANDLE;
        }
        
        // Create new statement for INSERT operations
        ret = SQLAllocHandle(SQL_HANDLE_STMT, g_odbcConn, &stmt);
        if (!SQL_SUCCEEDED(ret)) {
            PrintOdbcError(g_odbcConn, SQL_HANDLE_DBC);
            return false;
        }
        
        std::string modelName = "SIM_MODEL";
        std::string deviceID = "SIM_DEVICE";
        
        std::string startTime = GetCurrentTimestamp();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::string stopTime = GetCurrentTimestamp();
        
        const char* aoiResult = DefectTypeToString(defect);
        const char* status = (defect == DEFECT_OK) ? "Normal" : "Exception";
        
        // Insert into IVS_LCD_InspectionResult (using UniqueID as GUID for compatibility)
        std::ostringstream insertSQL;
        insertSQL << "INSERT INTO IVS_LCD_InspectionResult ("
                  << "GUID, ScreenID, DeviceID, PlatformID, ModelName, "
                  << "StartTime, StopTime, Status, AOIResult, "
                  << "MarkID, UniqueID, MainAoiFixID, "
                  << "OperatorID, L255_Grayscale, L0_Grayscale, "
                  << "Code_AOI, Grade_AOI, Level_AOI, "
                  << "DefClass_AOI, DefName_AOI"
                  << ") VALUES ("
                  << "'" << finalUniqueID << "', "
                  << "'" << screenID << "', "
                  << "'" << deviceID << "', "
                  << (fixtureNo - 1) << ", "
                  << "'" << modelName << "', "
                  << "'" << startTime << "', "
                  << "'" << stopTime << "', "
                  << "'" << status << "', "
                  << "'" << aoiResult << "', "
                  << "'" << std::setw(2) << std::setfill('0') << fixtureNo << "', "
                  << "'" << finalUniqueID << "', "
                  << "'" << fixtureNo << "', "
                  << "'SIM_OPERATOR', "
                  << "255, "
                  << "0, "
                  << "'" << aoiResult << "', "
                  << "'B', "
                  << "'1', "
                  << "'" << aoiResult << "', "
                  << "'Default'"
                  << ")";
        
        ret = SQLExecDirectA(stmt, (SQLCHAR*)insertSQL.str().c_str(), SQL_NTS);
        if (!SQL_SUCCEEDED(ret)) {
            PrintOdbcError(stmt, SQL_HANDLE_STMT);
            SQLFreeHandle(SQL_HANDLE_STMT, stmt);
            return false;
        }
        
        // Insert defect data into ivs_lcd_aoiresult (using UniqueID as GUID)
        if (defect != DEFECT_OK) {
            std::ostringstream insertDefect;
            insertDefect << "INSERT INTO ivs_lcd_aoiresult ("
                        << "GUID_IVS_LCD_InspectionResult, DefectIndex, Type, PatternID, PatternName, "
                        << "Pos_x, Pos_y, Pos_width, Pos_height, TrueSize, "
                        << "GrayScale, GrayScale_BK, GrayScaleDiff, "
                        << "Code_AOI, Grade_AOI, DefClass_AOI, DefName_AOI, ImagePath"
                        << ") VALUES ("
                        << "'" << finalUniqueID << "', "
                        << "1, "
                        << "'" << aoiResult << "', "
                        << "1, "
                        << "'White', "
                        << "100, "
                        << "200, "
                        << "5, "
                        << "5, "
                        << "2.5, "
                        << "128, "
                        << "64, "
                        << "64, "
                        << "'" << aoiResult << "', "
                        << "'B', "
                        << "'" << aoiResult << "', "
                        << "'Default_" << aoiResult << "', "
                        << "'C:\\\\LightingSimulator\\\\Images\\\\sample_defect.jpg'"
                        << ")";
            
            ret = SQLExecDirectA(stmt, (SQLCHAR*)insertDefect.str().c_str(), SQL_NTS);
            if (!SQL_SUCCEEDED(ret)) {
                PrintOdbcError(stmt, SQL_HANDLE_STMT);
                SQLFreeHandle(SQL_HANDLE_STMT, stmt);
                return false;
            }
            Log("[DB] Inserted defect for UniqueID=%s, Type=%s", finalUniqueID.c_str(), aoiResult);
        }
        
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        Log("Inserted result: Fixture=%02d, Result=%s, GUID=%s", fixtureNo, aoiResult, finalUniqueID.c_str());
        return true;
    }
    catch (const std::exception& e) {
        Log("[DB] ERROR: Insert result failed: %s", e.what());
        return false;
    }
#else
    if (!g_dbConnection) {
        Log("[DB] Database not connected, skipping insert");
        return false;
    }

    std::lock_guard<std::mutex> lock(g_dbMutex);
    
    try {
        std::unique_ptr<Statement> stmt(g_dbConnection->createStatement());
        
        // 使用传入的 uniqueID，而不是自己生成
        // 如果传入的 uniqueID 为空，则生成一个新的（兼容旧逻辑）
        std::string finalUniqueID = uniqueID.empty() ? GenerateUniqueID() : uniqueID;
        
        std::string screenID = barcode.empty() ? ("BARCODE_" + std::to_string(fixtureNo)) : barcode;
        std::string markID = std::to_string(fixtureNo);
        
        // Check if ivs_lcd_idmap has this fixture
        std::unique_ptr<ResultSet> resIdmap(stmt->executeQuery(
            "SELECT Barcode, MarkID FROM ivs_lcd_idmap WHERE MainAoiFixID = " + std::to_string(fixtureNo) + " LIMIT 1"));
        if (resIdmap->next()) {
            screenID = resIdmap->getString("Barcode");
            markID = resIdmap->getString("MarkID");
        }
        
        std::string modelName = "SIM_MODEL";
        std::string deviceID = "SIM_DEVICE";
        
        std::string startTime = GetCurrentTimestamp();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::string stopTime = GetCurrentTimestamp();
        
        const char* aoiResult = DefectTypeToString(defect);
        const char* status = (defect == DEFECT_OK) ? "Normal" : "Exception";
        
        // Insert into IVS_LCD_InspectionResult (using UniqueID as GUID for compatibility)
        std::ostringstream insertSQL;
        insertSQL << "INSERT INTO IVS_LCD_InspectionResult ("
                  << "GUID, ScreenID, DeviceID, PlatformID, ModelName, "
                  << "StartTime, StopTime, Status, AOIResult, "
                  << "MarkID, UniqueID, MainAoiFixID, "
                  << "OperatorID, L255_Grayscale, L0_Grayscale, "
                  << "Code_AOI, Grade_AOI, Level_AOI, "
                  << "DefClass_AOI, DefName_AOI"
                  << ") VALUES ("
                  << "'" << finalUniqueID << "', "
                  << "'" << screenID << "', "
                  << "'" << deviceID << "', "
                  << (fixtureNo - 1) << ", "
                  << "'" << modelName << "', "
                  << "'" << startTime << "', "
                  << "'" << stopTime << "', "
                  << "'" << status << "', "
                  << "'" << aoiResult << "', "
                  << "'" << std::setw(2) << std::setfill('0') << fixtureNo << "', "
                  << "'" << finalUniqueID << "', "
                  << "'" << fixtureNo << "', "
                  << "'SIM_OPERATOR', "
                  << "255, "
                  << "0, "
                  << "'" << aoiResult << "', "
                  << "'B', "
                  << "'1', "
                  << "'" << aoiResult << "', "
                  << "'Default'"
                  << ")";
        
        stmt->execute(insertSQL.str());
        
        // Insert defect data into ivs_lcd_aoiresult (using UniqueID as GUID)
        if (defect != DEFECT_OK) {
            std::ostringstream insertDefect;
            insertDefect << "INSERT INTO ivs_lcd_aoiresult ("
                        << "GUID_IVS_LCD_InspectionResult, DefectIndex, Type, PatternID, PatternName, "
                        << "Pos_x, Pos_y, Pos_width, Pos_height, TrueSize, "
                        << "GrayScale, GrayScale_BK, GrayScaleDiff, "
                        << "Code_AOI, Grade_AOI, DefClass_AOI, DefName_AOI, ImagePath"
                        << ") VALUES ("
                        << "'" << finalUniqueID << "', "
                        << "1, "
                        << "'" << aoiResult << "', "
                        << "1, "
                        << "'White', "
                        << "100, "
                        << "200, "
                        << "5, "
                        << "5, "
                        << "2.5, "
                        << "128, "
                        << "64, "
                        << "64, "
                        << "'" << aoiResult << "', "
                        << "'B', "
                        << "'" << aoiResult << "', "
                        << "'Default_" << aoiResult << "', "
                        << "'C:\\\\LightingSimulator\\\\Images\\\\sample_defect.jpg'"
                        << ")";
            stmt->execute(insertDefect.str());
            Log("[DB] Inserted defect for UniqueID=%s, Type=%s", finalUniqueID.c_str(), aoiResult);
        }
        
        Log("Inserted result: Fixture=%02d, Result=%s, GUID=%s", fixtureNo, aoiResult, finalUniqueID.c_str());
        return true;
    }
    catch (sql::SQLException& e) {
        Log("[DB] Database insert error: %s", e.what());
        return false;
    }
#endif
}

// Generate test result for multiple fixtures
// Format: FN$13141516@ (compact, no $ separators between fixture results)
std::string GenerateTestResult(const std::string& currentFixtures) {
    std::ostringstream result;
    result << "FN$";
    result << currentFixtures;  // 直接拼接，如 "13141516"
    result << "@";
    return result.str();
}

// Parse fixtures from currentFixtures string
std::vector<int> ParseFixtures(const std::string& currentFixtures) {
    std::vector<int> fixtures;
    for (size_t i = 0; i < currentFixtures.length(); i += 2) {
        std::string twoDigits = currentFixtures.substr(i, 2);
        int fixture = std::stoi(twoDigits);
        if (fixture > 0) {
            fixtures.push_back(fixture);
        }
    }
    return fixtures;
}

// Random delay helper
int RandomDelay(int minMs, int maxMs) {
    return minMs + rand() % (maxMs - minMs + 1);
}

// Query UniqueID for a fixture from ivs_lcd_idmap table
std::string QueryUniqueIDFromDatabase(int fixtureNo) {
#ifdef USE_ODBC
    if (g_odbcConn == SQL_NULL_HANDLE) {
        Log("[DB] Database not connected, cannot query UniqueID");
        return "";
    }
    
    std::lock_guard<std::mutex> lock(g_odbcMutex);
    
    try {
        SQLHSTMT stmt = SQL_NULL_HANDLE;
        SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_STMT, g_odbcConn, &stmt);
        if (!SQL_SUCCEEDED(ret)) {
            PrintOdbcError(g_odbcConn, SQL_HANDLE_DBC);
            return "";
        }
        
        std::ostringstream query;
        query << "SELECT UniqueID FROM ivs_lcd_idmap WHERE MainAoiFixID = '" << fixtureNo << "' LIMIT 1";
        
        ret = SQLExecDirectA(stmt, (SQLCHAR*)query.str().c_str(), SQL_NTS);
        if (!SQL_SUCCEEDED(ret)) {
            PrintOdbcError(stmt, SQL_HANDLE_STMT);
            SQLFreeHandle(SQL_HANDLE_STMT, stmt);
            return "";
        }
        
        SQLCHAR uniqueIDBuf[101];
        SQLLEN len;
        
        ret = SQLFetch(stmt);
        if (SQL_SUCCEEDED(ret)) {
            ret = SQLGetData(stmt, 1, SQL_C_CHAR, uniqueIDBuf, sizeof(uniqueIDBuf), &len);
            if (SQL_SUCCEEDED(ret) && len > 0) {
                std::string result((char*)uniqueIDBuf);
                Log("[DB] Query UniqueID for Fixture %02d: %s", fixtureNo, result.c_str());
                SQLFreeHandle(SQL_HANDLE_STMT, stmt);
                return result;
            }
        }
        
        Log("[DB] WARNING: No UniqueID found for Fixture %02d in ivs_lcd_idmap", fixtureNo);
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return "";
    } catch (const std::exception& e) {
        Log("[DB] ERROR: Query UniqueID failed: %s", e.what());
        return "";
    }
#else
    if (!g_dbConnection) {
        Log("[DB] Database not connected, cannot query UniqueID");
        return "";
    }
    
    std::lock_guard<std::mutex> lock(g_dbMutex);
    
    try {
        std::unique_ptr<Statement> stmt(g_dbConnection->createStatement());
        std::unique_ptr<ResultSet> res(stmt->executeQuery(
            "SELECT UniqueID FROM ivs_lcd_idmap WHERE MainAoiFixID = " + std::to_string(fixtureNo) + " LIMIT 1"));
        if (res->next()) {
            std::string uniqueID = res->getString("UniqueID");
            Log("[DB] Query UniqueID for Fixture %02d: %s", fixtureNo, uniqueID.c_str());
            return uniqueID;
        } else {
            Log("[DB] WARNING: No UniqueID found for Fixture %02d in ivs_lcd_idmap", fixtureNo);
            return "";
        }
    } catch (sql::SQLException& e) {
        Log("[DB] ERROR: Query UniqueID failed: %s", e.what());
        return "";
    }
#endif
}

// Handle client request
void HandleClient(std::shared_ptr<ClientInfo> client) {
    char buffer[Config::BUFFER_SIZE];
    
    Log("Client connected: %s:%d", client->address.c_str(), client->port);
    
    while (client->runningFlag) {
#ifdef _WIN32
        DWORD timeout = Config::TIMEOUT_SEC * 1000;
        setsockopt(client->socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
#else
        struct timeval timeout;
        timeout.tv_sec = Config::TIMEOUT_SEC;
        timeout.tv_usec = 0;
        setsockopt(client->socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
#endif
        
        int recvLen = recv(client->socket, buffer, Config::BUFFER_SIZE - 1, 0);
        
        if (recvLen > 0) {
            buffer[recvLen] = '\0';
            std::string data(buffer);

            std::string cleaned;
            for (char c : data) {
                if (c != '\r' && c != '\n' && c != ' ') {
                    cleaned += c;
                }
            }

            // 收到原始数据日志
            Log("[RECV] Raw data (%d bytes): [%s]", recvLen, data.c_str());
            Log("[RECV] Cleaned data: [%s]", cleaned.c_str());

            std::string response;

            // ========== DETAILED DEBUG LOGGING ==========
            Log("[DEBUG] Checking for 'Start$' prefix...");
            Log("[DEBUG] cleaned.find(\"Start$\") result: %d", (int)cleaned.find("Start$"));

            if (cleaned.find("Start$") == 0) {
                Log("[DEBUG] FOUND 'Start$' prefix, starting parse...");

                // Print ALL bytes from cleaned for debugging
                Log("[DEBUG] Raw bytes (%zu total):", cleaned.size());
                std::string byteLog;
                for (size_t i = 0; i < cleaned.size(); i++) {
                    char c = cleaned[i];
                    byteLog += (c >= 32 && c < 127) ? c : '.';
                    byteLog += " ";
                }
                Log("[DEBUG]     ASCII: [%s]", byteLog.c_str());
                std::string hexLog;
                for (size_t i = 0; i < cleaned.size(); i++) {
                    char buf[4];
                    snprintf(buf, sizeof(buf), "%02X ", (unsigned char)cleaned[i]);
                    hexLog += buf;
                }
                Log("[DEBUG]     HEX:   [%s]", hexLog.c_str());

                // Find first '$' right after "Start" (should be at position 5)
                size_t firstDollar = cleaned.find('$', 5);
                Log("[DEBUG] First '$' at position: %zu", firstDollar);

                if (firstDollar != std::string::npos) {
                    // Look for second '$' after the first one
                    size_t secondDollar = cleaned.find('$', firstDollar + 1);
                    size_t atPos = cleaned.find('@');
                    Log("[DEBUG] secondDollar: %zu, atPos: %zu", secondDollar, atPos);

                    // Method 1: Standard format Start$XX$XX@ (two $ separators)
                    if (secondDollar != std::string::npos && atPos != std::string::npos) {
                        Log("[DEBUG] Using 2-$ format (Start$XX$XX@)");

                        // Extract currentFixtures (between first $ and second $)
                        std::string currentFixtures;
                        for (size_t p = firstDollar + 1; p < secondDollar; p++) {
                            currentFixtures += cleaned[p];
                        }

                        // Extract maxFixtures (between second $ and @)
                        std::string maxFixtures;
                        for (size_t p = secondDollar + 1; p < atPos; p++) {
                            maxFixtures += cleaned[p];
                        }

                        Log("[DEBUG] currentFixtures = [%s]", currentFixtures.c_str());
                        Log("[DEBUG] maxFixtures = [%s]", maxFixtures.c_str());

                        g_testState.currentFixtures = currentFixtures;
                        g_testState.maxFixtures = maxFixtures;
                        g_testState.isTesting = true;
                        
                        // 清空之前的 UniqueID 映射
                        g_fixtureUniqueID.clear();
                        
                        // 查询数据库获取每个治具的 UniqueID
                        std::vector<int> fixtures = ParseFixtures(currentFixtures);
                        for (int fixture : fixtures) {
                            std::string uniqueID = QueryUniqueIDFromDatabase(fixture);
                            if (!uniqueID.empty()) {
                                g_fixtureUniqueID[fixture] = uniqueID;
                            }
                        }

                        Log("[TEST] Test STARTED - CurrentFixtures: %s, MaxFixtures: %s",
                            currentFixtures.c_str(), maxFixtures.c_str());

                        // ===== STEP 1: Send Running@ =====
                        Log("[STEP1] Waiting 100ms before sending Running@...");
                        std::this_thread::sleep_for(std::chrono::milliseconds(100));
                        response = "Running@";
                        int sentLen = send(client->socket, response.c_str(), (int)response.length(), 0);
                        Log("[STEP1] Sent Running@, bytes sent: %d", sentLen);

                        // ===== STEP 2: Simulate image capture =====
                        int captureDelay = RandomDelay(Config::MIN_IMAGE_DELAY, Config::MAX_IMAGE_DELAY);
                        Log("[STEP2] Simulating image capture, delay: %d ms...", captureDelay);
                        std::this_thread::sleep_for(std::chrono::milliseconds(captureDelay));
                        Log("[STEP2] Image capture DONE");

                        // ===== STEP 3: Send SnapFN@ =====
                        response = "SnapFN@";
                        sentLen = send(client->socket, response.c_str(), (int)response.length(), 0);
                        Log("[STEP3] Sent SnapFN@, bytes sent: %d", sentLen);

                        // ===== STEP 4: Simulate detection =====
                        int detectDelay = RandomDelay(Config::MIN_DETECT_DELAY, Config::MAX_DETECT_DELAY);
                        Log("[STEP4] Simulating defect detection, delay: %d ms...", detectDelay);
                        std::this_thread::sleep_for(std::chrono::milliseconds(detectDelay));
                        Log("[STEP4] Detection DONE");

                        // ===== STEP 5: Generate results and write to DB =====
                        Log("[STEP5] Generating test results...");
                        Log("[STEP5] Number of fixtures: %zu", fixtures.size());

                        for (size_t i = 0; i < fixtures.size(); i++) {
                            int fixture = fixtures[i];
                            DefectType defect = GenerateRandomDefect();
                            Log("[STEP5] Fixture %02d result: %s", fixture, DefectTypeToString(defect));
                            // 使用从数据库查询到的 UniqueID
                            std::string uniqueID = g_fixtureUniqueID.count(fixture) ? g_fixtureUniqueID[fixture] : "";
                            InsertInspectionResult("", fixture, defect, uniqueID);
                        }

                        // ===== STEP 6: Send FN result =====
                        response = GenerateTestResult(g_testState.currentFixtures);
                        Log("[STEP6] Generated result: [%s]", response.c_str());
                        sentLen = send(client->socket, response.c_str(), (int)response.length(), 0);
                        Log("[STEP6] Sent FN result, bytes sent: %d", sentLen);

                        Log("[TEST] Test COMPLETED");
                        g_testState.isTesting = false;
                    }
                    // Method 2: Compact format Start$XXXXXXXX@ (one $ separator, 8-char groups)
                    else if (firstDollar == 6 && atPos != std::string::npos) {
                        Log("[DEBUG] Using compact 1-$ format (Start$XXXXXXXX@)");

                        // Extract currentFixtures (8 chars after first $)
                        std::string currentFixtures;
                        for (size_t p = firstDollar + 1; p < firstDollar + 9 && p < atPos; p++) {
                            currentFixtures += cleaned[p];
                        }

                        // Extract maxFixtures (8 chars after currentFixtures)
                        std::string maxFixtures;
                        for (size_t p = firstDollar + 9; p < atPos; p++) {
                            maxFixtures += cleaned[p];
                        }

                        Log("[DEBUG] currentFixtures = [%s]", currentFixtures.c_str());
                        Log("[DEBUG] maxFixtures = [%s]", maxFixtures.c_str());

                        g_testState.currentFixtures = currentFixtures;
                        g_testState.maxFixtures = maxFixtures;
                        g_testState.isTesting = true;
                        
                        // 清空之前的 UniqueID 映射
                        g_fixtureUniqueID.clear();
                        
                        // 查询数据库获取每个治具的 UniqueID
                        std::vector<int> fixtures = ParseFixtures(currentFixtures);
                        for (int fixture : fixtures) {
                            std::string uniqueID = QueryUniqueIDFromDatabase(fixture);
                            if (!uniqueID.empty()) {
                                g_fixtureUniqueID[fixture] = uniqueID;
                            }
                        }

                        Log("[TEST] Test STARTED - CurrentFixtures: %s, MaxFixtures: %s",
                            currentFixtures.c_str(), maxFixtures.c_str());

                        // ===== STEP 1: Send Running@ =====
                        Log("[STEP1] Waiting 100ms before sending Running@...");
                        std::this_thread::sleep_for(std::chrono::milliseconds(100));
                        response = "Running@";
                        int sentLen = send(client->socket, response.c_str(), (int)response.length(), 0);
                        Log("[STEP1] Sent Running@, bytes sent: %d", sentLen);

                        // ===== STEP 2: Simulate image capture =====
                        int captureDelay = RandomDelay(Config::MIN_IMAGE_DELAY, Config::MAX_IMAGE_DELAY);
                        Log("[STEP2] Simulating image capture, delay: %d ms...", captureDelay);
                        std::this_thread::sleep_for(std::chrono::milliseconds(captureDelay));
                        Log("[STEP2] Image capture DONE");

                        // ===== STEP 3: Send SnapFN@ =====
                        response = "SnapFN@";
                        sentLen = send(client->socket, response.c_str(), (int)response.length(), 0);
                        Log("[STEP3] Sent SnapFN@, bytes sent: %d", sentLen);

                        // ===== STEP 4: Simulate detection =====
                        int detectDelay = RandomDelay(Config::MIN_DETECT_DELAY, Config::MAX_DETECT_DELAY);
                        Log("[STEP4] Simulating defect detection, delay: %d ms...", detectDelay);
                        std::this_thread::sleep_for(std::chrono::milliseconds(detectDelay));
                        Log("[STEP4] Detection DONE");

                        // ===== STEP 5: Generate results and write to DB =====
                        Log("[STEP5] Generating test results...");
                        Log("[STEP5] Number of fixtures: %zu", fixtures.size());

                        for (size_t i = 0; i < fixtures.size(); i++) {
                            int fixture = fixtures[i];
                            DefectType defect = GenerateRandomDefect();
                            Log("[STEP5] Fixture %02d result: %s", fixture, DefectTypeToString(defect));
                            // 使用从数据库查询到的 UniqueID
                            std::string uniqueID = g_fixtureUniqueID.count(fixture) ? g_fixtureUniqueID[fixture] : "";
                            InsertInspectionResult("", fixture, defect, uniqueID);
                        }

                        // ===== STEP 6: Send FN result =====
                        response = GenerateTestResult(g_testState.currentFixtures);
                        Log("[STEP6] Generated result: [%s]", response.c_str());
                        sentLen = send(client->socket, response.c_str(), (int)response.length(), 0);
                        Log("[STEP6] Sent FN result, bytes sent: %d", sentLen);

                        Log("[TEST] Test COMPLETED");
                        g_testState.isTesting = false;
                    }
                    else {
                        Log("[ERROR] Unrecognized Start$ format!");
                        Log("[ERROR] firstDollar=%zu, secondDollar=%zu, atPos=%zu",
                            firstDollar, secondDollar, atPos);
                        Log("[ERROR] Raw cleaned data: [%s]", cleaned.c_str());
                    }
                } else {
                    Log("[ERROR] First '$' separator NOT found!");
                }
            }
            else if (cleaned == "Running@") {
                Log("[CMD] Running@ query received");
                response = g_testState.isTesting ? "Running@" : "Idle@";
                int sentLen = send(client->socket, response.c_str(), (int)response.length(), 0);
                Log("[SEND] Sent: [%s], bytes: %d", response.c_str(), sentLen);
            }
            else if (cleaned == "SnapFN@") {
                Log("[CMD] SnapFN@ command received");
                response = "SnapFN@";
                int sentLen = send(client->socket, response.c_str(), (int)response.length(), 0);
                Log("[SEND] Sent: [%s], bytes: %d", response.c_str(), sentLen);
            }
            else {
                Log("[CMD] Unknown command received: [%s]", cleaned.c_str());
            }
        }
        else if (recvLen == 0) {
            Log("Client disconnected: %s:%d", client->address.c_str(), client->port);
            break;
        }
        else {
#ifdef _WIN32
            if (WSAGetLastError() == WSAETIMEDOUT) {
#else
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
#endif
                continue;
            }
            Log("Receive error: %s:%d", client->address.c_str(), client->port);
            break;
        }
    }
    
    CLOSE_SOCKET(client->socket);
    client->socket = INVALID_SOCKET;
    Log("Connection closed: %s:%d", client->address.c_str(), client->port);
}

// Cleanup disconnected clients
void CleanupClients() {
    std::lock_guard<std::mutex> lock(g_clients_mutex);
    auto it = g_clients.begin();
    while (it != g_clients.end()) {
        if ((*it)->socket == INVALID_SOCKET) {
            if ((*it)->thread && (*it)->thread->joinable()) {
                (*it)->thread->join();
                delete (*it)->thread;
            }
            it = g_clients.erase(it);
        }
        else {
            ++it;
        }
    }
}

// Initialize Winsock
bool InitWinsock() {
#ifdef _WIN32
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        Log("WSAStartup failed: %d", result);
        return false;
    }
#endif
    return true;
}

// Cleanup Winsock
void CleanupWinsock() {
#ifdef _WIN32
    WSACleanup();
#endif
}

// Main function
int main() {
    std::cout << "============================================================" << std::endl;
    std::cout << "    Lighting Test Server Simulator v1.0" << std::endl;
    std::cout << "============================================================" << std::endl;
    std::cout << std::endl;

    // Load config
    LoadConfig("config.ini");
    
    // Initialize database
    if (!InitDatabase()) {
        std::cout << "Warning: Database connection failed. Results will not be saved." << std::endl;
    }

    std::cout << "Protocol: Text-based TCP" << std::endl;
    std::cout << "Port: " << Config::SERVER_PORT << std::endl;
    std::cout << "Database: " << Config::DB_HOST << ":" << Config::DB_PORT << "/" << Config::DB_NAME << std::endl;
    std::cout << std::endl;
    std::cout << "Supported Commands:" << std::endl;
    std::cout << "  Start$XX$XX$XX$XX$YY$YY$YY$YY@ - Start detection" << std::endl;
    std::cout << "  Running@                           - Running status" << std::endl;
    std::cout << "  SnapFN@                            - Image capture complete" << std::endl;
    std::cout << "  FN$13141516@                        - Test result (compact, 4 fixtures)" << std::endl;
    std::cout << std::endl;
    std::cout << "Press Ctrl+C to stop server" << std::endl;
    std::cout << "============================================================" << std::endl;
    std::cout << std::endl;

#ifdef _WIN32
    // Register console close handler
    SetConsoleCtrlHandler(ConsoleCtrlHandler, TRUE);
#endif

    if (!InitWinsock()) {
        return 1;
    }

    SocketType serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        Log("socket create failed: %d", (int)WSAGetLastError());
        CleanupWinsock();
        return 1;
    }

    int opt = 1;
#ifdef _WIN32
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
#else
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#endif

    sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(Config::SERVER_IP.c_str());
    serverAddr.sin_port = htons(Config::SERVER_PORT);

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        Log("bind failed: %d", (int)WSAGetLastError());
        Log("Port %d may be in use", Config::SERVER_PORT);
        CLOSE_SOCKET(serverSocket);
        CleanupWinsock();
        return 1;
    }

    if (listen(serverSocket, 5) == SOCKET_ERROR) {
        Log("listen failed: %d", (int)WSAGetLastError());
        CLOSE_SOCKET(serverSocket);
        CleanupWinsock();
        return 1;
    }

    g_running = true;
    Log("============================================================");
    Log("Lighting Test Simulator Started");
    Log("Listen Address: %s:%d", Config::SERVER_IP.c_str(), Config::SERVER_PORT);
    Log("Database: %s:%d/%s", Config::DB_HOST.c_str(), Config::DB_PORT, Config::DB_NAME.c_str());
    Log("============================================================");

    while (g_running) {
#ifdef _WIN32
        DWORD timeout = 1000;
        setsockopt(serverSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
#else
        struct timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        setsockopt(serverSocket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
#endif

        sockaddr_in clientAddr;
        socklen_t addrLen = sizeof(clientAddr);
        SocketType clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &addrLen);

        if (clientSocket == INVALID_SOCKET) {
            if (g_running) continue;
            break;
        }

        auto client = std::make_shared<ClientInfo>();
        client->socket = clientSocket;
        client->runningFlag.store(true);

        char clientIP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN);
        client->address = clientIP;
        client->port = ntohs(clientAddr.sin_port);

        Log("Client connected from %s:%d", clientIP, ntohs(clientAddr.sin_port));

        client->thread = new std::thread(HandleClient, client);

        {
            std::lock_guard<std::mutex> lock(g_clients_mutex);
            g_clients.push_back(client);
        }

        CleanupClients();
    }

    Log("Stopping server...");
    g_running = false;

    {
        std::lock_guard<std::mutex> lock(g_clients_mutex);
        for (auto& client : g_clients) {
            if (client->socket != INVALID_SOCKET) {
                CLOSE_SOCKET(client->socket);
            }
            if (client->thread && client->thread->joinable()) {
                client->thread->join();
                delete client->thread;
            }
        }
        g_clients.clear();
    }

    CLOSE_SOCKET(serverSocket);
    CloseDatabase();
    CleanupWinsock();

    Log("Server stopped");

    return 0;
}
