#pragma once

#include "google/protobuf/compiler/plugin.h"
#include "google/protobuf/compiler/code_generator.h"
#include "google/protobuf/compiler/plugin.pb.h"
#include "google/protobuf/io/printer.h"
#include "google/protobuf/io/zero_copy_stream.h"
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>
#include <map>
#include <string>
#include <vector>

using namespace google::protobuf::compiler;
using namespace google::protobuf;

class PBExtendHelper {
public:
    typedef std::map<std::string, std::vector<std::string>> KVMap;

    template<typename DESC>
    PBExtendHelper(DESC* descriptor) {
        m_kvMap.clear();
        SourceLocation stLocation;
        if (descriptor->GetSourceLocation(&stLocation)) {
            Parse(stLocation);
        }
    }

    std::vector<std::string> Split(std::string str, std::string delim) {
        size_t cur = 0;
        size_t step = delim.size();
        size_t pos = str.find(delim);
        std::vector<std::string> res;
        while (pos != std::string::npos) {
            if (pos - cur > 0) {
                res.push_back(str.substr(cur, pos - cur));
            }
            cur = pos + step;
            pos = str.find(delim, cur);
        }
        if (str.size() > cur) {
            res.push_back(str.substr(cur));
        }
        return res;
    }

    void StripWhitespace(std::string* val) {
        size_t start = val->find_first_not_of(" ");
        size_t stop = val->find_last_not_of(" ");
        val->erase(stop + 1);
        val->erase(0, start);
    }
    
    bool ParseLine(KVMap& stkvMap, const std::string& szComment) {
        if (szComment.find("@") == std::string::npos) {
            stkvMap["desc"].push_back(szComment);
            return true;
        }

        std::vector<std::string> vList = Split(szComment, "@");
        for (int i = 0; i < vList.size(); i++) {
            std::string prop = vList[i];
            StripWhitespace(&prop);
            std::vector<std::string> vKV = Split(prop, ":");
            if (vKV.size() == 0) {
                stkvMap[prop].push_back("");
            } else if (vKV.size() == 1) {
                std::string key = vKV[0];
                StripWhitespace(&key);
                stkvMap[key].push_back("");
            } else {
                std::string key = vKV[0];
                StripWhitespace(&key);
                std::string value = "";
                for (int j = 1; j < vKV.size(); j++) {
                    if (j > 1) {
                        value += ":";
                    }
                    value += vKV[j];
                }
                StripWhitespace(&value);
                stkvMap[key].push_back(value);
            }
        }
        return true;
    }

    bool Parse(SourceLocation& stLocation) {
        std::string strComment = "";
        m_kvMap.clear();
        if (stLocation.leading_detached_comments.size() > 0) {
            for (int j = 0; j < stLocation.leading_detached_comments.size(); j++) {
                StripWhitespace(&stLocation.leading_detached_comments[j]);
                ParseLine(m_kvMap, stLocation.leading_detached_comments[j]);
            }
        }

        if (stLocation.leading_comments.length() > 0) {
            StripWhitespace(&stLocation.leading_comments);
            std::vector<std::string> vLines = Split(stLocation.leading_comments, "\n");
            for (auto it = vLines.begin(); it != vLines.end(); it++) {
                std::string sLine = *it;
                StripWhitespace(&sLine);
                ParseLine(m_kvMap, sLine);
            }
        }

        if (stLocation.trailing_comments.length() > 0) {
            StripWhitespace(&stLocation.trailing_comments);
            ParseLine(m_kvMap, stLocation.trailing_comments);
            //strComment += stLocation.trailing_comments;
        }

        //return ParseLine(m_kvMap, strComment);
        return true;
    }

    bool HasProp(const std::string& key) {
        return m_kvMap.find(key) != m_kvMap.end();
    }

    std::string GetProp(const std::string& key) {
        std::string sVal = "";
        for (auto& v : m_kvMap[key]) {
            if (sVal != "") {
                sVal += "\n";
            }
            sVal += v;
        }
        return sVal;
    }

    void Dump(io::Printer& printer) {
        for (auto it = m_kvMap.begin(); it != m_kvMap.end(); it++) {
            printer.Print("[^key^]=^value^\n", "key", it->first, "value", GetProp(it->first));
        }
    }

    std::string Dump() {
        rapidjson::StringBuffer s;
        rapidjson::Writer<rapidjson::StringBuffer> writer(s);
        writer.StartObject();

        for (auto it = m_kvMap.begin(); it != m_kvMap.end(); it++) {
            writer.Key(it->first.c_str());
            std::vector<std::string>& sValList = it->second;
            writer.StartArray();
            for (auto& v : sValList) {
                writer.String(v.c_str());
            }
            writer.EndArray();
        }
        writer.EndObject();
        return s.GetString();
    }

    void Dump(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) {
        writer.StartObject();
        for (auto it = m_kvMap.begin(); it != m_kvMap.end(); it++) {
            writer.Key(it->first.c_str());
            std::vector<std::string>& sValList = it->second;
            writer.StartArray();
            for (auto& v : sValList) {
                writer.String(v.c_str());
            }
            writer.EndArray();
        }
        writer.EndObject();
    }

private:
    KVMap m_kvMap;
};
