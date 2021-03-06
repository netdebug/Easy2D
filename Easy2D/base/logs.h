// Copyright (c) 2016-2018 Easy2D - Nomango
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#pragma once
#include "../macros.h"
#include "../common/Singleton.hpp"
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

#ifndef E2D_LOG
#	ifdef E2D_DEBUG
#		define E2D_LOG(FORMAT, ...) easy2d::Logger::Instance().Messageln((FORMAT), __VA_ARGS__)
#	else
#		define E2D_LOG __noop
#	endif
#endif

#ifndef E2D_WARNING_LOG
#	define E2D_WARNING_LOG(FORMAT, ...) easy2d::Logger::Instance().Warningln((FORMAT), __VA_ARGS__)
#endif

#ifndef E2D_ERROR_LOG
#	define E2D_ERROR_LOG(FORMAT, ...) easy2d::Logger::Instance().Errorln((FORMAT), __VA_ARGS__)
#endif

namespace easy2d
{
	namespace __console_colors
	{
		using ConsoleColor = std::wostream&(*)(std::wostream&);

#define DECLARE_COLOR(COLOR)\
		extern std::wostream&(stdout_##COLOR)(std::wostream&);\
		extern std::wostream&(stderr_##COLOR)(std::wostream&);

#define DECLARE_BG_COLOR(COLOR)\
		extern std::wostream&(stdout_##COLOR##_bg)(std::wostream&);\
		extern std::wostream&(stderr_##COLOR##_bg)(std::wostream&);

		DECLARE_COLOR(red);
		DECLARE_COLOR(green);
		DECLARE_COLOR(yellow);
		DECLARE_COLOR(blue);
		DECLARE_COLOR(white);
		DECLARE_COLOR(reset);

		DECLARE_BG_COLOR(red);
		DECLARE_BG_COLOR(green);
		DECLARE_BG_COLOR(yellow);
		DECLARE_BG_COLOR(blue);
		DECLARE_BG_COLOR(white);

#undef DECLARE_COLOR
#undef DECLARE_BG_COLOR
	}

	class E2D_API Logger
		: public Singleton<Logger>
	{
		E2D_DECLARE_SINGLETON(Logger);

	public:
		inline void Enable()
		{
			enabled_ = true;
		}

		inline void Disable()
		{
			enabled_ = false;
		}

		template <typename ..._Args>
		inline void Print(const wchar_t* format, _Args&&... args) const
		{
			using namespace __console_colors;
			Output(std::wcout, stdout_white, nullptr, format, std::forward<_Args>(args)...);
		}

		template <typename ..._Args>
		inline void Println(const wchar_t* format, _Args&&... args) const
		{
			using namespace __console_colors;
			OutputLine(std::wcout, stdout_white, nullptr, format, std::forward<_Args>(args)...);
		}

		template <typename ..._Args>
		inline void Message(const wchar_t * format, _Args&&... args) const
		{
			using namespace __console_colors;
			Output(std::wcout, stdout_blue, nullptr, format, std::forward<_Args>(args)...);
		}

		template <typename ..._Args>
		inline void Messageln(const wchar_t * format, _Args&&... args) const
		{
			using namespace __console_colors;
			OutputLine(std::wcout, stdout_blue, nullptr, format, std::forward<_Args>(args)...);
		}

		template <typename ..._Args>
		inline void Warning(const wchar_t* format, _Args&&... args) const
		{
			using namespace __console_colors;
			Output(std::wcerr, stdout_yellow_bg, L"Warning: ", format, std::forward<_Args>(args)...);
		}

		template <typename ..._Args>
		inline void Warningln(const wchar_t* format, _Args&&... args) const
		{
			using namespace __console_colors;
			OutputLine(std::wcerr, stdout_yellow_bg, L"Warning: ", format, std::forward<_Args>(args)...);
		}

		template <typename ..._Args>
		inline void Error(const wchar_t* format, _Args&&... args) const
		{
			using namespace __console_colors;
			Output(std::wcerr, stderr_red_bg, L"Error: ", format, std::forward<_Args>(args)...);
		}

		template <typename ..._Args>
		inline void Errorln(const wchar_t* format, _Args&&... args) const
		{
			using namespace __console_colors;
			OutputLine(std::wcerr, stderr_red_bg, L"Error: ", format, std::forward<_Args>(args)...);
		}

	private:
		Logger();

		template <typename ..._Args>
		inline void OutputLine(std::wostream& os, __console_colors::ConsoleColor color, const wchar_t* prompt, const wchar_t* format, _Args&&... args) const
		{
			if (!enabled_)
				return;

			Output(os, color, prompt, format, std::forward<_Args>(args)...);

			os << std::endl;
			::OutputDebugStringW(L"\r\n");
		}

		template <typename ..._Args>
		inline void Output(std::wostream& os, __console_colors::ConsoleColor color, const wchar_t* prompt, const wchar_t* format, _Args&&... args) const
		{
			if (!enabled_)
				return;

			std::wstring output = MakeOutputString(prompt, format, std::forward<_Args>(args)...);

			os << color << output;
			::OutputDebugStringW(output.c_str());

			ResetColor();
		}

		template <typename ..._Args>
		inline std::wstring MakeOutputString(const wchar_t* prompt, const wchar_t* format, _Args&&... args) const
		{
			static wchar_t temp_buffer[1024 * 3 + 1];

			const auto len = ::_scwprintf(format, std::forward<_Args>(args)...);
			::swprintf_s(temp_buffer, len + 1, format, std::forward<_Args>(args)...);

			std::wstringstream ss;
			ss << Logger::OutPrefix;

			if (prompt)
				ss << prompt;

			ss << temp_buffer;

			return ss.str();
		}

		inline void ResetColor() const
		{
			::SetConsoleTextAttribute(::GetStdHandle(STD_OUTPUT_HANDLE), default_stdout_color_);
			::SetConsoleTextAttribute(::GetStdHandle(STD_ERROR_HANDLE), default_stderr_color_);
		}

		static inline std::wostream& OutPrefix(std::wostream& out)
		{
			std::time_t unix = std::time(nullptr);
			std::tm tmbuf;
			localtime_s(&tmbuf, &unix);
			out << std::put_time(&tmbuf, L"[easy2d] %H:%M:%S ");
			return out;
		}

	private:
		bool enabled_;
		WORD default_stdout_color_;
		WORD default_stderr_color_;
	};
}

namespace easy2d
{
	inline void ThrowIfFailed(HRESULT hr)
	{
		if (FAILED(hr))
		{
			E2D_ERROR_LOG(L"Fatal error with HRESULT of %08X", hr);

			static char buffer[1024 + 1];
			sprintf_s(buffer, "Fatal error with HRESULT of %08X", hr);
			throw std::runtime_error(buffer);
		}
	}
}
