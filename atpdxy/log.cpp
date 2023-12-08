#include "log.h"

namespace atpdxy{

class MessageFormatItem : public LogFormatter::FormatItem{
public:
    // 构造函数
    MessageFormatItem(const std::string& str = "") {}
    // 打印信息
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override{
        os << event->getContent();
    }
};

class LevelFormatItem : public LogFormatter::FormatItem{
public:
    // 构造函数
    LevelFormatItem(const std::string& str = "") {}
    // 打印日志级别
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override{
        os << LogLevel::ToString(level);
    }
};

class ElapseFormatItem : public LogFormatter::FormatItem{
public:
    // 构造函数
    ElapseFormatItem(const std::string& str = "") {}
    // 打印运行时间函数
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override{
        os << event->getElapse();
    }
};

class NameFormatItem : public LogFormatter::FormatItem{
public:
    // 构造函数
    NameFormatItem(const std::string& str = "") {}
    // 打印日志器名称
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override{
        os << logger->getName();
    }
};

class ThreadIdFormatItem : public LogFormatter::FormatItem{
public:
    // 构造函数
    ThreadIdFormatItem(const std::string& str = "") {}
    // 打印线程id
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override{
        os << event->getThreadId();
    }
};

class FiberIdFormatItem : public LogFormatter::FormatItem{
public:
    // 构造函数
    FiberIdFormatItem(const std::string& str = "") {}
    // 打印协程id
    
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override{
        os << event->getFiberId();
    }
};

class ThreadNameFormatItem : public LogFormatter::FormatItem{
public:
    // 构造函数
    ThreadNameFormatItem(const std::string& str = "") {}
    // 打印协程id
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override{
        os << event->getThreadName();
    }
};

class DateTimeFormatItem : public LogFormatter::FormatItem{
public:
    // 构造函数，接受一个日期格式，提供了默认日期格式
    DateTimeFormatItem(const std::string& format = "%Y-%m-%d %H:%M:%S")
        :m_format(format){
        if(m_format.empty()){
            m_format = "%Y-%m-%d %H:%M:%S";
        }
    }
    // 打印日期
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override{
        struct tm tm;
        time_t time = event->getTime();
        localtime_r(&time, &tm);
        char buf[64];
        strftime(buf, sizeof(buf), m_format.c_str(), &tm);
        os << buf;
    }
private:
    // 日期格式
    std::string m_format;
};

class FilenameFormatItem : public LogFormatter::FormatItem{
public:
    // 构造函数
    FilenameFormatItem(const std::string& str = "") {}
    // 打印文件名
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override{
        os << event->getFile();
    }
};

class LineFormatItem : public LogFormatter::FormatItem{
public:
    // 构造函数
    LineFormatItem(const std::string& str = "") {}
    // 打印行号
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override{
        os << event->getLine();
    }
};

class NewLineFormatItem : public LogFormatter::FormatItem{
public:
    // 构造函数
    NewLineFormatItem(const std::string& str = "") {}
    // 换行
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override{
        os << std::endl;
    }
};

class TabFormatItem : public LogFormatter::FormatItem{
public:
    // 构造函数
    TabFormatItem(const std::string& str = "")
        :m_string(str){

    }
    // 制表符
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override{
        os << "\t";
    }
private:
    std::string m_string;
};

class StringFormatItem : public LogFormatter::FormatItem{
public:
    // 构造函数
    StringFormatItem(const std::string& str = "")
        :m_string(str){

    }
    // 换行
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override{
        os << m_string;
    }
private:
    std::string m_string;
};

LogLevel::Level LogLevel::FromString(std::string str){
#define XX(level, v) \
    if(str == #v) { \
        return LogLevel::level; \
    }
    XX(DEBUG, debug);
    XX(INFO, debug);
    XX(WARN, debug);
    XX(ERROR, debug);
    XX(FATAL, debug);
 
    XX(DEBUG, DEBUG);
    XX(INFO, INFO);
    XX(WARN, WARN);
    XX(ERROR, ERROR);
    XX(FATAL, FATAL);
    return LogLevel::UNKNOW;
#undef XX
}

const char* LogLevel::ToString(LogLevel::Level level){
    switch(level){
#define XX(name) \
    case LogLevel::name: \
        return #name; \
        break;
    XX(DEBUG);
    XX(INFO);
    XX(WARN);
    XX(ERROR);
    XX(FATAL);
#undef XX
    default:
        return "UNKNOW";
    }
    return  "UNKNOW";
}

LogEvent::LogEvent(const char* file, int32_t line, uint32_t elapse, 
        uint32_t thread_id, uint32_t fiber_id, uint64_t time)
    :m_file(file), m_line(line), m_elapse(elapse), m_threadId(thread_id)
    ,m_fiberId(fiber_id), m_time(time){
}

Logger::Logger(const std::string& name)
    :m_name(name),m_level(LogLevel::UNKNOW){
    m_formatter.reset(new LogFormatter("%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"));
}

void Logger::addAppender(LogAppender::ptr appender){
    if(!appender->getFormatter()){
        appender->setFormatter(m_formatter);
    }
    m_appenders.push_back(appender);
}

void Logger::delAppender(LogAppender::ptr appender){
    for(auto it = m_appenders.begin(); it != m_appenders.end(); ++it){
        if(*it == appender){
            m_appenders.erase(it);
            break;
        }
    }
}

void Logger::log(LogLevel::Level level, LogEvent::ptr event){
    if(level >= m_level){
        auto self = shared_from_this();
        for(auto& i : m_appenders){
            i->log(self, level, event);
        }
    }
}

void Logger::debug(LogEvent::ptr event){
    log(LogLevel::DEBUG, event);
}

void Logger::info(LogEvent::ptr event){
    log(LogLevel::INFO, event);
}

void Logger::warn(LogEvent::ptr event){
    log(LogLevel::WARN, event);
}

void Logger::error(LogEvent::ptr event){
    log(LogLevel::ERROR, event);
}

void Logger::fatal(LogEvent::ptr event){
    log(LogLevel::FATAL, event);
}

FileLogAppender::FileLogAppender(const std::string& filename)
    :m_filename(filename){

}

void StdoutLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event){
    if(level >= m_level){
        std::cout << m_formatter->format(logger, level, event);
    }
}

void FileLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event){
    if(level >= m_level){
        m_filestream << m_formatter->format(logger, level, event);
    }
}

bool FileLogAppender::reopen(){
    if(m_filestream){
        m_filestream.close();
    }
    m_filestream.open(m_filename);
    // 将m_filestream转换成bool类型，非零值转换成1；0还是0
    return !!m_filestream;
}

LogFormatter::LogFormatter(const std::string& pattern)
    :m_pattern(pattern){
    init();
}

std::string LogFormatter::format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event){
    std::stringstream ss;
    for(auto& i : m_items){
        i->format(ss, logger, level, event);
    }
    return ss.str();
}

void LogFormatter::init(){
    // 格式项名称-格式项格式-是否有格式项格式
    std::vector<std::tuple<std::string, std::string, int> > vec;
    std::string nstr;
    for(size_t i = 0; i < m_pattern.size(); ++i) {
        if(m_pattern[i] != '%') {
            nstr.append(1, m_pattern[i]);
            continue;
        }

        if((i + 1) < m_pattern.size()) {
            if(m_pattern[i + 1] == '%') {
                nstr.append(1, '%');
                continue;
            }
        }

        size_t n = i + 1;
        int fmt_status = 0;
        size_t fmt_begin = 0;

        std::string str;
        std::string fmt;
        while(n < m_pattern.size()) {
            if(!fmt_status && (!isalpha(m_pattern[n]) && m_pattern[n] != '{'
                    && m_pattern[n] != '}')) {
                str = m_pattern.substr(i + 1, n - i - 1);
                break;
            }
            if(fmt_status == 0) {
                if(m_pattern[n] == '{') {
                    str = m_pattern.substr(i + 1, n - i - 1);
                    // std::cout << "*" << str << std::endl;
                    fmt_status = 1; //解析格式
                    fmt_begin = n;
                    ++n;
                    continue;
                }
            } else if(fmt_status == 1) {
                if(m_pattern[n] == '}') {
                    fmt = m_pattern.substr(fmt_begin + 1, n - fmt_begin - 1);
                    // std::cout << "#" << fmt << std::endl;
                    fmt_status = 0;
                    ++n;
                    break;
                }
            }
            ++n;
            if(n == m_pattern.size()) {
                if(str.empty()) {
                    str = m_pattern.substr(i + 1);
                }
            }
        }

        if(fmt_status == 0) {
            if(!nstr.empty()) {
                vec.push_back(std::make_tuple(nstr, std::string(), 0));
                nstr.clear();
            }
            vec.push_back(std::make_tuple(str, fmt, 1));
            i = n - 1;
        } else if(fmt_status == 1) {
            std::cout << "pattern parse error: " << m_pattern << " - " << m_pattern.substr(i) << std::endl;
            // m_error = true;
            vec.push_back(std::make_tuple("<<pattern_error>>", fmt, 0));
        }
    }

    if(!nstr.empty()) {
        vec.push_back(std::make_tuple(nstr, "", 0));
    }
    static std::map<std::string,std::function<FormatItem::ptr(const std::string& str)>>s_format_items={
#define XX(str,C) \
    {#str,[](const std::string& fmt){return FormatItem::ptr (new C(fmt));}}
    
        XX(m, MessageFormatItem),           //m:消息
        XX(p, LevelFormatItem),             //p:日志级别
        XX(r, ElapseFormatItem),            //r:累计毫秒数
        XX(c, NameFormatItem),              //c:日志名称
        XX(t, ThreadIdFormatItem),          //t:线程id
        XX(n, NewLineFormatItem),           //n:换行
        XX(d, DateTimeFormatItem),          //d:时间
        XX(f, FilenameFormatItem),          //f:文件名
        XX(l, LineFormatItem),              //l:行号
        XX(T, TabFormatItem),               //T:Tab
        XX(F, FiberIdFormatItem),           //F:协程id
        XX(N, ThreadNameFormatItem),        //N:线程名称
#undef XX
    };

    for(auto& i : vec) {
        if(std::get<2>(i) == 0) {
            m_items.push_back(FormatItem::ptr(new StringFormatItem(std::get<0>(i))));
        } else {
            auto it = s_format_items.find(std::get<0>(i));
            if(it == s_format_items.end()) {
                m_items.push_back(FormatItem::ptr(new StringFormatItem("<<error_format %" + std::get<0>(i) + ">>")));
                // m_error = true;
            } else {
                m_items.push_back(it->second(std::get<1>(i)));
            }
        }

        // std::cout << "(" << std::get<0>(i) << ") - (" << std::get<1>(i) << ") - (" << std::get<2>(i) << ")" << std::endl;
    }
    //std::cout << m_items.size() << std::endl;
}

}