

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

enum MESSAGE_MODE
{
    DEGUG,
    NORMAL,
    WARNING,
    ERROR,
    FATAL
};

std::vector<std::string> MODE_LIST = {"DEGUG", "NORMAL", "WARNING", "ERROR", "FATAL"};

void LogMessage(int mode, std::string message, std::ostream &ofs = std::cout)
{
    std::string mode_str = MODE_LIST[mode];
    std::string output_message;
    std::stringstream output_message_stream;
    output_message_stream << "[" << mode_str << "]: " << message;
    std::getline(output_message_stream, output_message);
    ofs << output_message << std::endl;
}