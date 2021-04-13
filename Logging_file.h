#pragma once
#include "Logging_console.h"
#include <fstream>
#include <string>
#include <filesystem>

namespace logging {
	
	/// <summary>
	/// Logs to file, and mirrors to the provided ostream - typcally clog
	/// New Filenames are generated for each day
	/// </summary>
	template<typename MirrorBase = Console_Logger>
	class File_Logger : public MirrorBase {
	public:
		static constexpr int FILE_NAME_LENGTH = 8;		
		File_Logger(const std::filesystem::path& filePath) : File_Logger{ filePath, L_null } {}
		File_Logger(const std::filesystem::path& filePath, Flags initFlags, Streamable& mirrorStream = std::clog);
		File_Logger(const std::filesystem::path& filePath, Flags initFlags, Logger& mirror_chain) : File_Logger{ filePath, initFlags } { _mirror = &mirror_chain; }

		Streamable& stream() override;

		void flush() override;
		Logger* mirror_stream(Logger::ostreamPtr& mirrorStream) override;
		bool open();

	private:
		Logger& logTime() override;
		std::string generateFileName();

		Logger* _mirror = this;
		std::ofstream _dataFile;
		std::string _fileNameStem;
		std::filesystem::path _filePath;
		unsigned char _fileDayNo = 0;
	};

	template<typename MirrorBase>
	File_Logger<MirrorBase>::File_Logger(const std::filesystem::path& filePath, Flags initFlags, Streamable& mirrorStream)
		: MirrorBase{ initFlags, mirrorStream }
		, _filePath{ filePath } {
		_fileNameStem = _filePath.filename().string();
		_fileNameStem.resize(FILE_NAME_LENGTH - 4);
		if (!_filePath.has_extension()) _filePath += ".txt";
		MirrorBase::stream() << "\nFile_Logger: " << _fileNameStem << std::endl;
	}

	template<typename MirrorBase>
	Streamable& File_Logger<MirrorBase>::stream() {
		if (MirrorBase::is_cout() || !open()) {
			Logger::ostreamPtr streamPtr = &_dataFile;
			Logger* logger = mirror_stream(streamPtr);
			return *streamPtr;
		} else return _dataFile;
	}

	template<typename MirrorBase>
	std::string File_Logger<MirrorBase>::generateFileName() {
		if (Logger::log_date.dayNo == 0) Logger::getTime();
		_fileDayNo = Logger::log_date.dayNo;
		auto fileName = std::stringstream{};
		fileName << _fileNameStem << std::setfill('0') << std::setw(2) << (int)Logger::log_date.monthNo << std::setw(2) << (int)_fileDayNo;
		_filePath.replace_filename(fileName.str()) += _filePath.extension();
		return _filePath.string();
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
		Logger* logger = mirror_stream(streamPtr);
		while (streamPtr) {
			*streamPtr << _fileNameStem << " ";
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
			return MirrorBase::mirror_stream(mirrorStream);
		}
	}
}