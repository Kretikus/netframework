#include "testutil.h"

#include <serialisation.h>

namespace std {
    char * toString(const string & value) {
        return qstrdup(value.c_str());
    }
}

class NestedClass
{
public:
    std::ostream & serialize(std::ostream & out) const {
        ObjectSer obj(out);
        obj << k1 << k2 << vec;
        return out;
    }
private:
    std::string k1 = "Hello \" World!";
    int k2 = 22;
    std::vector<std::string> vec = { { "hello1" },{ "Hello2" } };
};

class TestClass
{
public:
    std::ostream & serialize(std::ostream & out) const {
        ObjectSer obj(out);
        obj << k1 << nc << k2 << s;
        return out;
    }
private:
    int k1 = 44;
    NestedClass nc;
    int k2 = 55;
    std::set<int> s = { 2, 3, 4, 5 };
};

class SimpleClass
{
public:
    SimpleClass() {}
    SimpleClass(int k) : _k(k) {}

    std::ostream & serialize(std::ostream & out) const {
        ObjectSer obj(out);
        obj << _k;
        return out;
    }

    std::istream & deserialize(std::istream & in) {
        ObjectDeser obj(in);
        obj >> _k;
        return in;
    }

    bool operator ==(const SimpleClass & rhs) const {
        return _k == rhs._k;
    }
private:
    int _k;
};

class SimpleClass2
{
public:
    SimpleClass2() {}
    SimpleClass2(int k, int k1) : _k(k), _k1(k1) {}

    std::ostream & serialize(std::ostream & out) const {
        ObjectSer obj(out);
        obj << _k << _k1;
        return out;
    }

    std::istream & deserialize(std::istream & in) {
        ObjectDeser obj(in);
        obj >> _k >> _k1;
        return in;
    }

    bool operator==(const SimpleClass2 & rhs) const {
        return _k == rhs._k &&
              _k1 == rhs._k1;
    }

    bool operator!=(const SimpleClass2 & rhs) const {
        return !operator==(rhs);
    }

private:
    int _k;
    int _k1;
};

class SimpleClass3
{
public:
    SimpleClass3() {}
    SimpleClass3(int k, int k1, const std::string & str) : _k(k), _k1(k1), _str(str) {}

    std::ostream & serialize(std::ostream & out) const {
        ObjectSer obj(out);
        obj << _k << _k1 << _str;
        return out;
    }

    std::istream & deserialize(std::istream & in) {
        ObjectDeser obj(in);
        obj >> _k >> _k1 >> _str;
        return in;
    }

    bool operator==(const SimpleClass3 & rhs) const {
        return _k == rhs._k && _k1 == rhs._k1 && _str == rhs._str;
    }

    bool operator!=(const SimpleClass3 & rhs) const {
        return !operator==(rhs);
    }

private:
    int _k;
    int _k1;
    std::string _str;
};

class ComplexClass
{
public:
    ComplexClass() {}
    ComplexClass(int k, int k1, const std::string & str, const SimpleClass3 & s3, const std::vector<SimpleClass3> & vec)
        : _k(k), _s3(s3), _k1(k1), _vec(vec), _str(str) {}

    std::ostream & serialize(std::ostream & out) const {
        ObjectSer obj(out);
        obj << _k << _s3 << _k1 << _vec << _str;
        return out;
    }

    std::istream & deserialize(std::istream & in) {
        ObjectDeser obj(in);
        obj >> _k >> _s3 >> _k1 >> _vec >> _str;
        return in;
    }

    bool operator==(const ComplexClass & rhs) const {
        return _k == rhs._k && _k1 == rhs._k1 && _str == rhs._str && _s3 == rhs._s3;
    }

    bool operator!=(const ComplexClass & rhs) const {
        return !operator==(rhs);
    }

private:
    int _k;
    SimpleClass3 _s3;
    int _k1;
    std::vector<SimpleClass3> _vec;
    std::string _str;
};


class SerialisationTest: public QObject
{
    Q_OBJECT
private slots:
    void tokenTest() {
        std::string input = "48,22,\"Hello, \\\" Test\",33,[11,{22,{2},33},44]";
        std::istringstream in(input);

        std::string token = nextToken(in);
        QCOMPARE(token, std::string("48"));
        token = nextToken(in);
        QCOMPARE(token, std::string("22"));
        token = nextToken(in);
        QCOMPARE(token, std::string("Hello, \\\" Test"));
        token = nextToken(in);
        QCOMPARE(token, std::string("33"));
        token = nextToken(in);
        QCOMPARE(token, std::string("[11,{22,{2},33},44]"));
        token = nextToken(in);
        QVERIFY(token.empty());
    }

    void basicSerTest() {
        {
            TestClass tc;
            std::stringstream s;
            serialize(s, tc);
            QCOMPARE(s.str(), std::string("{44,{\"Hello \\\" World!\",22,[\"hello1\",\"Hello2\"]},55,[2,3,4,5]}"));
        }
    }
    void deserializeTest() {
        {
            std::string str = "48";
            std::istringstream in(str);
            int k;
            deserialize(in, k);
            QCOMPARE(k, 48);
        }

        {
            std::string expected = "Hello \\\" World!";
            std::string data = "\"" + expected + "\"";
            std::istringstream in(data);
            std::string k;
            deserialize(in, k);
            QCOMPARE(k, expected);
        }

        {
            std::string data = "{67}";
            std::istringstream in(data);
            SimpleClass k;
            deserialize(in, k);
            SimpleClass expected(67);
            QCOMPARE(k, expected);
        }

        {
            std::string data = "{67,55}";
            std::istringstream in(data);
            SimpleClass2 k;
            deserialize(in, k);
            SimpleClass2 expected(67, 55);
            QCOMPARE(k, expected);
        }

        {
            std::string data = "{67,55,\"TEST\"}";
            std::istringstream in(data);
            SimpleClass3 k;
            deserialize(in, k);
            SimpleClass3 expected(67, 55, "TEST");
            QCOMPARE(k, expected);
        }
    }

    void serialiseAndDeserialiseTest()
    {
        const ComplexClass expected(11, 33, "TEST1",
            SimpleClass3(55, 66, "Hello World"),
            { { 88, 99, "Hello World1" }, { 00, 22, "Hello World2" } }
        );

        std::stringstream s;
        serialize(s, expected);

        ComplexClass actual;
        deserialize(s, actual);

        QCOMPARE(actual, expected);
    }
};
REGISTER_TEST(SerialisationTest);

#include "maintest.moc"


int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    TestUtil::TestRegistry::getInstance()->runTests(argc, argv);
}
