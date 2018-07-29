#define _FILE_OFFSET_BITS 64
 
#include "log.h"

#include <stdio.h>
#include <string.h>                  

#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)

CLog * CLog::s_pLog = NULL;


void  CLog::Release()
{
	if(s_pLog != NULL)
	{
		delete s_pLog;
	}
}

CLog * CLog::GetInstance()
{
	if(s_pLog == NULL)
	{
		s_pLog = new CLog;
	}
	return s_pLog;
}


CLog::CLog()
    :    m_bOutputScreen(false)
        ,m_bPrintMicroseconds(false)
        ,m_enuLogLevel(Info)
        ,m_bNameUseDate(false)
        ,m_nOpenFlag(0)
        ,m_nFds(NULL)
        ,m_bInited(false)
{
    m_pchFile[0] = '\0';
}

CLog::~CLog()
{
    if( m_bInited )
    {
        Log(System,__FILE__,__LINE__,"Logging System Stopped!");
        fclose(m_nFds);
         
        m_bInited = false;
    }
}

bool CLog::Init(const char * pchFile         
               ,const bool bOutputScreen     
               ,const Level enuLogLevel      
               ,const bool bNameUseDate      
               ,const int nOpenFlag          
               )
{
     
    sprintf_s(m_pchFile,pchFile);
    m_bOutputScreen = bOutputScreen;
    m_enuLogLevel = enuLogLevel;
    m_bNameUseDate = bNameUseDate;
     
    m_bInited = true;
    bool bRet = Log(System,__FILE__,__LINE__,"Logging System Started!");
    return bRet;
}

bool CLog::Lock()
{
	m_mutexLock.lock();
	return true;
}

bool CLog::Unlock()
{
	m_mutexLock.unlock();
    return true;
}

bool CLog::SetOutputScreen(bool bOut)
{
    if( m_bInited == false )
    {
        printf("{CLog::SetOutputScreen()}m_blnInited == false\n");
        return false;
    }
    if( Lock() == false )
    {
        printf("{CLog::SetOutputScreen()}Lock() == false\n");
        return false;
    }
     
    m_bOutputScreen = bOut;
     
    if( Unlock() == false )
    {
        printf("{CLog::SetOutputScreen()}Unlock() == false\n");
        return false;
    }
    return true;
}

bool CLog::SetPrintMicroseconds(bool bOut)
{
    if( m_bInited == false )
    {
        printf("{CLog::SetOutputScreen()}m_blnInited == false\n");
        return false;
    }
    if( Lock() == false )
    {
        printf("{CLog::SetOutputScreen()}Lock() == false\n");
        return false;
    }
     
    m_bPrintMicroseconds = bOut;
     
    if( Unlock() == false )
    {
        printf("{CLog::SetOutputScreen()}Unlock() == false\n");
        return false;
    }
    return true;
}

bool CLog::LockFile(const FILE * nFds,const int nCmd)const
{
    return true;
}

bool CLog::ReOpenFile(const CTime & dtCur)
{
    if( m_nFds != NULL )
    {
         
        if( fclose(m_nFds) != 0 )
        {
            printf("::close(m_nFds) != 0 {m_nFds=%p}\n",m_nFds);
            return false;
        }
        m_nFds = NULL;
    }
     
	std::string dir = current_process_dir() + "\\log\\";

    char pFileName[4096];
    sprintf_s(pFileName,"%s%s_%02d%02d%02d_socket.log",
            dir.c_str(),m_pchFile,(dtCur.GetYear() % 100),dtCur.GetMonth(),dtCur.GetDay());
	 
    m_nFds = fopen(pFileName, "a+");
    if( m_nFds == NULL )
    {
        printf("::open(pFileName,m_nOpenFlag,0777) == -1 {pFileName='%s',m_nOpenFlag=0x%x}\n",pFileName,m_nOpenFlag);
        return false;
    }

     
    return true;
}

bool CLog::_Log(const Level enuLevel, const char * pFile, const int intLineNo, const char *pMsg)
{
    if( enuLevel < m_enuLogLevel )
    { 
        return true;
    }
     
    if( Lock() == false )
    {
        return false;
    }
     
     
     
    const char *pLevel = NULL;
    switch( enuLevel )
    {
    case Info:
        pLevel = "I";
        break;
    case Warn:
        pLevel = "W";
        break;
    case Error:
        pLevel = "E";
        break;
    case Status:
        pLevel = "S";
        break;
     
     
     
    }
     
    CTime dtCur = CTime::GetCurrentTime();

    if (m_nFds == NULL ||
        dtCur.GetDay() != m_LastOpenTime.GetDay())
    {
        if( ReOpenFile(dtCur) == false )
        {
            return false;
        }
        else
        {
            m_LastOpenTime = dtCur;
        }
    }
     
    char buf[8192];
    _snprintf(buf,sizeof(buf),"[%02d:%02d:%02d][%s][%8lx][%16s][%8d] %s\n",
                        dtCur.GetHour(),
                        dtCur.GetMinute(),
                        dtCur.GetSecond(),
                        pLevel,
                         GetCurrentThreadId(),
                        pFile,
                        intLineNo,
                        pMsg
                    );
    Write(buf);
    if( Unlock() == false )
    {
        return false;
    }
    return true;
}

bool CLog::Log(const Level enuLevel, const char * pFile, const int intLineNo, const char *pFormat,va_list arglist)
{
    if( enuLevel < m_enuLogLevel )
    { 
        return true;
    }
     
    char    buffer[8192];
    vsnprintf(buffer,sizeof(buffer),pFormat,arglist);
    return _Log(enuLevel,pFile,intLineNo,buffer);
}

bool CLog::Log(const Level enuLevel, const char * pFile, const int intLineNo, const char *pFormat,...)
{
    if( enuLevel < m_enuLogLevel )
    { 
        return true;
    }
     
    va_list arglist;
    va_start(arglist, pFormat);
    char    buffer[8192];
    vsnprintf(buffer,sizeof(buffer),pFormat,arglist);
    return _Log(enuLevel,pFile,intLineNo,buffer);
}

bool CLog::_Log(const Level enuLevel, const char * pFile, const char * pFunc, const char *pMsg)
{
    if( enuLevel < m_enuLogLevel )
    { 
        return true;
    }
     
    if( Lock() == false )
    {
        return false;
    }
     
     
     
    const char *pLevel = NULL;
    switch( enuLevel )
    {
    case Info:
        pLevel = "I";
        break;
    case Warn:
        pLevel = "W";
        break;
    case Error:
        pLevel = "E";
        break;
    case Status:
        pLevel = "S";
        break;
     
     
     
    }
     
    CTime dtCur = CTime::GetCurrentTime();
    if (m_nFds == NULL ||
        dtCur.GetDay() != m_LastOpenTime.GetDay())
    {
        if( ReOpenFile(dtCur) == false )
        {
            return false;
        }
        else
        {
            m_LastOpenTime = dtCur;
        }
    }
     
    char buf[8192];

    _snprintf(buf,sizeof(buf),"[%02d:%02d:%02d][%s][%8lx][%16s][%8s] %s\n",
                        dtCur.GetHour(),
                        dtCur.GetMinute(),
                        dtCur.GetSecond(),
                        pLevel,
                         GetCurrentThreadId(),
                        pFile,
                        pFunc,
                        pMsg
                    );

    Write(buf);
     
     
     
     
     
     
    if( Unlock() == false )
    {
        return false;
    }
    return true;
}

bool CLog::Log(const Level enuLevel, const char * pFile, const char * pFunc, const char *pFormat,va_list arglist)
{
    if( enuLevel < m_enuLogLevel )
    { 
        return true;
    }
     
    char    buffer[8192];
    vsnprintf(buffer,sizeof(buffer),pFormat,arglist);
    return _Log(enuLevel,pFile,pFunc,buffer);
}

bool CLog::Log(const Level enuLevel, const char * pFile, const char * pFunc, const char *pFormat,...)
{
    if( enuLevel < m_enuLogLevel )
    { 
        return true;
    }
     
    va_list arglist;
    va_start(arglist, pFormat);
    char    buffer[8192];
    vsnprintf(buffer,sizeof(buffer),pFormat,arglist);
    return _Log(enuLevel,pFile,pFunc,buffer);
}


bool CLog::DumpBuf(const Level enuLevel
                ,const char *pchFile
                ,const char *pchFunc
                ,const char *pchTitle
                ,const char *pchBuf
                ,const int intBufSize
                ,const int intDumpSize
                )
{
    if( enuLevel < m_enuLogLevel )
    { 
        return true;
    }
     
    if( Lock() == false )
    {
        return false;
    }
     
     
     
    const char *pLevel = NULL;
    switch( enuLevel )
    {
    case Info:
        pLevel = "I";
        break;
    case Warn:
        pLevel = "W";
        break;
    case Error:
        pLevel = "E";
        break;
    case Status:
        pLevel = "S";
        break;
     
     
     
    }
     
    CTime dtCur = CTime::GetCurrentTime();

    if (m_nFds == NULL ||
        dtCur.GetDay() != m_LastOpenTime.GetDay())
    {
        if( ReOpenFile(dtCur) == false )
        {
            return false;
        }
        else
        {
            m_LastOpenTime = dtCur;
        }
    }
     
    char buf[8192];
    _snprintf(buf,sizeof(buf),"[%02d:%02d:%02d][%s][%8lx][%16s][%8s] %s{pchBuf=%p,intBufSize=%d,intDumpSize=%d}\n",
                        dtCur.GetHour(),
                        dtCur.GetMinute(),
                        dtCur.GetSecond(),
                        pLevel,
                         GetCurrentThreadId(),
                        pchFile,
                        pchFunc,
                        pchTitle,
                        pchBuf,
                        intBufSize,
                        intDumpSize
                    );
    Write(buf);
     
    _snprintf(buf,sizeof(buf),"---------------------------------------------------------\n");
    Write(buf);
    _snprintf(buf,sizeof(buf),"pchBuf=%8p,nSize=%d\n",pchBuf,intBufSize);
    Write(buf);
    _snprintf(buf,sizeof(buf),"---------------------------------------------------------\n");
    Write(buf);
    _snprintf(buf,sizeof(buf),"  |0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f--------\n");
    Write(buf);
     
    int intActualSize = intDumpSize;
    if( intActualSize < 16 )
    {
        intActualSize = 16;
    }
    else if( intActualSize > 4096 )
    {
        intActualSize = 4096;
    }
     
    char pchLineBuf[1024];
    int i;
    for( i = 0 ; (i < intBufSize) && (i < intActualSize) ; i++ )
    {
        sprintf(pchLineBuf + ((i%16) * 3),"%02x ",(const unsigned char)pchBuf[i]);
        if( i%16 == 15 )
        {
            _snprintf(buf,sizeof(buf),"%02x|%s\n",i/16,pchLineBuf);
            Write(buf);
        }
    }
    if( i%16 != 0 )
    {
        _snprintf(buf,sizeof(buf),"%02x|%s\n",i/16,pchLineBuf);
        Write(buf);
    }
    _snprintf(buf,sizeof(buf),"---------------------------------------------------------\n");
    Write(buf);
     
     
     
     
     
     
    if( Unlock() == false )
    {
        return false;
    }
    return true;
}

bool CLog::Write(const char* pBuf)
{
    int nRet = -1;
    nRet = fwrite(pBuf, strlen(pBuf), 1, m_nFds);
    if( nRet == -1 )
    {
        printf("::write(m_nFds,pBuf,strlen(pBuf)) == -1 {errno=%d,m_nFds=%d,pBuf=%8p,strlen(pBuf)=%d}\n",errno,m_nFds,pBuf,strlen(pBuf));
        return false;
    }
	
	fflush(m_nFds);
    return true;
}
