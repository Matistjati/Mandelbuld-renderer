#pragma once
#include <string>
#include <vector>

class FileManager
{
public:
	inline static std::string GetWorkingDirectory();
	static std::string readFile(std::string path);
	static std::string getFileName(std::string path);
	static bool fileExists(const std::string& name);
	static std::vector<std::string> GetDirectoryFileNames(const std::string directory);

private:
	static void SetCorrectDirectory();
	static bool correctWorkingDir;
};

