#include "test_runner.h"

#include <iostream>
#include <string>
#include <ctime>
#include <vector>
#include <algorithm>
#include <map>
#include <unordered_map>

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

    bool operator<(const Date& date) const {
        return vector<int>{ year, month, day } < vector<int>{date.year, date.month, date.day };
    }
};

istream& operator>>(istream& is, Date& date) {
    string str;
    is >> str;

    istringstream str_stream(move(str));
    str_stream >> date.year;
    str_stream.ignore(1);
    str_stream >> date.month;
    str_stream.ignore(1);
    str_stream >> date.day;
    return is;
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

class BudgetUnorderedMap {
public:
    double computeIncome(Date&& from, Date&& to) {
        if(to < from)
            return 0.0;

        double inc = 0.0;
        auto date = from.asTimestamp();
        auto next = to.next().asTimestamp();
        while(date != next) {
            inc += _income[date];
            date += SECONDS_IN_DAY;
        }
        return inc;
    }

    void earn(Date&& from, Date&& to, double value) {
        if(to < from)
            return;

        double valuePerDay = (double) value / (computeDaysDiff(to, from) + 1);

        auto date = from.asTimestamp();
        auto next = to.next().asTimestamp();
        while(date != next) {
            _income[date] += valuePerDay;
            date += SECONDS_IN_DAY;
        }
    }

    void payTax(Date&& from, Date&& to) {
        if(to < from)
            return;

        auto date = from.asTimestamp();
        auto next = to.next().asTimestamp();
        while(date != next) {
            _income[date] = _income[date] * 0.87;
            date += SECONDS_IN_DAY;
        }
    }

private:
    unordered_map<time_t /*Date*/, double/*, DateHasher*/> _income;
};

class BudgetMap {
public:
    double computeIncome(Date&& from, Date&& to) {
        double inc = 0.0;

        auto firstIt = _income.lower_bound(from);
        auto lastIt = _income.upper_bound(to);

        for(auto it = firstIt; it != lastIt; ++it) {
            inc += it->second;
        }

        return inc;
    }

    void earn(Date&& from, Date&& to, int value) {
        double valuePerDay = (double) value / (computeDaysDiff(to, from) + 1);

        Date date = from;
        auto next = to.next();
        while(date != next) {
            _income[date] += valuePerDay;
            date = date.next();
        }
    }

    void payTax(Date&& from, Date&& to) {
        auto firstIt = _income.lower_bound(from);
        auto lastIt = _income.upper_bound(to);

        for(auto it = firstIt; it != lastIt; ++it) {
            it->second *= 0.87;
        }
    }

private:
    map<Date, double> _income;
};

class BudgetVector {
public:
    BudgetVector() :
        _income(computeDaysDiff(_firstDate, _lastDate), 0)
    {
    }

    double computeIncome(Date&& from, Date&& to) {
        double inc = 0.0;

        for(auto i = getIndex(from); getIndex(to) < i; ++i) {
            inc += _income[i];
        }
        return inc;
    }

    void earn(Date&& from, Date&& to, int value) {
        double valuePerDay = (double) value / (computeDaysDiff(to, from) + 1);

        for(auto i = getIndex(from); getIndex(to) < i; ++i) {
            _income[i] += valuePerDay;
        }
    }

    void payTax(Date&& from, Date&& to) {
        for(auto i = getIndex(from); getIndex(to) < i; ++i) {
            _income[i] *= 0.87;
        }
    }

private:
    size_t getIndex(const Date& date) const {
        return computeDaysDiff(date, _firstDate);
    }

    static constexpr Date _firstDate = { 2000, 1, 1 };
    static constexpr Date _lastDate = { 2099, 12, 31 };
    vector<double> _income;
};

//*********************************
using Budget = BudgetUnorderedMap;
//*********************************

struct Query {
    Type type;
    Date from;
    Date to;
    double value = 0.0;

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
    budget.earn({2000, 1, 1}, {2000, 1, 1}, 10.0);
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


