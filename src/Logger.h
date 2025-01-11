#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <sstream>
#include <chrono>
#include <filesystem>

#include "SafeQueue.h"

namespace GosFrontline
{
    enum class MessageType : int
    {
        INFO = 0,
        WARNING,
        ERROR,
        DEBUG,
        FATAL
    };

    class Logger
    {
    public:
        Logger(std::string fileName);
        Logger(std::string fileName, std::string pathName);
        ~Logger();

        void log(const std::string &message, MessageType type);
        void infinite_run();
        void run();
        void setEndFlag();
        std::string getFileName() const;

    private:
        void write(const std::string &message);
        std::string fileName, pathName;
        std::ofstream m_file;
        SafeQueue<std::string> m_queue;
        static const std::vector<std::string> types;
        bool endFlag = false;
    };
} // namespace GosFrontline

const std::vector<std::string> GosFrontline::Logger::types = {"INFO", "WARNING", "ERROR", "DEBUG", "FATAL"};

GosFrontline::Logger::Logger(std::string fn)
{
    pathName = "log/";
    fileName = fn;
    if (not std::filesystem::exists(std::filesystem::path(pathName)))
    {
        std::filesystem::create_directory(std::filesystem::path(pathName));
    }
    fileName = "log/" + fn + "_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count() / 1000000) + ".log";
    m_file.open(fileName, std::ios::out | std::ios::app);
}

GosFrontline::Logger::Logger(std::string fn, std::string pn)
{
    this->pathName = pn;
    this->fileName = fn;
    if (not std::filesystem::exists(std::filesystem::path(pathName)))
    {
        std::filesystem::create_directory(std::filesystem::path(pathName));
    }
    fileName = pn + "/" + fn + "_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count() / 1000000) + ".log";
    m_file.open(fileName, std::ios::out | std::ios::app);
}

GosFrontline::Logger::~Logger()
{
    m_file.close();
}

void GosFrontline::Logger::log(const std::string &message, MessageType type = MessageType::INFO)
{
    m_queue.push(std::to_string(std::chrono::system_clock::now().time_since_epoch().count() / 1000000) + " " + types[static_cast<int>(type)] + " | " + message);
}

void GosFrontline::Logger::infinite_run()
{
    while (true)
    {
        if (m_queue.empty())
        {
            if (endFlag)
            {
                return;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }
        std::string message = m_queue.pop();
        write(message);
    }
}

void GosFrontline::Logger::run()
{
    while (not m_queue.empty())
    {
        std::string message = m_queue.pop();
        write(message);
    }
}

void GosFrontline::Logger::write(const std::string &message)
{
    m_file << message << std::endl;
}

std::string GosFrontline::Logger::getFileName() const
{
    return fileName;
}

void GosFrontline::Logger::setEndFlag()
{
    endFlag = true;
}

#endif // LOGGER_H