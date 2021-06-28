#pragma once

#include <set>
#include <algorithm>
#include <events.hpp>

template <class EventHandlerType>
struct EventDispatcher final : public IEventDispatcher<EventHandlerType> {
    bool addEventHandler(EventHandlerType* handler) override {
        return handlers.insert(handler).second;
    }

    bool removeEventHandler(EventHandlerType* handler) override {
        return handlers.erase(handler) != 0;
    }

    bool hasEventHandler(EventHandlerType* handler) override {
        return handlers.find(handler) != handlers.end();
    }

    template<typename Return, typename ...Params, typename ...Args>
    void dispatch(Return(EventHandlerType::* mf)(Params...), Args &&... args) {
        for (EventHandlerType* handler : handlers) {
            (handler->*mf)(std::forward<Args>(args)...);
        }
    }

    template<typename Fn>
    void all(Fn fn) {
        std::for_each(handlers.begin(), handlers.end(), fn);
    }

    template <typename Fn>
    auto stopAtFalse(Fn fn) {
        return std::all_of(handlers.begin(), handlers.end(), fn);
    }

private:
    std::set<EventHandlerType*> handlers;
};

template <class EventHandlerType, size_t Count>
struct IndexedEventDispatcher final : public IIndexedEventDispatcher<EventHandlerType, Count> {
    bool addEventHandler(EventHandlerType* handler, size_t index) override {
        if (index >= handlers.size()) {
            return false;
        }
        return handlers[index].insert(handler).second;
    }

    bool removeEventHandler(EventHandlerType* handler, size_t index) override {
        if (index >= handlers.size()) {
            return false;
        }
        return handlers[index].erase(handler) != 0;
    }

    bool hasEventHandler(EventHandlerType* handler, size_t index) override {
        if (index >= handlers.size()) {
            return false;
        }
        return handlers[index].find(handler) != handlers[index].end();
    }

    template<typename Return, typename ...Params, typename ...Args>
    void dispatch(size_t index, Return(EventHandlerType::* mf)(Params...), Args &&... args) {
        if (index >= Count) {
            return;
        }
        for (EventHandlerType* handler : handlers[index]) {
            (handler->*mf)(std::forward<Args>(args)...);
        }
    }

    template<typename Fn>
    void all(size_t index, Fn fn) {
        std::for_each(handlers[index].begin(), handlers[index].end(), fn);
    }

    template <typename Fn>
    void stopAtFalse(size_t index, Fn fn) {
        return std::all_of(handlers[index].begin(), handlers[index].end(), fn);
    }

private:
    std::array<std::set<EventHandlerType*>, Count> handlers;
};