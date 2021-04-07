#pragma once
#include "Logging_console.h"
#include <fstream>
#include <string>

namespace logging {

	/// <summary>
	/// Logs to file, and mirrors to the provided ostream - typcally cout
	/// New Filenames are generated for each day
	/// </summary>	
	class File_Logger : public Console_Logger {
	public:
		File_Logger(const std::string& fileNameStem, std::ostream& ostream);
		File_Logger(const File_Logger& fileLogger);
		std::ostream& stream() override;

		bool open();
		void flush() override { Console_Logger::flush(); _dataFile.flush(); }
	private:
		Logger& logTime() override;
		bool is_tabs() const override { return _SD_mustTabTime ? (_SD_mustTabTime = false, true) : _flags & L_tabs; }
		void setBaseTimeTab(bool timeTab) override { _mustTabTime = timeTab; }
		bool prePrint() const override { return true; }
		std::string generateFileName();

		std::ofstream _dataFile;
		std::string _fileNameStem;
		mutable bool _SD_mustTabTime = false;
	};
}