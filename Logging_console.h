#pragma once
#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>
#include <time.h>
#include <version>

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

		virtual std::ostream& stream();

		using ostreamPtr = std::ostream*;
		virtual Logger* mirror_stream(ostreamPtr& mirrorStream) { mirrorStream = nullptr; return this; }

	protected:
		Logger(Flags initFlag = L_null) : _flags{ initFlag } {}
		Logger(Flags initFlag = L_null, std::ostream& = std::clog) : _flags{ initFlag }  {}

		template<class T> friend Logger& operator <<(Logger& logger, T value);

		bool is_tabs() const { return _flags & L_tabs || has_time(); }
		bool is_null() const { return _flags == L_null; }
		bool is_cout() const { return _flags & L_cout; }
		bool has_time() const { return (_flags & 7) == L_time; }

		virtual Logger& logTime();

		static tm* getTime();
		struct Log_date {
			unsigned char dayNo;
			unsigned char monthNo;
		} inline static log_date{ 0,0 };

		Flags _flags = L_startWithFlushing;
	};

	// Streaming template	
	template<typename T>
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

	class Null_Buff : public std::streambuf { // derive because std::streambuf constructor is protected
	public:
		Null_Buff() { setp(nullptr, nullptr); }
	private:
		int_type overflow(int_type) override { return std::char_traits<char>::not_eof(0); }
	} inline null_buff{};

	inline std::ostream null_ostream{ &null_buff };

	inline std::ostream& Logger::stream() { return null_ostream; }
	
	/// <summary>
	/// Logs to console - clog(default), cerr or cout.
	/// clog leaves flushing to the client (more efficient).
	/// cerr flushes on every operation, cout generally flushes at every new line.
	/// </summary>
	class Console_Logger : public Logger {
	public:
		Console_Logger(Flags initFlags = L_null, std::ostream& ostream = std::clog);
		std::ostream& stream() override { return is_null() ? Logger::stream() : *_ostream; }
		Logger* mirror_stream(ostreamPtr& mirrorStream) override {
			if (mirrorStream == _ostream) mirrorStream = nullptr; else mirrorStream = _ostream;
			return this;
		}
	protected:
		std::ostream* _ostream = 0;
	};

	Logger& logger(); // to be defined by the client
}