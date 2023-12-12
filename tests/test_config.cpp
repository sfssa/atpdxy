#include "../atpdxy/log.h"
#include "../atpdxy/config.h"
#include <yaml-cpp/yaml.h>

#if 1
atpdxy::ConfigVar<int>::ptr g_int_value_config = 
    atpdxy::Config::Lookup("system.port", (int)8080, "system port");

atpdxy::ConfigVar<float>::ptr g_int_valuex_config = 
    atpdxy::Config::Lookup("system.port", (float)8080, "system port");

atpdxy::ConfigVar<float>::ptr g_float_value_config = 
    atpdxy::Config::Lookup("system.value", (float)120.1, "system value");

atpdxy::ConfigVar<std::vector<int>>::ptr g_int_vec_value_config = 
    atpdxy::Config::Lookup("system.int_vec", std::vector<int>{1,2,3}, "system int vec");

atpdxy::ConfigVar<std::list<int>>::ptr g_int_list_value_config = 
    atpdxy::Config::Lookup("system.int_list", std::list<int>{1,2,3}, "system int list");

atpdxy::ConfigVar<std::set<int>>::ptr g_int_set_value_config = 
    atpdxy::Config::Lookup("system.int_set", std::set<int>{4, 5, 6}, "system int set");

atpdxy::ConfigVar<std::unordered_set<int>>::ptr g_int_uset_value_config = 
    atpdxy::Config::Lookup("system.int_uset", std::unordered_set<int>{7, 8, 9}, "system int uset");

atpdxy::ConfigVar<std::map<std::string, int>>::ptr g_int_map_value_config = 
    atpdxy::Config::Lookup("system.int_map", std::map<std::string, int>{{"k",2}, {"l",3}}, "system int map");

atpdxy::ConfigVar<std::unordered_map<std::string, int>>::ptr g_int_umap_value_config = 
    atpdxy::Config::Lookup("system.int_umap", std::unordered_map<std::string, int>{{"p",2}, {"z",3}}, "system int umap");

void print_yaml(const YAML::Node& node, int level){
    if(node.IsScalar()) {
        ATPDXY_LOG_INFO(ATPDXY_LOG_ROOT()) << std::string(level * 4, ' ')
            << node.Scalar() << " - " << node.Type() << " - " << level;
    } else if(node.IsNull()) {
        ATPDXY_LOG_INFO(ATPDXY_LOG_ROOT()) << std::string(level * 4, ' ')
            << "NULL - " << node.Type() << " - " << level;
    } else if(node.IsMap()) {
        for(auto it = node.begin();
                it != node.end(); ++it) {
            ATPDXY_LOG_INFO(ATPDXY_LOG_ROOT()) << std::string(level * 4, ' ')
                    << it->first << " - " << it->second.Type() << " - " << level;
            print_yaml(it->second, level + 1);
        }
    } else if(node.IsSequence()) {
        for(size_t i = 0; i < node.size(); ++i) {
            ATPDXY_LOG_INFO(ATPDXY_LOG_ROOT()) << std::string(level * 4, ' ')
                << i << " - " << node[i].Type() << " - " << level;
            print_yaml(node[i], level + 1);
        }
    }
}

// 路径:/home/pzx/atpdxy/bin/conf/test.yml
void test_yaml(){
    YAML::Node root = YAML::LoadFile("/home/pzx/atpdxy/bin/conf/log.yml");
    print_yaml(root, 0);
    // ATPDXY_LOG_INFO(ATPDXY_LOG_ROOT()) << root;
}

void test(){
    ATPDXY_LOG_INFO(ATPDXY_LOG_ROOT()) << g_int_value_config->getValue();
    ATPDXY_LOG_INFO(ATPDXY_LOG_ROOT()) << g_int_value_config->toString();

    ATPDXY_LOG_INFO(ATPDXY_LOG_ROOT()) << g_float_value_config->getValue();
    ATPDXY_LOG_INFO(ATPDXY_LOG_ROOT()) << g_float_value_config->toString();
}

void test_config(){
    ATPDXY_LOG_INFO(ATPDXY_LOG_ROOT()) << "before: " << g_int_value_config->getValue();
    ATPDXY_LOG_INFO(ATPDXY_LOG_ROOT()) << "before: " << g_float_value_config->toString();
#define XX(g_var, name, prefix) \
    { \
        auto& v = g_var->getValue(); \
        for(auto& i : v){ \
            ATPDXY_LOG_INFO(ATPDXY_LOG_ROOT()) << #prefix " " #name ": " << i; \
        } \
        ATPDXY_LOG_INFO(ATPDXY_LOG_ROOT()) << #prefix " " #name " yaml: " <<g_var->toString(); \
    }
    
#define XX_M(g_var, name, prefix) \
    { \
        auto& v = g_var->getValue(); \
        for(auto& i : v){ \
            ATPDXY_LOG_INFO(ATPDXY_LOG_ROOT()) << #prefix " " #name ": {" \
                << i.first << " - " << i.second << "}"; \
        } \
        ATPDXY_LOG_INFO(ATPDXY_LOG_ROOT()) << #prefix " " #name " yaml: " <<g_var->toString(); \
    }
    XX(g_int_vec_value_config, int_vec, before);
    XX(g_int_list_value_config, int_list, before);
    XX(g_int_set_value_config, int_set, before);
    XX(g_int_uset_value_config, int_uset, before);
    XX_M(g_int_map_value_config, int_map, before);
    XX_M(g_int_umap_value_config, int_umap, before);

    YAML::Node root = YAML::LoadFile("/home/pzx/atpdxy/bin/conf/log.yml");
    atpdxy::Config::LoadFromYaml(root);

    ATPDXY_LOG_INFO(ATPDXY_LOG_ROOT()) << "after: " << g_int_value_config->getValue();
    ATPDXY_LOG_INFO(ATPDXY_LOG_ROOT()) << "after: " << g_float_value_config->toString();

    XX(g_int_vec_value_config, int_vec, after);
    XX(g_int_list_value_config, int_list, after);
    XX(g_int_set_value_config, int_set, after);
    XX(g_int_uset_value_config, int_uset, after);
    XX_M(g_int_map_value_config, int_map, after);
    XX_M(g_int_umap_value_config, int_umap, after); 
}

#endif
// 自定义类型需要提供转换成string函数并重载==运算符
// 自定义类型需要实现偏特化LexicalCast版本，自定义版本可以和常规stl配套使用
class Person {
public:
    Person() {};
    std::string m_name;
    int m_age = 0;
    bool m_sex = 0;

    std::string toString() const {
        std::stringstream ss;
        ss << "[Person name=" << m_name
           << " age=" << m_age
           << " sex=" << m_sex
           << "]";
        return ss.str();
    }

    bool operator==(const Person& oth) const {
        return m_name == oth.m_name
            && m_age == oth.m_age
            && m_sex == oth.m_sex;
    }
};

namespace atpdxy{

template<>
class LexicalCast<std::string, Person> {
public:
    Person operator()(const std::string& v) {
        YAML::Node node = YAML::Load(v);
        Person p;
        p.m_name = node["name"].as<std::string>();
        p.m_age = node["age"].as<int>();
        p.m_sex = node["sex"].as<bool>();
        return p;
    }
};

template<>
class LexicalCast<Person, std::string> {
public:
    std::string operator()(const Person& p) {
        YAML::Node node;
        node["name"] = p.m_name;
        node["age"] = p.m_age;
        node["sex"] = p.m_sex;
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

}

atpdxy::ConfigVar<Person>::ptr g_person = 
    atpdxy::Config::Lookup("class.person", Person(), "person");

atpdxy::ConfigVar<std::map<std::string, Person>>::ptr g_person_map = 
    atpdxy::Config::Lookup("class.map", std::map<std::string, Person>(), "class_person");

atpdxy::ConfigVar<std::map<std::string, std::vector<Person>>>::ptr g_person_vec_map = 
    atpdxy::Config::Lookup("class.vec_map", std::map<std::string, std::vector<Person>>(), "class_person");

void test_class(){
    // ATPDXY_LOG_INFO(ATPDXY_LOG_ROOT()) << "before: " << g_person->getValue().toString() << " - " << g_person->toString();
#define XX_PM(g_var, prefix) \
    { \
        auto m = g_person_map->getValue(); \
        for(auto& i : m){ \
            ATPDXY_LOG_INFO(ATPDXY_LOG_ROOT()) << #prefix << " : " << i.first << " - " <<i.second.toString(); \
        } \
        ATPDXY_LOG_INFO(ATPDXY_LOG_ROOT()) << #prefix << " : size=" << m.size(); \
    }

    // 测试变更配置
    g_person->addListener([](const Person& old_value, const Person& new_value){
        ATPDXY_LOG_INFO(ATPDXY_LOG_ROOT()) << "old_value:" << old_value.toString()
            << " new_value: " << new_value.toString();
    });

    XX_PM(g_person_map, "class.map before");
    ATPDXY_LOG_INFO(ATPDXY_LOG_ROOT()) << "before: " << g_person_vec_map->toString();
    YAML::Node root = YAML::LoadFile("/home/pzx/atpdxy/bin/conf/log.yml");
    atpdxy::Config::LoadFromYaml(root);

    // ATPDXY_LOG_INFO(ATPDXY_LOG_ROOT()) << "after: " << g_person->getValue().toString() << " - " << g_person->toString();
    XX_PM(g_person_map, "class.map after");
    ATPDXY_LOG_INFO(ATPDXY_LOG_ROOT()) << "after: " << g_person_vec_map->toString();
}

void test_log(){
    static atpdxy::Logger::ptr  system_log = ATPDXY_LOG_NAME("system");
    ATPDXY_LOG_INFO(system_log) << "hello system log" << std::endl;
    std::cout << atpdxy::LoggerMgr::GetInstance()->toYamlString() << std::endl;
    YAML::Node root = YAML::LoadFile("/home/pzx/atpdxy/bin/conf/log.yml");
    atpdxy::Config::LoadFromYaml(root);
    std::cout << "============================================================" << std::endl;
    std::cout << atpdxy::LoggerMgr::GetInstance()->toYamlString() << std::endl;
    std::cout << "============================================================" << std::endl;
    std::cout << root << std::endl;
    ATPDXY_LOG_INFO(system_log) << "hello system log" << std::endl;

    system_log->setFormatter("%d - %m%n");
    ATPDXY_LOG_INFO(system_log) << "hello system log" << std::endl;
}

int main(){
    // std::cout << "Hello World!" << std::endl;
    // test_yaml();
    // test_config();
    // atpdxy::Config::ShowAllConfig();
    // test_class();
    test_log();
    atpdxy::Config::Visit([](atpdxy::ConfigVarBase::ptr var){
        ATPDXY_LOG_INFO(ATPDXY_LOG_ROOT()) << "name=" << var->getName()
            << " description=" << var->getDescription()
            << " type.name=" << var->getTypeName()
            << " value=" << var->toString()
            << std::endl;
    });
    return 0;
}
