//
// Created by ka on 10.12.2021.
//

#include "test_runner.h"

#include <cstddef>  // нужно для nullptr_t

using namespace std;

// Реализуйте шаблон класса UniquePtr
template <typename T>
class UniquePtr {
private:
    T* _ptr;
public:

    UniquePtr() : _ptr(nullptr) {}

    UniquePtr(T* ptr) : _ptr(ptr) {}

    UniquePtr(const UniquePtr&) = delete;

    UniquePtr(UniquePtr&& other) {
        _ptr = other.Release();
    }

    UniquePtr& operator= (const UniquePtr& ptr) = delete;

    UniquePtr& operator= (nullptr_t) {
        Destroy();
        return *this;
    }

    UniquePtr& operator= (UniquePtr&& other) {
        if (this != &other) {
            Destroy();
            _ptr = other.Release();
        }
        return *this;
    }

    ~UniquePtr() {
        Destroy();
    }

    T& operator * () const {
        return *_ptr;
    }

    T* operator -> () const {
        return _ptr;
    }

    T* Release() {
        T* tmp = _ptr;
        _ptr = nullptr;
        return tmp;
    }

    void Reset(T* ptr) {
        Destroy();
        _ptr = ptr;
    }

    void Swap(UniquePtr& other) {
        std::swap(_ptr, other._ptr);
    }

    T* Get() const {
        return _ptr;
    }

    void Destroy() {
        if(_ptr != nullptr)
            delete _ptr;
        _ptr = nullptr;
    }
};


struct Item {
    static int counter;
    int value;
    Item(int v = 0): value(v) {
        ++counter;
    }
    Item(const Item& other): value(other.value) {
        ++counter;
    }
    ~Item() {
        --counter;
    }
};

int Item::counter = 0;


void TestLifetime() {
    Item::counter = 0;
    {
        UniquePtr<Item> ptr(new Item);
        ASSERT_EQUAL(Item::counter, 1);

        ptr.Reset(new Item);
        ASSERT_EQUAL(Item::counter, 1);
    }
    ASSERT_EQUAL(Item::counter, 0);

    {
        UniquePtr<Item> ptr(new Item);
        ASSERT_EQUAL(Item::counter, 1);

        auto rawPtr = ptr.Release();
        ASSERT_EQUAL(Item::counter, 1);

        delete rawPtr;
        ASSERT_EQUAL(Item::counter, 0);
    }
    ASSERT_EQUAL(Item::counter, 0);
}

void TestGetters() {
    UniquePtr<Item> ptr(new Item(42));
    ASSERT_EQUAL(ptr.Get()->value, 42);
    ASSERT_EQUAL((*ptr).value, 42);
    ASSERT_EQUAL(ptr->value, 42);
}

void TestSwap() {
    UniquePtr<Item> ptr(new Item(42));
    UniquePtr<Item> ptr2(new Item(24));

    ASSERT_EQUAL(Item::counter, 2);

    ptr.Swap(ptr2);

    ASSERT_EQUAL(Item::counter, 2);

    ASSERT_EQUAL(ptr.Get()->value, 24);
    ASSERT_EQUAL((*ptr2).value, 42);

    ptr = move(ptr2);
    ASSERT_EQUAL(ptr.Get()->value, 42);
    ASSERT_EQUAL(ptr2.Get(), nullptr);

    ASSERT_EQUAL(Item::counter, 1);

    UniquePtr<Item> ptr3(UniquePtr<Item>(new Item(33)));
    ASSERT_EQUAL(ptr3.Get()->value, 33);

    ASSERT_EQUAL(Item::counter, 2);
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestLifetime);
    RUN_TEST(tr, TestGetters);
    RUN_TEST(tr, TestSwap);

    return 0;
}

