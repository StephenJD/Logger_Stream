#include "Logging_console.h"
#include <iostream>
#include <string>
#include <chrono>

using namespace std;
using namespace logging;

Logger & Logger::operator <<(Flags flag) {
	if (is_null()) return *this;
	switch (flag) {
	case L_time:	logTime(); break;
	case L_flush: 
		_flags = static_cast<Flags>(_flags & L_allwaysFlush); // all zero's except L_allwaysFlush if set.
		*this << " |F|\n"; 
		flush();
		break;
	case L_endl: {
			if (_flags & L_allwaysFlush) { *this << " |F|"; } else if (_flags == L_startWithFlushing) { *this << " |SF|"; }
			auto streamPtr = &stream();
			Logger* logger = this;
			do {
				*streamPtr << "\n";
				logger = logger->mirror_stream(streamPtr);
			} while (streamPtr);
			if (_flags & L_allwaysFlush || _flags == L_startWithFlushing) flush();
		}
		[[fallthrough]];
	case L_clearFlags:
		if (_flags != L_startWithFlushing) {
			_flags = static_cast<Flags>(_flags & L_allwaysFlush); // all zero's except L_allwaysFlush if set.
		}
		break;
	case L_allwaysFlush: _flags += L_allwaysFlush; break;
	case L_concat:	removeFlag(L_tabs); break;
	default:
		addFlag(flag);
	}
	return *this;
}

tm* Logger::getTime() {
	std::time_t now = std::time(nullptr);
	auto localTime = std::localtime(&now);
	log_date.dayNo = localTime->tm_mday;
	log_date.monthNo = localTime->tm_mon + 1;
	return localTime;
}

Logger& Logger::logTime() {
	*this << std::put_time(getTime(), "%d/%m/%y %H:%M:%S");
	_flags += L_time;
	return *this;
}