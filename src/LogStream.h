#pragma once

#ifndef __LOG_H_
#define __LOG_H_

#include <iostream>
#include <sstream>
#include "Windows.h"

enum LogLevel { LOG_ERROR , LOG_DEBUG, };

class LogStream 
{

	private:
		LogStream();

	public:

		LogStream(LogLevel level) { 

			std::string level_str;

			switch (level) {
				case LOG_ERROR:
					level_str = "[E]";
					break;
				case LOG_DEBUG:
					level_str = "[D]";
			}

			ss << "[SLOG]"<<level_str<<" - "; 
		}

	    ~LogStream() {
	      	OutputDebugString(ss.str().c_str()); 
	    }

	    // General for all types supported by stringstream
	    template<typename T>
	    LogStream& operator<<(const T& arg) {
	       ss << arg;
	       return *this;
	    }

    private:

	    std::stringstream ss;

};

#endif