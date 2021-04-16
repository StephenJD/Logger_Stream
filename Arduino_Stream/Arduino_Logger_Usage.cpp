//#include "Logging.h"
//#include "Clock.h"
//#include "Stream_Arduino.h"
//#define Streamable Stream_Arduino;
#include "Logging_console.h"
#include <ostream>

namespace logging {

    constexpr auto SERIAL_RATE = 115200;

    //Clock& clock_() {
    //    static Clock _clock{};
    //    return _clock;
    //}
    //
    //Logger& logger() {
    //    static Serial_Logger ser_log{ SERIAL_RATE, clock_(), L_null };
    //    return ser_log;
    //}
    //
    //Logger& f1_logger() {
    //    static File_Logger<Serial_Logger> f1_log{ "F1", SERIAL_RATE, clock_(), L_flush};
    //    return f1_log;
    //}

   //auto& logger() {
   //     static Console_Logger ser_log{};
   //     return ser_log;
   // }

    Logger& logger() {
        static Console_Logger<Stream_Arduino> ser_log{ stream_arduino };
        return ser_log;
    }
}

using namespace logging;
//using namespace std;

class Widget {
public:
    Widget() { /*logger() << "Constructing Widget" << L_endl;*/ }
    int val = 5;
} widget{};

Logger& operator <<(Logger & log, const Widget& w) {
    log << "Widget-Object val: " << w.val;
    return log;
}

int main() {
    logger() << L_time << "Console_Logger is null" << endl;
    logger().activate();
    logger() << L_time << "Console_Logger Active " << widget << endl;
    logger() << "Console_Logger hex 0x" << hex << 700 << L_flush;
    logger() << "Console_Logger dec " << dec << 700 << endl;

    logger() << "Console_Logger tabs " << L_tabs << 700 << 300 << L_concat << "Done" << endl;
    //f1_logger() << "Start F1" << L_endl;
    //f1_logger() << L_time << "Time F1" << L_endl;
    //f1_logger() << "f1_logger hex 0x" << L_hex << 700 << L_flush;
    //f1_logger() << L_cout << "f1_logger cout" << L_endl;
    //f1_logger() << "f1_logger more" << L_endl;

    logger().activate(false);
    logger() << L_time << "Console_Logger is null" << endl;

    //ard_logger() << L_time << "ard_logger is null" << endl;
    //ard_logger().activate();
    //ard_logger() << L_time << "ard_logger Active " << widget << endl;
    //ard_logger() << "ard_logger hex 0x" << hex << 700 << L_flush;
    //ard_logger() << "ard_logger dec " << dec << 700 << endl;

}
