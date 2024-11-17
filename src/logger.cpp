#include "./logger.hpp"

namespace Logger 
{

void err(const std::string& tag, const std::string& message)
{
	//std::ofstream errorFile("error.ans");
	//std::cerr.rdbuf(errorFile.rdbuf());
	auto formatted = std::format("[{}] {}", tag, message);
	std::cerr << RED << formatted << RESET << std::endl;
}

}