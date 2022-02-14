// Copyright(C), Edward-Elric233
// Author: Edward-Elric233
// Version: 1.0
// Date: 2021/12/28
// Description: 
#ifndef SIGNALS_SLOTS_NATIVESIGNAL_H
#define SIGNALS_SLOTS_NATIVESIGNAL_H

namespace NativeSignal {

    class Method {
    public:
        int method1(int);
        int method2(int);
        int method3(int);
    };

    class Signal {
    public:
        typedef int (Method::*method)(int);
        void connect(Method *, method);
        int operator()(int);
    private:
        Method *obj_ = nullptr;
        method mth_ = nullptr;
    };

}



#endif //SIGNALS_SLOTS_NATIVESIGNAL_H
