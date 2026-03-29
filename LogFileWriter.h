#ifndef LOG_FILE_WRITER_H
#define LOG_FILE_WRITER_H


//Including <queue> gives you access to std::queue, which is a FIFO (First-In, First-Out) container adapter
#include <queue>

// Needed by linux g++ compiler
#include <string>
#include <condition_variable>
#include <thread>

class LogFileWriter {

public:

    // Keep the enum for internal C++ use
    enum LogLevel { DEBUG = 0, INFO = 1, WARN = 2, ERROR = 3, FATAL = 4 };

    // Internal C++ macro-friendly log. For use in log_gd (gdscript) and LOG_XXX macros in C++
    // It is called by log_gd which is called from GDSCRIPT
    // __FILE__ returns a literal string char*, that's why the p_file parameter is char*.
    void _log_internal(LogLevel p_level, const std::string& p_msg, const std::string& p_file, int p_line, bool isStdOutput);

    // Generic log function and the log_level setter exported to GDSCRIPT
    // Sets the minimum log level below which logs are not stored
    void set_min_level(int p_level);

    // LogFileWriter is a singleton; this is the method that returns the single instance of the object.
    static LogFileWriter* get_singleton() { return singleton; }

    // Constructor & Destructor
    LogFileWriter();
    ~LogFileWriter();

private:

    // Log input object structure
    struct LogEntry {
        int level;          // Nivel del LOG
        std::string message;     // Mensaje del LOG
        std::string timestamp;   // Marca de tiempo del LOG
        std::string file;
        int line;
        bool isStdOutput;
    };

    // Singleton pointer
    static inline LogFileWriter* singleton = nullptr;

    //std::atomic<int> ensures that the operation happens as a single, indivisible unit.
    std::atomic<int> min_level{0};
    
    // FIFO queue of LogEntry objects
    std::queue<LogEntry> log_queue;

    // Building a Thread-Safe Queue.
    std::mutex queue_mutex;

    // Final piece to the puzzle for a high-performance Producer-Consumer system!
    std::condition_variable cv;

    // object that actually runs your code in parallel.
    std::thread worker_thread;

    //std::atomic<int> ensures that the operation happens as a single, indivisible unit.
    std::atomic<bool> should_exit{false};

    // private methods
    // Method that manages the writing of a log when it occurs
    void process_logs();

    // a method that provides us with the date professionally
    std::string get_timestamp();
};

// C++ Helper Macros. To use in pure C++
#define LOG_DEBUG(m) { std::string _temp_file = __FILE__; LogFileWriter::get_singleton()->_log_internal(LogFileWriter::DEBUG, m, _temp_file, __LINE__) }
#define LOG_INFO(m) { std::string _temp_file = __FILE__; LogFileWriter::get_singleton()->_log_internal(LogFileWriter::INFO, m, _temp_file, __LINE__) }
#define LOG_WARN(m) { std::string _temp_file = __FILE__; LogFileWriter::get_singleton()->_log_internal(LogFileWriter::WARN, m, _temp_file, __LINE__) }
#define LOG_ERR(m)  { std::string _temp_file = __FILE__; LogFileWriter::get_singleton()->_log_internal(LogFileWriter::ERROR, m, _temp_file, __LINE__) }
#define LOG_FATAL(m)  { std::string _temp_file = __FILE__; LogFileWriter::get_singleton()->_log_internal(LogFileWriter::FATAL, m, _temp_file, __LINE__) }

#endif