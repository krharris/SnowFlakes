#include "log.h"

#include <stdarg.h>
#include <android/log.h>

namespace guildhall {

void Log::info( const char* pMessage, ... )
{
	va_list lVarArgs;
	va_start( lVarArgs, pMessage );
	__android_log_vprint( ANDROID_LOG_INFO, "GUILDHALL", pMessage, lVarArgs );
	__android_log_print( ANDROID_LOG_INFO, "GUILDHALL", "\n" );
	va_end( lVarArgs );
}

void Log::error( const char* pMessage, ... )
{
	va_list lVarArgs;
	va_start( lVarArgs, pMessage );
	__android_log_vprint( ANDROID_LOG_ERROR, "GUILDHALL", pMessage, lVarArgs );
	__android_log_print( ANDROID_LOG_ERROR, "GUILDHALL", "\n" );
	va_end( lVarArgs );
}

void Log::warn( const char* pMessage, ... )
{
	va_list lVarArgs;
	va_start( lVarArgs, pMessage );
	__android_log_vprint( ANDROID_LOG_WARN, "GUILDHALL", pMessage, lVarArgs );
	__android_log_print( ANDROID_LOG_WARN, "GUILDHALL", "\n" );
	va_end( lVarArgs );
}

void Log::debug( const char* pMessage, ... )
{
	va_list lVarArgs;
	va_start( lVarArgs, pMessage );
	__android_log_vprint( ANDROID_LOG_DEBUG, "GUILDHALL", pMessage, lVarArgs );
	__android_log_print( ANDROID_LOG_DEBUG, "GUILDHALL", "\n" );
	va_end( lVarArgs );
}

}
