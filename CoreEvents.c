//
//  CoreEvents.c
//  SonIR
//
//  Created by LiamZ on 30/05/12.
//

#include <stdio.h>
#include "CoreEvents.h"

/*!
 @function EmitKeyEventFlag
 Sends a keyboard event
 @param keycode the keyboard key code
 @param press indicates the type of event (key press or release)
 @param flags event modifiers
 */
void EmitKeyEventFlag(const long keycode, const bool press, const CGEventFlags flags){
    //printf("EmitKeyEventFlag : \n");
    if(keycode != (CGKeyCode)-1){
        CGEventSourceRef source = CGEventSourceCreate(kCGEventSourceStateHIDSystemState);
        CGEventRef keyEvent = CGEventCreateKeyboardEvent (source, keycode, press);
        if(flags != (CGEventFlags)-1) CGEventSetFlags(keyEvent, flags);
        CGKeyEvent_toString(keyEvent);
        CGEventPost(kCGSessionEventTap, keyEvent);
        CFRelease(keyEvent);
    }
}

void EmitKeyEventFlagMode(const long keycode, const bool press, const CGEventFlags flags){
    if(keycode != (CGKeyCode)-1){
        if (inputMode == CUSTOM_AppSwitch) {
            switch (keycode) {
                case 12: // the Q key on an ANSI layout
                case kVK_LeftArrow:
                case kVK_RightArrow:
                    if(flags == (CGEventFlags)-1){
                        CGEventFlags flags_add = kCGEventFlagMaskCommand;
                        EmitKeyEventFlag(keycode, press, flags_add);
                    } else {
                        exitMode(CUSTOM_AppSwitch);
                        EmitKeyEventFlag(keycode, press, flags);
                    }
                    break;
                case kVK_Return:
                    if (press) {
                        if(flags == (CGEventFlags)-1){
                            CGEventFlags flags_add = kCGEventFlagMaskCommand;
                            EmitKeyEventFlag(keycode, press, flags_add);
                        } else EmitKeyEventFlag(keycode, press, flags);
                        inputMode = CUSTOM_Default;
                    }
                    
                default:
                    // escape
                    exitMode(CUSTOM_AppSwitch);
                    EmitKeyEventFlag(keycode, press, flags);
                    break;
            }
            
        } else{
            EmitKeyEventFlag(keycode, press, flags);
        }
    }
}

/*!
 @function EmitKeyEvent
 Sends a keyboard event
 @param keycode the event key code
 @param press indicates the type of event (key press or release)
 */
void EmitKeyEvent(const long keycode, const bool press){
    EmitKeyEventFlagMode(keycode, press, -1);
}

char* CGKeyEvent_toString(CGEventRef const event){
    CGEventFlags flags;
    char* unicode = malloc(100 * sizeof(char));
    unicode[0] = '\0';
    
    flags = CGEventGetFlags(event);
    if(flags & kCGEventFlagMaskControl)
        strncat(unicode, "⌃", 100);
    if(flags & kCGEventFlagMaskAlternate)
        strncat(unicode, "⌥", 100);
    if(flags & kCGEventFlagMaskShift)
        strncat(unicode, "⇧", 100);
    if(flags & kCGEventFlagMaskCommand)
        strncat(unicode, "⌘", 100);
    
    return unicode;
}