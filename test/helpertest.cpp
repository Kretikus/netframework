#include "testutil.h"

#include <string>

#include <type_traits>
#include <sstream>
#include <QDebug>
#include <typeinfo>
#include <iostream>
#include <utility>
#include <functional>

#include <consthelpers.hpp>


class HelperTest: public QObject
{
    Q_OBJECT
private slots:
    void readAnswerTest() {

        qDebug() << typeid(std::remove_cv<const std::string&>::type()).name();

    }
};

REGISTER_TEST(HelperTest);

#include "helpertest.moc"
