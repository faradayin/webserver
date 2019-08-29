//
// Created by Feng on 2019/8/15.
//

//noncopyable: 实现不可拷贝的类提供了简单清晰的解决方案
//希望禁止复制类的实例。这是一个很经典的C++惯用语法，只要私有化拷贝构造函数和拷贝赋值操作函数即可。

#ifndef WEBSERVER_NONCOPYABLE_H
#define WEBSERVER_NONCOPYABLE_H

class noncopyable
{
protected:
    noncopyable(){}
    ~noncopyable(){}

private:
    noncopyable(const noncopyable&);//拷贝构造函数
    const noncopyable& operator=(const noncopyable&);//拷贝赋值操作符
};


#endif //WEBSERVER_NONCOPYABLE_H
