#pragma once

namespace atpdxy{

template <class T, class X = void, int N = 0>
class Singleton{
public:
    static T* GetInstance(){
        static T instance;
        return &instance;
    }
};

template <class T, class X = void, int N = 0>
class SingletonPtr{
public:
    static std::shared_ptr<T> GetInstance(){
        static std::shared_ptr<T> instance(new T);
        return instance;
    }
};

}