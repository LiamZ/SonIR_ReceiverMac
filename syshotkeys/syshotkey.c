//
//  File.c
//  SonIR
//
//  Created by LiamZ on 18/05/12.
//

#include "syshotkey.h"
#include "../CoreEvents.h"
#include "../HID_Event/HID_Event.h"
#import <IOKit/hidsystem/ev_keymap.h>

void execSysFunction(long type){
    // comment : unfinished work here
    switch (type) {
        case sysMissionControl:
            //system("open '/Applications/Mission Control.app'");
            EmitKeyEventFlagMode(kVK_missionControl, true, (CGEventFlags)-1);
            EmitKeyEventFlagMode(kVK_missionControl, false, (CGEventFlags)-1);
            break;
        case sysDashboard:
            EmitKeyEventFlagMode(kVK_dashboard, true, (CGEventFlags)-1);
            EmitKeyEventFlagMode(kVK_dashboard, false, (CGEventFlags)-1);
            break;
        case sysLaunchPad: system("open '/Applications/Launchpad.app'");
            break;
        case sysMediaPlay: HIDPostAuxKey(NX_KEYTYPE_PLAY);
            break;
        case sysMediaPrev: HIDPostAuxKey(NX_KEYTYPE_REWIND);
            break;
        case sysMediaNext: HIDPostAuxKey(NX_KEYTYPE_FAST);
            break;
        case sysMediaVolUp: HIDPostAuxKey(NX_KEYTYPE_SOUND_UP);
            break;
        case sysMediaVolDown: HIDPostAuxKey(NX_KEYTYPE_SOUND_DOWN);
            break;
        case sysMediaVolMute: HIDPostAuxKey(NX_KEYTYPE_MUTE);
            break;
            
        default:
            break;
    }
}