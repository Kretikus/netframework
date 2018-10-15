#include "serialisation.h"

std::string replaceInString(std::string subject, const std::string& search, const std::string& replace)
{
    size_t pos = 0;
    while ((pos = subject.find(search, pos)) != std::string::npos) {
        subject.replace(pos, search.length(), replace);
        pos += replace.length();
    }
    return subject;
}

std::ostream & serialize(std::ostream & out, int d)
{
    out << std::to_string(d);
    return out;
}

std::ostream & serialize(std::ostream & out, const std::string & str)
{
    out << "\"" << replaceInString(str, "\"", "\\\"") << "\"";
    return out;
}

ObjectSer::ObjectSer(std::ostream &out)
  : _first(true), _out(out)
{
    out << "{";
}

ObjectSer::~ObjectSer() {
    _out << "}";
}

std::string nextToken(std::istream &in)
{
    std::string ret;
    int c = in.peek();
    if (c == EOF) return ret;
    if (c == '[' || c == '{')
    {
        const char startBrace = static_cast<char>(c);
        const char endBrace = c == '[' ? ']' : '}';
        ret.push_back(static_cast<char>(in.get()));
        int scopeDepth = 1;
        c = in.get();
        while (c != EOF && scopeDepth > 0) {
            ret.push_back(static_cast<const char>(c));
            if (c == startBrace) ++scopeDepth;
            else if (c == endBrace) --scopeDepth;
            c = in.get();
        }
        if (in.peek() == ',') in.get();
    }
    else if (c == '"') {
        in.get(); // consume "
        c = in.get();
        bool hadBackSlash = false;
        while (c != EOF && !(c == '\"' && !hadBackSlash)) {
            ret.push_back(static_cast<const char>(c));
            hadBackSlash = (c == '\\');
            c = in.get();
        }
        if (in.peek() == ',') in.get();
    }
    else {
        c = in.get();
        while (c != EOF && c != ',') {
            ret.push_back(static_cast<const char>(c));
            c = in.get();
        }
    }
    return ret;
}

std::istream & deserialize(std::istream &in, int &k)
{
    std::string str = nextToken(in);
    k = std::stoi(str);
    return in;
}

std::istream & deserialize(std::istream &in, std::string &k)
{
    k = nextToken(in);
    return in;
}

ObjectDeser::ObjectDeser(std::istream &in)
{
    std::string objData = nextToken(in);
    if (objData.length() > 2) {
        objData.pop_back(); // cut '}'
        _in = std::istringstream(objData);
        _in.get();  // cut '{'
    }
}
