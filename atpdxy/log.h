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

namespace atpdxy{

class Logger;
// 日志事件
class LogEvent{
public:
    // 日志事件智能指针
    typedef std::shared_ptr<LogEvent> ptr;
    // 构造函数
    LogEvent(const char* file, int32_t line, uint32_t elapse, 
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
};

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
private:    
    // 格式模板
    std::string m_pattern;
    // 存放要输出的格式项
    std::vector<FormatItem::ptr> m_items;
};

// 日志输出器
class LogAppender{
public:
    // 日志输出器智能指针
    typedef std::shared_ptr<LogAppender> ptr;
    // 虚析构函数
    virtual ~LogAppender() {}
    // 打印日志函数接口
    virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
    // 设置输出器格式
    void setFormatter(LogFormatter::ptr val) { m_formatter = val;}
    // 获得输出器格式
    LogFormatter::ptr getFormatter() const { return m_formatter;}
protected:
    // 日志级别，要初始化，否则可能导致日志级别不够无法输出日志内容
    LogLevel::Level m_level = LogLevel::DEBUG;
    // 输出器的格式
    LogFormatter::ptr m_formatter;
};

// 日志器
class Logger : public std::enable_shared_from_this<Logger>{
public:
    // 智能指针
    typedef std::shared_ptr<Logger> ptr;
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
    // 获得日志器的级别
    LogLevel::Level getLevel() const { return m_level;}
    // 设置日志器的级别
    void setLevel(LogLevel::Level val) { m_level = val;}
    // 获得日志器名称
    const std::string& getName() const { return m_name;}
private:
    // 日志名称
    std::string m_name;
    // 日志级别
    LogLevel::Level m_level;
    // 输出器的集合，可以输出到控制台和多个文件中
    std::list<LogAppender::ptr> m_appenders;
    // 日志输出格式
    LogFormatter::ptr m_formatter;
};

// 输出到控制台的appender
class StdoutLogAppender : public LogAppender{
public:
    // 输出到控制台appender的智能指针
    typedef std::shared_ptr<StdoutLogAppender> ptr;
    // 打印日志函数
    void log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override;
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
private:
    // 文件名
    std::string m_filename;
    // 文件流
    std::ofstream m_filestream;
};

}


