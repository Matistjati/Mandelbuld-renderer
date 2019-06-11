#include "headers/FileManager.h"
#include "headers/Debug.h"
#include <fstream>
#include <Shlwapi.h>
#include <sys/stat.h>

bool FileManager::correctWorkingDir = FileManager::SetCorrectDirectory();

inline std::string FileManager::GetWorkingDirectory()
{
	const int bufferSize = 512;
	char path[bufferSize];
	GetCurrentDirectory(bufferSize, path);
	return std::string(path);
}

std::string FileManager::readFile(std::string path)
{
	if (!correctWorkingDir)
	{
		SetCorrectDirectory();
	}

	std::ifstream t(path);
	if (t.fail())
	{
		char buff[256];
		strerror_s(buff, errno);
		std::cout << "Ifstream error trying to open file " << getFileName(path) << ". Error: " << buff << std::endl;
		return "";
	}
	return std::string((std::istreambuf_iterator<char>(t)),
		std::istreambuf_iterator<char>());
}

std::string FileManager::getFileName(std::string path)
{
	int lastPos = path.find_last_of('\\');
	if (lastPos == std::string::npos)
	{
		lastPos = path.find_last_of('/');
		if (lastPos == std::string::npos)
		{
			return path;
		}
	}
	return path.substr(lastPos + 1);
}

bool FileManager::fileExists(const std::string& name)
{
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}

bool FileManager::SetCorrectDirectory()
{
	if (correctWorkingDir) return true;

	std::string workingDir = GetWorkingDirectory();

	DebugPrint("Initial working directory: " + workingDir);

	if (getFileName(workingDir) == "Debug" || getFileName(workingDir) == "Release")
	{
		workingDir = workingDir.substr(0, workingDir.find_last_of("/\\"));

		DebugPrint("New working directory: " + workingDir);


		if (!SetCurrentDirectory(workingDir.c_str()))
		{
			DebugPrint("Error setting working directory");
			return false;
		}
	}

	return true;
}
