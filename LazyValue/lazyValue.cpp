//
// Created by ka on 28.11.2021.
//

#include "test_runner.h"

#include <functional>
#include <string>
using namespace std;

template <typename T>
class LazyValue {
public:
    explicit LazyValue(function<T()> init) :
        _initiator(move(init)) {}

    ~LazyValue() {
        if(HasValue())
            delete _object;
    }

    bool HasValue() const {
        return _object != nullptr;
    }

    const T& Get() const {
        if(HasValue())
            return *_object;

        T t = _initiator();
        _object = new T(move(t));
        return *_object;
    }

private:
    function<T()> _initiator;
    mutable T* _object = nullptr;
};

void UseExample() {
    const string big_string = "Giant amounts of memory";

    LazyValue<string> lazy_string([&big_string] { return big_string; });

    ASSERT(!lazy_string.HasValue());
    ASSERT_EQUAL(lazy_string.Get(), big_string);
    ASSERT_EQUAL(lazy_string.Get(), big_string);
}

void TestInitializerIsntCalled() {
    bool called = false;

    {
        LazyValue<int> lazy_int([&called] {
            called = true;
            return 0;
        });
    }
    ASSERT(!called);
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, UseExample);
    RUN_TEST(tr, TestInitializerIsntCalled);

    return 0;
}