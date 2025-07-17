#include "Log.h"

void Log::setLogLevel(LogLevel level)
{
	this->logLevel_ = level;
}

void Log::log(LogLevel level, const char* msg)
{
	if (level < this->logLevel_)return;
	
	std::string formated = formatMessage(level, msg);	//��ʽ����־

	{
		std::lock_guard<std::mutex> lock(this->queueMutex_);
		logQueue_.push(std::move(formated));
	}
	queueCond_.notify_one();
}

void Log::setLogFile(const std::string& path)
{
	logFilePath_ = path;
}

void Log::shutdown()
{
	if (running_)
	{
		running_ = false;
		queueCond_.notify_one();
		if (workerThread_.joinable())
		{
			workerThread_.join();
		}
		if (logFile_.is_open())
		{
			logFile_.close();
		}
	}
}

Log& Log::getInstance()
{
    // TODO: �ڴ˴����� return ���	
	static Log instance;
	return instance;
}

Log::Log():running_(true), logLevel_(INFO)
{
	workerThread_ = std::thread(&Log::workerThread, this);
}

Log::~Log()
{
	shutdown();
}

void Log::workerThread()
{
	//������־������Ԥ���ռ���Ϊ������
	std::vector<std::string> batch;
	batch.reserve(100);

	while (this->running_)
	{
		std::unique_lock<std::mutex> lock(this->queueMutex_);
		queueCond_.wait_for(lock, std::chrono::seconds(1),
			[this]() {
				return !logQueue_.empty() || !running_;
			});

		//��������ת����������
		while (!logQueue_.empty())
		{
			batch.push_back(std::move(logQueue_.front()));
			logQueue_.pop();
		}
	}

	//д��־
	if (!batch.empty())
	{
		if (!logFile_.is_open())
		{
			logFile_.open(logFilePath_, std::ios::app);
		}
		for (const auto& msg : batch)
		{
			logFile_ << msg << std::endl;
		}

		batch.clear();
		logFile_.flush();
	}
}

std::string Log::formatMessage(LogLevel level, const std::string& msg)
{
	//��ȡ��ǰʱ��
	auto now = std::chrono::system_clock::now();
	auto time = std::chrono::system_clock::to_time_t(now);
	char timestr[20];
	std::strftime(timestr, sizeof(timestr), "%Y-%m-%d %H:%M:%S", std::localtime(&time));

	//��ȡ�߳�ID
	std::ostringstream threadId;
	threadId << std::this_thread::get_id();

	//��ȡ��־�ȼ�
	 const char* levelStr = "";
	 switch (level)
	 {
	 case Log::DEBUG:
		 levelStr = "DEBUG";
		 break;
	 case Log::INFO:
		 levelStr = "INFO";
		 break;
	 case Log::WARNING:
		 levelStr = "WARNING";
		 break;
	 case Log::ERROR:
		 levelStr = "ERROR";
		 break;
	 case Log::FATAL:
		 levelStr = "FATAL";
		 break;
	 default:
		 break;
	 }

	 std::ostringstream formatted;
	 formatted << "[" << timestr << "]"
		 << "[" << levelStr << "]"
		 << "[" << threadId.str() << "]"
		 << msg;

	return formatted.str();
}
