#include"Log.h"
#include<functional>
using namespace std;

int main()
{
	Log& logger = Log::getInstance();
	logger.setLogFile("log.log");
	logger.setLogLevel(Log::DEBUG);

	logger.log(Log::INFO, "LOG INFO");
	logger.log(Log::ERROR, "LOG ERROR");
	logger.log(Log::WARNING, "LOG WARNING");
	
	thread t1([] {
		LOG_FATAL("thread t1 FATAL");
		});
	thread t2([] {
		LOG_ERROR("thread t2 ERROR");
		LOG_ERROR("thread t2 ERROR");
		});
	thread t3([] {
		LOG_INFO("thread t3 INFO");
		});
	t1.join();
	t2.join();
	t3.join();
	return 0;
}