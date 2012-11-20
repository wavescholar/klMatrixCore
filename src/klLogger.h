#pragma once 
using namespace System;
using namespace System::Runtime::InteropServices;
using namespace System::Xml;
using namespace System::Text;
using namespace log4net;

#include <string>

//How to expose in unmanaged code - 
//#include vcclr.h  gcroot<LogWrapper ^> managedLogger

namespace InteropLogger
{
	public ref class LogWrapper 
    {
	public: 
		LogWrapper(Object^ type)
		{
			_log = LogManager::GetLogger( type->GetType() );
			_log->Info("");
		}

		//bbcrevisit remove for V2
		ILog^ getLogger()
		{
			return _log;
		}
		
		void logDebug (std::string msg)
		{
			_log->Debug(gcnew String(msg.c_str()) );

		}
		
		void logError(std::string msg)
		{
			_log->Error(gcnew String(msg.c_str() ) );
		}

		void logInfo(std::string msg)
		{
			_log->Info(gcnew String(msg.c_str() ) );
		}

		void logPerformance(std::string msg)
		{
			_log->Info(gcnew String(msg.c_str() ) );
		}

	protected:

		ILog^ _log;

    };
};


