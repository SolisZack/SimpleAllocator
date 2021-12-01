//
// Created by zack solis on 2021/11/17.
//

#ifndef MEMORY_BLOCK_TCC
#define MEMORY_BLOCK_TCC

template <typename T, size_t BlockSize>
Memory_pool<T, BlockSize>::Memory_pool() {
    current_block_ = nullptr;
    current_slot_ = nullptr;
    last_slot_ = nullptr;
    free_slots_ = nullptr;
}

template <typename T, size_t BlockSize>
Memory_pool<T, BlockSize>::Memory_pool(const Memory_pool& memory_pool) {
    std::cout << "copy construct" << std::endl;
    Memory_pool();
}

template <typename T, size_t BlockSize>
template <class U>
Memory_pool<T, BlockSize>::Memory_pool(const Memory_pool<U>& memory_pool) noexcept {
    std::cout << "template copy construct" << std::endl;
    Memory_pool();
}


template <typename T, size_t BlockSize>
Memory_pool<T, BlockSize>::Memory_pool(Memory_pool&& memory_pool)  noexcept {
    std::cout << "move construct" << std::endl;
    current_block_ = memory_pool.current_block_;
    memory_pool.current_block_ = nullptr;
    current_slot_ = memory_pool.current_slot_;
    last_slot_ = memory_pool.last_slot_;
    free_slots_ = memory_pool.free_slots_;
}

// 不用考虑slot内储存的元素的析构，这个留给destroy等函数
template <typename T, size_t BlockSize>
Memory_pool<T, BlockSize>::~Memory_pool() {
    slot_pointer_ cur_block = current_block_;
    while (cur_block != nullptr) {
        slot_pointer_ next = cur_block->next;
        operator delete(reinterpret_cast<void*>(cur_block));  // operate delete 只删除内存空间，不会调用析构函数
        cur_block = next;
    }
}

// 分配slot
// 如果有free_slots_就直接从中分配
// 没有的话有2种可能：
// 1.前面分配的slot都被使用，当前block还有多余slot，则直接分配
// 2.所有block里的slot都被使用，分配新的block
template <typename T, size_t BlockSize>
inline typename Memory_pool<T, BlockSize>::pointer
Memory_pool<T, BlockSize>::allocate(size_type n, const_pointer hint) {
//    std::cout << "allocating slot" << std::endl;
    if (free_slots_ != nullptr) {
//        std::cout << "allocating free slot" << std::endl;
        pointer allocate_slot = reinterpret_cast<pointer>(free_slots_);
        free_slots_ = free_slots_->next;
        return allocate_slot;
    }
    else {
//        std::cout << "allocating current slot" << std::endl;
        if (current_slot_ >= last_slot_)
            allocate_block();
        return reinterpret_cast<pointer>(current_slot_++);
    }
}

template <typename T, size_t BlockSize>
inline void Memory_pool<T, BlockSize>::deallocate(pointer p, size_type n) {
//    std::cout << "deallocating" << std::endl;
    if (p != nullptr) {
        reinterpret_cast<slot_pointer_>(p)->next = free_slots_;  // 直接强转
        free_slots_ = reinterpret_cast<slot_pointer_>(p);
    }
}

// 调用存储元素的构造函数
template <typename T, size_t BlockSize>
template <class U, class... Args>
inline void Memory_pool<T, BlockSize>::construct(U *p, Args ...args) {
    // placement new
    new(p) U(std::forward<Args>(args)...);
}

// 调用存储元素的析构函数
template <typename T, size_t BlockSize>
template <class U>
inline void Memory_pool<T, BlockSize>::destroy(U *p) {
    p->~U();
}

// 最大存储元素数量
// 返回当前计算机可以创建的最大slot数
template <typename T, size_t BlockSize>
inline typename Memory_pool<T, BlockSize>::size_type
Memory_pool<T, BlockSize>::max_size() const {
    size_t max_blocks = -1 / BlockSize;
    // 内存块（block）的首地址是用char*占位标识的，所以剪掉了
    size_t max_slots = (BlockSize - sizeof(data_pointer_)) / sizeof(Slot_) * max_blocks;
    return max_slots;
}

// 返回内存池内给定元素地址
template <typename T, size_t BlockSize>
inline typename Memory_pool<T, BlockSize>::pointer
Memory_pool<T, BlockSize>::address(reference x) const {
    return &x;
}

// 返回内存池内给定元素地址
template <typename T, size_t BlockSize>
inline typename Memory_pool<T, BlockSize>::const_pointer
Memory_pool<T, BlockSize>::address(const_reference x) const {
    return &x;
}


// 内存对齐，结合allocate_block理解
template <typename T, size_t BlockSize>
inline typename Memory_pool<T, BlockSize>::size_type
Memory_pool<T, BlockSize>::pad_pointer(data_pointer_ head , data_pointer_ p, size_type align)
noexcept
{
    uintptr_t head_address = reinterpret_cast<uintptr_t>(head);
    uintptr_t cur_address = reinterpret_cast<uintptr_t>(p);
    uintptr_t position = cur_address - head_address + 1;
//    printf("align:%i, position:%i\n", align, position);
    if (position % align == 1) {
        return 0;
    }else {
        return align - position % align;
    }
}

// 分配新的内存块
template <typename T, size_t BlockSize>
void Memory_pool<T, BlockSize>::allocate_block() {
//    printf("allocating block\n");
    data_pointer_ new_block = reinterpret_cast<data_pointer_>(operator new(BlockSize));
    // 重新链接内存块
    reinterpret_cast<slot_pointer_>(new_block)->next = current_block_;
    current_block_ = reinterpret_cast<slot_pointer_>(new_block);
    // 获取可用内存空间首地址
    data_pointer_ body = new_block + sizeof(slot_pointer_);
    // 考虑内存对齐，计算内存位移量
    size_t padding = pad_pointer(new_block, body, alignof(slot_type_));
    // 获得slot信息
    current_slot_ = reinterpret_cast<slot_pointer_>(body + padding);
    last_slot_ = reinterpret_cast<slot_pointer_>(new_block + BlockSize - sizeof(slot_type_));
}

#endif // MEMORY_BLOCK_TCC