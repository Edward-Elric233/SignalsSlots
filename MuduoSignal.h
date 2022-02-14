// Copyright(C), Edward-Elric233
// Author: Edward-Elric233
// Version: 1.0
// Date: 2022/1/9
// Description:

#ifndef MUDUO_BASE_SIGNALSLOT_H
#define MUDUO_BASE_SIGNALSLOT_H

#include <vector>
#include <functional>
#include <memory>
#include <mutex>

using namespace std;

namespace muduo
{

    namespace detail
    {

        template<typename Callback>
        struct SlotImpl;

        template<typename Callback>
        struct SignalImpl
        {
            //储存SlotImpl的weak_ptr
            typedef std::vector<weak_ptr<SlotImpl<Callback> > > SlotList;

            SignalImpl()
                    : slots_(new SlotList)
            {
            }

            void copyOnWrite()
            {
                //其他地方(call)在读
                if (!slots_.unique())
                {
                    slots_.reset(new SlotList(*slots_));
                }
            }

            //清除已经失效的SlotImpl的weak_ptr
            void clean()
            {
                lock_guard<mutex> lock(mutex_);
                copyOnWrite();
                SlotList& list(*slots_);
                typename SlotList::iterator it(list.begin());
                while (it != list.end())
                {
                    if (it->expired())
                    {
                        it = list.erase(it);
                    }
                    else
                    {
                        ++it;
                    }
                }
            }

            mutex mutex_;
            //保存的是SlotImpl的weak_ptr
            shared_ptr<SlotList> slots_;
        };

        template<typename Callback>
        struct SlotImpl
        {
            typedef SignalImpl<Callback> Data;
            SlotImpl(const shared_ptr<Data>& data, Callback&& cb)
                    : data_(data), cb_(cb), tie_(), tied_(false)
            {
            }

            SlotImpl(const shared_ptr<Data>& data, Callback&& cb,
                     const shared_ptr<void>& tie)
                    : data_(data), cb_(cb), tie_(tie), tied_(true)
            {
            }

            ~SlotImpl()
            {
                //在析构的时候清除SignalImpl中保存的SlotImpl的weak_ptr
                //SlotImpl在析构的时候其智能指针已经失效
                shared_ptr<Data> data(data_.lock());
                if (data)
                {
                    data->clean();
                }
            }

            //保存SignalImpl的weak_ptr
            weak_ptr<Data> data_;
            Callback cb_;
            weak_ptr<void> tie_;
            bool tied_;
        };

    }

/// This is the handle for a slot
///
/// The slot will remain connected to the signal fot the life time of the
/// returned Slot object (and its copies).
    typedef shared_ptr<void> Slot;

    template<typename Signature>
    class Signal;

    template <typename RET, typename... ARGS>
    class Signal<RET(ARGS...)>
    {
    public:
        typedef std::function<void (ARGS...)> Callback;
        typedef detail::SignalImpl<Callback> SignalImpl;
        typedef detail::SlotImpl<Callback> SlotImpl;

        Signal() : impl_(new SignalImpl)
        {
        }

        ~Signal()
        {
        }

        Slot connect(Callback&& func)
        {
            shared_ptr<SlotImpl> slotImpl(
                    new SlotImpl(impl_, std::forward<Callback>(func)));
            add(slotImpl);
            return slotImpl;
        }

        Slot connect(Callback&& func, const shared_ptr<void>& tie)
        {
            shared_ptr<SlotImpl> slotImpl(new SlotImpl(impl_, func, tie));
            add(slotImpl);
            return slotImpl;
        }

        void call(ARGS&&... args)
        {
            SignalImpl& impl(*impl_);
            shared_ptr<typename SignalImpl::SlotList> slots;
            {
                //read
                lock_guard<mutex> lock(impl.mutex_);
                slots = impl.slots_;
            }
            typename SignalImpl::SlotList& s(*slots);
            for (typename SignalImpl::SlotList::const_iterator it = s.begin(); it != s.end(); ++it)
            {
                shared_ptr<SlotImpl> slotImpl = it->lock();
                if (slotImpl)
                {
                    shared_ptr<void> guard;
                    if (slotImpl->tied_)
                    {
                        guard = slotImpl->tie_.lock();
                        if (guard)
                        {
                            slotImpl->cb_(args...);
                        }
                    }
                    else
                    {
                        slotImpl->cb_(args...);
                    }
                }
            }
        }

    private:

        void add(const shared_ptr<SlotImpl>& slot)
        {
            SignalImpl& impl(*impl_);
            {
                //write
                lock_guard<mutex> lock(impl.mutex_);
                impl.copyOnWrite();
                impl.slots_->push_back(slot);
            }
        }

        const shared_ptr<SignalImpl> impl_;
    };

}


#endif //SIGNALS_SLOTS_MUDUOSIGNAL_H
