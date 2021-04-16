#include "Logging_console.h"
#include "Stream_Arduino.h"
#include <Logging.h>
#include "Clock.h"

constexpr auto SERIAL_RATE = 115200;

Clock& clock_() {
    static Clock _clock{};
    return _clock;
}

Logger& logger() {
    static Serial_Logger ser_log{ SERIAL_RATE, clock_(), L_null };
    return ser_log;
}

Logger& f1_logger() {
    static File_Logger<Serial_Logger> f1_log{ "F1", SERIAL_RATE, clock_(), L_flush};
    return f1_log;
}

class Widget {
public:
    Widget() { /*logger() << "Constructing Widget" << L_endl;*/ }
    int val = 5; 
} widget{};

Logger& operator <<(Logger& log, const Widget& w) {
    log << "Widget-Object val: " << w.val;
    return log;
}

int main()
{
    logger() << L_time << "Console_Logger is null" << L_endl;
    logger().activate();
    logger() << L_time << "Console_Logger Active " << widget << L_endl;
    logger() << "Console_Logger hex 0x" << L_hex << 700 << L_flush;
    logger() << "Console_Logger dec " << L_dec << 700 << L_endl;

    logger() << "Console_Logger tabs " << L_tabs << 700 << 300 << L_concat << "Done" << L_endl;
    f1_logger() << "Start F1" << L_endl;
    f1_logger() << L_time << "Time F1" << L_endl;
    f1_logger() << "f1_logger hex 0x" << L_hex << 700 << L_flush;
    f1_logger() << L_cout << "f1_logger cout" << L_endl;
    f1_logger() << "f1_logger more" << L_endl;

    logger().activate(false);
    logger() << L_time << "Console_Logger is null" << L_endl;

}
