#pragma once
#include <Arduino.h>
#include <Type_Traits.h>
#include <Conversions.h>

	class Stream_Arduino : public Print {
	public:
		template<class T>
		void streamToPrintInt(Print* stream, T value) {
			stream->print(value);
		}

		template<class T>
		void streamToPrint(Print* stream, T value, typetraits::__true_type) { streamToPrintInt(stream, value); }

		template<class T>
		void streamToPrint(Print* stream, T value, typetraits::__false_type) { stream->print(value); }

	protected:
		size_t write(uint8_t) override { return 1; }
		size_t write(const uint8_t *buffer, size_t size) override { return size; }

		template<class T> friend Stream_Arduino& operator <<(Stream_Arduino& stream, T value);
	};

	// Streaming template	
	template<typename T>
	Stream_Arduino& operator <<(Stream_Arduino& stream, T value) {
		streamToPrint(stream, value, typename typetraits::_Is_integer<T>::_Integral());
		return *this;
	}
