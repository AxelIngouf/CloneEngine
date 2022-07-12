#include <ctime>

#include <direct.h>

#include "CLog.h"


#include <cassert>
#include <iostream>

namespace Core
{
    CLogMessage::CLogMessage(const ELogLevel lvl, const std::string& msg, int line, const char* file, const ELogChannel chn)
    {
        message = msg;
        channel = chn;
        level = lvl;
        logLine = line;
        filePath = file;
    }

    CLog::CLog()
    {
        const std::string configText(IS_RELEASE ? "release_" : "debug_");
        const std::string file = "log_" + configText + LogTime(true) + ".txt";

        OpenFile(file);
        SetCallback(PrintLogMessageConsole);
        SetAssertCallback(PrintLogMessageConsole);
    }

    CLog::~CLog()
    {
        if (logFile.is_open())
        {
            logFile.close();
        }
    }

    void CLog::LogMessage(const ELogLevel level, const std::string& message, int line, const char* file, const ELogChannel channel)
    {
        const std::string messageFormatted(FormatMessage(level, message, channel));

        {
            std::lock_guard<std::mutex> lock(logMutex);

            if (logFile.is_open())
            {
                logFile << messageFormatted << std::endl;
            }

            logMessages.emplace_back(level, messageFormatted, line, file, channel);

            if(callbackLog != nullptr)
            callbackLog(&logMessages.back());
        }
    }

    std::string CLog::LogTime(const bool cleanString)
    {
        time_t timestamp = time(nullptr);
        struct tm timeInfos {};
        const errno_t err = localtime_s(&timeInfos, &timestamp);
        char buffer[80];
        std::string formattedDate;
        if (!err)
        {
            if (cleanString)
            {
                strftime(buffer, 80, "%H%M-%d_%m_%Y", &timeInfos);
            }
            else
            {
                strftime(buffer, 80, "%d/%m/%Y-%H:%M.%S", &timeInfos);
            }
            formattedDate = buffer;
        }

        return formattedDate;
    }

    void CLog::SetCallback(const std::function<void(CLogMessage*)>& callback)
    {
        callbackLog = callback;
    }

    void CLog::SetAssertCallback(const std::function<void(CLogMessage*)>& callback)
    {
        assertCallback = callback;
    }

    void CLog::OpenFile(const std::string& filepath)
    {
        const std::string& logSubDirectory = CreateDirectories();

        logFile.open(logSubDirectory + filepath);
    }

    std::string CLog::CreateDirectories()
    {
        std::string logSubDirectory(IS_RELEASE ? "Release" : "Debug");
        logSubDirectory = "Logs/" + logSubDirectory + "/";

        if (!_mkdir("Logs")) // create logs directory
        {}
        if (!_mkdir(logSubDirectory.c_str())) // create debug or release sub-directory
        {}

        return logSubDirectory;
    }

    std::string CLog::FormatMessage(const ELogLevel level, const std::string& message, const ELogChannel channel) const
    {
        return "[" + LogTime() + "] " + logChannels[static_cast<int>(channel)] + " " +
            logLevels[static_cast<int>(level)] + " : " + message + "";
    }

    CLog& GetLogSingleton()
    {
        static CLog log;

        return log;
    }

    void SetLogCallback(const std::function<void(CLogMessage*)>& callback)
    {
        GetLogSingleton().SetCallback(callback);
    }

    void SetAssertCallback(const std::function<void(CLogMessage*)>& assertCallback)
    {
        GetLogSingleton().SetAssertCallback(assertCallback);
    }

    void Log(const ELogLevel level, const std::string& message, const int line, const char* file, const ELogChannel channel)
    {
        GetLogSingleton().LogMessage(level, message, line, file, channel);
    }

    void Assert(bool statement, const std::string& failMessage, const int line, const char* file, const ELogChannel channel)
    {
        if(!statement)
        {
            GetLogSingleton().LogMessage(LOG_ERROR, failMessage, line, file, channel);
            GetLogSingleton().assertCallback(&GetLogMessages(LOG_ERROR, channel).back());
            assert(false);
        }
    }

    const std::vector<CLogMessage>& GetLogMessages()
    {
        return GetLogSingleton().GetLogMessages();
    }

    std::vector<CLogMessage> GetLogMessages(const ELogLevel level, const ELogChannel channel)
    {
        std::vector<CLogMessage> filteredMessages;

        for (const CLogMessage& logMessage : GetLogSingleton().GetLogMessages())
        {
            if (logMessage.level == level)
            {
                if (channel == ELogChannel::CLOG_ALL_CHANNELS || logMessage.channel == channel)
                {
                    filteredMessages.emplace_back(logMessage);
                }
            }
        }

        return filteredMessages;
    }
}
