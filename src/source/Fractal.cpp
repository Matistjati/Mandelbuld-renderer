#include "headers/Fractal.h"
#include <iostream>

bool Fractal::replace(std::string& str, const std::string& from, const std::string& to)
 {
	 size_t start_pos = str.find(from);
	 if (start_pos == std::string::npos)
		 return false;
	 str.replace(start_pos, from.length(), to);
	 return true;
}

void Fractal::replaceSection(Section section, std::string& origin, std::string& dest)
{
	bool recording = false;
	for (size_t i = 0; i < origin.length(); i++)
	{
		if (!recording && origin[i] == '%')
		{
			bool same = true;
			for (size_t j = 0; j < section.start.length(); j++, i++)
			{
				if (origin[i] != section.start[j])
				{
					same = false;
					break;
				}
			}
			if (same)
			{
				section.startIndex1 = i;
				recording = true;
			}
		}

		if (recording && origin[i] == '%')
		{
			bool same = true;
			for (size_t j = 0; j < section.end.length(); j++, i++)
			{
				if (origin[i] != section.end[j])
				{
					break;
				}
			}
			if (same)
			{
				section.endIndex1 = i - section.end.length() - 1;
				break;
			}
		}
	}

	if (section.startIndex1 == 0 && section.endIndex1 == 0)
	{
#if _DEBUG
		std::cout << "Startindex and endindex were zero on 1 " << section.start << std::endl;
#endif
		return;
	}

	for (size_t i = 0; i < dest.length(); i++)
	{
		if (dest[i] == '%')
		{
			bool same = true;
			for (size_t j = 0; j < section.start.length(); j++, i++)
			{
				if (dest[i] != section.start[j])
				{
					same = false;
					break;
				}
			}
			if (same)
			{
				section.startIndex2 = i;
				recording = true;
				continue;
			}
		}
	}

	if (section.startIndex2 == 0)
	{
#if _DEBUG
		std::cout << "Startindex was zero on 2 " << section.start << std::endl;
#endif
		return;
	}

	if (!replace(dest, section.start, origin.substr(section.startIndex1, section.endIndex1 - section.startIndex1)))
	{
#if _DEBUG
		std::cout << "Failed" << std::endl;
#endif
	}
}
