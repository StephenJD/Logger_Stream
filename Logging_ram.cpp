#include "Logging_ram.h"
#include <streambuf>
#include <sstream>

using namespace std;
using namespace logging;

////////////////////////////////////
//            RamStream           //
////////////////////////////////////

void RamStream::emptyStream() {
	_logger->writeToFile(pbase(), pptr());
	setp(pbase(), epptr());
}

std::streambuf::int_type RamStream::overflow(int_type ch) {
	emptyStream();
	sputc(ch);
	return std::char_traits<char>::not_eof(std::char_traits<char>::to_int_type(*this->pptr()));
}

////////////////////////////////////
//            RAM_Logger          //
////////////////////////////////////

RAM_Logger::RAM_Logger(uint16_t ramFile_size, const std::string& fileNameStem, Flags initFlags, std::ostream& ostream)
	: File_Logger(fileNameStem, initFlags, ostream)
	, _ramFile(make_unique<char[]>(ramFile_size))
	, _ramStream(_ramFile.get(), ramFile_size, *this)
	, _stream(&_ramStream)
	{}

ostream& RAM_Logger::stream() {
	return _stream;
}

void RAM_Logger::flush() {
	_ramStream.emptyStream();
}

void RAM_Logger::writeToFile(char* start, char* end) {
	for (char* c = start; c < end; ++c) {
		File_Logger::stream() << *c;
	}
}

Logger& RAM_Logger::logTime() {
	Console_Logger::logTime();
	return *this;
}