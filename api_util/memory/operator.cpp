#include "operator.h"
#include <list>
#include <fstream>

#ifdef __linux__
    #include <sys/stat.h>
    #include <fcntl.h>
#endif

using namespace std;
static const char* folderPath = "/home/liqiang/source/util/api_util/memory/";

void *operator new(std::size_t size)
{
    static bool isCreate = false;
    // 使用 mkdir 函数创建文件夹，权限设置为 0777（可读、可写、可执行）
    if(!isCreate){
#ifdef __linux__
        int result = mkdir(folderPath, 0777);
#endif
        isCreate = true;
    }
    void* p = std::malloc(size);
    if (!p) {
        throw std::bad_alloc();
    }
    std::cout << "Allocating " << size << " bytes.p:" << p  << std::endl;
    std::ofstream outFile("example.txt");
    //create one file

    return p;
}


void operator delete(void *p) noexcept
{
    std::cout << "Deallocating memory p:" << p << std::endl;
    std::free(p);
}
