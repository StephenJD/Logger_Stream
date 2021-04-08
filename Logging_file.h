#pragma once
#include "Logging_console.h"
#include <fstream>
#include <string>

namespace logging {

	/// <summary>
	/// Logs to file, and mirrors to the provided ostream - typcally cout
	/// New Filenames are generated for each day
	/// </summary>
	template<typename baseLogger = Console_Logger>
	class File_Logger : public baseLogger {
	public:
		File_Logger(const std::string& fileNameStem, Flags initFlags, std::ostream& mirrorStream = std::clog);
		std::ostream& stream() override { open(); return _dataFile; }
		std::ostream& mirror_stream() override { return baseLogger::stream(); }
		void flush() override { baseLogger::flush(); _dataFile.flush(); }
		bool open();
	private:
		Logger& logTime() override;
		std::string generateFileName();

		std::ofstream _dataFile;
		std::string _fileNameStem;
	};

	template<typename baseLogger>
	File_Logger<baseLogger>::File_Logger(const std::string& fileNameStem, Flags initFlags, std::ostream& mirrorStream)
		: baseLogger(initFlags, mirrorStream)
		, _fileNameStem(fileNameStem) {
		_fileNameStem[4] = 0;
		baseLogger::stream() << "\nFile_Logger: " << _fileNameStem << std::endl;
	}

	template<typename baseLogger>
	std::string File_Logger<baseLogger>::generateFileName() {
		auto fileName = std::ostringstream{};
		fileName << _fileNameStem << std::setfill('0') << std::setw(2) << Logger::monthNo() << std::setw(2) << Logger::dayNo() << ".txt";
		return fileName.str();
	}

	template<typename baseLogger>
	bool File_Logger<baseLogger>::open() {
		if (!_dataFile.is_open()) {
			_dataFile.open(generateFileName(), std::ios::app);	// Append
		}
		return _dataFile.good();
	}

	template<typename baseLogger>
	Logger& File_Logger<baseLogger>::logTime() {
		mirror_stream() << _fileNameStem << " ";
		baseLogger::logTime();
		return *this;
	}
}