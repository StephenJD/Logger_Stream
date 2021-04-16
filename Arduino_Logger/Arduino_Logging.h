#pragma once
#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>
#include <time.h>
#include <version>

#define USING_ARDUINO

#ifdef USING_ARDUINO
#include <C:\Users\Stephen\Documents\MyData\Arduino\Arduino_SJD\VS_Projects\Std_Logger\Arduino_Stream\Stream_Arduino.h>
#define OStream Stream_Arduino
#else
#define OStream std::ostream
#endif

#ifdef __cpp_lib_source_location
#include <source_location>
#define L_location location()
#else
#define L_location location(__FILE__,__LINE__)
#endif

namespace logging {
	enum Flags { L_clearFlags, L_concat, L_time, L_flush, L_endl, L_startWithFlushing, L_null, L_cout = 8, L_tabs = 16, L_allwaysFlush = 32 };
	inline Flags operator +=(Flags& l_flag, Flags r_flag) { return l_flag = static_cast<Flags>(l_flag | r_flag); }
	inline Flags operator -=(Flags& l_flag, Flags r_flag) { return l_flag = static_cast<Flags>(l_flag & ~r_flag); }

#ifdef __cpp_lib_source_location
	inline std::string location(const std::source_location& location = std::source_location::current()) {
		auto ss = std::stringstream{};
		ss << location.file_name()
			<< " : " << location.line()
			<< " '" << location.function_name() << "'";
		return ss.str();
	}
#else
	inline std::string location(const char* file, int lineNo) {
		auto ss = std::stringstream{};
		ss << file << "\t" << lineNo;
		return ss.str();
	}
#endif	

	class Logger {
	public:
		void activate(bool makeActive = true) { makeActive ? _flags -= L_null : _flags += L_null; }
		Flags addFlag(Flags flag) { return _flags += flag; }
		Flags removeFlag(Flags flag) { return _flags -= flag; }
		virtual void flush() { stream().flush(); _flags -= L_startWithFlushing; }
		virtual bool open() { return false; }

		template<typename T>
		Logger& log(T value);

		Logger& operator <<(Flags);

		Logger& operator <<(decltype(std::endl<char, std::char_traits<char>>)) {
			return *this << L_endl;
		}

		Logger& operator <<(decltype(std::hex) manip) {
			stream() << manip; return *this;
		}

		Logger& operator <<(decltype(std::setw) manip) {
			stream() << manip; return *this;
		}

		virtual OStream& stream();

		using ostreamPtr = OStream*;
		virtual Logger* mirror_stream(ostreamPtr& mirrorStream) { mirrorStream = nullptr; return this; }

		virtual Logger& logTime();

		static tm* getTime();
		struct Log_date {
			unsigned char dayNo;
			unsigned char monthNo;
		} inline static log_date{ 0,0 };
	protected:
		Logger(Flags initFlag = L_null) : _flags{ initFlag } {}

		template<class T> friend Logger& operator <<(Logger& logger, T value);

		bool is_tabs() const { return _flags & L_tabs || has_time(); }
		bool is_null() const { return _flags == L_null; }
		bool is_cout() const { return _flags & L_cout; }
		bool has_time() const { return (_flags & 7) == L_time; }

		Flags _flags = L_startWithFlushing;
	};

	// Streaming template	
	template<typename T >
	Logger& operator <<(Logger& logger, T value) {
		return logger.log(value);
	}

	template<typename T>
	Logger& Logger::log(T value) {
		if (is_null()) return *this;
		auto streamPtr = &stream();
		Logger* logger = this;
		do {
			if (is_tabs()) {
				*streamPtr << "\t";
			}
			*streamPtr << value;
			logger = logger->mirror_stream(streamPtr);
		} while (streamPtr);
		removeFlag(L_time);
		return *this;
	}

#ifndef USING_ARDUINO
	class Null_Buff : public std::streambuf { // derive because std::streambuf constructor is protected
	public:
		Null_Buff() { setp(nullptr, nullptr); }
	private:
		int_type overflow(int_type) override { return std::char_traits<char>::not_eof(0); }
	} inline null_buff{};

	inline std::ostream null_ostream{ &null_buff };

	inline OStream& Logger::stream() { return null_ostream; }

#else
	inline OStream& Logger::stream() {
		static OStream null;
		return null;
	}
#endif

	/// <summary>
	/// Logs to console - clog(default), cerr or cout.
	/// clog leaves flushing to the client (more efficient).
	/// cerr flushes on every operation, cout generally flushes at every new line.
	/// </summary>
	template<typename Stream = OStream>
	class Console_Logger : public Logger {
	public:
		Console_Logger(Stream& stream = std::clog, Flags initFlags = L_null) : Logger{ initFlags }, _ostream{ &stream } { _ostream->flush(); }

		Stream& stream() override { return is_null() ? Logger::stream() : *_ostream; }
		bool open() override { return (bool)_ostream->good(); }
		Logger* mirror_stream(Logger::ostreamPtr& mirrorStream) override {
			if (mirrorStream == _ostream) mirrorStream = nullptr; else mirrorStream = _ostream;
			return this;
		}
	protected:
		Stream* _ostream;
	};

	//Logger& logger(); // to be defined by the client
}