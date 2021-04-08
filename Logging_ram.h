#pragma once
#include "Logging_file.h"
#include <streambuf>
#include <string>
#include <memory>

namespace logging {

	class RAM_Logger;

	class RamStream : public std::streambuf // derive because std::streambuf constructor is protected
	{
	public:
		RamStream(char* start, size_t size, RAM_Logger& logger) : _logger(&logger) { setp(start, start + size); }
		void emptyStream();
	private:
		int_type overflow(int_type ch) override;
		RAM_Logger* _logger;
	};

	/// <summary>
	/// Logs to RAM and flushes to file when ram-buffer is full.
	/// Mirrors to the provided ostream - typcally cout.
	/// </summary>
	class RAM_Logger : public File_Logger<Console_Logger> {
	public:
		RAM_Logger(uint16_t ramFile_size, const std::string& fileNameStem, Flags initFlags, std::ostream& ostream = std::clog);
		std::ostream& stream() override;
		void flush() override;
	private:
		friend class RamStream;
		Logger& logTime() override;
		void writeToFile(char* start, char* end);

		std::unique_ptr<char[]> _ramFile;
		RamStream _ramStream;
		std::ostream _stream;
	};
}

