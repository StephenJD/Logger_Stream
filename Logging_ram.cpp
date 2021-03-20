#include "Logging_ram.h"
#include <streambuf>
#include <sstream>

using namespace std;

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

RAM_Logger::RAM_Logger(uint16_t ramFile_size, const std::string& fileNameStem, std::ostream& ostream)
	: File_Logger(fileNameStem, ostream)
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
	auto file = static_cast<File_Logger&>(*this);
	for (char* c = start; c < end; ++c) {
		file.stream() << *c;
	}
}

Logger& RAM_Logger::logTime() {
	Cout_Logger::logTime();
	_ram_mustTabTime = true;
	return *this;
}