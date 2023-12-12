#pragma once
#include <string>
#include <stdint.h>
#include <memory>
#include <list>
#include <sstream>
#include <fstream>
#include <iostream>
#include <functional>
#include <vector>
#include <map>
#include <stdarg.h>
#include "util.h"
#include "singleton.h"
#include "mutex.h"

// 当走出日志包装器的作用域后调用析构函数完成打印日志
#define ATPDXY_LOG_LEVEL(logger, level) \
    if(logger->getLevel() <= level) \
        atpdxy::LogEventWrap(atpdxy::LogEvent::ptr(new atpdxy::LogEvent(logger, level, \
                        __FILE__, __LINE__, 0, atpdxy::GetThreadId(),\
                atpdxy::GetFiberId(), time(0)))).getSS()

// 写debug级别日志
#define ATPDXY_LOG_DEBUG(logger) ATPDXY_LOG_LEVEL(logger, atpdxy::LogLevel::DEBUG)
// 写info级别日志
#define ATPDXY_LOG_INFO(logger) ATPDXY_LOG_LEVEL(logger, atpdxy::LogLevel::INFO)
// 写warn级别日志
#define ATPDXY_LOG_WARN(logger) ATPDXY_LOG_LEVEL(logger, atpdxy::LogLevel::WARN)
// 写error级别日志
#define ATPDXY_LOG_ERROR(logger) ATPDXY_LOG_LEVEL(logger, atpdxy::LogLevel::ERROR)
// 写fatal级别日志
#define ATPDXY_LOG_FATAL(logger) ATPDXY_LOG_LEVEL(logger, atpdxy::LogLevel::FATAL)

#define ATPDXY_LOG_FMT_LEVEL(logger, level, fmt, ...) \
    if(logger->getLevel() <= level) \
        atpdxy::LogEventWrap(atpdxy::LogEvent::ptr(new atpdxy::LogEvent(logger, level, \
        __FILE__, __LINE__, 0, atpdxy::GetThreadId(), \
        atpdxy::GetFiberId(), time(0)))).getEvent()->format(fmt, __VA_ARGS__)

// 格式化写入debug日志
#define ATPDXY_LOG_FMT_DEBUG(logger, fmt, ...) ATPDXY_LOG_FMT_LEVEL(logger, atpdxy::LogLevel::DEBUG, fmt, __VA_ARGS__)
// 格式化写入info日志
#define ATPDXY_LOG_FMT_INFO(logger, fmt, ...) ATPDXY_LOG_FMT_LEVEL(logger, atpdxy::LogLevel::INFO, fmt, __VA_ARGS__)
// 格式化写入warn日志
#define ATPDXY_LOG_FMT_WARN(logger, fmt, ...) ATPDXY_LOG_FMT_LEVEL(logger, atpdxy::LogLevel::WARN, fmt, __VA_ARGS__)
// 格式化写入error日志
#define ATPDXY_LOG_FMT_ERROR(logger, fmt, ...) ATPDXY_LOG_FMT_LEVEL(logger, atpdxy::LogLevel::ERROR, fmt, __VA_ARGS__)
// 格式化写入fatal日志
#define ATPDXY_LOG_FMT_FATAL(logger, fmt, ...) ATPDXY_LOG_FMT_LEVEL(logger, atpdxy::LogLevel::FATAL, fmt, __VA_ARGS__)
// 获得主日志器
#define ATPDXY_LOG_ROOT() atpdxy::LoggerMgr::GetInstance()->getRoot()
// 获得名字为name的日志器
#define ATPDXY_LOG_NAME(name) atpdxy::LoggerMgr::GetInstance()->getLogger(name)

namespace atpdxy{

class Logger;
class LoggerManager;
// 日志级别
class LogLevel{
public:
    enum Level{
        UNKNOW = 0,
        DEBUG = 1,
        INFO = 2,
        WARN = 3,
        ERROR = 4,
        FATAL = 5
    };

    // 将字符串类型转换成枚举类型
    static LogLevel::Level FromString(std::string str);
    // 将枚举类型转换成字符串
    static const char* ToString(LogLevel::Level level);
};

// 日志事件
class LogEvent{
public:
    // 日志事件智能指针
    typedef std::shared_ptr<LogEvent> ptr;
    // 构造函数
    LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level, const char* file, int32_t line, uint32_t elapse, 
        uint32_t thread_id, uint32_t fiber_id, uint64_t time);
    // 获得文件名
    const char* getFile() const { return m_file;}
    // 获得行号
    int32_t getLine() const { return m_line;}
    // 获得运行时间
    int32_t getElapse() const { return m_elapse;}
    // 获得线程id
    uint32_t getThreadId() const { return m_threadId;}
    // 获得协程id
    uint32_t getFiberId() const { return m_fiberId;}
    // 获得时间戳
    uint64_t getTime() const { return m_time;}
    // 获得日志事件内容
    std::string getContent() const { return m_ss.str();}
    // 获得流文件
    std::stringstream& getSS() { return m_ss;}
    // 获得线程名称
    const std::string& getThreadName() const { return m_threadName;}
    // 获得日志器
    std::shared_ptr<Logger> getLogger() const { return m_logger;}
    // 获得日志事件的级别
    LogLevel::Level getLevel() const { return m_level;}
    // 格式化日志内容
    void format(const char* fmt, ...);
    void format(const char* fmt, va_list al);
private:
    // 日志文件名
    const char* m_file = nullptr;   
    // 行号
    int32_t m_line = 0;  
    // 程序开始到现在的执行时间
    uint32_t m_elapse = 0;         
    // 线程id  
    uint32_t m_threadId = 0;
    // 协程id
    uint32_t m_fiberId = 0;
    // 时间戳
    uint64_t m_time = 0;
    // 日志内容
    std::stringstream m_ss;
    // 线程名称
    std::string m_threadName;
    // 日志事件所在日志器
    std::shared_ptr<Logger> m_logger;
    // 事件级别
    LogLevel::Level m_level;
};

// 日志包装器
class LogEventWrap{
public:
    // 构造函数，接受一个日志事件
    LogEventWrap(LogEvent::ptr event);
    // 析构函数，负责释放日志事件
    ~LogEventWrap();
    // 返回日志事件流
    std::stringstream& getSS();
    // 返回日志事件
    LogEvent::ptr getEvent() const { return m_event;}
private:
    LogEvent::ptr m_event;
};

// 日志格式器
class LogFormatter{
public:
    // 日志格式器智能指针
    typedef std::shared_ptr<LogFormatter> ptr;
    // 格式化输出
    std::string format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);
    // 构造函数，接受一个格式化模板
    LogFormatter(const std::string& pattern);
public:
    class FormatItem{
    public:
        // 格式项智能指针
        typedef std::shared_ptr<FormatItem> ptr;
        // 虚析构函数
        virtual ~FormatItem() {}
        // 打印日志内容
        virtual void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
    };
    // 解析pattern
    void init();
    // 解析时是否出错
    bool isError() const { return m_error;}
    // 返回格式模板
    const std::string getPattern() const { return m_pattern;}
private:    
    // 格式模板
    std::string m_pattern;
    // 存放要输出的格式项
    std::vector<FormatItem::ptr> m_items;
    // 格式是否有错误
    bool m_error = false;
};

// 日志输出器
class LogAppender{
friend class Logger;
public:
    // 日志输出器智能指针
    typedef std::shared_ptr<LogAppender> ptr;
    typedef Mutex MutexType;
    // 虚析构函数
    virtual ~LogAppender() {}
    // 打印日志函数接口
    virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
    // 设置输出器格式
    void setFormatter(LogFormatter::ptr val);
    // 获得输出器格式
    LogFormatter::ptr getFormatter();
    // 获得日志级别
    LogLevel::Level getLevel() const { return m_level;}
    // 设置日志级别
    void  setLevel(LogLevel::Level level) { m_level = level;}
    // 转换成yaml string接口
    virtual std::string toYamlString() = 0;
protected:
    // 日志级别，要初始化，否则可能导致日志级别不够无法输出日志内容
    LogLevel::Level m_level = LogLevel::DEBUG;
    // 输出器的格式
    LogFormatter::ptr m_formatter;
    // 是否有自己的formatter
    bool m_hasFormatter = false;
    // 互斥量，主要是输出控制台或者写到文件中，
    MutexType m_mutex;
};

// 日志器
class Logger : public std::enable_shared_from_this<Logger>{
friend class LoggerManager;
public:
    // 智能指针
    typedef std::shared_ptr<Logger> ptr;
    typedef Mutex MutexType;
    // 构造函数
    Logger(const std::string& name = "root");
    // 写日志
    void log(LogLevel::Level level, LogEvent::ptr event);
    // 写debug级别日志
    void debug(LogEvent::ptr event);
    // 写info级别日志
    void info(LogEvent::ptr event);
    // 写warn级别日志
    void warn(LogEvent::ptr event);
    // 写error级别日志
    void error(LogEvent::ptr event);
    // 写fatal级别日志
    void fatal(LogEvent::ptr event);
    // 添加输出器
    void addAppender(LogAppender::ptr appender);
    // 删除输出器
    void delAppender(LogAppender::ptr appender);
    // 清空输出器
    void clearAppenders();
    // 获得日志器的级别
    LogLevel::Level getLevel() const { return m_level;}
    // 设置日志器的级别
    void setLevel(LogLevel::Level val) { m_level = val;}
    // 获得日志器名称
    const std::string& getName() const { return m_name;}
    // 设置输出格式器
    void setFormatter(LogFormatter::ptr val);
    void setFormatter(const std::string& val);
    // 获得格式输出器
    LogFormatter::ptr getFormatter();
    // 转换成yaml string
    std::string toYamlString();
private:
    // 日志名称
    std::string m_name;
    // 日志级别
    LogLevel::Level m_level;
    // 输出器的集合，可以输出到控制台和多个文件中
    std::list<LogAppender::ptr> m_appenders;
    // 日志输出格式
    LogFormatter::ptr m_formatter;
    // 主日志器，用来初始化一些日志器
    Logger::ptr m_root;
    MutexType m_mutex;
};

// 输出到控制台的appender
class StdoutLogAppender : public LogAppender{
public:
    // 输出到控制台appender的智能指针
    typedef std::shared_ptr<StdoutLogAppender> ptr;
    // 打印日志函数
    void log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override;
    // 转换成yaml string
    std::string toYamlString() override;
private:

};

// 输出到文件的appender
class FileLogAppender : public LogAppender{
public:
    // 输出到文件appender的智能指针
    typedef std::shared_ptr<FileLogAppender> ptr;
    // 构造函数
    FileLogAppender(const std::string& filename);
    // 打印日志函数
    void log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override;
    // 重新打开文件
    bool reopen();
    // 转换成yaml string
    std::string toYamlString() override;
private:
    // 文件名
    std::string m_filename;
    // 文件流
    std::ofstream m_filestream;
};

class LoggerManager{
public:
    typedef Mutex MutexType;
    LoggerManager();
    // 获得日志器
    Logger::ptr getLogger(const std::string& name);
    // 初始化函数，从配置初始化日志管理器
    void init();
    // 获取主日志器
    Logger::ptr getRoot() const { return m_root;}
    std::string toYamlString();
private:
    // 存储日志器的容器：日志器名称和智能指针
    std::map<std::string, Logger::ptr> m_loggers;
    // 默认主日志器
    Logger::ptr m_root;
    MutexType m_mutex;
};

typedef atpdxy::Singleton<LoggerManager> LoggerMgr;

}


