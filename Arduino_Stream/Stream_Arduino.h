#pragma once
#include <Arduino.h>
#include <Type_Traits.h>
#include <Conversions.h>

//#define OStream Stream_Arduino

	class Stream_Arduino : public Print {
	public:
		enum Base {dec = DEC, hex = HEX};
		Stream_Arduino& operator <<(Base base) { _base = base; return *this; }
		void flush() { Serial.flush(); }
		bool good() { return bool(Serial); }

		template<class T>
		void streamToPrintInt(Print* stream, T value) {
			stream->print(value, _base);
		}

		template<class T>
		void streamToPrint(Print* stream, T value, typetraits::__true_type) { streamToPrintInt(stream, value); }

		template<class T>
		void streamToPrint(Print* stream, T value, typetraits::__false_type) { stream->print(value); }

	protected:
		size_t write(uint8_t) override { return 1; }
		size_t write(const uint8_t *buffer, size_t size) override { return size; }

		template<class T> friend Stream_Arduino& operator <<(Stream_Arduino& stream, T value);
		Base _base = dec;
	} inline stream_arduino{};

	// Streaming template	
	template<typename T>
	Stream_Arduino& operator <<(Stream_Arduino& stream, T value) {
		stream.streamToPrint(&stream, value, typename typetraits::_Is_integer<T>::_Integral());
		return stream;
	}
