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

	//������־��д����ͼ���
	void setLogLevel(LogLevel level);

	//��־�ӿ�
	void log(LogLevel level, const char* msg);

	//������־�ļ�·��
	void setLogFile(const std::string& path);

	//ˢ����־
	void flush();

	//ֹͣ��־ϵͳ
	void shutdown();

	static Log& getInstance();
	
	
	Log(const Log&) = delete;
	Log& operator=(const Log&) = delete;
	
private:
	Log();
	~Log();
	
	//��̨�̹߳�������
	void workerThread();

	//��ʽ����־��Ϣ
	std::string formatMessage(LogLevel level, const std::string& msg);

	std::atomic<bool> running_;				//���б�־
	std::atomic<LogLevel> logLevel_;		//��ǰ��־�ȼ�
	std::queue<std::string> logQueue_;		//��־����
	std::mutex queueMutex_;					//���л�����
	std::condition_variable queueCond_;		//������������
	std::thread workerThread_;				//��̨�߳�
	std::ofstream logFile_;					//��־�ļ���
	std::string logFilePath_ = "app.log";	//��־�ļ�·��

#define LOG_DEBUG(msg)		Log::getInstance().log(Log::DEBUG,msg)
#define LOG_INFO(msg)		Log::getInstance().log(Log::INFO,msg)
#define LOG_WARNING(msg)	Log::getInstance().log(Log::WARNING,msg)
#define LOG_ERROR(msg)		Log::getInstance().log(Log::ERROR,msg)
#define LOG_FATAL(msg)		Log::getInstance().log(Log::FATAL,msg)
};

