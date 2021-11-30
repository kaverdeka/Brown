//
// Created by ka on 29.11.2021.
//
#include "test_runner.h"
#include <functional>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <deque>

using namespace std;


struct Email {
    string from;
    string to;
    string body;
};


class Worker {
public:
    virtual ~Worker() = default;
    virtual void Process(unique_ptr<Email> email) = 0;
    virtual void Run() {
        // только первому worker-у в пайплайне нужно это имплементировать
        throw logic_error("Unimplemented");
    }

protected:
    // реализации должны вызывать PassOn, чтобы передать объект дальше
    // по цепочке обработчиков
    void PassOn(unique_ptr<Email> email) const {
        if(_next)
            _next->Process(move(email));
    }

public:
    void SetNext(unique_ptr<Worker> next) {
        _next = move(next);
    }

private:
    unique_ptr<Worker> _next;
};


class Reader : public Worker {
public:
    Reader(istream& in) : _in(in) {
        for(string from; getline(_in, from);) {
            _emails.emplace_back(unique_ptr<Email>(new Email));
            _emails.back()->from = from;
            getline(_in, _emails.back()->to);
            getline(_in, _emails.back()->body);
        }
    }

    void Run() {
        for(auto&& email : _emails) {
            Process(move(email));
        }
    }

    void Process(unique_ptr<Email> email) override {
        PassOn(move(email));
    }

private:
    istream& _in;
    vector<unique_ptr<Email>> _emails;
};


class Filter : public Worker {
public:
    using Function = function<bool(const Email&)>;

public:
    Filter(Function f) : _f(f) {}

    void Process(unique_ptr<Email> email) override {
        if(_f(*email))
            PassOn(move(email));
    }

private:
    Function _f;
};


class Copier : public Worker {
public:
    Copier(const string& to) : _to(to) {}

    void Process(unique_ptr<Email> email) override {
        auto emailCopy = *email;

        PassOn(move(email));

        if(emailCopy.to != _to) {
            PassOn(unique_ptr<Email>(new Email{ emailCopy.from, _to, emailCopy.body}));
        }
    }

private:
    string _to;
};


class Sender : public Worker {
public:
    Sender(ostream& os) : _os(os) {}

    void Process(unique_ptr<Email> email) override {

        _os << email->from << "\n";
        _os << email->to << "\n";
        _os << email->body << "\n";

        PassOn(move(email));
    }

private:
    ostream& _os;
};


// реализуйте класс
class PipelineBuilder {
public:
    // добавляет в качестве первого обработчика Reader
    explicit PipelineBuilder(istream& in) {
        _workers.emplace_back(unique_ptr<Worker>(new Reader(in)));
    }

    // добавляет новый обработчик Filter
    PipelineBuilder& FilterBy(Filter::Function filter) {
        _workers.emplace_back(unique_ptr<Worker>(new Filter(filter)));
        return *this;
    }

    // добавляет новый обработчик Copier
    PipelineBuilder& CopyTo(string recipient) {
        _workers.emplace_back(unique_ptr<Worker>(new Copier(recipient)));
        return *this;
    }

    // добавляет новый обработчик Sender
    PipelineBuilder& Send(ostream& out) {
        _workers.emplace_back(unique_ptr<Worker>(new Sender(out)));
        return *this;
    }

    // возвращает готовую цепочку обработчиков
    unique_ptr<Worker> Build() {
        for(auto it = _workers.rbegin(); it != next(_workers.rend(), -1); ++it) {
            auto currentIt = next(it, 1);
            (*currentIt)->SetNext(move(*it));
        }

        return move(_workers.front());
    }

    deque<unique_ptr<Worker>> _workers;
};


void TestSanity() {
    string input = (
            "erich@example.com\n"
            "richard@example.com\n"
            "Hello there\n"

            "erich@example.com\n"
            "ralph@example.com\n"
            "Are you sure you pressed the right button?\n"

            "ralph@example.com\n"
            "erich@example.com\n"
            "I do not make mistakes of that kind\n"
    );
    istringstream inStream(input);
    ostringstream outStream;

    PipelineBuilder builder(inStream);
    builder.FilterBy([](const Email& email) {
        return email.from == "erich@example.com";
    });
    builder.CopyTo("richard@example.com");
    builder.Send(outStream);
    auto pipeline = builder.Build();

    pipeline->Run();

    string expectedOutput = (
            "erich@example.com\n"
            "richard@example.com\n"
            "Hello there\n"

            "erich@example.com\n"
            "ralph@example.com\n"
            "Are you sure you pressed the right button?\n"

            "erich@example.com\n"
            "richard@example.com\n"
            "Are you sure you pressed the right button?\n"
    );

    ASSERT_EQUAL(expectedOutput, outStream.str());
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestSanity);
    return 0;
}
