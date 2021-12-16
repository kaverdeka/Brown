#include "test_runner.h"

#include <iostream>
#include <string>
#include <ctime>

using namespace std;
static const int SECONDS_IN_DAY = 60 * 60 * 24;

enum class Type {
    ComputeIncome,
    Earn,
    PayTax
};

istream& operator>>(istream& is, Type& type) {
    string str;
    is >> str;
    if (str == "ComputeIncome") {
        type = Type::ComputeIncome;
    } else if(str == "Earn") {
        type = Type::Earn;
    } else if(str == "PayTax") {
        type = Type::PayTax;
    }
    return  is;
}

struct Date {
    int year;
    int month;
    int day;

    time_t asTimestamp() const {
        std::tm t;
        t.tm_sec   = 0;
        t.tm_min   = 0;
        t.tm_hour  = 0;
        t.tm_mday  = day;
        t.tm_mon   = month - 1;
        t.tm_year  = year - 1900;
        t.tm_isdst = 0;
        return mktime(&t);
    }

    Date next() const {
        const time_t nextDayTimestamp = asTimestamp() + SECONDS_IN_DAY;
        const tm* nextDay = localtime(&nextDayTimestamp);
        return { 1900 + nextDay->tm_year, 1 + nextDay->tm_mon, nextDay->tm_mday };
    }

    bool operator!=(const Date& date) const {
        return year != date.year || month != date.month || day != date.day;
    }

    bool operator==(const Date& date) const {
        return year == date.year && month == date.month && day == date.day;
    }
};

istream& operator>>(istream& is, Date& date) {
    string str;
    is >> str;
    date.year = stoi(str.substr(0, 4));
    date.month = stoi(str.substr(5, 2));
    date.day = stoi(str.substr(8,2));
    return  is;
}

struct DateHasher {
    size_t operator() (const Date& date) const {
        size_t r1 = ihash(date.year);
        size_t r2 = ihash(date.month);
        size_t r3 = ihash(date.day);
        return  x * x * r1 + x * r2 + r3;
    }

private:
    hash<int> ihash;
    static constexpr size_t x = 13;
};

int computeDaysDiff(const Date& date_to, const Date& date_from) {
    const time_t timestamp_to = date_to.asTimestamp();
    const time_t timestamp_from = date_from.asTimestamp();
    return (timestamp_to - timestamp_from) / SECONDS_IN_DAY;
}

class Budget {
public:
    double computeIncome(Date&& from, Date&& to) {
        double inc = 0.0;
        Date date = from;
        while(date != to.next()) {
            inc += _income[date];
            date = date.next();
        }
        return inc;
    }

    void earn(Date&& from, Date&& to, int value) {
        double valuePerDay = (double) value / computeDaysDiff(to.next(), from);

        Date date = from;
        while(date != to.next()) {
            _income[date] += valuePerDay;
            date = date.next();
        }
    }

    void payTax(Date&& from, Date&& to) {
        Date date = from;
        while(date != to.next()) {
            _income[date] = _income[date] * 0.87;
            date = date.next();
        }
    }

private:
    unordered_map<Date, double, DateHasher> _income;
};

struct Query {
    Type type;
    Date from;
    Date to;
    int value = 0;

    void run(Budget& budget) {
        switch (type) {
            case Type::Earn: {
                budget.earn(move(from), move(to), value);
                break;
            }
            case Type::ComputeIncome: {
                double res = budget.computeIncome(move(from), move(to));
                cout.precision(25);
                cout << fixed << res << "\n";
                break;
            }
            case Type::PayTax: {
                budget.payTax(move(from), move(to));
                break;
            }
            default:
                break;
        }
    }
};

istream& operator>>(istream& is, Query& query) {
    is >> query.type >> query.from >> query.to;
    if (query.type == Type::Earn) {
        is >> query.value;
    }
    return  is;
}

vector<Query> read(istream& stream) {
    size_t queryCount;
    cin >> queryCount;
    string str;
    getline(stream, str);
    vector<Query> result;
    Query query;
    for(size_t i = 0; i < queryCount; ++i) {
        stream >> query;
        result.push_back(query);
    }
    return result;
}

void Test() {
    Budget budget;
    budget.earn({2000, 1, 1}, {2000, 1, 1}, 10);
    ASSERT_EQUAL(budget.computeIncome({2000, 1, 1}, {2000, 1, 1}), 10.0);
    budget.payTax({2000, 1, 1}, {2000, 1, 1});
    ASSERT_EQUAL(budget.computeIncome({2000, 1, 1}, {2000, 1, 1}), 10.0 * 0.87);
}

int main() {
//    TestRunner tr;
//    RUN_TEST(tr, Test);

    Budget budget;
    auto queries = read(cin);
    for(auto& query : queries) {
        query.run(budget);
    }
    return 0;
}


