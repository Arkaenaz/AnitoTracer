#pragma once

#include <ctime>
#include <sstream>
#include <iomanip>

namespace Anito
{
	class Time
	{
	public:
		/* Get Time in format '00:00:00' */
		/* Stripped = 000000 */
		static std::wstring getTime(bool stripped = false)
		{
			time_t now = time(0);
			tm ltm;
			localtime_s(&ltm, &now);
			std::wstringstream wss;
			wss << std::put_time(&ltm, L"%T");

			std::wstring timeString = wss.str();

			if (stripped) 
			{
				std::wstring chars = L":";
				for (wchar_t c : chars) 
				{
					timeString.erase(std::remove(timeString.begin(), timeString.end(), c), timeString.end());
				}
			}
			return timeString;
		}

		/* Get date in format '00/00/00' */
		/* Stripped = 000000 */
		static std::wstring getDate(bool stripped = false)
		{
			time_t now = time(0);
			tm ltm;
			localtime_s(&ltm, &now);
			std::wstringstream wss;
			wss << std::put_time(&ltm, L"%d/%m/%y");
			std::wstring timeString = wss.str();

			if (stripped) {
				std::wstring chars = L"/";
				for (wchar_t c : chars)
				{
					timeString.erase(std::remove(timeString.begin(), timeString.end(), c), timeString.end());
				}
			}

			return timeString;
		}

		/* Get date time in format '00/00/00 00:00:00' */
		/* Stripped = 000000000000 */
		static std::wstring getDateTimeString(bool stripped = false)
		{
			std::wstring timeString = getDate(stripped) + L" " + getTime(stripped);

			if (stripped) {
				std::wstring chars = L" ";
				for (wchar_t c : chars)
				{
					timeString.erase(std::remove(timeString.begin(), timeString.end(), c), timeString.end());
				}
			}

			return timeString;
		}
	};
}