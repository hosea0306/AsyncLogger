#pragma once
#include<mutex>
#include<thread>
#include<iostream>
#include<string>
#include<fstream>
#include<atomic>
#include<condition_variable>
#include<queue>
#include<sstream>
class Log
{
public:
	enum LogLevel
	{
		DEBUG,
		INFO,
		WARNING,
		ERROR,
		FATAL
	};

	//设置日志可写入最低级别
	void setLogLevel(LogLevel level);

	//日志接口
	void log(LogLevel level, const char* msg);

	//设置日志文件路径
	void setLogFile(const std::string& path);

	//刷新日志
	void flush();

	//停止日志系统
	void shutdown();

	static Log& getInstance();
	
	
	Log(const Log&) = delete;
	Log& operator=(const Log&) = delete;
	
private:
	Log();
	~Log();
	
	//后台线程工作函数
	void workerThread();

	//格式化日志消息
	std::string formatMessage(LogLevel level, const std::string& msg);

	std::atomic<bool> running_;				//运行标志
	std::atomic<LogLevel> logLevel_;		//当前日志等级
	std::queue<std::string> logQueue_;		//日志队列
	std::mutex queueMutex_;					//队列互斥锁
	std::condition_variable queueCond_;		//队列条件变量
	std::thread workerThread_;				//后台线程
	std::ofstream logFile_;					//日志文件流
	std::string logFilePath_ = "app.log";	//日志文件路径

#define LOG_DEBUG(msg)		Log::getInstance().log(Log::DEBUG,msg)
#define LOG_INFO(msg)		Log::getInstance().log(Log::INFO,msg)
#define LOG_WARNING(msg)	Log::getInstance().log(Log::WARNING,msg)
#define LOG_ERROR(msg)		Log::getInstance().log(Log::ERROR,msg)
#define LOG_FATAL(msg)		Log::getInstance().log(Log::FATAL,msg)
};

