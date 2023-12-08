#include "../atpdxy/log.h"
#include "../atpdxy/util.h"
#include <iostream>

int main(){
    atpdxy::Logger::ptr logger(new atpdxy::Logger);
    logger->addAppender(atpdxy::LogAppender::ptr(new atpdxy::StdoutLogAppender));
    atpdxy::FileLogAppender::ptr file_appender(new atpdxy::FileLogAppender("./log.txt"));
    atpdxy::LogFormatter::ptr fmt(new atpdxy::LogFormatter("%d%T%m%n"));
    file_appender->setFormatter(fmt);
    file_appender->setLevel(atpdxy::LogLevel::ERROR);
    logger->addAppender(file_appender);
    // atpdxy::LogEvent::ptr event(new atpdxy::LogEvent(logger, atpdxy::LogLevel::DEBUG, __FILE__, __LINE__, 0, atpdxy::GetThreadId(), atpdxy::GetFiberId(), time(0)));
    // event->getSS() << "Hello World!";
    // logger->log(atpdxy::LogLevel::DEBUG, event);
    // std::cout << "Hello Log!" << std::endl;
    // ATPDXY_LOG_DEBUG(logger) << "test debug";
    ATPDXY_LOG_INFO(logger) << "test info";
    // ATPDXY_LOG_WARN(logger) << "test warn";
    ATPDXY_LOG_ERROR(logger) << "test error";
    // ATPDXY_LOG_FATAL(logger) << "test fatal";

    ATPDXY_LOG_FMT_ERROR(logger, "test fmt debug %s", "pzx");

    auto l = atpdxy::LoggerMgr::GetInstance()->getLogger("xx");
    ATPDXY_LOG_INFO(l) << "xxx";

    return 0;
}