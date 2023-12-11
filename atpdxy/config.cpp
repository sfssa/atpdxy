#include "config.h"

namespace atpdxy{

// 类中静态成员要在类外初始化
// Config::ConfigVarMap Config::s_datas;

// 将所有的node提取到list中
static void ListAllMember(const std::string& prefix, const YAML::Node& node,
    std::list<std::pair<std::string, const YAML::Node>>& output){
    if(prefix.find_first_not_of("abcdefghijklmnopqrstuvwxyz._012345678") != std::string::npos){
        ATPDXY_LOG_ERROR(ATPDXY_LOG_ROOT()) << "Config invalid name: " << prefix << " : " << node;
        return;
    }
    output.push_back(std::make_pair(prefix, node));
    if(node.IsMap()){
        for(auto it = node.begin(); it != node.end(); ++it){
            ListAllMember(prefix.empty() ? it->first.Scalar() : prefix + "." + it->first.Scalar(), it->second, output);
        }
    }
}

// 从yaml文件导入配置，已有的配置会被修改，但不会增加新的配置
void Config::LoadFromYaml(const YAML::Node& root){
    std::list<std::pair<std::string, const YAML::Node>> all_nodes;
    ListAllMember("", root, all_nodes);
    for(auto& i : all_nodes){
        std::string key = i.first;
        if(key.empty()){
            continue;
        }
        std::transform(key.begin(), key.end(), key.begin(), ::tolower);
        ConfigVarBase::ptr var = LookupBase(key);
        if(var){
            if(i.second.IsScalar()){
                var->fromString(i.second.Scalar());
            }else{
                std::stringstream ss;
                ss << i.second;
                var->fromString(ss.str());
            }
        }
    }
}

ConfigVarBase::ptr Config::LookupBase(const std::string& name){
    auto it = GetDatas().find(name);
    return it == GetDatas().end() ? nullptr : it->second;
}

void Config::ShowAllConfig(){
    for(auto& i : GetDatas()){
        ATPDXY_LOG_INFO(ATPDXY_LOG_ROOT()) << i.first << " ";
    }
}

}