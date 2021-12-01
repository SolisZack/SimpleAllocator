//
// Created by zack solis on 2021/11/17.
//


#ifndef MYPROJECT_MEMORY_POOL_H
#define MYPROJECT_MEMORY_POOL_H

#include <cstdio>
#include <cstddef>

template <typename T, size_t BlockSize = 4096>
class Memory_pool {
public:
    // 官方格式定义
    typedef T               value_type;
    typedef T*              pointer;
    typedef T&              reference;
    typedef const T*        const_pointer;
    typedef const T&        const_reference;
    typedef size_t          size_type;
    typedef ptrdiff_t       difference_type;

    template <typename U> struct rebind {
        typedef Memory_pool<U> other;
    };

    // Construct
    Memory_pool();
    // Copy construct
    Memory_pool(const Memory_pool& memory_pool);
    // 泛化的Copy construct，官方接口
    template <class U> Memory_pool(const Memory_pool<U>& memoryPool) noexcept;
    // Move construct
    Memory_pool(Memory_pool&& memory_pool) noexcept;
    // Destruct
    ~Memory_pool();

    // 内存空间的分配与释放 allocate & deallocate
    // n是能存储的元素的数量，本设计只考虑存储1个元素， hint可忽略
    pointer allocate(size_type n = 1, const_pointer hint = 0);
    void deallocate(pointer p, size_type n = 1);
    // 内存中存储元素的构造与析构 construct & destroy
    template <class U, class... Args> void construct(U* p, Args ...args);
    template <class U> void destroy(U* p);
    // Allocator接口需要提供的其他函数
    size_type max_size() const;                         // 最大存储元素数量
    pointer address(reference x) const;                 // 返回内存池内给定元素地址
    const_pointer address(const_reference x) const;     // 返回内存池内给定元素地址

private:
    // 基于union来组织内存池的结构
    union Slot_ {
        T element;    // 内存块内部的slot的结构
        Slot_* next;  // 内存块、free_slots_的结构
    };

    typedef char*   data_pointer_;  // char是1字节，作为内存分配的基础单位
    typedef Slot_*  slot_pointer_;
    typedef Slot_   slot_type_;

    slot_pointer_  current_block_;  // 当前可用block
    slot_pointer_  current_slot_;   // 当前可用slot
    slot_pointer_  last_slot_;      // 当前block最后一个slot的内存地址
    slot_pointer_  free_slots_;     // 空闲slots的链表

    // 内存对齐
    size_type pad_pointer(data_pointer_ head, data_pointer_ p, size_type align) noexcept;
    // 分配新的内存块
    void allocate_block();
};

#include "memory_pool.tcc"

#endif //MYPROJECT_MEMORY_POOL_H
