#include "Logging_cout.h"
#include <iostream>
#include <string>
#include <chrono>

using namespace std;

	Logger & Logger::operator <<(Flags flag) {
		switch (flag) {
		case L_flush: 
			_flags = static_cast<Flags>(_flags & L_allwaysFlush); // all zero's except L_allwaysFlush if set.
			(*this) << " |F|" << endl; flush();
			break;
		case L_endl: // fall through	
			if (_flags & L_allwaysFlush) { (*this) << " |F|"; }
			else if (_flags == L_startWithFlushing) { (*this) << " |SF|"; }
			if (prePrint()) {
				auto std_cout(static_cast<Cout_Logger&>(*this));
				std_cout.stream() << endl;
			}
			stream() << endl; // fall through
			if (_flags & L_allwaysFlush || _flags == L_startWithFlushing) flush();
		case L_default:
			if (_flags != L_startWithFlushing && flag != L_time) {
				_flags = static_cast<Flags>(_flags & L_allwaysFlush); // all zero's except L_allwaysFlush if set.
			}
			break;
		case L_allwaysFlush: _flags = L_allwaysFlush; break;
		case L_time:	logTime(); break;
		case L_concat:	removeFlag(L_tabs); break;
		default:
			addFlag(flag);
		}
		return *this;
	}

	Logger& Logger::logTime() {
		std::time_t now = std::time(nullptr);
		(*this) << std::put_time(std::localtime(&now), "%d/%m/%y %H:%M:%S");
		_mustTabTime = true;
		return *this;
	}

	int Logger::monthNo() {
		std::time_t now = std::time(nullptr);
		return std::localtime(&now)->tm_mon + 1;
	}

	int Logger::dayNo() {
		std::time_t now = std::time(nullptr);
		return std::localtime(&now)->tm_mday;
	}

////////////////////////////////////
//            Cout_Logger         //
////////////////////////////////////

	Cout_Logger::Cout_Logger(ostream& ostream) : _ostream(&ostream) {
		ostream.flush();
	}

