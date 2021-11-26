//
// Created by ka on 25.11.2021.
//

#include "test_runner.h"

#include <iostream>
#include <map>
#include <string>
#include <unordered_map>

using namespace std;

struct Record {
    string id;
    string title;
    string user;
    int timestamp;
    int karma;
};

class Database {
    struct Data {
        Record _record;
        multimap<int, const Record*>::iterator _timestamp;
        multimap<int, const Record*>::iterator _karma;
        multimap<string, const Record*>::iterator _user;
    };

public:
    bool Put(const Record& record) {
        auto result = _store.emplace(record.id, Data {record, {}, {}, {}});
        if(!result.second)
            return false;

        const Record* recordPtr = GetById(record.id);
        Data& data = result.first->second;
        data._timestamp = _timestamps.emplace(record.timestamp, recordPtr);
        data._karma = _karmas.emplace(record.karma, recordPtr);
        data._user = _users.emplace(record.user, recordPtr);
        return true;
    }

    const Record* GetById(const string& id) const {
        if(_store.count(id) > 0)
            return &_store.at(id)._record;

        return nullptr;
    }

    bool Erase(const string& id) {
        if(_store.count(id) == 0)
             return false;

        const Data& data = _store.at(id);
        _timestamps.erase(data._timestamp);
        _karmas.erase(data._karma);
        _users.erase(data._user);

        return _store.erase(id);
    }

    template <typename Callback>
    void RangeByTimestamp(int low, int high, Callback callback) const {
        RangeBy(low, high, callback, _timestamps);
    }

    template <typename Callback>
    void RangeByKarma(int low, int high, Callback callback) const {
        RangeBy(low, high, callback, _karmas);
    }

    template <typename Callback>
    void AllByUser(const string& user, Callback callback) const {
        RangeBy(user, user, callback, _users);
    }

private:
    template<class Callback, class Type>
    void RangeBy(Type low, Type high, Callback callback, const multimap<Type, const Record*>& map) const {
        auto lowIt = map.lower_bound(low);
        auto highIt = map.upper_bound(high);
        for(auto it = lowIt; it != highIt; ++it) {
            bool result = callback(*it->second);
            if(!result)
                return;
        }
    }

    unordered_map<string, Data> _store;
    multimap<int, const Record*> _timestamps;
    multimap<int, const Record*> _karmas;
    multimap<string, const Record*> _users;
};

void TestRangeBoundaries() {
    const int good_karma = 1000;
    const int bad_karma = -10;

    Database db;
    db.Put({"id1", "Hello there", "master", 1536107260, good_karma});
    db.Put({"id2", "O>>-<", "general2", 1536107260, bad_karma});

    int count = 0;
    db.RangeByKarma(bad_karma, good_karma, [&count](const Record&) {
        ++count;
        return true;
    });

    ASSERT_EQUAL(2, count);

    count = 0;
    db.RangeByKarma(good_karma, bad_karma, [&count](const Record&) {
        ++count;
        return true;
    });

    ASSERT_EQUAL(0, count);

    count = 0;
    db.RangeByKarma(-20, -15, [&count](const Record&) {
        ++count;
        return true;
    });

    ASSERT_EQUAL(0, count);

    count = 0;
    db.RangeByKarma(-20, 1000, [&count](const Record&) {
        ++count;
        return true;
    });

    ASSERT_EQUAL(2, count);
}

void TestSameUser() {
    Database db;
    db.Put({"id0", "Don't sell", "ma", 1536107263, 1001});
    db.Put({"id8", "Don't sell", "mas", 1536107260, 1000});
    db.Put({"id1", "Don't sell", "maste", 1536107260, 1000});
    db.Put({"id2", "Rethink life", "master", 1536107260, 2000});
    db.Put({"id10", "Don't sell", "maste", 1536107260, 1000});

//    db.Erase("id1");
//    db.Erase("id10");

    int count = 0;
    db.AllByUser("maste", [&count](const Record&) {
        ++count;
        return true;
    });

    ASSERT_EQUAL(2, count);

    count = 0;
    db.AllByUser("maste", [&count](const Record&) {
        ++count;
        return false;
    });

    ASSERT_EQUAL(1, count);
}

void TestReplacement() {
    const string final_body = "Feeling sad";

    Database db;
    db.Put({"id", "Have a hand", "not-master", 1536107260, 10});
    db.Erase("id");
    db.Put({"id", final_body, "not-master", 1536107260, -10});

    auto record = db.GetById("id");
    ASSERT(record != nullptr);
    ASSERT_EQUAL(final_body, record->title);
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestRangeBoundaries);
    RUN_TEST(tr, TestSameUser);
    RUN_TEST(tr, TestReplacement);
    return 0;
}
