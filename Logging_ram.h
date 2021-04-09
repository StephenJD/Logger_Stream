#pragma once
#include "Logging_file.h"
#include <streambuf>
#include <string>
#include <streambuf>
#include <sstream>
#include <memory>

namespace logging {

	class Ram_Buffer : public std::streambuf // derive because std::streambuf constructor is protected
	{
	public:
		Ram_Buffer(char* start, size_t size, Logger& logger) : _logger(&logger) { setp(start, start + size); }
		void empty_buffer() { setp(pbase(), epptr()); }
		auto start() const { return pbase(); }
		auto pos() const { return pptr(); }
	private:
		int_type overflow(int_type ch) override {
			_logger->flush();
			sputc(ch);
			return std::char_traits<char>::not_eof(0);
		}
		Logger* _logger;
	};

	/// <summary>
	/// Logs to RAM and flushes to file when ram-buffer is full.
	/// Mirrors to the provided ostream - typcally cout.
	/// </summary>
	template<typename MirrorBase = Logger>
	class RAM_Logger : public File_Logger<MirrorBase> {
	public:
		RAM_Logger(uint16_t ramFile_size, const std::string& fileNameStem, Flags initFlags, std::ostream& ostream = std::clog);
		std::ostream& stream() override { return _stream; }
		void flush() override;
	private:
		std::unique_ptr<char[]> _ramFile;
		Ram_Buffer _ramBuffer;
		std::ostream _stream;
	};

	template<typename MirrorBase>
	RAM_Logger<MirrorBase>::RAM_Logger(uint16_t ramFile_size, const std::string& fileNameStem, Flags initFlags, std::ostream& ostream)
		: File_Logger<MirrorBase>(fileNameStem, initFlags, ostream)
		, _ramFile(std::make_unique<char[]>(ramFile_size))
		, _ramBuffer(_ramFile.get(), ramFile_size, *this)
		, _stream(&_ramBuffer) {}

	template<typename MirrorBase>
	void RAM_Logger<MirrorBase>::flush() {
		for (char* c = _ramBuffer.start(); c < _ramBuffer.pos(); ++c) {
			File_Logger<MirrorBase>::stream() << *c;
		}
		_ramBuffer.empty_buffer();
	}
}