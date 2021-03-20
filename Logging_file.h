#pragma once
#include "Logging_cout.h"
#include <fstream>
#include <string>

/// <summary>
/// Logs to file, and mirrors to the provided ostream - typcally cout
/// New Filenames are generated for each day
/// </summary>	
class File_Logger : public Cout_Logger {
public:
	File_Logger(const std::string & fileNameStem, std::ostream& ostream);
	File_Logger(const File_Logger & fileLogger);
	std::ostream& stream() override;

	bool open();
	void flush() override { Cout_Logger::flush(); _dataFile.flush(); }
private:
	Logger& logTime() override;
	bool is_tabs() override { return _SD_mustTabTime ? (_SD_mustTabTime = false, true) : _flags & L_tabs; }
	void setBaseTimeTab(bool timeTab) override { _mustTabTime = timeTab; }
	bool prePrint() override { return true; }
	std::string generateFileName();

	std::ofstream _dataFile;
	std::string _fileNameStem;
	bool _SD_mustTabTime = false;
};