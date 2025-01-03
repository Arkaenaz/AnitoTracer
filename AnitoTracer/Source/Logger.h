#pragma once

#include <iostream>
#include <string>
#include <exception>

#include <Windows.h>

#define DEBUG_LOGS true
#define ERROR_LOGS true
#define SUCCESS_LOGS false

namespace Anito
{
    class Logger
    {
    public:
        static void log(const std::string& message)
        {
            if (!DEBUG_LOGS)
                return;

            std::cout << message << "\n";
        }

        template <class T>
        static void debug(T* sender, const std::string& message)
        {
            if (!DEBUG_LOGS)
                return;

            const std::string logMessage = "[" + getType(sender) + "][DEBUG]: " + message;
            std::cout << logMessage << std::endl;
        }

        static void debug(const std::string& message)
        {
            if (!DEBUG_LOGS)
                return;

            const std::string logMessage = "[DEBUG]: " + message;
            std::cout << "[DEBUG]: " << message << "\n";
        }

        static void debug(const std::wstring& message)
        {
            if (!DEBUG_LOGS)
                return;

            const std::wstring logMessage = L"[DEBUG]: " + message;
            std::wcout << logMessage << "\n";
        }

        template <class T>
        static bool debug(T* sender, const HRESULT result)
        {
            if (FAILED(result))
            {
                const std::string message = std::system_category().message(result);
                debug(sender, message);
                return false;
            }

            if (SUCCESS_LOGS)
                debug(sender, "Operation was successful.");

            return true;
        }

        template <class T>
        static void error(T* sender, const std::string& msg)
        {
            if (!ERROR_LOGS)
                return;

            const std::string errorMessage = "[" + getType(sender) + "][ERROR]: " + msg;
            std::cout << errorMessage << "\n";
        }

        static void throw_exception(const std::string& msg)
        {
            std::cout << "[EXCEPTION]: " << msg << "\n";
            throw std::exception(msg.c_str());
        }

        static void out_of_range(const std::string& msg)
        {
            std::cout << "[OUT OF RANGE]: " << msg << "\n";
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