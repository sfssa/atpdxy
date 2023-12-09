#pragma once 
#include <memory>
#include <string>
#include <sstream>
#include <boost/lexical_cast.hpp>
#include <yaml-cpp/yaml.h>
#include <list>
#include <vector>
#include <map>
#include <set>
#include <unordered_map>
#include <functional>
#include <unordered_set>
#include "log.h"

namespace atpdxy{

class ConfigVarBase{
public:
    // 智能指针
    typedef std::shared_ptr<ConfigVarBase> ptr;
    // 构造函数
    ConfigVarBase(const std::string& name, const std::string& description)
        :m_name(name), m_description(description){
        std::transform(m_name.begin(), m_name.end(), m_name.begin(), ::tolower);
    }
    // 虚拟析构函数
    ~ConfigVarBase() {}
    // 获得配置名称
    const std::string& getName() const { return m_name;}
    // 获得配置描述
    const std::string& getDescription() const { return m_description;}
    // 将配置转换成string接口
    virtual std::string toString() = 0;
    // 解析字符串来初始化配置接口
    virtual bool fromString(const std::string& val) = 0;
    // 获得配置值的类型的函数接口
    virtual std::string getTypeName() const = 0;
protected:
    // 配置名称
    std::string m_name;
    // 配置的描述
    std::string m_description;
};

// 从F转向T
template <class F, class T>
class LexicalCast{
public:
    T operator () (const F& v){
        return boost::lexical_cast<T>(v);
    }
};

// 从string转换成vector<T>
template <class T>
class LexicalCast<std::string, std::vector<T>>{
public:
    std::vector<T> operator()(const std::string& v){
        YAML::Node node = YAML::Load(v);
        typename std::vector<T> vec;
        std::stringstream ss;
        for(size_t i = 0;i < node.size(); ++i){
            ss.str("");
            ss << node[i];
            vec.push_back(LexicalCast<std::string, T>()(ss.str()));
        }
        return vec;
    }
};

// 从vector<T>转换成string
template <class T>
class LexicalCast<std::vector<T>, std::string>{
public:
    std::string operator()(const std::vector<T>& v){
        YAML::Node node;
        for(auto& i : v){
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

// 从string转换成list<T>
template <class T>
class LexicalCast<std::string, std::list<T>>{
public:
    std::list<T> operator()(const std::string& v){
        YAML::Node node = YAML::Load(v);
        typename std::list<T> vec;
        std::stringstream ss;
        for(size_t i = 0;i < node.size(); ++i){
            ss.str("");
            ss << node[i];
            vec.push_back(LexicalCast<std::string, T>()(ss.str()));
        }
        return vec;
    }
};

// 从list<T>转换成string
template <class T>
class LexicalCast<std::list<T>, std::string>{
public:
    std::string operator()(const std::list<T>& v){
        YAML::Node node;
        for(auto& i : v){
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

// 从string转换成set<T>
template <class T>
class LexicalCast<std::string, std::set<T>>{
public:
    std::set<T> operator()(const std::string& v){
        YAML::Node node = YAML::Load(v);
        typename std::set<T> vec;
        std::stringstream ss;
        for(size_t i = 0;i < node.size(); ++i){
            ss.str("");
            ss << node[i];
            vec.insert(LexicalCast<std::string, T>()(ss.str()));
        }
        return vec;
    }
};

// 从set<T>转换成string
template <class T>
class LexicalCast<std::set<T>, std::string>{
public:
    std::string operator()(const std::set<T>& v){
        YAML::Node node;
        for(auto& i : v){
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

// 从string转换成unordered_set<T>
template <class T>
class LexicalCast<std::string, std::unordered_set<T>>{
public:
    std::unordered_set<T> operator()(const std::string& v){
        YAML::Node node = YAML::Load(v);
        typename std::unordered_set<T> vec;
        std::stringstream ss;
        for(size_t i = 0;i < node.size(); ++i){
            ss.str("");
            ss << node[i];
            vec.insert(LexicalCast<std::string, T>()(ss.str()));
        }
        return vec;
    }
};

// 从unordered_set<T>转换成string
template <class T>
class LexicalCast<std::unordered_set<T>, std::string>{
public:
    std::string operator()(const std::unordered_set<T>& v){
        YAML::Node node;
        for(auto& i : v){
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

// 从string转换成map<T>
template <class T>
class LexicalCast<std::string, std::map<std::string, T>>{
public:
    std::map<std::string, T> operator()(const std::string& v){
        YAML::Node node = YAML::Load(v);
        typename std::map<std::string, T> vec;
        std::stringstream ss;
        for(auto it = node.begin(); it != node.end(); ++it){
            ss.str("");
            ss << it->second;
            vec.insert(std::make_pair(it->first.Scalar(),LexicalCast<std::string, T>()(ss.str())));
        }
        return vec;
    }
};

// 从map<T>转换成string
template <class T>
class LexicalCast<std::map<std::string, T>, std::string>{
public:
    std::string operator()(const std::map<std::string, T>& v){
        YAML::Node node;
        for(auto& i : v){
            node[i.first]=YAML::Load(LexicalCast<T, std::string>()(i.second));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

// 从string转换成unordered_map<T>
template <class T>
class LexicalCast<std::string, std::unordered_map<std::string, T>>{
public:
    std::unordered_map<std::string, T> operator()(const std::string& v){
        YAML::Node node = YAML::Load(v);
        typename std::unordered_map<std::string, T> vec;
        std::stringstream ss;
        for(auto it = node.begin(); it != node.end(); ++it){
            ss.str("");
            ss << it->second;
            vec.insert(std::make_pair(it->first.Scalar(),LexicalCast<std::string, T>()(ss.str())));
        }
        return vec;
    }
};

// 从unordered_map<T>转换成string
template <class T>
class LexicalCast<std::unordered_map<std::string, T>, std::string>{
public:
    std::string operator()(const std::unordered_map<std::string, T>& v){
        YAML::Node node;
        for(auto& i : v){
            node[i.first]=YAML::Load(LexicalCast<T, std::string>()(i.second));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

// 支持序列化，转成string和从string转成需要的类型
template <class T, class FromStr = LexicalCast<std::string, T>, class ToStr = LexicalCast<T,std::string>>
class ConfigVar : public ConfigVarBase{
public:
    // 智能指针
    typedef std::shared_ptr<ConfigVar> ptr;
    // 函数指针
    typedef std::function<void(const T& old_value, const T& new_value)> on_change_cb;
    // 构造函数
    ConfigVar(const std::string& name, const T& default_value, const std::string& description = "")
        :ConfigVarBase(name, description),m_val(default_value){
        
    }
    // 将配置转换成string
    std::string toString() override{
        try{
            // return boost::lexical_cast<std::string>(m_val);
            return ToStr()(m_val);
        }catch(std::exception& e){
            ATPDXY_LOG_ERROR(ATPDXY_LOG_ROOT()) << "ConfigVar::toString exception"
                << e.what() << " convert: " << typeid(m_val).name() << " to string";
        }
        return "";
    }
    // 解析字符串来初始化配置
    bool fromString(const std::string& val) override{
        try{
            // m_val = boost::lexical_cast<T>(val);
            setValue(FromStr()(val));
        }catch(std::exception& e){
            ATPDXY_LOG_ERROR(ATPDXY_LOG_ROOT()) << "ConfigVar::fromString exception"
                << e.what() << " convert: string to " << typeid(m_val).name()
                << " - " << val;
        }
        return "";
    }
    // 返回配置的值
    const T getValue() const { return m_val;}
    // 设置配置的值
    void setValue(const T& v){ 
        if(v == m_val){
            return;
        }
        for(auto& i : m_cbs){
            i.second(m_val, v);
            m_val = v;
        }
    }
    // 返回值的类型
    std::string getTypeName() const override { return typeid(T).name();}
    // 增加监听器
    void addListener(uint64_t key, on_change_cb cb){
        m_cbs[key] = cb;
    }
    // 删除监听器
    void delListener(uint64_t key){
        if(m_cbs.find(key) != m_cbs.end()){
            m_cbs.erase(key);
        }
    }
    // 获得监听器
    on_change_cb getListener(uint64_t key){
        auto it = m_cbs.find(key);
        return it == m_cbs.end() ? nullptr : it->second;
    }
    // 清空监听器
    void clearListener(){
        m_cbs.clear();
    }
private:
    // 配置的值
    T m_val;
    // 变更回调函数，uint64_t要求唯一，避免覆盖
    std::map<uint64_t, on_change_cb> m_cbs;
};

class Config{
public:
    // 配置映射表
    typedef std::map<std::string,ConfigVarBase::ptr> ConfigVarMap;
    // 查找配置，有则返回；无则创建
    template <class T>
    static typename ConfigVar<T>::ptr Lookup(const std::string& name, const T& default_value,
        const std::string& description = ""){
        auto it = s_datas.find(name);
        if(it != s_datas.end()){
            auto tmp = std::dynamic_pointer_cast<ConfigVar<T>>(it->second);
            if(tmp){
                ATPDXY_LOG_INFO(ATPDXY_LOG_ROOT()) << "Lookup name=" << name << " exists";
                return tmp;
            }else{
                ATPDXY_LOG_ERROR(ATPDXY_LOG_ROOT()) << "Lookup name=" << name << " exists but type not "
                    << typeid(T).name() << " real_type=" << it->second->getTypeName()
                    << " " <<it->second->toString();
                return nullptr;
            }
        }
        // 配置格式不正确
        if(name.find_first_not_of("abcdefghijklmnopqrstuvwxyz._012345678") != std::string::npos){
            ATPDXY_LOG_ERROR(ATPDXY_LOG_ROOT()) << "Lookup name invalid " << name;
            throw std::invalid_argument(name);
        }
        // 创建配置
        typename ConfigVar<T>::ptr v(new ConfigVar<T>(name, default_value, description));
        s_datas[name] = v;
        return v;
    }
    // 查找配置(确定有这个配置)
    template <class T>
    static typename ConfigVar<T>::ptr Lookup(const std::string& name){
        auto it = s_datas.find(name);
        if(it == s_datas.end()){
            return nullptr;
        }
        return std::dynamic_pointer_cast<ConfigVar<T>>(it->second);
    }

    // 从yaml文件导入设置
    static void LoadFromYaml(const YAML::Node& root);
    // 查找有没有当前命名的项
    static ConfigVarBase::ptr LookupBase(const std::string& name);
    // 显示所有配置
    static void ShowAllConfig();
private:    
    // 静态映射表
    static ConfigVarMap s_datas;
};
}
