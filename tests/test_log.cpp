#include "../atpdxy/log.h"
#include <iostream>

int main(){
    atpdxy::Logger::ptr logger(new atpdxy::Logger);
    logger->addAppender(atpdxy::LogAppender::ptr(new atpdxy::StdoutLogAppender));
    atpdxy::LogEvent::ptr event(new atpdxy::LogEvent(__FILE__, __LINE__, 0, 1, 2, time(0)));
    event->getSS() << "Hello World!";
    logger->log(atpdxy::LogLevel::DEBUG, event);
    std::cout << "Hello Log!" << std::endl;
    return 0;
}