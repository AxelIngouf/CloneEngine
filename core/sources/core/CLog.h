#pragma once
#include <fstream>
#include <functional>
#include <vector>
#include <mutex>

#define ID(x) x
#define GET_MACRO(_1, _2, _3, NAME, ...) NAME
/**
 * LOG(Level, Message, Channel (optional))
 */
#define LOG(...) ID(GET_MACRO(__VA_ARGS__, LOG3, LOG2)(__VA_ARGS__))
/**
 * ASSERT(Boolean Statement, Fail message, Channel(optional))
 */
#define ASSERT(...) ID(GET_MACRO(__VA_ARGS__, ASSERT3, ASSERT2)(__VA_ARGS__))

#ifndef _SHIPPING
constexpr bool IS_RELEASE = false;
#define LOG2(level, message) Log(level, message, __LINE__, __FILE__)
#define LOG3(level, message, channel) Log(level, message, __LINE__, __FILE__, channel)
#else
constexpr bool IS_RELEASE = true;
#ifndef LOG_MIN_LEVEL // Minimum required level to log messages
#define LOG_MIN_LEVEL 2
#endif
#define LOG2(level, message) if constexpr ((int)(level) >= LOG_MIN_LEVEL) Log(level, message, __LINE__, __FILE__)
#define LOG3(level, message, channel) if constexpr ((int)(level) >= LOG_MIN_LEVEL) Log(level, message, __LINE__, __FILE__, channel)
#endif

#define ASSERT2(statement, message) Core::Assert(statement, message, __LINE__, __FILE__)
#define ASSERT3(statement, message, channel) Core::Assert(statement, message, __LINE__, __FILE__, channel)


namespace Core
{
	enum class ELogChannel
	{
		CLOG_GENERAL,
		CLOG_EDITOR,
		CLOG_RENDER,
		CLOG_NETWORK,
		CLOG_PHYSICS,
		CLOG_VULKAN_GENERAL,
		CLOG_VULKAN_VALIDATION,
		CLOG_VULKAN_PERFORMANCE,
		CLOG_ECS,
		CLOG_SCRIPT,
		CLOG_SOUND,
		CLOG_REFLECTION,
		CLOG_MODEL,

		CLOG_ALL_CHANNELS
	};

	enum class ELogLevel
	{
		CLOG_DEBUG,
		CLOG_INFO,
		CLOG_WARNING,
		CLOG_ERROR
	};

	struct CLogMessage
	{
		CLogMessage() = delete;
		CLogMessage(ELogLevel lvl, const std::string& msg, int line, const char* file, ELogChannel chn);

		std::string message;
		ELogChannel channel;
		ELogLevel level;
		const char* filePath;
		int logLine;
	};

	class CLog final
	{
	public:
		CLog();
		CLog(const CLog&) = delete;
		CLog(CLog&&) = delete;

		~CLog();

		CLog& operator=(const CLog&) = delete;
		CLog& operator=(CLog&&) = delete;

		void LogMessage(ELogLevel level, const std::string& message, int line, const char* file, ELogChannel channel);

		static std::string LogTime(bool cleanString = false);

		[[nodiscard]] const std::vector<CLogMessage>& GetLogMessages() const { return logMessages; }

		void SetCallback(const std::function<void(CLogMessage*)>&);
		void SetAssertCallback(const std::function<void(CLogMessage*)>&);

		void OpenFile(const std::string& filepath);

		template <typename ... Args>
		static std::string FormatString(const std::string& format, Args ... args)
		{
			std::string buffer;
			buffer.resize(snprintf(nullptr, 0, format.c_str(), args ...) + 1);
			snprintf(&buffer[0], buffer.size(), format.c_str(), args ...);
			return buffer;
		}

		std::function<void(CLogMessage*)> callbackLog;
		std::function<void(CLogMessage*)> assertCallback;
	private:

		static std::string CreateDirectories();

		[[nodiscard]] std::string FormatMessage(ELogLevel level, const std::string& message, ELogChannel channel) const;

		std::ofstream logFile;
		inline static std::string logLevels[4]{"Debug", "Info", "Warning", "Error"};
		inline static std::string logChannels[static_cast<int>(ELogChannel::CLOG_ALL_CHANNELS)]{
			"General", "Editor", "Render", "Network", "Physics", "Vulkan General", "Vulkan Validation",
			"Vulkan Performance", "ECS", "Script", "Sound", "Reflection", "Model"
		};

		std::vector<CLogMessage> logMessages;

		std::mutex logMutex;
	};

	CLog& GetLogSingleton();

	inline void PrintLogMessageConsole(CLogMessage* logMessage) { printf("%s\n", logMessage->message.c_str()); }

	void SetLogCallback(const std::function<void(CLogMessage*)>& = PrintLogMessageConsole);

	void SetAssertCallback(const std::function<void(CLogMessage*)>& = PrintLogMessageConsole);

	void Log(ELogLevel level, const std::string& message, int line, const char* file,
	                     ELogChannel channel = ELogChannel::CLOG_GENERAL);

	void Assert(bool statement, const std::string& failMessage, int line, const char* file,
	                        ELogChannel channel = ELogChannel::CLOG_GENERAL);

	const std::vector<CLogMessage>& GetLogMessages();

	std::vector<CLogMessage> GetLogMessages(ELogLevel level,
	                                                    ELogChannel channel = ELogChannel::CLOG_ALL_CHANNELS);
}

#define LOG_ERROR Core::ELogLevel::CLOG_ERROR
#define LOG_WARNING Core::ELogLevel::CLOG_WARNING
#define LOG_INFO Core::ELogLevel::CLOG_INFO
#define LOG_DEBUG Core::ELogLevel::CLOG_DEBUG
