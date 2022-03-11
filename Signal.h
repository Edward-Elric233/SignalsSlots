// Copyright(C), Edward-Elric233
// Author: Edward-Elric233
// Version: 1.0
// Date: 2022/1/10
// Description: 
#ifndef SIGNALS_SLOTS_SIGNAL_H
#define SIGNALS_SLOTS_SIGNAL_H

#include <mutex>
#include <memory>
#include <functional>
#include <unordered_map>

namespace Utils {

    using Slot = std::shared_ptr<void>;

    //前置声明
    template<typename Signature>
    class Signal;
    template<typename Ret, typename... Args>
    class Signal<Ret(Args...)>;

    namespace detail {
        //前置声明
        template<typename Callback> class SlotImpl;

        template<typename Callback>
        class SignalImpl {
        public:
            using SlotList = std::unordered_map<SlotImpl<Callback> *, std::weak_ptr<SlotImpl<Callback>>>;

            SignalImpl()
            : slots_(new SlotList)
            , mutex_() {

            }
            ~SignalImpl() {

            }

            //只能在加锁后使用
            void cowWithLock() {
                if (!slots_.unique()) {
                    slots_.reset(new SlotList(*slots_));
                }
            }

            //添加槽函数
            void add(const std::shared_ptr<SlotImpl<Callback>> &slot) {
                std::lock_guard<std::mutex> lockGuard(mutex_);
                cowWithLock();
                slots_->insert({slot.get(), slot});
            }
            //供SlotImpl调用，删除槽函数
            void remove(SlotImpl<Callback> *slot) {
                std::lock_guard<std::mutex> lockGuard(mutex_);
                cowWithLock();
                slots_->erase(slot);
            }

            std::shared_ptr<SlotList> getSlotList() {
                std::lock_guard<std::mutex> lockGuard(mutex_);
                return slots_;
            }
        private:
            std::mutex mutex_;
            //保存SlotImpl的weak_ptr
            //之所以不保存SlotList而是保存其shared_ptr是为了实现COW
            std::shared_ptr<SlotList> slots_;
        };

        template<typename Callback>
        class SlotImpl {
        public:
            SlotImpl(Callback&& cb, const std::shared_ptr<SignalImpl<Callback>> &signal)
            : cb_(cb)
            , signal_(signal) {

            }
            ~SlotImpl() {
                auto signal = signal_.lock();
                if (signal) {
                    signal->remove(this);
                }
            }

            Callback cb_;
        private:
            //保存SignalImpl的weak_ptr
            std::weak_ptr<SignalImpl<Callback>> signal_;
        };
    }

    template<typename Ret, typename... Args>
    class Signal<Ret(Args...)> {
    public:
        using Callback = std::function<Ret(Args...)>;
        using SignalImpl = detail::SignalImpl<Callback>;
        using SlotImpl = detail::SlotImpl<Callback>;

        Signal()
        : impl_(new SignalImpl) {

        }

        ~Signal() {

        }

        /*!
         * 添加槽函数
         * @param cb 槽函数
         * @return 需要保存这个智能指针，否则会自动从槽函数列表中删除
         */
        template<typename Func>
        Slot connect(Func&& cb) {
            std::shared_ptr<SlotImpl> slot(new SlotImpl(std::forward<Func>(cb), impl_));
            impl_->add(slot);
            return slot;
        }

        template<typename ...ARGS>
        void operator() (ARGS&&... args) {
            std::shared_ptr<std::unordered_map<SlotImpl *, std::weak_ptr<SlotImpl>>> slots = impl_->getSlotList();
            //使用引用避免智能指针的解引用
            std::unordered_map<SlotImpl *, std::weak_ptr<SlotImpl>> &s = *slots;
            for (auto &pr : s) {
                std::weak_ptr<SlotImpl> &pWkSlotImpl = pr.second;
                std::shared_ptr<SlotImpl> pSlotImpl = pWkSlotImpl.lock();
                if (pSlotImpl) {
                    pSlotImpl->cb_(std::forward<ARGS>(args)...);
                }
            }
        }

    private:
        //保存shared_ptr的原因是需要传递给SlotImpl，在SlotImpl析构的时候会清除自己
        const std::shared_ptr<SignalImpl> impl_;
    };
}

#endif //SIGNALS_SLOTS_SIGNAL_H
