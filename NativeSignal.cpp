// Copyright(C), Edward-Elric233
// Author: Edward-Elric233
// Version: 1.0
// Date: 2021/12/28
// Description: 
#include "NativeSignal.h"
#include <iostream>

using namespace std;

namespace NativeSignal {

    int Method::method1(int x) {
        cout << __func__ << " " << x << " ";
        return x + 1;
    }
    int Method::method2(int x) {
        cout << __func__ << " " << x << " ";
        return x + 2;
    }
    int Method::method3(int x) {
        cout << __func__ << " " << x << " ";
        return x + 3;
    }

    void Signal::connect(Method *obj, method mth) {
        obj_ = obj;
        mth_ = mth;
    }

    int Signal::operator()(int x) {
        if (obj_ && mth_) {
            return (obj_->*mth_)(x);
        } else {
            return int();
        }
    }

}
