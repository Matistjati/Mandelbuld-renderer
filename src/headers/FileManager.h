#pragma once
#include <string>
#include <Shlwapi.h>

class FileManager
{
public:
	static bool correctWorkingDir;
	inline static std::string GetWorkingDirectory();
	static std::string readFile(std::string path);
	static std::string getFileName(std::string path);

private:
	static bool SetCorrectDirectory();
};

