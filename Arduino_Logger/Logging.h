#pragma once
//#include <SD.h>
//#include <SPI.h>
#include <Arduino.h>
#include <Type_Traits.h>
#include <Conversions.h>
//#include <Streaming.h>
//#pragma message( "Logging.h loaded" )
/*
To reduce timeout on missing SD card from 2s to 7mS modify Sd2Card.cpp:
In Sd2Card::init() on first timeout:  while ((status_ = cardCommand(CMD0, 0)) != R1_IDLE_STATE) {...
Set timeout to 1mS.
SD.h/.cpp modified to provide sd_exists();
#define DEC 10
#define HEX 16
#define OCT 8
#define BIN 2
*/
	class EEPROMClass;
	EEPROMClass & eeprom();
	class Clock;

	enum Flags {L_clearFlags, L_dec, L_int, L_concat, L_endl, L_time, L_flush, L_startWithFlushing, L_null = 255, L_cout = 8, L_hex = 16, L_fixed = 32, L_tabs = 64, L_allwaysFlush = 128 };
	inline Flags operator +=(Flags& l_flag, Flags r_flag) { return l_flag = static_cast<Flags>(l_flag | r_flag); }
	inline Flags operator -=(Flags& l_flag, Flags r_flag) { return l_flag = static_cast<Flags>(l_flag & ~r_flag); }

	class Logger : public Print {
	public:
		void activate(bool makeActive = true) { makeActive ? _flags = L_clearFlags : _flags = L_null; }
		Flags addFlag(Flags flag) { _flags += flag; return _flags; }
		Flags removeFlag(Flags flag) { _flags -= flag; return _flags; }
		virtual void flush() { _flags -= L_startWithFlushing; }
		virtual Print& stream() { return *this; }
		
		template<typename T>
		Logger& log(T value);

		virtual bool isWorking() { return true; }
		virtual void readAll() {}
		virtual void begin(uint32_t baudRate = 0) {	flush(); }
		Logger & operator <<(Flags);
		
		template<class T>
		void streamToPrintInt(Print* stream, T value) {
			if (is_fixed()) toFixed(value);
			else stream->print(value, base());
		}

		template<class T>
		void streamToPrint(Print* stream, T value, typetraits::__true_type) { streamToPrintInt(stream, value); }

		template<class T>
		void streamToPrint(Print* stream, T value, typetraits::__false_type) { stream->print(value); }

		using ostreamPtr = Print*;
		virtual bool mirror_stream(ostreamPtr& mirrorStream) { mirrorStream = nullptr; return false; }


	protected:
		Logger(Flags initFlag = L_null) : _flags{ initFlag } {}
		Logger(Clock & clock, Flags initFlag = L_null);
		Logger(const char * fileNameStem, uint32_t baudRate, Clock & clock, Flags initFlag = L_null) {}

		virtual Logger& logTime();

		size_t write(uint8_t) override { return 1; }
		size_t write(const uint8_t *buffer, size_t size) override { return size; }

		template<class T> friend Logger & operator <<(Logger & stream, T value);
		
		bool is_tabs() const { return _flags & L_tabs || has_time(); }
		bool is_null() const { return _flags == L_null; }
		bool is_cout() const { return _flags & L_cout; }
		bool has_time() const { return (_flags & 7) == L_time; }
		bool is_fixed() const { return _flags & L_fixed; }
		int base() {return _flags & L_hex ? HEX : DEC;}
		Logger & toFixed(int decimal);

		Clock * _clock = 0;
		Flags _flags = L_startWithFlushing;
	};

	// Streaming template	
	template<typename T>
	Logger& operator <<(Logger& logger, T value) {
		return logger.log(value);
	}

	template<typename T>
	Logger& Logger::log(T value) {
		if (is_null()) return *this;
		auto streamPtr = &stream();
		do {
			if (is_tabs()) {
				streamPtr->print("\t");
			}
			streamToPrint(streamPtr, value, typename typetraits::_Is_integer<T>::_Integral());
		} while (mirror_stream(streamPtr));
		removeFlag(L_time);
		return *this;
	}

	// **********  Serial_Logger ************

	class Serial_Logger : public Logger {
	public:
		Serial_Logger(uint32_t baudRate, Clock& clock, Flags initFlags = L_flush);
		Serial_Logger(uint32_t baudRate, Flags initFlags = L_flush);
		Print& stream() override { return Serial; }

		void flush() override { Serial.flush(); Logger::flush(); }
		void begin(uint32_t baudRate) override;
		bool mirror_stream(ostreamPtr& mirrorStream) override {
			if (mirrorStream == &Serial) mirrorStream = nullptr; else mirrorStream = &Serial;
			return mirrorStream == &Serial ? true : false; 
		}
	protected:
		size_t write(uint8_t) override;
		size_t write(const uint8_t* buffer, size_t size) override;
	};

	/*/// <summary>
	/// Per msg: 100uS Due/ 700uS Mega
	/// Save 1KB to SD: 250mS Due / 150mS Mega
	/// </summary>
	class RAM_Logger : public Logger {
	public:
		RAM_Logger(const char * fileNameStem, uint16_t ramFile_size, bool keepSaving, Clock & clock);
		RAM_Logger(const char * fileNameStem, uint16_t ramFile_size, bool keepSaving);
		size_t write(uint8_t) override;
		size_t write(const uint8_t *buffer, size_t size) override;
		void readAll() override;
	private:
		Logger & logTime() override;
		bool is_tabs() override { return _ram_mustTabTime ? (_ram_mustTabTime = false, true) : _flags & L_tabs; }
		bool _ram_mustTabTime = false;
		uint8_t * _ramFile = 0;
		char _fileNameStem[5];
		uint16_t _ramFile_size;
		uint16_t _filePos = 0;
		bool _keepSaving;
	};

	/// <summary>
	/// Per msg: 115mS Due/ 170mS Mega
	/// Save 1KB to SD: 660mS Due / 230mS Mega
	/// </summary>
	class EEPROM_Logger : public Logger {
	public:
		EEPROM_Logger(const char * fileNameStem, uint16_t startAddr, uint16_t endAddr, bool keepSaving, Clock & clock);
		EEPROM_Logger(const char * fileNameStem, uint16_t startAddr, uint16_t endAddr, bool keepSaving);
		size_t write(uint8_t) override;
		size_t write(const uint8_t *buffer, size_t size) override;
		void readAll() override;
		void begin(uint32_t = 0) override;
	private:
		Logger & logTime() override;
		bool is_tabs() override { return _ee_mustTabTime ? (_ee_mustTabTime = false, true) : _flags & L_tabs; }
		void saveOnFirstCall();
		void findStart();
		bool _ee_mustTabTime = false;
		char _fileNameStem[5];
		uint16_t _startAddr;
		uint16_t _endAddr;
		uint16_t _currentAddress = 0;
		bool _hasWrittenToSD = false;
		bool _hasRecycled = false;
		bool _keepSaving;
	};*/

	///// <summary>
	///// Per un-timed msg: 1.4mS
	///// Per timed msg: 3mS
	///// </summary>	
	//class SD_Logger : public Serial_Logger {
	//public:
	//	SD_Logger(const char * fileNameStem, uint32_t baudRate, Clock & clock);
	//	SD_Logger(const char * fileNameStem, uint32_t baudRate);
	//	bool isWorking() override;
	//	size_t write(uint8_t) override;
	//	size_t write(const uint8_t *buffer, size_t size) override;
	//	File open()();
	//	void close()/* override */{ _dataFile.close(); _dataFile = File{}; }
	//	void flush() override { Serial_Logger::flush(); close(); }
	//private:
	//	Logger & logTime() override;
	//	bool is_tabs() override { return _SD_mustTabTime ? (_SD_mustTabTime = false, true) : _flags & L_tabs; }
	//	void setBaseTimeTab(bool timeTab) override { _mustTabTime = timeTab; }
	//	bool prePrint() override { return true; }

	//	File _dataFile;
	//	char _fileNameStem[5];
	//	bool _SD_mustTabTime = false;
	//};

#ifdef ZPSIM
#include <fstream>
	/// <summary>
	/// Per un-timed msg: 1.4mS
	/// Per timed msg: 3mS
	/// </summary>	
	template<typename MirrorBase = Serial_Logger>
	class File_Logger : public MirrorBase {
	public:
		static constexpr int FILE_NAME_LENGTH = 8;		
		File_Logger(const char * fileNameStem, uint32_t baudRate, Clock & clock, Flags initFlags = L_flush);
		File_Logger(const char * fileNameStem, uint32_t baudRate, Flags initFlags = L_flush);
		Print& stream() override;
		void flush() override { close(); MirrorBase::flush(); }
		bool mirror_stream(Logger::ostreamPtr& mirrorStream) override { return MirrorBase::mirror_stream(mirrorStream); }
	
		bool open();
		bool isWorking() override;
		void close() /*override*/ { _dataFile.close(); }
	protected:
		Logger & logTime() override;
		GP_LIB::CStr_20 generateFileName();

		size_t write(uint8_t) override;
		size_t write(const uint8_t *buffer, size_t size) override;

		Logger* _mirror = this;
		std::ofstream _dataFile;
		char _fileNameStem[5];
		unsigned char _fileDayNo = 0;
	};

	////////////////////////////////////
	//            File_Logger         //
	////////////////////////////////////

	template<typename MirrorBase>
	File_Logger<MirrorBase>::File_Logger(const char* fileNameStem, uint32_t baudRate, Flags initFlags) 
		: MirrorBase(baudRate, initFlags) {
		strncpy(_fileNameStem, fileNameStem, 5);
		_fileNameStem[4] = 0;
		// Avoid calling Serial_Logger during construction, in case clock is broken.
		Serial.print(F("\nFile_Logger Begun without Clock: "));
		Serial.print(_fileNameStem);
		Serial.println();
	}

	template<typename MirrorBase>
	File_Logger<MirrorBase>::File_Logger(const char* fileNameStem, uint32_t baudRate, Clock& clock, Flags initFlags)
		: MirrorBase(baudRate, clock, initFlags) {
		strncpy(_fileNameStem, fileNameStem, 5);
		_fileNameStem[4] = 0;
		// Avoid calling Serial_Logger during construction, in case clock is broken.
		Serial.print(F("\nFile_Logger Begun with clock: "));
		Serial.print(_fileNameStem);
		Serial.println();
	}

	template<typename MirrorBase>
	bool File_Logger<MirrorBase>::isWorking() { return true; }

	template<typename MirrorBase>
	Print& File_Logger<MirrorBase>::stream() { 
		if (File_Logger<MirrorBase>::is_cout() || !open()) return MirrorBase::stream();
		else return *this; 
	}

	template<typename MirrorBase>
	GP_LIB::CStr_20 File_Logger<MirrorBase>::generateFileName() {
		GP_LIB::CStr_20 fileName;
		strcpy(fileName.str(), _fileNameStem);
		if (File_Logger<MirrorBase>::_clock) {
			_fileDayNo = File_Logger<MirrorBase>::_clock->day();
			strcat(fileName.str(), GP_LIB::intToString(File_Logger<MirrorBase>::_clock->month(), 2));
			strcat(fileName.str(), GP_LIB::intToString(_fileDayNo, 2));
		}
		strcat(fileName.str(), ".txt");
		return fileName;
	}

	template<typename MirrorBase>
	bool File_Logger<MirrorBase>::open() {
		if (File_Logger<MirrorBase>::_clock && _fileDayNo != File_Logger<MirrorBase>::_clock->day()) _dataFile.close();
		if (!_dataFile.is_open()) {
			_dataFile.open(generateFileName(), std::ios::app);	// Append
		}
		return _dataFile.good();
	}

	template<typename MirrorBase>
	size_t File_Logger<MirrorBase>::write(uint8_t chr) {
		_dataFile.write((char*)&chr, 1);
		return 1;
	}

	template<typename MirrorBase>
	size_t File_Logger<MirrorBase>::write(const uint8_t* buffer, size_t size) {
		_dataFile.write((char*)buffer, size);
		return size;
	}

	template<typename MirrorBase>
	Logger& File_Logger<MirrorBase>::logTime() {
		auto streamPtr = &stream();
		while (mirror_stream(streamPtr)) {
			streamPtr->print(_fileNameStem);
			streamPtr->print(" ");
		}
		MirrorBase::logTime();
		return *this;
	}

#endif

	Logger & logger(); // to be defined by the client
