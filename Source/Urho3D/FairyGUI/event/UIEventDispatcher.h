#ifndef __UIEVENTDISPATCHER_H__
#define __UIEVENTDISPATCHER_H__

#include "FairyGUIMacros.h"
#include "cocos2d.h"
#include "EventContext.h"
#include "UIEventType.h"

NS_FGUI_BEGIN

typedef std::function<void(EventContext* context)> EventCallback;

class EventTag
{
public:
    static const EventTag None;

    EventTag();
    explicit EventTag(void* ptr);
    explicit EventTag(int value);
    explicit EventTag(const EventTag& other);
    explicit EventTag(EventTag&& other);
    ~EventTag();

    EventTag& operator= (const EventTag& other);
    EventTag& operator= (EventTag&& other);
    EventTag& operator= (void* ptr);
    EventTag& operator= (int v);

    bool operator!= (const EventTag& v);
    bool operator!= (const EventTag& v) const;
    bool operator== (const EventTag& v);
    bool operator== (const EventTag& v) const;

    bool isNone() const { return _value == 0; }

private:
    uintptr_t _value;
};

class InputProcessor;

class UIEventDispatcher : public cocos2d::Ref
{
public:
    UIEventDispatcher();
    virtual ~UIEventDispatcher();

    void addEventListener(UIEventType eventType, const EventCallback& callback) { return addEventListener(eventType, callback, EventTag::None); }
    void addEventListener(UIEventType eventType, const EventCallback& callback, const EventTag& tag);
    void removeEventListener(UIEventType eventType) { removeEventListener(eventType, EventTag::None); }
    void removeEventListener(UIEventType eventType, const EventTag& tag);
    void removeEventListeners();
    bool hasEventListener(UIEventType eventType) const { return hasEventListener(eventType, EventTag::None); }
    bool hasEventListener(UIEventType eventType, const EventTag& tag) const;

    bool dispatchEvent(UIEventType eventType, void* data = nullptr, const cocos2d::Value& dataValue = cocos2d::Value::Null);
    bool bubbleEvent(UIEventType eventType, void* data = nullptr, const cocos2d::Value& dataValue = cocos2d::Value::Null);

    bool isDispatchingEvent(UIEventType eventType);

private:
    void doDispatch(UIEventType eventType, EventContext* context);
    void doBubble(UIEventType eventType, EventContext* context);

    struct EventCallbackItem
    {
        EventCallback callback;
        UIEventType eventType;
        EventTag tag;
        int dispatching;
    };
    std::vector<EventCallbackItem*> _callbacks;
    int _dispatching;
};

NS_FGUI_END

#endif
