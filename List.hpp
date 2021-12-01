/*-
 * Copyright (c) 2013 Cosku Acay, http://www.coskuacay.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */


/*-
 * A template class that implements a simple stack structure.
 * This demostrates how to use alloctor_traits (specifically with MemoryPool)
 */

#ifndef STACK_ALLOC_H
#define STACK_ALLOC_H

#include <memory>

template <typename T>
struct Node_
{
  T data;
  Node_* prev;
};

// T是存储元素的类型、Allocator默认使用STL库的
template <class T, class Alloc = std::allocator<T> >
class List
{
  public:
    typedef Node_<T> Node;
    typedef typename Alloc::template rebind<Node>::other allocator;

    // 构造函数
    List() { head_ = nullptr; }
    // 析构函数
    ~List() { clear();}

    // 当链表为空，返回真
    bool empty() { return (head_ == nullptr); }

    // 析构所有元素、释放内存空间
    void clear() {
      Node* curr = head_;
      while (curr != 0)
      {
        Node* tmp = curr->prev;
        allocator_.destroy(curr);
        allocator_.deallocate(curr, 1);
        curr = tmp;
      }
      head_ = nullptr;
    }

    // 在链表头部添加新元素
    // 先分配内存空间，再调用构造函数
    void push_front(T element) {
      Node* newNode = allocator_.allocate(1);  // 分配内存空间
      allocator_.construct(newNode, Node());
      newNode->data = element;
      newNode->prev = head_;
      head_ = newNode;
    }

    // 删除链表头部元素
    // 先调用析构函数，再释放内存空间
    T pop_front() {
      T result = head_->data;
      Node* tmp = head_->prev;
      allocator_.destroy(head_);
      allocator_.deallocate(head_, 1);  // 释放内存空间
      head_ = tmp;
      return result;
    }

  private:
    allocator allocator_;
    Node* head_;
};

#endif // STACK_ALLOC_H
