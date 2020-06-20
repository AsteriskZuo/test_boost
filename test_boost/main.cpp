//
//  main.cpp
//  test_boost
//
//  Created by AsteriskZuo on 2020/6/13.
//  Copyright © 2020 AsteriskZuo. All rights reserved.
//

#include <iostream>
#include <string>
#include "test_boost_pool.hpp"

//static std::string& test_string(std::string* filename)
//{
//    return *(static_cast<std::string**>(filename));
//}

static void * & test_nextof(void * const ptr)
{
  return *(static_cast<void **>(ptr));
}
static void *  test_nextof2(void * ptr)
{
    return ptr;
}
static void * & test_nextof3(void* a)
{
    //空指针指向空对象
    //空指针的值是空对象的地址
    //指针a保存的是空对象的地址
    //指针a强转成a的地址，在取值，它和a不是同一个对象，只是类型相同。
    //返回的是临时对象？通过引用延迟生命周期？
    return *((void**)(a));//void指针的地址上的值（它是a吗？显然不是），void指针是局部变量，却被引用到了外面？
}
void * & test_nextof4(void* a)
{
    return *((void**)(a));//void指针的地址上的值（它是a吗？显然不是），void指针是局部变量，却被引用到了外面？
}
static void test_pp()
{
    int b1 = sizeof(void*);
    int b2 = sizeof(void**);
    int b3 = sizeof(std::string*);
    int b4 = sizeof(std::string**);
    void* chunk1 = (void*)std::malloc(sizeof(void*));
    void* chunk2 = (void*)std::malloc(sizeof(void*));
    void* chunk3 = nullptr;
    void* chunk6 = (void*)std::malloc(sizeof(void*));
    memcpy(chunk1, "hello", sizeof("hello"));
    memcpy(chunk2, "world", sizeof("world"));
    std::cout << (char*)chunk1 << std::endl;
    std::cout << (char*)chunk2 << std::endl;
//    {
//        test_nextof(chunk1) = chunk2;
//        chunk3 = test_nextof(chunk1);
//        void* & chunk4 = test_nextof(chunk1);
//        chunk4 = chunk2;
//        chunk3 = chunk2;
//    }
//    test_nextof(chunk1) = chunk2;
//    test_nextof2(chunk1) = chunk2;// error
//    *(static_cast<void **>(chunk1)) = chunk2;
//    chunk1 = chunk2;
//    test_nextof4(chunk1) = chunk2;
    void* & chunk5 = test_nextof(chunk1);
    chunk5 = chunk2;
    void* & chunk7 = test_nextof(chunk6);
    std::cout << (char*)chunk1 << std::endl;
    std::cout << (char*)test_nextof4(chunk1) << std::endl;
    std::cout << (char*)chunk2 << std::endl;
    int a = 0;
}
static void test_pp2()
{
    int b = sizeof(void*);
    void* chunk1 = nullptr;
    chunk1 = (void*)std::malloc(sizeof(void*));
    void* chunk2 = (void*)std::malloc(sizeof(void*));
    void* s = (void*)std::malloc(sizeof(void*));
    memcpy(chunk1, "hello", sizeof("hello"));
    memcpy(chunk2, "world", sizeof("world"));
//    chunk1 = nullptr;
    void * & chunk3 = test_nextof(chunk1);
    std::cout << (char*)chunk1 << std::endl;
    std::cout << (char*)chunk2 << std::endl;
//    std::cout << (char*)chunk3 << std::endl;
    chunk3 = chunk2;
//    memcpy(chunk2, "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890", sizeof("1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"));
    std::cout << (char*)chunk2 << std::endl;
    std::cout << (char*)chunk3 << std::endl;
    std::cout << (char*)(*static_cast<void**>(chunk1)) << std::endl;
    free(chunk1);
    free(chunk2);
    int a = 0;
}
static void test_pp3()
{
    int b = sizeof(void*);
    void* chunk1 = (void*)std::malloc(sizeof(void*));
    void* chunk2 = (void*)std::malloc(sizeof(void*));
    memcpy(chunk1, "hello", sizeof("hello"));
    memcpy(chunk2, "world", sizeof("world"));
    std::cout << (char*)chunk1 << std::endl;
    std::cout << (char*)chunk2 << std::endl;
    {
        // chunk1 改变了值，原内存泄漏
        void * chunk3 = chunk2;
        chunk1 = &chunk3;
        std::cout << (char*)chunk3 << std::endl;
    }
    {
        // chunk1 改变了值，原内存泄漏
        chunk1 = &chunk2;
        std::cout << (char*)(*static_cast<void**>(chunk1)) << std::endl;
    }
    {
        // chunk1的值是chunk3的地址，chunk3的值变成了chunk2的值
        // chunk3等价于chunk1值的内存块(即chunk3的地址是内存块地址，chunk3的值是内存块的值)
//        void * & chunk3 = *static_cast<void**>(chunk1);
        void * & chunk3 = *(void**)(chunk1);//c style
        chunk3 = chunk2;
        std::cout << (char*)chunk3 << std::endl;
        std::cout << (char*)(*static_cast<void**>(chunk1)) << std::endl;
    }
    {
        // chunk1的值是内存块，该内存块保存了chunk2的值
        *static_cast<void**>(chunk1) = chunk2;
        std::cout << (char*)(*static_cast<void**>(chunk1)) << std::endl;
    }
    free(chunk1);
    free(chunk2);
    
    int a = 0;
}


int main(int argc, const char * argv[]) {
    // insert code here...
//    std::cout << "Hello, World!\n";
    test_boost_pool();
    return 0;
}
