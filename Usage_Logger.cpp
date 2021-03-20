// StdLogger.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "Logging_cout.h"
#include "Logging_file.h"
#include "Logging_ram.h"

using namespace std;
 
Logger& logger() {
    static Cout_Logger std_cout(std::cout);
    return std_cout;
}

Logger& flogger() {
    static File_Logger logFile("Test", std::cout);
    return logFile;
}

Logger& ram_logger() {
    static RAM_Logger logFile(5, "Rest", std::cout);
    return logFile;
}

int main()
{
    std::cout << "Hello World!\n";
    logger() << "Cout_Logger" << endl;
    logger() << "Cout_Logger hex " << hex << 700 << L_flush;
    logger() << "Cout_Logger dec " << dec << 700 << std::endl;
    logger() << "Cout_Logger tabs " << L_tabs << dec << 700 << 300 << L_concat << "Done" << endl;
    logger() << "Cout_Logger width " << setbase(16) << setw(10) << 10 << setw(5) << 19 << "Done" << endl;
    logger() << L_time << "Cout_Logger time" << endl;
    flogger() << L_flush << "StartFile" << endl;
    flogger() << L_time << "new data" << endl;
    flogger() << L_time << "Flushed more data" << L_flush;
    flogger() << L_time << L_tabs << "yet" << "more" << "data" << endl;

    ram_logger() << L_flush << "RamFile" << endl;
    ram_logger() << L_time << "Ram data" << endl;
    ram_logger() << L_time << "Flushed Ram data" << L_flush;
    ram_logger() << L_time << L_tabs << "yet" << "more" << "Ram data" << L_flush;
}
