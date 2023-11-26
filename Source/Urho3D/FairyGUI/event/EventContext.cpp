#include "EventContext.h"
#include "GObject.h"

NS_FGUI_BEGIN

EventContext::EventContext() :
    _sender(nullptr),
    _data(nullptr),
    _inputEvent(nullptr),
    _isStopped(false),
    _defaultPrevented(false),
    _touchCapture(0),
    _type(UIEventType::None)
{
}

EventContext::~EventContext()
{

}


NS_FGUI_END
