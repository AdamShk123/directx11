#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <iostream>
#include <fstream>
#include <string>

namespace Logger 
{
constexpr auto RESET = "\033[0m";
constexpr auto RED = "\033[31m";
constexpr auto WHITE = "\033[37m";

//void log(const std::string& tag, const std::string& string);

void err(const std::string& tag, const std::string& string);

}

#endif // LOGGER_HPP