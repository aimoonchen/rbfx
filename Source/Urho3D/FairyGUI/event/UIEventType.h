#ifndef __UIEVENTTYPE_H__
#define __UIEVENTTYPE_H__

#include "FairyGUIMacros.h"

NS_FGUI_BEGIN
enum class UIEventType
{
    Enter = 0,
    Exit = 1,
    Changed = 2,
    Submit = 3,

    TouchBegin = 10,
    TouchMove = 11,
    TouchEnd = 12,
    Click = 13,
    RollOver = 14,
    RollOut = 15,
    MouseWheel = 16,
    RightClick = 17,
    MiddleClick = 18,

    PositionChange = 20,
    SizeChange = 21,

    KeyDown = 30,
    KeyUp = 31,

    Scroll = 40,
    ScrollEnd = 41,
    PullDownRelease = 42,
    PullUpRelease = 43,

    ClickItem = 50,
    ClickLink = 51,
    ClickMenu = 52,
    RightClickItem = 53,

    DragStart = 60,
    DragMove = 61,
    DragEnd = 62,
    Drop = 63,

    GearStop = 70,

    MOVE = 100,
    END = 101,
    None = 255,
};
// class UIEventType
// {
// public:
//     static const int Enter = 0;
//     static const int Exit = 1;
//     static const int Changed = 2;
//     static const int Submit = 3;
// 
//     static const int TouchBegin = 10;
//     static const int TouchMove = 11;
//     static const int TouchEnd = 12;
//     static const int Click = 13;
//     static const int RollOver = 14;
//     static const int RollOut = 15;
//     static const int MouseWheel = 16;
//     static const int RightClick = 17;
//     static const int MiddleClick = 18;
// 
//     static const int PositionChange = 20;
//     static const int SizeChange = 21;
// 
//     static const int KeyDown = 30;
//     static const int KeyUp = 31;
// 
//     static const int Scroll = 40;
//     static const int ScrollEnd = 41;
//     static const int PullDownRelease = 42;
//     static const int PullUpRelease = 43;
// 
//     static const int ClickItem = 50;
//     static const int ClickLink = 51;
//     static const int ClickMenu = 52;
//     static const int RightClickItem = 53;
// 
//     static const int DragStart = 60;
//     static const int DragMove = 61;
//     static const int DragEnd = 62;
//     static const int Drop = 63;
// 
//     static const int GearStop = 70;
// };

NS_FGUI_END

#endif
