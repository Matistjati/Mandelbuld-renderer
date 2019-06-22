#pragma once
#include <string>
#include <vector>

class FileManager
{
public:
	inline static std::string GetWorkingDirectory();
	static std::string ReadFile(std::string path);
	static std::string GetFileName(std::string path);
	static bool FileExists(const std::string& name);
	static std::vector<std::string> GetDirectoryFileNames(const std::string directory);

private:
	static void SetCorrectDirectory();
	static bool correctWorkingDir;
};

