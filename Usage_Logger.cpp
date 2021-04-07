// StdLogger.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "Logging_console.h"
#include "Logging_file.h"
#include "Logging_ram.h"

using namespace std;
using namespace logging;
namespace logging {
    Logger& logger() {
        static Console_Logger std_cout(L_null);
        return std_cout;
    }

    Logger& flogger() {
        static File_Logger logFile("Test", cout);
        return logFile;
    }

    Logger& ram_logger() {
        static RAM_Logger logFile(5, "Rest", cout);
        return logFile;
    }
}

int main()
{
    std::cout << "Hello World!\n";
    logger() << "Console_Logger is null" << endl;
    logger().activate();
    logger() << "Console_Logger is active\n";
    logger() << L_location << endl;
    logger() << "Console_Logger hex " << hex << 700 << L_flush;
    logger() << "Console_Logger dec " << dec << 700 << std::endl;
    logger() << "Console_Logger tabs " << L_tabs << dec << 700 << 300 << L_concat << "Done" << endl;
    logger() << "Console_Logger width " << setbase(16) << setw(10) << 10 << setw(5) << 19 << "Done" << endl;
    logger() << L_time << "Console_Logger time" << endl;
    flogger() << L_flush << "StartFile" << endl;
    flogger() << L_time << "new data" << endl;
    flogger() << L_time << "Flushed more data" << L_flush;
    flogger() << L_time << L_tabs << "yet" << "more" << "data" << endl;

    ram_logger() << L_flush << "RamFile" << endl;
    ram_logger() << L_time << "Ram data" << endl;
    ram_logger() << L_time << "Flushed Ram data" << L_flush;
    ram_logger() << L_time << L_tabs << "yet" << "more" << "Ram data" << L_flush;
}
