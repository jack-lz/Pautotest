#include "LOG.h"

#define MAX_PATH 200
using namespace std;

string CLog::g_logFileName = GetLogFilePath();

CLog::CLog()
{

}

CLog::~CLog()
{

}

void CLog::WriteLog(const char *format, ...)
{
    if (g_logFileName == "") {
        GetLogFilePath();
    }
    ofstream fout(g_logFileName,ios::app);
    fout.seekp(ios::end);
    fout << GetSystemTime();

    va_list ap;
    va_start(ap, format);
    char *index = (char *)format;

    while (*index)
    {
        if('%' == *index)
        {
            //获得下一个字符
            index++;
            switch(*index)
            {
            case 's':		//字符串
            {
                char *sTmp = va_arg(ap, char*);  // one by one
                while(*sTmp)
                {
                    fout << *sTmp;
                    sTmp++;
                }
                break;
            }
            case 'S':		//宽字符串
            {
                wchar_t *wsTmp = va_arg(ap, wchar_t*);
                while(*wsTmp)
                {
                    fout << *wsTmp;
                    wsTmp++;
                }

                break;
            }
            case 'c':		//字符
            {
                char cTmp = va_arg(ap, char);
                fout << cTmp;
                break;
            }
            case 'd':		//十进制
            {
                int dTmp = va_arg(ap, int);
                fout << dTmp;
                break;
            }
            case '0':		//二进制02X
            {
                char *cNext = index;
                if (('2' == *(++index)) && ('X' == *(++index)))
                {
                    int dTmp = va_arg(ap, int);
                    fout << dTmp;
                }
                break;
            }
            case 'f':		//浮点型
            {
                float fTmp = (float)va_arg(ap, double);
                fout << fTmp;
                break;
            }
            case 'l':		//LONGLONG型
            {
                char *cNext = index;
                if (('l' == *(++index)) && ('d' == *(++index)))
                {
                    long long lTmp = (long long)va_arg(ap, long long);
                    fout << lTmp;
                }
                break;
            }
            default :
                break;
            }
        }
        else
        {
            fout << *index;
        }
        index++;
    }

    fout << endl;
    fout.close();
}

template <class T>
void CLog::WriteFunc(T x)
{
    if (g_logFileName == "") {
        GetLogFilePath();
    }
    std::ofstream fout(g_logFileName,std::ios::app);
    fout.seekp(std::ios::end);
    fout << GetSystemTime() << "Fuction name : "<< x << std::endl;
    fout.close();
}

template <class T>
void CLog::WriteFuncBegin(T x)
{
    if (g_logFileName == "") {
        GetLogFilePath();
    }
    std::ofstream fout(g_logFileName,std::ios::app);
    fout.seekp(std::ios::end);
    fout << GetSystemTime() << "--------------------"<<x<<"  Begin--------------------" <<endl;
    fout.close();
}

template <class T>
void CLog::WriteFuncEnd(T x)
{
    if (g_logFileName == "") {
        GetLogFilePath();
    }
    ofstream fout(g_logFileName,std::ios::app);
    fout.seekp(std::ios::end);
    fout << GetSystemTime() << "--------------------"<<x<<"  End  --------------------" <<endl;
    fout.close();
}

string CLog::GetLogFilePath()
{
    char szWorkDir[MAX_PATH] = {0} ;
    if(!getcwd(szWorkDir, MAX_PATH))
    {
        return "";
    }


    string paraStr_CfgFileName(szWorkDir);
    paraStr_CfgFileName.append("/log") ;
    paraStr_CfgFileName.append(GetSystemTime()) ;
    paraStr_CfgFileName.append(".txt") ;
    return paraStr_CfgFileName;
}


string CLog::GetSystemTime()
{
    time_t tNowTime;
    time(&tNowTime);
    tm* tLocalTime = localtime(&tNowTime);
    char szTime[30] = {'\0'};
    strftime(szTime, 30, "[%Y-%m-%d %H:%M:%S]", tLocalTime);
    string strTime = szTime;
    return strTime;
}
