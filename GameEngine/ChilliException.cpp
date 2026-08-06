#include "ChilliException.h"
#include <sstream>

ChilliException::ChilliException(int lineNumber, const char* file) noexcept : lineNumber(lineNumber), file(file)
{

}

const char* ChilliException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl;
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* ChilliException::GetType() const noexcept
{
	return "Chilli Exception";
}

int ChilliException::GetLine() const noexcept
{
	return lineNumber;
}

const std::string& ChilliException::GetFile() const noexcept
{
	return file;
}

std::string ChilliException::GetOriginString() const noexcept
{
	std::ostringstream oss;
	oss << "[FILE] " << file << std::endl
	<< "[LINE] " << lineNumber;
	
	return oss.str();
}
