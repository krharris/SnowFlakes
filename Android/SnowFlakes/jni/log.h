#ifndef _GUILDHALL_LOG_H_
#define _GUILDHALL_LOG_H_

namespace guildhall {

class Log
{
public:

	static void error( const char* pMessage, ... );
	static void warn( const char* pMessage, ... );
	static void info( const char* pMessage, ... );
	static void debug( const char* pMessage, ... );
};

#ifndef NDEBUG
#define guildhall_Log_debug(...) guildhall::Log::debug(__VA_ARGS__)
#else
#define guildhall_Log_debug(...)
#endif

}
#endif // _GUILDHALL_LOG_H_
