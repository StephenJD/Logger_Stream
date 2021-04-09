#pragma once
#include "Logging_console.h"
#include <fstream>
#include <string>

namespace logging {

	/// <summary>
	/// Logs to file, and mirrors to the provided ostream - typcally clog
	/// New Filenames are generated for each day
	/// </summary>
	template<typename MirrorBase = Console_Logger>
	class File_Logger : public MirrorBase {
	public:
		File_Logger(const std::string& fileNameStem, Flags initFlags, std::ostream& mirrorStream = std::clog);
		File_Logger(const std::string& fileNameStem, Flags initFlags, Logger& mirror_chain) : File_Logger(fileNameStem, initFlags) { _mirror = &mirror_chain; }

		std::ostream& stream() override { open(); return _dataFile; }
		void flush() override;
		Logger* mirror_stream(Logger::ostreamPtr& mirrorStream) override;
		bool open();

	private:
		Logger& logTime() override;
		std::string generateFileName();
		Logger* _mirror = this;

		std::ofstream _dataFile;
		std::string _fileNameStem;
		unsigned char _fileDayNo = 0;
	};

	template<typename MirrorBase>
	File_Logger<MirrorBase>::File_Logger(const std::string& fileNameStem, Flags initFlags, std::ostream& mirrorStream)
		: MirrorBase(initFlags, mirrorStream)
		, _fileNameStem(fileNameStem) {
		_fileNameStem.erase(4);
		MirrorBase::stream() << "\nFile_Logger: " << _fileNameStem << std::endl;
	}

	template<typename MirrorBase>
	std::string File_Logger<MirrorBase>::generateFileName() {
		auto fileName = std::ostringstream{};
		if (Logger::log_date.dayNo == 0) Logger::getTime();
		fileName << _fileNameStem << std::setfill('0') << std::setw(2) << (int)Logger::log_date.monthNo << std::setw(2) << (int)Logger::log_date.dayNo << ".txt";
		_fileDayNo = Logger::log_date.dayNo;
		return fileName.str();
	}

	template<typename MirrorBase>
	bool File_Logger<MirrorBase>::open() {
		if (_fileDayNo != Logger::log_date.dayNo) _dataFile.close();
		if (!_dataFile.is_open()) {
			_dataFile.open(generateFileName(), std::ios::app);	// Append
		}
		return _dataFile.good();
	}

	template<typename MirrorBase>
	Logger& File_Logger<MirrorBase>::logTime() {
		auto streamPtr = &stream();
		Logger* logger = mirror_stream(streamPtr);;
		while (streamPtr) {
			(*streamPtr) << _fileNameStem << " ";
			logger = logger->mirror_stream(streamPtr);
		}
		MirrorBase::logTime();
		return *this;
	}

	template<typename MirrorBase>
	void File_Logger<MirrorBase>::flush() {
		auto streamPtr = &stream();
		Logger* logger = mirror_stream(streamPtr);
		while (streamPtr && logger != this) {
			logger->flush();
			logger = logger->mirror_stream(streamPtr);
		}
		MirrorBase::flush();
		_dataFile.flush();
	}

	template<typename MirrorBase>
	Logger* File_Logger<MirrorBase>::mirror_stream(Logger::ostreamPtr& mirrorStream) {
		bool isChainedMirror = this != _mirror;
		if (isChainedMirror) {
			mirrorStream = &_mirror->stream();
			return _mirror;
		} else {
			return MirrorBase::mirror_stream(mirrorStream);;
		}
	}
}