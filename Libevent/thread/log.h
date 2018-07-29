#ifndef CLOG_H
#define CLOG_H
#include "mutex.h"

#include<atltime.h>
#include <stdarg.h>          

static std::string current_process_path()
{
	std::string filePath;

	char szPath[MAX_PATH];

	//¨º¡ì¡ã¨¹¡¤¦Ì??0
	if (!GetModuleFileNameA(NULL, szPath, MAX_PATH))
	{
		return filePath;
	}
	filePath = szPath;

	return filePath;
}

static std::string current_process_dir()
{
	std::string filePath = current_process_path();
	int pos = filePath.find_last_of('\\');
	int len = filePath.size();

	return filePath.substr(0, pos);
}

class   CLog
{
public:
    enum Level  {   Info = 1, Warn, Error, Status, System = Status };

 
public:
	static CLog * GetInstance();
	static void  Release();
    virtual ~CLog();
     
    bool Init(const char * pchFile              
            ,const bool bOutputScreen = true
            ,const Level enuLogLevel = Info  
            ,const bool bNameUseDate = true 
            ,const int nOpenFlag = 0    
              );
     
    bool Log(const Level enuLevel, const char * pFile, const int intLineNo, const char *pFormat,...);
    bool Log(const Level enuLevel, const char * pFile, const int intLineNo, const char *pFormat,va_list arglist);
    bool Log(const Level enuLevel, const char * pFile, const char * pFunc, const char *pFormat,...);
    bool Log(const Level enuLevel, const char * pFile, const char * pFunc, const char *pFormat,va_list arglist);
    bool DumpBuf(const Level enuLevel
                ,const char *pchFile
                ,const char *chpFunc
                ,const char *pchTitle
                ,const char *pchBuf
                ,const int intBufSize
                ,const int intDumpSize
                );
     
    bool SetOutputScreen(bool bOut);
    bool SetPrintMicroseconds(bool bOut);
    bool LockFile(const int nCmd) {   return LockFile(m_nFds,nCmd);    };

private:
	CLog();
    bool _Log(const Level enuLevel, const char * pFile, const int intLineNo, const char *pMsg);
    bool _Log(const Level enuLevel, const char * pFile, const char * pFunc, const char *pMsg);
    bool ReOpenFile(const CTime & dtCur);
    bool Write(const char* pBuf);
    bool Lock();
    bool Unlock();
     
    bool LockFile(const FILE * nFds,const int nCmd)const;

     
private:
    char            m_pchFile[4096];         
    bool            m_bOutputScreen;         
    bool            m_bPrintMicroseconds;    
    Level           m_enuLogLevel;           
    bool            m_bNameUseDate;          
    int             m_nOpenFlag;             
    CTime			m_LastOpenTime;
     
    FILE *			m_nFds;
    bool            m_bInited;
	base::win32::thread::Mutex			m_mutexLock;

     
public:
	 static  CLog *   s_pLog;
};

					 
#define EM_LOG_STATUS(__fmt, ...) CLog::GetInstance()->Log(CLog::Status, __FILE__, __FUNCTION__, __fmt, ##__VA_ARGS__)
#define EM_LOG_WARN(__fmt, ...)  CLog::GetInstance()->Log(CLog::Warn, __FILE__, __FUNCTION__, __fmt, ##__VA_ARGS__)
#define EM_LOG_ERROR(__fmt, ...)  CLog::GetInstance()->Log(CLog::Error, __FILE__, __FUNCTION__, __fmt, ##__VA_ARGS__)
#define EM_LOG_INFO(__fmt, ...)  CLog::GetInstance()->Log(CLog::Info, __FILE__, __FUNCTION__, __fmt, ##__VA_ARGS__)

#endif  
