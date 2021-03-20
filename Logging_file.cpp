#include "Logging_file.h"
#include <iostream>
#include <string>
#include <chrono>

using namespace std;

////////////////////////////////////
//            File_Logger           //
////////////////////////////////////

File_Logger::File_Logger(const string & fileNameStem, std::ostream& ostream) 
	: Cout_Logger(ostream) 
	, _fileNameStem(fileNameStem)
	{
		_fileNameStem[4] = 0;
		Cout_Logger::stream() << "\nFile_Logger: " << _fileNameStem << endl;
	}

File_Logger::File_Logger(const File_Logger& fileLogger) 
	: Cout_Logger(fileLogger)
	, _fileNameStem(fileLogger._fileNameStem)
	{}

string File_Logger::generateFileName() {
	auto fileName = ostringstream{};
	fileName << _fileNameStem << setfill('0') << setw(2) << monthNo() << setw(2) << dayNo() << ".txt";
	return fileName.str();
}

bool File_Logger::open() {
	if (!_dataFile.is_open()) {
		_dataFile.open(generateFileName(), ios::app);	// Append
	}
	return _dataFile.good();
}

std::ostream& File_Logger::stream() { 
	open();
	return _dataFile; 
}

Logger& File_Logger::logTime() {
	Cout_Logger std_out(static_cast<Cout_Logger&>(*this));
	std_out << _fileNameStem << " ";
	Cout_Logger::logTime();
	_SD_mustTabTime = true;
	return *this;
}
