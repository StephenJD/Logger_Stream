#pragma once
#include <iostream>
#include <string>
#include <iomanip>

enum Flags { L_default, L_concat, L_time, L_flush, L_endl, L_startWithFlushing, L_cout = 8, L_tabs = 16, L_allwaysFlush = 32 };
inline Flags operator +=(Flags & l_flag, Flags r_flag) { return l_flag = static_cast<Flags>(l_flag | r_flag); }
inline Flags operator -=(Flags & l_flag, Flags r_flag) { return l_flag = static_cast<Flags>(l_flag & ~r_flag); }

class Logger {
public:
	Logger() = default;

	Flags addFlag(Flags flag) { return _flags += flag; }
	Flags removeFlag(Flags flag) { return _flags -= flag; }
	int monthNo();
	int dayNo();

	virtual std::ostream & stream() = 0;
	virtual void flush() {}

	Logger& operator <<(Flags);
	Logger& operator <<(decltype(std::endl<char, std::char_traits<char>>) manip) {
		operator <<(L_endl); return *this;
	}
	Logger& operator <<(decltype(std::hex) manip) {
		stream() << manip; return *this;
	}	
	Logger& operator <<(decltype(std::setw) manip) {
		stream() << manip; return *this;
	}

protected:
	template<class T> friend Logger& operator <<(Logger& logger, T value);
	virtual bool is_tabs() { return false; }
	virtual void setBaseTimeTab(bool timeTab) {}
	virtual bool prePrint() { return false; }
	virtual Logger& logTime();
	bool is_cout() { return _flags & L_cout; }

	Flags _flags = L_startWithFlushing;
	bool _mustTabTime = false;
};

/// <summary>
/// Logs to any provided ostream - typically cout.
/// </summary>
class Cout_Logger : public Logger {
public:
	Cout_Logger(std::ostream& ostream);
	std::ostream& stream() override { return *_ostream; }
	void flush() override { stream().flush(); }
protected:
	bool is_tabs() override { return _mustTabTime ? (_mustTabTime = false, true) : _flags & L_tabs; }
	std::ostream* _ostream = 0;
};

// Streaming templates	
template<class T>
Logger& operator <<(Logger& logger, T value) {
	if (logger.prePrint()) {
		Cout_Logger std_out(static_cast<Cout_Logger&>(logger));
		std_out << value;
		logger.setBaseTimeTab(false);
	}
	if (logger.is_tabs()) 
		logger.stream() << "\t";
	logger.stream() << value;
	return logger;
}

Logger& logger(); // to be defined by the client