#pragma once
#include <exception>
#include <string>

class ChilliException : public std::exception
{
public:
	ChilliException(int lineNumber, const char* file) noexcept;
	const char* what() const noexcept override;
	virtual const char* GetType() const noexcept;
	int GetLine() const noexcept;
	const std::string& GetFile() const noexcept;
	std::string GetOriginString() const noexcept;

private:
	int lineNumber;
	std::string file;

protected:
	mutable std::string whatBuffer;
};
