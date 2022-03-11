#include "NativeSignal.h"
#include "TemplateSignal.h"
#include "MuduoSignal.h"
#include "Signal.h"
#include <iostream>
#include <cmath>
#include <functional>
#include <boost/signals2.hpp>
#include <boost/bind.hpp>

using namespace std;

void test_native_signal() {
    using namespace NativeSignal;
    Signal signal;
    Method method;
    int x = 0;
    signal.connect(&method, &Method::method1);
    cout << signal(x) << "\n";
    signal.connect(&method, &Method::method2);
    cout << signal(x) << "\n";
    signal.connect(&method, &Method::method3);
    cout << signal(x) << "\n";
}

void test2() {
    cout << "Test2\n";
}

class Test3 {
    int x_;
public:
    Test3(int x):x_(x) {}
    void test3() {
//        x_ = 4;
        cout << "Test3 " << x_ << "\n";
    }
};

void test_template_signal() {
    using namespace TemplateSignal;
    Signal<void()> signal;
    signal.connect([](){
        cout << "Test1\n";
    });
    function<void()> test2_func = test2;
    signal.connect(test2);
    //signal.connect(test2_func);
    signal.connect(Test3(0), &Test3::test3);
    Test3 test4(4);
    signal.connect(&test4, &Test3::test3);
    //signal();

    Signal<double(string, string)> signal1;
    signal1.connect([](string x, string y) -> double {
        return x.size() - y.size();
    });

    cout << "signal1: " << signal1("123", "1") << "\n";
    string x = "123", y = "1";
    string &&z = "hello right value";
    //cout << "signal1: " << signal1(x, y) << "\n";
}

void test_muduo_signal() {
    using namespace muduo;
    vector<Slot> slots;
    Signal<void()> signal;
    slots.push_back(
            signal.connect([](){
                cout << "test1\n";
            }));
    slots.push_back(signal.connect(test2));
    slots.push_back(signal.connect(bind(&Test3::test3, Test3(0))));
    signal.call();
}

double sqrt1(int x, int y) {
    cout << "native function: " << std::sqrt(x * x +  y * y) << endl;
}

class Sqrt {
public:
    double cal(int x, int y) {
        cout << "member function: " << std::sqrt(x * x +  y * y) << endl;
    }
};

void test_signal() {
    using namespace Utils;

    vector<Slot> slots;
    Signal<void()> signal;
    slots.push_back(
            signal.connect([](){
                cout << "test1\n";
            }));
    signal.connect(test2);
//    slots.push_back(signal.connect(test2));
    slots.push_back(signal.connect(bind(&Test3::test3, Test3(0))));
    signal();

    Signal<double(int, int)> signal1;
    slots.push_back(signal1.connect([](int x, int y) -> double {
        cout << "lambda function: " << std::sqrt(x * x +  y * y) << endl;
    }));
    slots.push_back(signal1.connect(sqrt1));
    slots.push_back(signal1.connect(std::bind(&Sqrt::cal, Sqrt(), std::placeholders::_1, std::placeholders::_2)));

    signal1(3, 4);
}

void print() {
    cout << "\n";
}
template<typename T, typename... Args>
void print(T t, Args&&... args) {
    cout << t << " ";
    print(std::forward<Args>(args)...);
}

void test_boost_signal() {
    boost::signals2::signal<bool(const string&, const string&)> signal;
    function<bool(const string&, const string&)> compare = [](const string& x, const string& y) -> bool {
        if (x < y) {
            print(x, "is less than", y);
        }
    };
    signal.connect(compare);
    signal.connect([](const string& x, const string& y) -> bool {
        if (x > y) {
            print(x, "is greater than", y);
        }
    });
    signal("123", "1");
    string x = "1", y = "123";
    signal(x, y);
}

int main() {
    //test_template_signal();
    test_boost_signal();
//    const Test3 t(10);
//    bind(&Test3::test3, t)();
    //test_signal();
    return 0;
}
