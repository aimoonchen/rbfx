/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 
http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#include "2d/CCActionInterval.h"

#include <stdarg.h>

#include "2d/CCSprite.h"
#include "2d/CCNode.h"
#include "2d/CCSpriteFrame.h"
//#include "2d/CCActionInstant.h"
#include "base/CCDirector.h"
#include "base/CCEventCustom.h"
#include "base/CCEventDispatcher.h"
#include "platform/CCStdC.h"
//#include "base/CCScriptSupport.h"

NS_CC_BEGIN

// Extra action for making a Sequence or Spawn when only adding one action to it.
class ExtraAction : public FiniteTimeAction
{
public:
    static ExtraAction* create();
    virtual ExtraAction* clone() const;
    virtual ExtraAction* reverse() const;
    virtual void update(float time);
    virtual void step(float dt);
};

ExtraAction* ExtraAction::create()
{
    ExtraAction* ret = new (std::nothrow) ExtraAction();
    if (ret)
    {
        ret->autorelease();
    }
    return ret;
}

ExtraAction* ExtraAction::clone() const
{
    // no copy constructor
    return ExtraAction::create();
}

ExtraAction* ExtraAction::reverse() const
{
    return ExtraAction::create();
}

void ExtraAction::update(float /*time*/)
{
}

void ExtraAction::step(float /*dt*/)
{
}

//
// IntervalAction
//

bool ActionInterval::initWithDuration(float d)
{
    _duration = d;

    _elapsed = 0;
    _firstTick = true;
    _done = false;
    
    return true;
}

bool ActionInterval::sendUpdateEventToScript(float dt, Action *actionObject)
{
#if CC_ENABLE_SCRIPT_BINDING
    if (_scriptType == kScriptTypeJavascript)
    {
        if (ScriptEngineManager::sendActionEventToJS(actionObject, kActionUpdate, (void *)&dt))
            return true;
    }
#endif
    return false;
}

bool ActionInterval::isDone() const
{
    return _done;
}

void ActionInterval::step(float dt)
{
    if (_firstTick)
    {
        _firstTick = false;
        _elapsed = 0;
    }
    else
    {
        _elapsed += dt;
    }
    
    
    float updateDt = std::max(0.0f,                                  // needed for rewind. elapsed could be negative
                              std::min(1.0f, _elapsed / _duration)
                              );

    if (sendUpdateEventToScript(updateDt, this)) return;
    
    this->update(updateDt);

    _done = _elapsed >= _duration;
}

void ActionInterval::setAmplitudeRate(float /*amp*/)
{
    // Abstract class needs implementation
    CCASSERT(0, "Subclass should implement this method!");
}

float ActionInterval::getAmplitudeRate()
{
    // Abstract class needs implementation
    CCASSERT(0, "Subclass should implement this method!");

    return 0;
}

void ActionInterval::startWithTarget(Node *target)
{
    FiniteTimeAction::startWithTarget(target);
    _elapsed = 0.0f;
    _firstTick = true;
    _done = false;
}

NS_CC_END
