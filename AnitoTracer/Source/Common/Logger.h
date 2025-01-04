#pragma once

#include <iostream>
#include <string>
#include <exception>

#include <Windows.h>

#include "Time.h"

#define ALL_LOGS true
#define DEBUG_LOGS true
#define ERROR_LOGS true
#define SUCCESS_LOGS false

namespace Anito
{
    class Logger
    {
    public:
        static void log(const std::wstring& msg, bool includeTime = false)
        {
            if (!ALL_LOGS)
                return;

            if (includeTime)
                std::wcout << "[" << Time::getTime() << "]";

        	std::wcout << msg << "\n";
        }

        static void log(const std::string& msg, bool includeTime = false)
        {
            if (!ALL_LOGS)
                return;

            std::wstring wmsg(msg.begin(), msg.end());
            log(wmsg, includeTime);
        }

        template <class T>
        static void log(T* sender, const std::string& msg)
        {
            if (!ALL_LOGS)
                return;

            log("[" + getType(sender) + "]: " + msg, true);
        }

        template <class T>
        static void logHResult(T* sender, const HRESULT hr)
        {
            if (FAILED(hr))
            {
                const std::string msg = std::system_category().message(hr);
                debug(sender, msg);
            }

            if (SUCCESS_LOGS)
                debug(sender, "Operation was successful.");
        }

        template <class T>
        static void debug(T* sender, const std::string& msg)
        {
            if (!DEBUG_LOGS)
                return;

            log("[" + getType(sender) + "][DEBUG]: " + msg, true);
        }

        static void debug(const std::string& msg)
        {
            if (!DEBUG_LOGS)
                return;

            log("[DEBUG]: " + msg, true);
        }

        static void debug(const std::wstring& msg)
        {
            if (!DEBUG_LOGS)
                return;

            log(L"[DEBUG]: " + msg, true);
        }

        static void error(const std::string& msg)
        {
            if (!ERROR_LOGS)
                return;

            log("[ERROR]: " + msg, true);
        }

        template <class T>
        static void error(T* sender, const std::string& msg)
        {
            if (!ERROR_LOGS)
                return;

            log("[" + getType(sender) + "][ERROR]: " + msg, true);
        }

        static void throw_exception(const std::string& msg)
        {
            error(msg);
            throw std::exception(msg.c_str());
        }

        static void throw_out_of_range(const std::string& msg)
        {
            error(msg);
            throw std::out_of_range(msg.c_str());
        }

    private:
        template <class T>
        static std::string getType(T* type)
        {
            return typeid(*type).name();
        }
    };
}