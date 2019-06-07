#include "headers/Fractal.h"
#include "headers/Debug.h"
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
	int endOrigin = origin.find(originSection.end, startOrigin);

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
	int endOrigin = origin.find(section.end, startOrigin);

	return replace(dest, section.start, origin.substr(startOrigin, endOrigin - startOrigin));
}

std::string Fractal::getSection(Section s, std::string from)
{
	int startIndex = from.find(s.start);
	int endIndex = from.find(s.end, startIndex);
	if (startIndex == std::string::npos || endIndex == std::string::npos)
	{
		return "";
	}
	startIndex += s.start.length();

	return from.substr(startIndex, endIndex - startIndex);
}

std::string Fractal::getSectionName(std::string str)
{
	int startIndex = str.find('<') + 1;
	int endIndex = str.find('>', startIndex);
	return str.substr(startIndex, endIndex - startIndex);
}

std::string Fractal::getSectionValue(std::string str)
{
	int startIndex = str.find('>') + 1;
	int endIndex = str.find('<', startIndex);
	return str.substr(startIndex, endIndex - startIndex);
}

void Fractal::cleanString(std::string& str, std::vector<char> chars)
{
	for (size_t i = 0; i < chars.size(); i++)
	{
		str.erase(std::remove(str.begin(), str.end(), chars[i]), str.end());
	}
}

bool Fractal::RemoveOuterSection(std::string& str)
{
	int sectionStart = str.find('<');
	int sectionEnd = str.find('>', sectionStart);
	std::string name = str.substr(sectionStart + 1, sectionEnd - sectionStart - 1);

	if (name != "")
	{
		Section s(name);
		replace(str, s.start, "");
		replace(str, s.end, "");
		return true;
	}
	else
	{
		return false;
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
	size_t lastIndex = 0;
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

	std::string end = str.substr(lastIndex + ((str.length() > lastIndex + 1) ? 1 : 0));
	if (!(end.length() == 1 && end[0] == splitBy))
	{
		result.push_back(end);
	}

	return result;
}

std::string Fractal::GetSpecificationByIndex(std::string specification, int index, const std::string presets)
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
				DebugPrint("Invalid format: more closing than opening brackets");
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

	std::string section = specification.substr(startIndex, endIndex - startIndex);
	if (section.find(Section("include").start) != std::string::npos)
	{
		std::vector<std::string> includes = split(getSectionValue(getSection(Section("include"), section)), ',');
		if (includes.size() != 0)
		{
			for (size_t i = 0; i < includes.size(); i++)
			{
				cleanString(includes[i], { '\n', '\t', ' ' });
				std::string preset = getSection(Section(includes[i]), presets);
				LinkSpecification(preset, section);
			}
		}
	}
	return section;
}

void Fractal::LinkSpecification(std::string& source, std::string& target)
{
	std::vector<std::string> sections = GetOuterSections(source);

	for (size_t i = 0; i < sections.size(); i++)
	{
		std::string sectionName = getSectionName(sections[i]);  
		if (target.find(sectionName) == std::string::npos)
		{
			target += sections[i];
		}
		else
		{
			RemoveOuterSection(sections[i]);
			std::vector<std::string> innerSections = splitNotInChar(sections[i], ',', '[', ']');
			int sectionStart = target.find(sectionName) + sectionName.length();
			for (size_t i = 0; i < innerSections.size(); i++)
			{
				// Don't overwrite
				if (target.find(getSectionName(innerSections[i])) == std::string::npos)
				{
					target.insert(sectionStart + 1, innerSections[i]+ ",");
				}
			}
		}
	}
}

std::vector<std::string> Fractal::GetOuterSections(std::string& source)
{
	std::vector<std::string> sections(0);
	cleanString(source, { '\n', '\t' });

	int sectionStart = -1;
	int sectionEnd = -1;
	std::string sectionName;
	for (size_t i = 0; i < source.length(); i++)
	{
		if (source[i] == '<')
		{
			sectionStart = i;
			sectionEnd = source.find('>', i);
			sectionName = source.substr(i + 1, sectionEnd - i - 1);

			std::string end = "</" + sectionName + ">";
			int sectionCloserIndex = source.find(end, sectionEnd);
			if (sectionCloserIndex != std::string::npos)
			{
				sections.push_back(source.substr(i, sectionCloserIndex + (sectionName.length() + 3) - i));
				i = sectionCloserIndex + sectionName.length();
			}
		}
	}
	return sections;
}

bool Fractal::StringToBool(std::string str)
{
	if (str.length() == 0) return false;
	else if (str.length() == 1) return str[0] != '0';
	else if (str == "true") return true;
	else if (str == "false") return false;
	else return false;
}
