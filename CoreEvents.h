//
//  CoreEvents.h
//  SonIR
//
//  Created by LiamZ 30/05/12.
//

#ifndef SonIR_CoreEvents_h
#define SonIR_CoreEvents_h

#include <CoreFoundation/CoreFoundation.h>
#include <Carbon/Carbon.h>

long inputMode; // global variable, don't know if it's clean

enum{ // input modes
    CUSTOM_Default = 0,
    CUSTOM_AppSwitch = 1, // Application switching mode (cmd-tab)
    CUSTOM_UniversalAccess = 2 // FFU (-_-)
};

void enterMode(long mode);

void exitMode(long mode);

void EmitKeyEvent(const long keycode, const bool press);

void EmitKeyEventFlag(const long keycode, const bool press, const CGEventFlags flags);

void EmitKeyEventFlagMode(const long keycode, const bool press, const CGEventFlags flags);

char* CGKeyEvent_toString(CGEventRef const event);

#endif
