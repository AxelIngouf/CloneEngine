#pragma once
#include <fstream>
#include <functional>
#include <sstream>
#include <vector>

/**
 * It allows the creation of a log file and the addition of messages under custom severity levels; error, warning, info and debug.
 * The Logger needs to be initialized in the first place in order to log messages, and there can only be one opened log file at a time.
 */
namespace Core
{
    /**
     * Channels list used to differentiate log messages channels
     */
    enum class ELogChannel
    {
        CLOG_GENERAL,
        CLOG_EDITOR,
        CLOG_RENDER,
        CLOG_NETWORK,
        CLOG_VULKAN_GENERAL,
        CLOG_VULKAN_VALIDATION,
        CLOG_VULKAN_PERFORMANCE,

        CLOG_ALL_CHANNELS
    };

    /**
     * Levels list used to differentiate log messages levels
     */
    enum class ELogLevel
    {
        CLOG_DEBUG,
        CLOG_INFO,
        CLOG_WARNING,
        CLOG_ERROR
    };

    /**
     * Message structures containing the formatted message, the log channel and the log severity level.
     */
    struct CLogMessage
    {
        CLogMessage() = delete;
        CLogMessage(ELogLevel lvl, const std::string& msg, ELogChannel chn);

        std::string message;
        ELogChannel channel;
        ELogLevel   level;
    };

    /**
     * Internal class used to easily simplify logging. This class and its functions must not be called externally.
     */
    class CLog
    {
    public:
        CLog();
        CLog(const CLog&) = delete;
        CLog(CLog&&) = delete;

        virtual ~CLog();

        CLog& operator=(const CLog&) = delete;
        CLog& operator=(CLog&&) = delete;

        /**
         * Internal function to add a specified message to the current log file and print it to the console with a given level.
         * Current time and date will automatically be appended to the message.
         *
         * @param level - Message level
         * @param message - Message to log and print
         * @param channel  - Message channel
         */
        void                LogMessage(ELogLevel level, const std::string& message, ELogChannel channel);

        /**
         * Returns current time and date in string format.
         *
         * @param cleanString - Indicates if the returned string needs to be cleaned for a file name format (replaces '/' and ':' characters)
         * @return Current time and date in string format.
         */
        static std::string  LogTime(bool cleanString = false);

        /**
         * Retrieves all current messages.
         *
         * @return Logged messaged
         */
        [[nodiscard]] std::vector<CLogMessage> GetLogMessages() const { return logMessages; }

        /**
         * Sets a new callback function to call whenever a message is added to logs.
         *
         * @param callback - Callback function
         */
        void                SetCallback(const std::function<void(CLogMessage*)>& callback);

        /**
         * Open a file with the specified filepath.
         * 
         * @param filepath - Filepath of the file to open/create
         */
        void                OpenFile(const std::string& filepath);

        /**
         * Callback function called every time a message is added to the log.
         */
        std::function<void(CLogMessage*)> callbackLog;

        /**
         * Callback function called when an assertion fails
         */
        std::function<void(CLogMessage*)> assertCallback;

    private:
        /**
         * Automatically create 'Logs' directory and the corresponding 'Debug' or 'Release' sub-directory depending on the current project configuration
         *
         * @return Relative file path to the corresponding sub-directory
         */
        static std::string          CreateDirectories();

        /**
         * Formats the message by adding the current date and time, the channel and the level in this order in front of it.
         *
         * @param level - Message level
         * @param message - Unformatted message
         * @param channel - Message channel
         * @return Formatted message
         */
        [[nodiscard]] std::string   FormatMessage(ELogLevel level, const std::string& message, ELogChannel channel) const;

        /**
         * Log File where log messages are stored.
         */
        std::ofstream   logFile;

        /**
         * List of different severity levels name corresponding to their ELogLevel index.
         */
        std::string     logLevels[4]{ "Debug", "Info", "Warning", "Error" };

        /**
         * List of different channels name corresponding to their ELogChannel index.
         */
        std::string     logChannels[7]{ "General", "Editor", "Render", "Network", "Vulkan General", "Vulkan Validation", "Vulkan Performance" };

        /**
         * Vector of every messages received since the start of the editor.
         */
        std::vector<CLogMessage> logMessages;
    };

    /**
     * Return a static CLog object. Singleton.
     * 
     * @return Static CLog singleton
     */
    CLog&   GetLogSingleton();

    /**
     * Default log callback. Print the message on the console.
     * 
     * @param logMessage - Callback message
     */
    inline void         PrintLogMessageConsole(CLogMessage* logMessage) { printf("%s\n", logMessage->message.c_str()); }

    /**
     * Initializes logger, by opening a log file.
     * (note: opening an already existing file will erase its previous content)
     *
     * @param callback - Callback function to call whenever a message is added to logs (default will print messages in the console)
     * @param assertCallback - Callback function to call whenever an assertion fails (default will print fail message in the console)
     * @param filepath - File path to the log file to create/open
     */
     void    InitLogger(const std::function<void(CLogMessage*)> & callback = PrintLogMessageConsole, const std::function<void(CLogMessage*)> & assertCallback = PrintLogMessageConsole, const std::string& filepath = "");

    /**
     * Sets a new callback function to call whenever a message is added to logs (default will print messages in the console)
     *
     * @param callback - Callback function
     */
     void    SetLogCallback(const std::function<void(CLogMessage*)>& callback = PrintLogMessageConsole);

    /**
     * Sets a new callback function to call whenever an assertion fails (default will print fail message in the console)
     *
     * @param callback - Callback function
     */
     void    SetAssertCallback(const std::function<void(CLogMessage*)> & = PrintLogMessageConsole);

    /**
     * Calls LogMessage function of the current CLog object in order to
     * add a specified message to the log file, and call the logs callback.
     *
     * @param level - Message level
     * @param message - Message to log and print
     * @param channel - Message channel, default is General channel
     */
     void    Log(ELogLevel level, const std::string& message, ELogChannel channel = ELogChannel::CLOG_GENERAL);

    /**
     * Checks if a statement is true. If not, an error is added to the logs and the program is stopped with EXIT_FAILURE.
     * 
     * @param statement - Boolean statement to check.
     * @param failMessage - Message to log if the assertion fails.
     * @param channel - Assertion channel, default is General channel.
     */
     void    Assert(bool statement, const std::string& failMessage, ELogChannel channel = ELogChannel::CLOG_GENERAL);

    /**
     * Returns all current log messages from the global logger.
     *
     * @return All log messages
     */
     const std::vector<CLogMessage>&    GetLogMessages();

    /**
     * Returns all current log messages from the global logger corresponding to a specified level and channel (default all channels)
     *
     * @param level - Message level to retrieve
     * @param channel - Message channel to retrieve
     * @return All corresponding messaged based on specified level and channel
     */
     std::vector<CLogMessage>    GetLogMessages(ELogLevel level, ELogChannel channel = ELogChannel::CLOG_ALL_CHANNELS);
}

