#include "headers/Fractal.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>

bool Fractal::replace(std::string& str, const std::string& from, const std::string& to)
 {
	 size_t start_pos = str.find(from);
	 if (start_pos == std::string::npos)
		 return false;
	 str.replace(start_pos, from.length(), to);
	 return true;
}

bool Fractal::replaceSection(Section originSection, Section destSection, std::string& origin, std::string& dest)
{
	int index = origin.find(originSection.start);
	if (index == std::string::npos)
	{
		return false;
	}
	int startOrigin = index + originSection.start.length();
	int endOrigin = origin.find(originSection.end);

	return replace(dest, destSection.start, origin.substr(startOrigin, endOrigin - startOrigin));
}

bool Fractal::replaceSection(Section section, std::string& origin, std::string& dest)
{
	int index = origin.find(section.start);
	if (index == std::string::npos)
	{
		return false;
	}
	int startOrigin = index + section.start.length();
	int endOrigin = origin.find(section.end);

	return replace(dest, section.start, origin.substr(startOrigin, endOrigin - startOrigin));
}

std::string Fractal::getSection(Section s, std::string from)
{
	int startIndex = from.find(s.start);
	int endIndex = from.find(s.end);
	if (startIndex == std::string::npos || endIndex == std::string::npos)
	{
		return "";
	}
	startIndex += s.start.length();

	return from.substr(startIndex, endIndex - startIndex);
}

std::string Fractal::getFileName(std::string path)
{
	int lastPos = path.find('\\');
	if (lastPos == std::string::npos)
	{
		lastPos = path.find('/');
		if (lastPos == std::string::npos)
		{
			return path;
		}
	}
	return path.substr(lastPos + 1);
}

std::string Fractal::getSectionName(std::string str)
{
	int startIndex = str.find('<') + 1;
	int endIndex = str.find('>');
	return str.substr(startIndex, endIndex - startIndex);
}

std::string Fractal::getSectionValue(std::string str)
{
	int startIndex = str.find('>') + 1;
	int endIndex = str.find('<', startIndex);
	return str.substr(startIndex, endIndex - startIndex);
}

std::string Fractal::readFile(std::string path)
{
	std::ifstream t(path);
	return std::string((std::istreambuf_iterator<char>(t)),
		std::istreambuf_iterator<char>());
}

void Fractal::cleanString(std::string& str, std::vector<char> chars)
{
	for (size_t i = 0; i < chars.size(); i++)
	{
		str.erase(std::remove(str.begin(), str.end(), chars[i]), str.end());
	}
}

std::vector<std::string> Fractal::split(std::string str, char splitBy)
{
	std::stringstream test(str);
	std::string segment;
	std::vector<std::string> seglist;

	while (std::getline(test, segment, splitBy))
	{
		seglist.push_back(segment);
	}
	return seglist;
}

std::vector<std::string> Fractal::splitNotInChar(std::string str, char splitBy, char opener, char closer)
{
	std::vector<std::string> result;

	cleanString(str, { '\n','\t', ' '});

	int level = 0;
	int lastIndex = 0;
	for (size_t i = 0; i < str.length(); i++)
	{
		if (str[i] == splitBy && level == 0)
		{
			result.push_back(str.substr(lastIndex + ((lastIndex == 0) ? 0 : 1), i - lastIndex - ((lastIndex == 0) ? 0 : 1)));
			lastIndex = i;
		}
		else if (str[i] == opener) level++;
		else if (str[i] == closer) level--;
	}

	if (result.size() == 0)
	{
		result.push_back(str);
	}

	return result;
}

std::string Fractal::GetSpecificationByIndex(std::string specification, int index)
{
	size_t n = std::count(specification.begin(), specification.end(), '{');
	int bracketCount = 0;
	int bracketLevel = 0;
	int startIndex = 0;
	for (size_t i = 0; i < specification.length(); i++)
	{
		if (specification[i] == '{')
		{
			if (bracketCount == index)
			{
				startIndex = i + 1;
				break;
			}
			bracketCount++;
			bracketLevel++;
		}
		else if (specification[i] == '}')
		{
			bracketLevel--;
			if (bracketLevel < 0)
			{
				std::cout << "Invalid format: more closing than opening brackets" << std::endl;
				return "";
			}
		}
	}
	

	if (startIndex == 0)
	{
		return "";
	}

	int endIndex = specification.find('}', startIndex);

	if (endIndex == std::string::npos)
	{
		return "";
	}

	return specification.substr(startIndex, endIndex - startIndex);
}

void Fractal::Print(std::string c)
{
	std::cout << c << std::endl;
}
