//
//  HID_Event.h
//  SonIR
//
//  Created by LiamZ on 19/04/12.
//

#ifndef SonIR_HID_Event_h
#define SonIR_HID_Event_h

//#include <stdlib.h>

#include <Carbon/Carbon.h>
#import <IOKit/hidsystem/ev_keymap.h>
#import <IOKit/hidsystem/IOHIDLib.h>

io_connect_t get_event_driver(void);

void HIDPostVirtualKey(const UInt8 virtualKeyCode, const Boolean postUp );

void HIDPressVirtualKey(const UInt8 virtualKeyCode );

void HIDReleaseVirtualKey(const UInt8 virtualKeyCode );

void HIDPostSpecialKey(const UInt8 specialKeyCode, const Boolean postUp);

void HIDPressSpecialKey(const UInt8 specialKeyCode );

void HIDReleaseSpecialKey(const UInt8 specialKeyCode );

void HIDPostSysDefinedKey(const UInt8 sysKeyCode );

void HIDPostAuxKey(const UInt8 auxKeyCode );

#endif
