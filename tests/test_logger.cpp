#include "logger/logging.hpp"
#include <gtest/gtest.h>
#include <fstream>
#include <string>
#include <thread>
#include <vector>

namespace factdb {

std::string getLastLogLine(const std::string& filePath) {
    std::ifstream file(filePath);
    std::string line, lastLine;
    while (std::getline(file, line)) {
        lastLine = line;
    }
    return lastLine;
}

class LoggerTest : public ::testing::Test {
protected:
    const std::string logFilePath = "test_log.log";

    void SetUp() override {
        std::ofstream file(logFilePath, std::ios::trunc);
    }

    void TearDown() override {
        std::remove(logFilePath.c_str());
    }
};

TEST_F(LoggerTest, ConfigureLogFilePath) {
    Logger::get_instance().configure(logFilePath, LogLevel::INFO);
    std::ofstream file(logFilePath);
    EXPECT_TRUE(file.good());
}

TEST_F(LoggerTest, LogInfoMessage) {
    Logger::get_instance().configure(logFilePath, LogLevel::INFO);
    Logger::get_instance().info("Info message test.");
    EXPECT_TRUE(getLastLogLine(logFilePath).find("INFO") > 0);
}

TEST_F(LoggerTest, LogWarningMessage) {
    Logger::get_instance().configure(logFilePath, LogLevel::WARNING);
    Logger::get_instance().warn("Warning message test.");
    EXPECT_TRUE(getLastLogLine(logFilePath).find("WARNING") > 0);
}

TEST_F(LoggerTest, LogErrorMessage) {
    Logger::get_instance().configure(logFilePath, LogLevel::ERROR);
    Logger::get_instance().error("Error message test.");
    EXPECT_TRUE(getLastLogLine(logFilePath).find("ERROR") > 0);
}

TEST_F(LoggerTest, LogDebugMessage) {
    Logger::get_instance().configure(logFilePath, LogLevel::DEBUG);
    Logger::get_instance().debug("Debug message test.");
    EXPECT_TRUE(getLastLogLine(logFilePath).find("DEBUG") > 0);
}

// Test that logging below the configured level does not write to file
TEST_F(LoggerTest, DoesNotLogBelowLevel) {
    Logger::get_instance().configure(logFilePath, LogLevel::WARNING);
    Logger::get_instance().info("This should not be logged.");
    std::ifstream file(logFilePath);
    EXPECT_TRUE(file.peek() == std::ifstream::traits_type::eof());
}

TEST_F(LoggerTest, MultiThreadedLogging) {
    Logger::get_instance().configure(logFilePath, LogLevel::INFO);

    auto logTask = []() {
        for (int i = 0; i < 10; ++i) {
            Logger::get_instance().info("Thread-safe logging test.");
        }
    };

    std::thread thread1(logTask);
    std::thread thread2(logTask);

    thread1.join();
    thread2.join();

    // Check if log file has 20 lines as each thread logged 10 times
    int lineCount = 0;
    std::string line;
    std::ifstream file(logFilePath);
    while (std::getline(file, line)) {
        lineCount++;
    }
    EXPECT_EQ(lineCount, 20);
}

TEST_F(LoggerTest, SingletonInstance) {
    Logger& logger1 = Logger::get_instance();
    Logger& logger2 = Logger::get_instance();
    EXPECT_EQ(&logger1, &logger2);
}

TEST_F(LoggerTest, LogEmptyMessage) {
    Logger::get_instance().configure(logFilePath, LogLevel::INFO);
    Logger::get_instance().info("");
    std::string lastLine = getLastLogLine(logFilePath);
    EXPECT_TRUE(lastLine.find("INFO") != std::string::npos);
}

TEST_F(LoggerTest, LogSpecialCharacters) {
    Logger::get_instance().configure(logFilePath, LogLevel::INFO);
    std::string specialMessage = "Special chars !@#$%^&*() test.";
    Logger::get_instance().info(specialMessage);
    EXPECT_TRUE(getLastLogLine(logFilePath).find(specialMessage) != std::string::npos);
}
}
