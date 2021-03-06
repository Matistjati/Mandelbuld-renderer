#include "headers/FileManager.h"
#include "headers/Debug.h"
#include <Shlwapi.h>
#include <sys/stat.h>
#include <fstream>

bool FileManager::correctWorkingDir = false;

inline std::string FileManager::GetWorkingDirectory()
{
	const int bufferSize = 512;
	char path[bufferSize];
	GetCurrentDirectory(bufferSize, path);
	return std::string(path);
}

std::string FileManager::ReadFile(std::string path)
{
	SetCorrectDirectory(); // Does nothing if correct

	std::ifstream t(path);
	if (t.fail())
	{
		char buff[256];
		strerror_s(buff, errno);
		std::cout << "Ifstream error trying to open file " << GetFileName(path) << ". Error: " << buff << std::endl;
		return "";
	}
	return std::string((std::istreambuf_iterator<char>(t)),
		std::istreambuf_iterator<char>());
}

std::string FileManager::GetFileName(std::string path)
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

bool FileManager::FileExists(const std::string& name)
{
	SetCorrectDirectory(); // Does nothing if correct

	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}

std::vector<std::string> FileManager::GetDirectoryFileNames(const std::string directory)
{
	SetCorrectDirectory(); // Does nothing if correct

	std::vector<std::string> names;
	std::string search_path = directory + "*.*";
	WIN32_FIND_DATA fd;
	HANDLE hFind = ::FindFirstFile(search_path.c_str(), &fd);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			// read all (real) files in current folder
			// , delete '!' read other 2 default folder . and ..
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				names.push_back(fd.cFileName);
			}
		} while (::FindNextFile(hFind, &fd));
		::FindClose(hFind);
	}
	return names;
}

void FileManager::SetCorrectDirectory()
{
	if (correctWorkingDir) return;

	std::string workingDir = GetWorkingDirectory();

	DebugPrint("Initial working directory: " + workingDir);

	if (GetFileName(workingDir) == "Debug" || GetFileName(workingDir) == "Release")
	{
		workingDir = workingDir.substr(0, workingDir.find_last_of("/\\"));

		DebugPrint("New working directory: " + workingDir);


		if (!SetCurrentDirectory(workingDir.c_str()))
		{
			DebugPrint("Error setting working directory");
			return;
		}
		correctWorkingDir = true;
	}
	else
	{
		correctWorkingDir = true;
	}
}
