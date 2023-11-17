/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2010-2012 cocos2d-x.org
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

#ifndef __ACTION_CCINTERVAL_ACTION_H__
#define __ACTION_CCINTERVAL_ACTION_H__

#include <vector>

#include "2d/CCAction.h"
#include "2d/CCAnimation.h"
#include "base/CCProtocols.h"
#include "base/CCVector.h"

NS_CC_BEGIN

class Node;
class SpriteFrame;
class EventCustom;

/**
 * @addtogroup actions
 * @{
 */

/** @class ActionInterval
@brief An interval action is an action that takes place within a certain period of time.
It has an start time, and a finish time. The finish time is the parameter
duration plus the start time.

These ActionInterval actions have some interesting properties, like:
- They can run normally (default)
- They can run reversed with the reverse method
- They can run with the time altered with the Accelerate, AccelDeccel and Speed actions.

For example, you can simulate a Ping Pong effect running the action normally and
then running it again in Reverse mode.

Example:

@code
auto action = MoveBy::create(1.0f, Vec2::ONE);
auto pingPongAction = Sequence::create(action, action->reverse(), nullptr);
@endcode
*/
class CC_DLL ActionInterval : public FiniteTimeAction
{
public:
    /** How many seconds had elapsed since the actions started to run.
     *
     * @return The seconds had elapsed since the actions started to run.
     */
    float getElapsed() { return _elapsed; }

    /** Sets the amplitude rate, extension in GridAction
     *
     * @param amp   The amplitude rate.
     */
    void setAmplitudeRate(float amp);
    
    /** Gets the amplitude rate, extension in GridAction
     *
     * @return  The amplitude rate.
     */
    float getAmplitudeRate();

    //
    // Overrides
    //
    virtual bool isDone() const override;
    /**
     * @param dt in seconds
     */
    virtual void step(float dt) override;
    virtual void startWithTarget(Node *target) override;
    virtual ActionInterval* reverse() const override
    {
        CC_ASSERT(0);
        return nullptr;
    }

    virtual ActionInterval *clone() const override
    {
        CC_ASSERT(0);
        return nullptr;
    }

CC_CONSTRUCTOR_ACCESS:
    /** initializes the action */
    bool initWithDuration(float d);

protected:
    float _elapsed;
    bool _firstTick;
    bool _done;
    
protected:
    bool sendUpdateEventToScript(float dt, Action *actionObject);
};

NS_CC_END

#endif //__ACTION_CCINTERVAL_ACTION_H__
