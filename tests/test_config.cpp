#include "../atpdxy/log.h"
#include "../atpdxy/config.h"
#include <yaml-cpp/yaml.h>

atpdxy::ConfigVar<int>::ptr g_int_value_config = 
    atpdxy::Config::Lookup("system.port", (int)8080, "system port");

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

// 路径:/home/pzx/atpdxy/bin/conf/log.yml
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

int main(){
    // std::cout << "Hello World!" << std::endl;
    // test_yaml();
    test_config();
    // atpdxy::Config::ShowAllConfig();
    return 0;
}
