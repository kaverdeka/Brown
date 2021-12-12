//
// Created by ka on 12.12.2021.
//

#ifndef BOOKING_BOOKING_H
#define BOOKING_BOOKING_H

#include <iostream>

namespace RAII {
    template<typename Provider>
    class Booking {
    public:
        Booking(Provider* provider, int& counter) :
            _provider(provider), _counter(counter) {
        }

        Booking(const Booking&) = delete;
        Booking& operator=(const Booking&) = delete;

        Booking(Booking&& b) :
            _provider(b._provider), _counter(b._counter) {
            b._provider = nullptr;
        }
        Booking& operator= (Booking&& b) {
            _provider = b._provider;
            _counter = b._counter;
            b._provider = nullptr;
            return *this;
        }

        ~Booking() {
            if(_provider != nullptr)
                _provider->CancelOrComplete(*this);
        }
    private:
        Provider* _provider;
        int& _counter;
    };
}

#endif //BOOKING_BOOKING_H
