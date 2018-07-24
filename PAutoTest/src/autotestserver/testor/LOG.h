/**  
 * 用于输出log文件的类.  
 */    
  
#ifndef LOG_H    
#define LOG_H    

#include <fstream>    
#include <string>    
#include <ctime>
#include <unistd.h>
#include <stdarg.h>
    
//启用开关
//#define LOG_ENABLE

    
class CLog    
{    
public:
    CLog();
    ~CLog();

    static void WriteLog(const char *format, ...);

    template <class T>
    static void WriteFunc(T x);

   
    template <class T>  
    static void WriteFuncBegin(T x);
   
    template <class T>  
    static void WriteFuncEnd(T x);

    static std::string GetLogFilePath();
      
private:
    static std::string g_logFileName;
    static std::string GetSystemTime();
};

  
#ifdef LOG_ENABLE  
   
#define LOG(format, ...)   CLog::WriteLog(format, ##__VA_ARGS__)   //括号内可以是字符串(ascii)、整数、浮点数、bool等
#define LOG_FUNC()         CLog::WriteFunc(__FUNCTION__)          //输出当前所在函数名
#define LOG_FUNC_BEGIN()   CLog::WriteFuncBegin(__FUNCTION__)    //形式如：[时间]"------------FuncName  Begin------------"
#define LOG_FUNC_END()     CLog::WriteFuncEnd(__FUNCTION__)      //形式如：[时间]"------------FuncName  End------------"
  
#else  
  
#define LOG(format, ...)   printf(format, ##__VA_ARGS__)
#define LOG_FUNC()         printf(__FUNCTION__)
#define LOG_FUNC_BEGIN()
#define LOG_FUNC_END()
  
#endif  
  
#endif    
