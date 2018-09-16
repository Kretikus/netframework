#pragma once

#include <iostream>

#include <string>
#include <vector>
#include <set>

#include <sstream>

#define VERIFY(actual, expected) \
{ \
    if (actual != expected) { \
        std::cerr << "Verify failed! " << std::endl; \
        std::cerr << "Actual: '" << actual << "'" << " Expected: '" << expected << "'" << std::endl; \
    } \
} \

#define VERIFY_SIMPLE(actual, expected) \
{ \
    if (actual != expected) { \
        std::cerr << "Verify failed! " << std::endl; \
    } \
} \



/*********************************************************************************
*  Serialize
*/
std::ostream & serialize(std::ostream & out, int d)
{
	out << std::to_string(d);
	return out;
}

std::string replaceInString(std::string subject, const std::string& search,
	const std::string& replace) {
	size_t pos = 0;
	while ((pos = subject.find(search, pos)) != std::string::npos) {
		subject.replace(pos, search.length(), replace);
		pos += replace.length();
	}
	return subject;
}

std::ostream & serialize(std::ostream & out, const std::string & str)
{

	out << "\"" << replaceInString(str, "\"", "\\\"") << "\"";
	return out;
}

template<typename T>
std::ostream & serializeSequence(std::ostream & out, const T & vec)
{
	out << "[";
	bool first = true;
	for (const auto & elem : vec) {
		if (!first) out << ",";
		else first = false;
		serialize(out, elem);
	}
	out << "]";
	return out;
}

template<typename T>
std::ostream & serialize(std::ostream & out, const std::vector<T> & vec)
{
	return serializeSequence(out, vec);
}

template<typename T>
std::ostream & serialize(std::ostream & out, const std::set<T> & set)
{
	return serializeSequence(out, set);
}

template <class T>
std::ostream & serialize(std::ostream & out, const T& obj)
{
	obj.serialize(out);
	return out;
}

class ObjectSer
{
public:
	ObjectSer(std::ostream & out) : _first(true), _out(out) {
		out << "{";
	}

	~ObjectSer() {
		_out << "}";
	}

	template <typename T>
	ObjectSer & operator <<(const T & t) {
		if (!_first) {
			_out << ",";
		}
		else {
			_first = false;
		}
		serialize(_out, t);
		return *this;
	}

private:
	bool _first;
	std::ostream & _out;
};

/*********************************************************************************
*  Deserialize
*/
std::string nextToken(std::istream & in)
{
	std::string ret;
	int c = in.peek();
	if (c == EOF) return ret;
	if (c == '[' || c == '{')
	{
		const char startBrace = c;
		const char endBrace = c == '[' ? ']' : '}';
		ret.push_back(in.get());
		int scopeDepth = 1;
		c = in.get();
		while (c != EOF && scopeDepth > 0) {
			ret.push_back((const char)c);
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
			ret.push_back((const char)c);
			hadBackSlash = (c == '\\');
			c = in.get();
		}
		if (in.peek() == ',') in.get();
	}
	else {
		c = in.get();
		while (c != EOF && c != ',') {
			ret.push_back((const char)c);
			c = in.get();
		}
	}
	return ret;
}

std::istream & deserialize(std::istream & in, int & k)
{
	std::string str = nextToken(in);
	k = std::stoi(str);
	return in;
}

std::istream & deserialize(std::istream & in, std::string & k)
{
	k = nextToken(in);
	return in;
}

template<typename T>
std::istream & deserializeSequence(std::istream & in, T & vec)
{
	std::string seqData = nextToken(in);
	if (seqData.length() > 2) {
		seqData.pop_back();
		std::istringstream sq(seqData);
		sq.get(); // consume [

		while (!sq.eof()) {
			std::string elem = nextToken(sq);
			std::istringstream elemData(elem);
			typename T::value_type element;
			deserialize(elemData, element);
			vec.push_back(element);
		}
	}
	return in;
}

template<typename T>
std::istream & deserialize(std::istream & in, std::vector<T> & vec)
{
	return deserializeSequence(in, vec);
}


template<class T>
std::istream & deserialize(std::istream & in, T & obj)
{
	obj.deserialize(in);
	return in;
}


class ObjectDeser
{
public:
	ObjectDeser(std::istream & in) {
		std::string objData = nextToken(in);
		if (objData.length() > 2) {
			objData.pop_back(); // cut '}'
			_in = std::istringstream(objData);
			_in.get();  // cut '{'
		}
	}

	template <typename T>
	ObjectDeser & operator >>(T & t) {
		deserialize(_in, t);
		return *this;
	}

private:
	std::istringstream _in;
};
