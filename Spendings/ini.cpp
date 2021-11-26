//
// Created by ka on 26.11.2021.
//

#include "ini.h"

namespace Ini {

    Section& Document::AddSection(string name) {
        return _sections[name];
    }

    const Section& Document::GetSection(const string& name) const {
        return _sections.at(name);
    }

    size_t Document::SectionCount() const {
        return _sections.size();
    }

    Document Load(istream& input) {
        Document document;
        Section* currentSection = nullptr;
        string line;
        while(getline(input, line)) {
            if (line.empty())
                continue;

            auto pos = line.find('=');
            if (pos == string::npos) {
                currentSection = &document.AddSection(line.substr(1, line.size() - 2));
            } else {
                // TODO !nullptr
                currentSection->insert({line.substr(0, pos), line.substr(pos+1, string::npos)});
            }
        }
        return document;

    }
}
