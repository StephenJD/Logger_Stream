#include "Logging_console.h"
#include "Logging_file.h"
#include "Logging_ram.h"

using namespace std;

namespace logging {

    Logger& logger() {
        static Console_Logger std_log{};
        return std_log;
    }

    Logger& file1_logger() {
        static File_Logger logFile{ R"(C:\LF1_)", L_flush };
        return logFile;
    }    
    
    Logger& file2_logger() {
        static File_Logger logFile{ "LF2_", L_startWithFlushing, file1_logger() };
        return logFile;
    }    
    
    Logger& ram_logger() {
        static RAM_Logger<Console_Logger> logFile{ 50, "Ram", L_allwaysFlush };
        return logFile;
    }
}

using namespace logging;

class Widget {
public:
    Widget() { file1_logger() << "Constructing Widget" << endl; }
    int val = 5; 
} widget{};

Logger& operator <<(Logger& log, const Widget& w) {
    log << "Widget-Object val: " << w.val;
    return log;
}

int main()
{
    std::cerr << "Hello World!\n";
    logger() << L_time << "Console_Logger is null" << endl;
    logger().activate();
    logger() << "Console_Logger is active\n";
    file1_logger() << "StartFile1" << endl;

    logger() << "Location: " << L_location << endl;
    logger() << "Console_Logger hex " << hex << 700 << L_flush;
    logger() << "Console_Logger dec " << dec << 700 << endl;
    logger() << "Console_Logger tabs " << L_tabs << dec << 700 << 300 << L_concat << "Done" << endl;
    logger() << "Console_Logger width " << setbase(16) << setw(10) << 10 << setw(5) << 19 << "Done" << endl;
    logger() << L_time << "Console_Logger widget: " << widget << endl;
    file1_logger() << L_time << "new data" << endl;
    file1_logger() << L_time << "Flushed more data" << L_flush;
    file1_logger() << L_time << L_tabs << "yet" << "more" << "data" << endl;
    file2_logger() << L_flush << "StartFile2" << endl;
    file2_logger() << L_time << "File2 time" << endl;
    ram_logger() << L_time << "Ram data" << endl;
}
