// Copyright(C), Edward-Elric233
// Author: Edward-Elric233
// Version: 1.0
// Date: 2021/12/29
// Description: 
#ifndef SIGNALS_SLOTS_TEMPLATESIGNAL_H
#define SIGNALS_SLOTS_TEMPLATESIGNAL_H

#include <vector>
#include <functional>

namespace TemplateSignal {

    template<typename Signature> class Signal;

    //模板偏特化
    template <typename Ret, typename... Args>
    class Signal<Ret(Args...)> {
        using Slot = std::function<Ret(Args...)>;
    public:
        template<typename SLOT>
        void connect(SLOT&& slot) {
            slots_.push_back(std::forward<SLOT>(slot));
        }

        template<typename O>
        void connect(O *obj, Ret (O::*objSlot)(Args...)) {
//            objSlots_.push_back({static_cast<Obj*>(obj), static_cast<ObjSlot>(objSlot)});
            slots_.push_back(std::bind(objSlot, obj));
        }

        template<typename O>
        void connect(const O& obj, Ret (O::*objSlot)(Args...)) {
            slots_.push_back(std::bind(objSlot, obj));
        }

        template<typename ...ARGS>
        Ret operator() (ARGS&&... args) {
            for (const Slot& slot : slots_) {
                //完美转发
                slot(std::forward<ARGS>(args)...);
            }
        }
    private:
        std::vector<Slot> slots_;
    };

}



#endif //SIGNALS_SLOTS_TEMPLATESIGNAL_H
