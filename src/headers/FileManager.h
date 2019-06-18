#pragma once
#include <string>

class FileManager
{
public:
	inline static std::string GetWorkingDirectory();
	static std::string readFile(std::string path);
	static std::string getFileName(std::string path);
	static bool fileExists(const std::string& name);

private:
	static void SetCorrectDirectory();
	static bool correctWorkingDir;
};

