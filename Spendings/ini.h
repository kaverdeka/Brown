//
// Created by ka on 26.11.2021.
//

#ifndef SPENDINGS_INI_H
#define SPENDINGS_INI_H

#include <string>
#include <unordered_map>
#include <cstddef>
#include <iostream>

using namespace std;

namespace Ini {
    using Section = unordered_map<string, string>;

    class Document {
    public:
        Section& AddSection(string name);
        const Section& GetSection(const string& name) const;
        size_t SectionCount() const;

    private:
        unordered_map<string, Section> _sections;
    };

    Document Load(istream& input);
}


#endif //SPENDINGS_INI_H
