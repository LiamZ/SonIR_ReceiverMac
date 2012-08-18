//
//  HID_Event.c
//  SonIR
//
//  Created by LiamZ on 19/04/12.
//

#include "HID_Event.h"
//#import <Cocoa/Cocoa.h>
#import <IOKit/hidsystem/IOHIDLib.h>
//#import <IOKit/hidsystem/ev_keymap.h>

// from IM:Text, p. C-5
enum
{
    kVirtualCommandKey    = 0x37,
    kVirtualShiftKey    = 0x38,
    kVirtualCapsLockKey    = 0x39,
    kVirtualOptionKey    = 0x3A,
    kVirtualControlKey    = 0x3B
};

UInt32 gModifiers = 0;

io_connect_t get_event_driver(void)
{
    static  mach_port_t        sEventDrvrRef = 0;
    mach_port_t                masterPort, service, iter;
    kern_return_t            kr;
    
    if (!sEventDrvrRef) {
        // Get master device port
        kr = IOMasterPort( bootstrap_port, &masterPort );
        check( KERN_SUCCESS == kr);
        
        kr = IOServiceGetMatchingServices( masterPort,
                                          IOServiceMatching(kIOHIDSystemClass), &iter );
        check( KERN_SUCCESS == kr);
        
        service = IOIteratorNext( iter );
        check( service );
        
        kr = IOServiceOpen( service, mach_task_self(),
                           kIOHIDParamConnectType, &sEventDrvrRef );
        check( KERN_SUCCESS == kr );
        
        IOObjectRelease( service );
        IOObjectRelease( iter );
    }
    
    return sEventDrvrRef;
}

void HIDPostVirtualKey(const UInt8 virtualKeyCode, const Boolean
                              postUp )
{
    NXEventData        event;
    kern_return_t    kr;
    IOGPoint        loc = { 0, 0 };
    UInt32 modifiers = 0;
    
    bzero(&event, sizeof(NXEventData));
    
    switch ( virtualKeyCode )
    {
        case kVirtualShiftKey:
        case kVirtualCommandKey:
        case kVirtualOptionKey:
        case kVirtualControlKey:
        case kVirtualCapsLockKey:
            switch ( virtualKeyCode )
        {
            case kVirtualShiftKey: modifiers = NX_SHIFTMASK;
                break;
            case kVirtualCommandKey: modifiers = NX_COMMANDMASK;
                break;
            case kVirtualOptionKey: modifiers = NX_ALTERNATEMASK;
                break;
            case kVirtualControlKey: modifiers = NX_CONTROLMASK;
                break;
            case kVirtualCapsLockKey: modifiers = NX_ALPHASHIFTMASK;
                break;
        }
            if (postUp)
                gModifiers &= ~modifiers;
            else
                gModifiers |= modifiers;
            
            kr = IOHIDPostEvent( get_event_driver(), NX_FLAGSCHANGED, loc,
                                &event, kNXEventDataVersion, gModifiers, TRUE );
            check( KERN_SUCCESS == kr );
            break;
        default:
            event.key.repeat = FALSE;
            event.key.keyCode = virtualKeyCode;
            event.key.origCharSet = event.key.charSet = NX_ASCIISET;
            // ?!
            event.key.origCharCode = event.key.charCode = 0;
            kr = IOHIDPostEvent( get_event_driver(), postUp ? NX_KEYUP :
                                NX_KEYDOWN, loc, &event, kNXEventDataVersion, 0, FALSE );
            check( KERN_SUCCESS == kr );
            break;
    }
}

void HIDPressVirtualKey(const UInt8 virtualKeyCode )
{
    HIDPostVirtualKey(virtualKeyCode, FALSE);
}

void HIDReleaseVirtualKey(const UInt8 virtualKeyCode )
{
    HIDPostVirtualKey(virtualKeyCode, TRUE);
}

void HIDPostSpecialKey(const UInt8 specialKeyCode, const Boolean
                              postUp)
{
    NXEventData        event;
    kern_return_t    kr;
    IOGPoint        loc = { 0, 0 };
    
    bzero(&event, sizeof(NXEventData));
    
    event.key.repeat = FALSE;
    event.key.keyCode = specialKeyCode;
    event.key.charSet = NX_NOSPECIALKEY;
    event.key.charCode = 0;
    event.key.origCharSet = event.key.charSet;
    event.key.origCharCode = event.key.charCode;
    kr = IOHIDPostEvent( get_event_driver(), postUp ? NX_KEYUP : NX_KEYDOWN,
                        loc, &event, kNXEventDataVersion, 0, FALSE );
    check( KERN_SUCCESS == kr );
}

void HIDPressSpecialKey(const UInt8 specialKeyCode )
{
    HIDPostSpecialKey(specialKeyCode, FALSE);
}

void HIDReleaseSpecialKey(const UInt8 specialKeyCode )
{
    HIDPostSpecialKey(specialKeyCode, TRUE);
}

void HIDPostSysDefinedKey(const UInt8 sysKeyCode )
{
    NXEventData        event;
    kern_return_t    kr;
    IOGPoint        loc = { 0, 0 };
    
    bzero(&event, sizeof(NXEventData));
    
    event.compound.subType = sysKeyCode;
    kr = IOHIDPostEvent( get_event_driver(), NX_SYSDEFINED, loc, &event,
                        kNXEventDataVersion, 0, FALSE );
    check( KERN_SUCCESS == kr );
}

// separate keyup and keydown?
void HIDPostAuxKey(const UInt8 auxKeyCode )
{
    NXEventData        event;
    kern_return_t    kr;
    IOGPoint        loc = { 0, 0 };
    
    UInt32      evtInfo = auxKeyCode << 16 | NX_KEYDOWN << 8;
    
    bzero(&event, sizeof(NXEventData));
    
    event.compound.subType = NX_SUBTYPE_AUX_CONTROL_BUTTONS;
    event.compound.misc.L[0] = evtInfo;
    //event.compound.misc.S[0] = auxKeyCode;
    //event.compound.misc.C[2] = NX_KEYDOWN;
    
    
    kr = IOHIDPostEvent( get_event_driver(), NX_SYSDEFINED, loc, &event,
                        kNXEventDataVersion, 0, FALSE );
    check( KERN_SUCCESS == kr );
    
    evtInfo = auxKeyCode << 16 | NX_KEYUP << 8;
    bzero(&event, sizeof(NXEventData));
    event.compound.subType = NX_SUBTYPE_AUX_CONTROL_BUTTONS;
    event.compound.misc.L[0] = evtInfo;
    //event.compound.misc.S[0] = auxKeyCode;
    //event.compound.misc.C[2] = NX_KEYUP;
    
    kr = IOHIDPostEvent( get_event_driver(), NX_SYSDEFINED, loc, &event,
                        kNXEventDataVersion, 0, FALSE );
    check( KERN_SUCCESS == kr );
}
