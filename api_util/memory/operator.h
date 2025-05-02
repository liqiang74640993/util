#ifndef OPERATOR_H
#define OPERATOR_H

#include <iostream>
#include <cstdlib>

void* operator new(std::size_t size);

// 重载全局的 operator delete
void operator delete(void* p) noexcept;

#endif // OPERATOR_H
