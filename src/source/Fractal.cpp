#include "headers/Fractal.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

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

std::string Fractal::readFile(std::string path)
{
	std::ifstream t(path);
	return std::string((std::istreambuf_iterator<char>(t)),
		std::istreambuf_iterator<char>());
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
