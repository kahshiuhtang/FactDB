#ifndef LOGGING_FACTDB_HPP
#define LOGGING_FACTDB_HPP

#include <fstream>
#include <iostream>
#include <mutex>
#include <string>
#include <ctime>
#include <memory>

namespace factdb {

enum class LogLevel {
    INFO,
    WARNING,
    ERROR,
    DEBUG
};

class Logger {
public:
    static Logger& get_instance() {
        static Logger instance;
        return instance;
    }

    void configure(const std::string& log_file_path, LogLevel level = LogLevel::INFO) {
        log_level_ = level;
        if (!log_file_path.empty()) {
            log_file_ = std::make_unique<std::ofstream>(log_file_path, std::ios::out | std::ios::app);
            if (!log_file_ || !log_file_->is_open()) {
                std::cerr << "Failed to open log file: " << log_file_path << std::endl;
                log_file_.reset();  // reset to release if it was partially opened
            }
        }
    }

    void log(LogLevel level, const std::string& message) {
        if (level >= log_level_) {
            std::lock_guard<std::mutex> guard(log_mutex_);
            std::string log_message = format_message_(level, message);

            if (log_file_ && log_file_->is_open()) {
                *log_file_ << log_message << std::endl;
            } else {
                std::cerr << log_message << std::endl;
            }
        }
    }

    void info(const std::string& message) { log(LogLevel::INFO, message); }
    void warn(const std::string& message) { log(LogLevel::WARNING, message); }
    void error(const std::string& message) { log(LogLevel::ERROR, message); }
    void debug(const std::string& message) { log(LogLevel::DEBUG, message); }

private:
    std::unique_ptr<std::ofstream> log_file_;
    LogLevel log_level_ = LogLevel::INFO;
    std::mutex log_mutex_;

    Logger() = default;

    std::string log_level_tostring_(LogLevel level) const {
        switch (level) {
            case LogLevel::INFO: return "INFO";
            case LogLevel::WARNING: return "WARNING";
            case LogLevel::ERROR: return "ERROR";
            case LogLevel::DEBUG: return "DEBUG";
        }
        return "UNKNOWN";
    }

    std::string format_message_(LogLevel level, const std::string& message) {
        time_t now = time(nullptr);
        tm* localTime = localtime(&now);
        char timeBuffer[20];
        strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", localTime);

        return std::string("[") + timeBuffer + "] [" + log_level_tostring_(level) + "] " + message;
    }

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
};

}  

#endif