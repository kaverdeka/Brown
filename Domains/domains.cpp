//
// Created by ka on 12.12.2021.
//
#include <algorithm>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

#include "test_runner.h"
using namespace std;


bool IsSubdomain(string_view subdomain_view, string_view domain_view) {
    const string subdomain(rbegin(subdomain_view), rend(subdomain_view));
    const string domain(rbegin(domain_view), rend(domain_view));
    int i = subdomain.size() - 1;
    int j = domain.size() - 1;
    while (i >= 0 && j >= 0) {
        if (subdomain[i--] != domain[j--]) {
            return false;
        }
    }
    return (i < 0 && j < 0)
           || (i < 0 && domain[j] == '.')
           || (j < 0 && subdomain[i] == '.');
}


vector<string> ReadDomains() {
    size_t count = 0;
    cin >> count;
    string domain;
    getline(cin, domain);

    vector<string> domains;
    for (size_t i = 0; i < count; ++i) {
        getline(cin, domain);
        domains.push_back(domain);
    }
    return domains;
}

//void Test() {
//    std::array<std::string, 10> ar {"a",
//                                    "a.a",
//                                    "a.a.a",
//                                    "aa.aa.aa",
//                                    "aaa.aa.aaa",
//                                    "b",
//                                    "b.b",
//                                    "b.b.b",
//                                    "bb.bb.bb",
//                                    "bbb.bb.bbb"};
//
//    std::vector<bool> result;
//    string dom = "a";
//    for(size_t i = 0; i < 10; ++i) {
//        result.emplace_back(IsSubdomain(ar[i], dom));
//    }
//    std::vector<bool> correct = {true, true, true, false, false, false, false, false, false, false};
//    ASSERT_EQUAL(result, correct);
//
//    dom = "b.b";
//    result.clear();
//    for(size_t i = 0; i < 10; ++i) {
//        result.emplace_back(IsSubdomain(ar[i], dom));
//    }
//    correct = {false, false, false, false, false, false, true, true, false, false};
//    ASSERT_EQUAL(result, correct);
//}


int main() {
//    {
//        TestRunner tr;
//        RUN_TEST(tr, Test);
//    }

    vector<string> banned_domains = ReadDomains();
    vector<string> domains_to_check = ReadDomains();

    for (string& domain : banned_domains) {
        reverse(begin(domain), end(domain));
    }
    sort(begin(banned_domains), end(banned_domains));

    size_t insert_pos = 0;
    for (string& domain : banned_domains) {
        if (insert_pos == 0 || !IsSubdomain(domain, banned_domains[insert_pos - 1])) {
            swap(banned_domains[insert_pos++], domain);
        }
    }
    banned_domains.resize(insert_pos);

    for (string &domain : domains_to_check) {
        reverse(begin(domain), end(domain));
        if (const auto it = upper_bound(begin(banned_domains), end(banned_domains), domain);
                it != begin(banned_domains) && IsSubdomain(domain, *prev(it))) {
            cout << "Bad" << endl;
        } else {
            cout << "Good" << endl;
        }
    }
    return 0;
}
