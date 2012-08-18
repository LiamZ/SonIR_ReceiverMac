//
//  main.c
//  SonIR
//
//  Created by LiamZ on 16/04/12.
//

//*****************************************************
#pragma mark - includes & imports
//-----------------------------------------------------

#include <stdlib.h>
#include <stdio.h>

//#import <Cocoa/Cocoa.h>
#include <Carbon/Carbon.h>
#include <IOKit/hid/IOHIDKeys.h>
//#import <IOKit/hidsystem/IOHIDLib.h>
#import <IOKit/hidsystem/ev_keymap.h>

#include <ApplicationServices/ApplicationServices.h>

#include "HID_Utilities/HID_Utilities.h"
#include "HID_Event/HID_Event.h"
#import "CoreEvents.h"
#import "syshotkeys/syshotkey.h"

/*!
 @typedef sonyKeys
 @abstract the key codes
 @constant skeyStandby standby key code
 @constant skeyMute mute key code
 These are the key codes recieved in HID report messages.
 */
enum {
    skeyStandby	 = 44,
    skeyMute = 20
};
typedef int sonyKeys;

typedef struct assocArray
{ // associative array of strings
    long size;
	void ** keys;
	void ** values;
}assocArray;


typedef struct CUSTOM_Device
{
	// device
	long vendorID;
	long productID;
    uint32_t usagePage;
    uint32_t usage;
}CUSTOM_Device;

const CUSTOM_Device Sony_IR = {1356, 212, 256, 0};
const char * config_path = "/Library/Preferences/SonIR.Config.plist";

typedef struct CUSTOM_Config{
	CFDictionaryRef configRoot; // root
    CFDictionaryRef keycodes_KB; // keyboard key code
    CFDictionaryRef Mapping; // mappings
    assocArray code_IR; // IR remote control key code
}CUSTOM_Config;

// config
static CUSTOM_Config configStruct;

static assocArray keyHash;

/* loglevel indicates verbosity level
 level 0 : only critical errors
 level 1 : adds uncritical errors
 level 2+ : adds warning (with varying level)
 */
static int loglevel = 0;

//static OSStatus Handle_New( IOHIDDeviceRef inIOHIDDeviceRef );

void MyLog(const int level, const char* message);

const char* getKeyForValue(assocArray array, char* value){
    long i;
    const char* val;
    for(i = 0; i < array.size; i++){
        val = CFStringGetCStringPtr(array.values[i], CFStringGetSystemEncoding());
        if(strcmp(val, value) == 0){
            val = CFStringGetCStringPtr(array.keys[i], CFStringGetSystemEncoding());
            return val;
        }
    }
    return NULL;
}

assocArray CreateKeyHash(){
    int size = 52;
    char** paramKeys = malloc(size * sizeof(char*));
    int* paramValues = malloc(size * sizeof(int));
    paramKeys[0]  = "F1" ; paramValues[0]  = kVK_F1 ;
    paramKeys[1]  = "F2" ; paramValues[1]  = kVK_F2 ;
    paramKeys[2]  = "F3" ; paramValues[2]  = kVK_F3 ;
    paramKeys[3]  = "F4" ; paramValues[3]  = kVK_F4 ;
    paramKeys[4]  = "F5" ; paramValues[4]  = kVK_F5 ;
    paramKeys[5]  = "F6" ; paramValues[5]  = kVK_F6 ;
    paramKeys[6]  = "F7" ; paramValues[6]  = kVK_F7 ;
    paramKeys[7]  = "F8" ; paramValues[7]  = kVK_F8 ;
    paramKeys[8]  = "F9" ; paramValues[8]  = kVK_F9 ;
    paramKeys[9]  = "F10"; paramValues[9]  = kVK_F10;
    paramKeys[10] = "F11"; paramValues[10] = kVK_F11;
    paramKeys[11] = "F12"; paramValues[11] = kVK_F12;
    paramKeys[12] = "F13"; paramValues[12] = kVK_F13;
    paramKeys[13] = "F14"; paramValues[13] = kVK_F14;
    paramKeys[14] = "F15"; paramValues[14] = kVK_F15;
    paramKeys[15] = "F16"; paramValues[15] = kVK_F16;
    paramKeys[16] = "F17"; paramValues[16] = kVK_F17;
    paramKeys[17] = "F18"; paramValues[17] = kVK_F18;
    paramKeys[18] = "F19"; paramValues[18] = kVK_F19;
    paramKeys[19] = "F20"; paramValues[19] = kVK_F20;
    
    paramKeys[20] = "Escape"; paramValues[20] = kVK_Escape;
    paramKeys[21] = "Tab"; paramValues[21] = kVK_Tab;
    paramKeys[22] = "Space"; paramValues[22] = kVK_Space;
    paramKeys[23] = "Backspace"; paramValues[23] = kVK_Delete;
    paramKeys[24] = "Enter"; paramValues[24] = kVK_Return;
    
    paramKeys[25] = "Insert"; paramValues[25] = kVK_Help;
    paramKeys[26] = "Suppr"; paramValues[26] = kVK_ForwardDelete;
    paramKeys[27] = "Home"; paramValues[27] = kVK_Home;
    paramKeys[28] = "End"; paramValues[28] = kVK_End;
    paramKeys[29] = "Page Up"; paramValues[29] = kVK_PageUp;
    paramKeys[30] = "Page Down"; paramValues[30] = kVK_PageDown;
    
    paramKeys[31] = "Arrow Up"; paramValues[31] = kVK_UpArrow;
    paramKeys[32] = "Arrow Down"; paramValues[32] = kVK_DownArrow;
    paramKeys[33] = "Arrow Left"; paramValues[33] = kVK_LeftArrow;
    paramKeys[34] = "Arrow Right"; paramValues[34] = kVK_RightArrow;
    
    // ANSI :
    paramKeys[35] = "Numpad Clear"; paramValues[35] = kVK_ANSI_KeypadClear;
    paramKeys[36] = "Numpad /"; paramValues[36] = kVK_ANSI_KeypadDivide;
    paramKeys[37] = "Numpad *"; paramValues[37] = kVK_ANSI_KeypadMultiply;
    paramKeys[38] = "Numpad -"; paramValues[38] = kVK_ANSI_KeypadMinus;
    paramKeys[39] = "Numpad +"; paramValues[39] = kVK_ANSI_KeypadPlus;
    paramKeys[40] = "Numpad 0"; paramValues[40] = kVK_ANSI_Keypad0;
    paramKeys[41] = "Numpad 1"; paramValues[41] = kVK_ANSI_Keypad1;
    paramKeys[42] = "Numpad 2"; paramValues[42] = kVK_ANSI_Keypad2;
    paramKeys[43] = "Numpad 3"; paramValues[43] = kVK_ANSI_Keypad3;
    paramKeys[44] = "Numpad 4"; paramValues[44] = kVK_ANSI_Keypad4;
    paramKeys[45] = "Numpad 5"; paramValues[45] = kVK_ANSI_Keypad5;
    paramKeys[46] = "Numpad 6"; paramValues[46] = kVK_ANSI_Keypad6;
    paramKeys[47] = "Numpad 7"; paramValues[47] = kVK_ANSI_Keypad7;
    paramKeys[48] = "Numpad 8"; paramValues[48] = kVK_ANSI_Keypad8;
    paramKeys[49] = "Numpad 9"; paramValues[49] = kVK_ANSI_Keypad9;
    paramKeys[50] = "Numpad Enter"; paramValues[50] = kVK_ANSI_KeypadDecimal;
    paramKeys[51] = "Numpad Decimal"; paramValues[51] = kVK_ANSI_KeypadEnter;
    
    /*
    kVK_Command                   = 0x37,
    kVK_Shift                     = 0x38,
    kVK_CapsLock                  = 0x39,
    kVK_Option                    = 0x3A,
    kVK_Control                   = 0x3B,
    
    kVK_RightShift                = 0x3C,
    kVK_RightOption               = 0x3D,
    kVK_RightControl              = 0x3E,
    kVK_Function                  = 0x3F,
    kVK_VolumeUp                  = 0x48,
    kVK_VolumeDown                = 0x49,
    kVK_Mute                      = 0x4A,
    */
    
    assocArray keycodes;
    keycodes.size = size;
    keycodes.keys = (void **)paramKeys;
    keycodes.values = (void **)paramValues;
    return keycodes;
}

bool seek_str(const char * src, const char * str){
    int len = strlen(str);
    const char * pos = src;
    const char * tmp;
    while (strlen(pos)>0) {
        tmp = strchr(pos, str[0]);
        if(tmp != NULL){
            if(strncmp(tmp, str, len) == 0)
                return true;
            else
                pos = tmp + 1;
        } else
            return false;
    }
    
    return false;
}

CGEventFlags readModifiers(const char* str){
    CGEventFlags flags = 0;
    
    if(seek_str(str, "Shift") || seek_str(str, "⇧"))
        flags |= kCGEventFlagMaskShift;
    if(seek_str(str, "Ctrl") || seek_str(str, "Control") || seek_str(str, "⌃") || seek_str(str, "^"))
        flags |= kCGEventFlagMaskControl;
    if(seek_str(str, "Alt") || seek_str(str, "Option") || seek_str(str, "⌥"))
        flags |= kCGEventFlagMaskAlternate;
    if(seek_str(str, "Cmd") || seek_str(str, "Command") || seek_str(str, "⌘"))
        flags |= kCGEventFlagMaskCommand;
    
    return flags;
}

int search_Assoc(const assocArray hash, const char* key){
	int i;
    for(i = 0; i < hash.size; i++)
        if(strcmp(hash.keys[i], key) == 0)
            return i;
    
    // if there's no match
    return -1;
}

// required to compile, but I'm sure it's ugly and wrong
extern void CFSetApplierFunctionCopyToCFArray(const void *value, void *context);

//static void Handle_DeviceMatchingCallback( void * inContext, IOReturn inResult, void * inSender, IOHIDDeviceRef inIOHIDDeviceRef );
//static void Handle_IOHIDValueCallback( void * inContext, IOReturn inResult, void * inSender, IOHIDValueRef inIOHIDValueRef );

/*static void Handle_DeviceMatchingCallback( void * inContext, IOReturn inResult, void * inSender, IOHIDDeviceRef inIOHIDDeviceRef )
{
#pragma unused (  inContext, inSender )
//	printf("Handle_DeviceMatchingCallback\n");
//	printf( "%s(context: %p, result: %p, sender: %p, device: %p).\n", __PRETTY_FUNCTION__, inContext, ( void* )  inResult, inSender,
//		   ( void* ) inIOHIDDeviceRef );
	HIDDumpDeviceInfo( inIOHIDDeviceRef );
	
	//bzHandle_New( inIOHIDDeviceRef );
	
	IOHIDDeviceRegisterInputValueCallback( inIOHIDDeviceRef, Handle_IOHIDValueCallback, ( void * ) inIOHIDDeviceRef );
	
	inResult = kIOReturnSuccess;
}   // Handle_DeviceMatchingCallback
*/

/*static void Handle_IOHIDValueCallback( void * inContext, IOReturn inResult, void * inSender, IOHIDValueRef inIOHIDValueRef )
{
#pragma unused( inContext, inSender )
	IOHIDDeviceRef tIOHIDDeviceRef = ( IOHIDDeviceRef ) inContext;
	IOHIDElementRef tIOHIDElementRef = IOHIDValueGetElement( inIOHIDValueRef );
    //printf( "%s(context: %p, result: %p, sender: %p, Value: %p).\n", "Handle_IOHIDValueCallback", inContext, ( void* )  inResult, inSender, ( void* ) inIOHIDValueRef );
    
	do {
		// valid device & element?
		if ( !( tIOHIDDeviceRef && tIOHIDElementRef ) ) {
			break;  // (no)
		}
        HIDDumpElementInfo( tIOHIDElementRef );
        
		CFIndex length = IOHIDValueGetLength(inIOHIDValueRef);
		if (length > sizeof(double_t)) {
			break;
		}
        
        
		CFIndex value = IOHIDValueGetIntegerValue( inIOHIDValueRef );
		
		// new min value?
		CFIndex minValue = IOHIDElement_GetCalibrationSaturationMin( tIOHIDElementRef );
		
		if ( value < minValue ) {           // (yes)
			IOHIDElement_SetCalibrationSaturationMin( tIOHIDElementRef, value );
		}
		
		// new max value?
		CFIndex maxValue = IOHIDElement_GetCalibrationSaturationMax( tIOHIDElementRef );
		
		if ( value > maxValue ) {           // (yes)
			IOHIDElement_SetCalibrationSaturationMax( tIOHIDElementRef, value );
		}
		
        
        printf("value : %ld , min : %ld , max : %ld\n", value, minValue, maxValue);
        
	} while ( FALSE );
	inResult = kIOReturnSuccess;
}   // Handle_IOHIDValueCallback
*/

/*
 Returns a string representing the currently focused app.
 */
char* appfocusname(){
    ProcessSerialNumber psn = { 0L, 0L };
    OSStatus err = GetFrontProcess(&psn);
    
    char* result;
    
    CFStringRef processName = NULL;
    err = CopyProcessName(&psn, &processName);
    
    CFIndex longueur = CFStringGetMaximumSizeForEncoding(CFStringGetLength(processName), CFStringGetSystemEncoding());
    
    result = malloc((longueur+1) * sizeof(char));
    CFStringGetCString(processName, result, longueur+1, CFStringGetSystemEncoding());
    //result = CFStringGetCStringPtr(processName, CFStringGetSystemEncoding());
    CFRelease(processName);
    
    return result;
}

void MyLog(const int level, const char* message){
    if(loglevel >= level)
        fprintf(stderr, "%s\n", message);
}

// TODO : improve modes management, and add it to the config file

void enterMode(long mode){
    switch (mode) {
        case CUSTOM_AppSwitch:
            MyLog(3, "Entering the AppSwitch mode\n");
            EmitKeyEventFlag((CGKeyCode)kVK_Tab, TRUE, kCGEventFlagMaskCommand);
            EmitKeyEventFlag((CGKeyCode)kVK_Tab, FALSE, kCGEventFlagMaskCommand);
            EmitKeyEventFlag((CGKeyCode)kVK_LeftArrow, TRUE, kCGEventFlagMaskCommand);
            EmitKeyEventFlag((CGKeyCode)kVK_LeftArrow, FALSE, kCGEventFlagMaskCommand);
            
            inputMode = mode;
            break;
            
        default:
            break;
    }
}

void exitMode(long mode){
    switch (mode) {
        case CUSTOM_AppSwitch:
            MyLog(3, "Exiting AppSwitch mode\n");
            EmitKeyEventFlag(kVK_Escape, TRUE, kCGEventFlagMaskCommand);
            EmitKeyEventFlag(kVK_Escape, FALSE, 0);
            inputMode = CUSTOM_Default;
            break;
            
        default:
            break;
    }
}


/*!
 @function executer_applescript_system
 Executer des commandes actionscript
 Deprecated
 @param command une commande actionscript
 */
void executer_applescript_system(const char* command){
    int n = strlen(command);
    char* osascript = malloc((n+16)*sizeof(char));
    // TODO : needs better string management and quotes sanitising
    snprintf(osascript, n+16, "osascript -e '%s'", command);
    system(osascript);
    free(osascript);
}

/*!
 @function executer_applescript
 Execute applescript commands
 @param command an applescript command
 */
void executer_applescript(const char * command) {
    pid_t pid;
    int   status;
    
    if ((pid = fork()) < 0) { // we fork a process
        printf("*** ERROR: forking child process failed\n");
        //exit(1);
    } else if (pid == 0) { // the child process
        if (execlp("osascript", "osascript", "-e", command, (char *)NULL) < 0) { // execute the command
            printf("*** ERROR: exec failed\n");
            //exit(1);
        }
    } else { // the parent
        while (wait(&status) != pid); // synchronous waiting
    }
}

assocArray DictToAssoc(CFDictionaryRef dict){
    assocArray result;
    CFIndex compte = CFDictionaryGetCount(dict);
    result.keys = malloc(compte * sizeof(CFTypeRef));
    result.values = malloc(compte * sizeof(CFTypeRef));
    result.size = compte;
    CFDictionaryGetKeysAndValues(dict, (const void **)result.keys, (const void **)result.values);
    return result;
}

/*!
 @function DoAction
 Based on clean remote control events
 reads configuration to know what action to perform
 @param code IR Remote control event code
 @param mode Current remote mode
 @param bool event type (true:pressed false:released)
 
 @result void
 */
void DoAction(const int code, const int mode, const bool pressee){
    CGKeyCode keycode = -1;
    CGEventFlags keyFlags = -1;
    
    CFDictionaryRef defautAction, modeAction, perApp;
    
    if(CFDictionaryContainsKey(configStruct.Mapping, CFSTR("Default")))
        defautAction = CFDictionaryGetValue(configStruct.Mapping, CFSTR("Default"));
    if(CFDictionaryContainsKey(configStruct.Mapping, CFSTR("Per App")))
        perApp = CFDictionaryGetValue(configStruct.Mapping, CFSTR("Per App"));
    
    char modeKey[10];
    sprintf(modeKey, "Mode %i", mode);
    CFStringRef modeKeyCFStr = CFStringCreateWithCString(kCFAllocatorDefault, modeKey, CFStringGetSystemEncoding());
    if(CFDictionaryContainsKey(configStruct.Mapping, modeKeyCFStr))
        modeAction = CFDictionaryGetValue(configStruct.Mapping, modeKeyCFStr);
    
    char* val = malloc(200 * sizeof(char));
    snprintf(val, 200, "%i", code);
    const char* keyname = getKeyForValue(configStruct.code_IR, val);
    free(val);
    
    CFStringRef codename = CFStringCreateWithCString(kCFAllocatorDefault, keyname, CFStringGetSystemEncoding());
    
    if(strlen(keyname) > 0)
        printf("key %s", keyname);
    else{
        printf("unknown key (%i)", code);
    }
    
    if(pressee) printf(" pressed\n");
    else printf(" released\n");
    
    CFDictionaryRef data = NULL; // configuration of the selected key
    // TODO : better handling on the config file
    char *appname = appfocusname(); // name of the currently focused app
    
    CFStringRef appNameStr = CFStringCreateWithCString(kCFAllocatorDefault, appname, CFStringGetSystemEncoding());
    free(appname);
    
    if(perApp && CFDictionaryContainsKey(perApp, appNameStr)){
        CFDictionaryRef appConfig = CFDictionaryGetValue(perApp, appNameStr);
        data = CFDictionaryGetValue(appConfig, codename);
    } // TODO : better handling of cases
    
    if ((!data) && modeAction && CFDictionaryContainsKey(modeAction, codename))
        data = CFDictionaryGetValue(modeAction, codename);
    if ((!data) && CFDictionaryContainsKey(defautAction, codename))
        data = CFDictionaryGetValue(defautAction, codename);
    
    // TODO : review the configuration model
    if(data){
        //TODO : add options (system function {volume, sleep, ...}, custom actions {mouse, cmd-tab})
        // menu item (name or number), mouse function (mousekeys)
        CFStringRef actionType = CFDictionaryGetValue(data, CFSTR("Type"));
        const char *actionTypeCstr = CFStringGetCStringPtr(actionType, CFStringGetSystemEncoding());
        if (strcmp(actionTypeCstr, "Keypress") == 0) {
            CFStringRef keycodeCSR = CFDictionaryGetValue(data, CFSTR("Action"));
            const char *keycodeCstr = CFStringGetCStringPtr(keycodeCSR, CFStringGetSystemEncoding());
            keycode = atoi(keycodeCstr);
        } else if (strcmp(actionTypeCstr, "Key") == 0) {
            // TODO : enhance configurability : modifier (symbols, names) (duration, repeat)?
            CFStringRef keycodeCSR = NULL; // key code
            CFStringRef keyName, keyModif;
            if(!CFDictionaryGetValueIfPresent(data, CFSTR("Action"), (const void **)&keyName)){ // explicit mapping?
                keyName = codename; // we try implicit mapping
            }
            
            if(pressee && CFDictionaryGetValueIfPresent(data, CFSTR("Modifiers"), (const void **)&keyModif)){ // explicit mapping?
                const char *keyModifCstr = CFStringGetCStringPtr(keyModif, CFStringGetSystemEncoding());
                keyFlags = readModifiers(keyModifCstr);
            }
            
            if(CFDictionaryGetValueIfPresent(configStruct.keycodes_KB, keyName, (const void **)&keycodeCSR)){
                const char *keycodeCstr = CFStringGetCStringPtr(keycodeCSR, CFStringGetSystemEncoding());
                keycode = atoi(keycodeCstr);
            } else {
                const char * keyNameCstr = CFStringGetCStringPtr(keyName, CFStringGetSystemEncoding());
                // we search in the static mapping
                int i = search_Assoc(keyHash, keyNameCstr);
                if(i>=0){
                    keycode = (CGKeyCode) ((int*)keyHash.values)[i];
                }
            }
        } else if (strcmp(actionTypeCstr, "Applescript") == 0) {
            CFStringRef actionstring = CFDictionaryGetValue(data, CFSTR("Action"));
            const char *actionstringCstr = CFStringGetCStringPtr(actionstring, CFStringGetSystemEncoding());
            if(pressee) executer_applescript(actionstringCstr);
        } else if (strcmp(actionTypeCstr, "Shell") == 0) {
            CFStringRef actionstring = CFDictionaryGetValue(data, CFSTR("Action"));
            const char *actionstringCstr = CFStringGetCStringPtr(actionstring, CFStringGetSystemEncoding());
            if(pressee) system(actionstringCstr);
        } else if (strcmp(actionTypeCstr, "Function") == 0) {
            keycode = -1; // to avoid sending a keyboard event at the end of the processing
            CFStringRef actionstring = CFDictionaryGetValue(data, CFSTR("Action"));
            const char *actionstringCstr = CFStringGetCStringPtr(actionstring, CFStringGetSystemEncoding());
            if(pressee) {
                if(strcmp(actionstringCstr, "Tab") == 0){
                    if(inputMode != CUSTOM_AppSwitch)
                        enterMode(CUSTOM_AppSwitch);
                    else{
                        exitMode(CUSTOM_AppSwitch);
                        printf("exiting AppSwitch\n");
                    }
                } else if(strcmp(actionstringCstr, "Media Play") == 0){
                    execSysFunction(sysMediaPlay);
                } else if(strcmp(actionstringCstr, "Media Next") == 0){
                    execSysFunction(sysMediaNext);
                } else if(strcmp(actionstringCstr, "Media Prev") == 0){
                    execSysFunction(sysMediaPrev);
                } else if(strcmp(actionstringCstr, "Volume Up") == 0){
                    execSysFunction(sysMediaVolUp);
                } else if(strcmp(actionstringCstr, "Volume Down") == 0){
                    execSysFunction(sysMediaVolDown);
                } else if(strcmp(actionstringCstr, "Volume Mute") == 0){
                    execSysFunction(sysMediaVolMute);
                } else if(strcmp(actionstringCstr, "Mission Control") == 0){
                    execSysFunction(sysMissionControl);
                } else if(strcmp(actionstringCstr, "Dashboard") == 0){
                    execSysFunction(sysDashboard);
                } else if(strcmp(actionstringCstr, "Launchpad") == 0){
                    execSysFunction(sysLaunchPad);
                }
            }
            
        } else {
            fprintf(stderr, "unknown action\n");
        }
    } else {
        if (pressee) printf("no action\n");
    }
    
    if(keycode != (CGKeyCode)-1)
        EmitKeyEventFlagMode(keycode, pressee, keyFlags);
    
}

bool compareBinary(const unsigned char * a, const unsigned char * b, const int size) {
    int i;
    
    for(i = 0; i < size; i++)
        if( a[i] != b[i])
            return false;
    
    return true;
}

/*!
 @function strToHex
 Convert a byte array to its hexadecimal representation.
 @param array byte array
 @param size size of the array
 */
char* strToHex(const unsigned char* array, const int size){
    //const char hexval[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
    char * finalhash = malloc((2 * size + 1) * sizeof(char));
    int j;
    for(j = 0; j < size; j++){
        //finalhash[j*2] = hexval[((array[j] >> 4) & 0xF)];
        //finalhash[(j*2) + 1] = hexval[(array[j]) & 0x0F];
        snprintf(finalhash+(j*2), 3, "%02x", array[j]);
    }
    return finalhash;
}

void binaryConjunction(unsigned char* conjunction, const unsigned char* a, const unsigned char* b, const int size){
	int i;
    for(i = 0; i < size; i++)
        conjunction[i] = a[i] & b[i];
}

/*
 Receives an activity report of the HID IR device, decode it and trigger the correct action.
 */
void Sony_IR_Callback(void *context, IOReturn result, void *sender, IOHIDReportType type, uint32_t reportID, uint8_t *report, CFIndex reportLength){
    char * finalhash = strToHex((unsigned char*)report, reportLength);
    
    // three report id :
    // reportID = 1 : key code 6 bytes
    // reportID = 2 : control code 2 bytes
    // reportID = 3 : unknown code 9 bytes
    if ((reportID == 1) & (reportLength == 6)) { // key code 6 bytes
        // inactivity : 0x 01 FF FF FF FF FF
        // activity :   0x 01 00 __ 1a __ 00
        //  byte 0 : 1
        //  byte 1 : 0
        //  byte 2 : code touche
        //   bit 0 : (0 : pressed ; 1 : released)
        //   bit 1..7 : code touche
        //  byte 3 : 0x1a
        //  byte 4 : mode (1 : 0xb2 ; 2 : 0x33 ; 3 : 0x3b)
        //  byte 5 : 0
        
        //      printf("Sony_IR_Callback %i | %i : %s\n", reportID, reportLength, report);
        unsigned char idle[6] = { 0x01, 0xff, 0xff, 0xff, 0xff, 0xff};
        unsigned char activity_mask[6] = { 0xff, 0xff, 0x00, 0xff, 0x00, 0xff};
        unsigned char activity[6] = { 0x01, 0x00, 0x00, 0x1a, 0x00, 0x00};
        
        
        unsigned char mask[6];
        binaryConjunction(mask, report, activity_mask, 6);
        
        if(compareBinary(mask, activity, 6)){ // activitiy mask : report & activity_mask = activity
            
            //mode
            unsigned char modeChar = report[4];
            int mode = 0; // remote control mode
            switch (modeChar) {
                case (unsigned char)0xb2: mode = 1; break;
                case (unsigned char)0x33: mode = 2; break;
                case (unsigned char)0x3b: mode = 3; break;
                default: break;
            }
            if (mode != 0)
                printf("Mode : %i ", mode);
            else
                printf("Mode unknown : %i ", (int)modeChar);
            
            unsigned char codeToucheMask = 0x7F;
            unsigned char presseMask = 0x80;
            
            int code = report[2] & codeToucheMask; // key code
            bool pressee = ((report[2] & presseMask) == 0); // action pressed or released
            
            DoAction(code, mode, pressee);
            
            // additionnal actions
            switch (code) { // there is 47 keys to emulate
                    // TODO : review the volume contrôl (simulate keys)
                case skeyStandby: break; // standby key, not sent in mode 1
                    // keypad
                    
                // options
                case 29: break; // rec key
                    
                // contrôle de la lecture
                case 24: break; // stop key
                case 25: break; // pause key
                case 26: break; // play key
                
                // bottom
                case 52: break;  // enter key
            }
            
        } else if(!compareBinary(report, idle, 6)){ // unknown action
            printf("ReportID 1 unknown activity : 0x%s\n", finalhash);
        }
        
    } else if((reportID == 2) & (reportLength == 2)){
        switch(report[1]){
            case 0: break;
            case 1:
                printf("standby key pressed\n"); // we simulate normal standby event
                DoAction(skeyStandby, 1, true);
                DoAction(skeyStandby, 1, false);
                break;
            default:
                fprintf(stderr, "!! unknown reportID 2 %i !!\n", report[1]); break;
                break;
        }
    } else if((reportID == 3) & (reportLength == 9)){ // constant 0x 03 00 00 00 00 00 00 00 00
        unsigned char nullMessage[9] = { 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        if(!compareBinary(report, nullMessage, 9))
            fprintf(stderr, "unknown reportID 3 message : 0x%s\n", finalhash);
        
    } else {
        fprintf(stderr, "!! unknown reportID/reportLength : %i %li !!\n", reportID, reportLength);
    }
    free(finalhash);
}

static void logIt(CFStringRef format, ...) {
    va_list args;
    va_start(args, format); 
    CFStringRef str = CFStringCreateWithFormatAndArguments(kCFAllocatorSystemDefault, NULL, format, args);
    if (!str) return;
    
    CFIndex blen = CFStringGetMaximumSizeForEncoding(CFStringGetLength(str), kCFStringEncodingUTF8);
    char *buf = str ? (char *)malloc(blen + 1) : 0;
    if (buf) {
        Boolean converted = CFStringGetCString(str, buf, blen, kCFStringEncodingUTF8);
        if (converted) {
            // null-terminate
            buf[blen] = 0;
            printf("%s\n", buf);
        }
    }
    if (buf) free(buf);
    if (str) CFRelease(str);      va_end(args);
}

static CFMutableDataRef createDataFromFile(const char *fname) {
    int fd = open(fname, O_RDONLY);
    CFMutableDataRef res = CFDataCreateMutable(kCFAllocatorSystemDefault, 0);
    char buf[4096];
    
    ssize_t amountRead;
    while ((amountRead = read(fd, buf, 4096)) > 0) {
        CFDataAppendBytes(res, (const UInt8 *)buf, amountRead);
    }
    
    close(fd);
    return res;
}

/* Configuration management
 */
bool getConfig(const char* config_path){
    CFMutableDataRef configFileData = createDataFromFile(config_path);
    CFPropertyListFormat fmt;
    CFErrorRef err;
    configStruct.configRoot = CFPropertyListCreateWithData(kCFAllocatorSystemDefault, (CFDataRef)configFileData, 0, &fmt, &err);
    if (configStruct.configRoot) {
        if(!CFDictionaryGetValueIfPresent(configStruct.configRoot, CFSTR("Mapping"), (const void **)&configStruct.Mapping)){
            fprintf(stderr, "'Mapping' dictionnary not found\n");
            return false;
        }
        
        CFDictionaryRef Keycodes, code_IR;
        if(CFDictionaryGetValueIfPresent(configStruct.configRoot, CFSTR("Keycodes"), (const void **)&Keycodes)){
            if(!CFDictionaryGetValueIfPresent(Keycodes, CFSTR("Keyboard"), (const void **)&configStruct.keycodes_KB)){
                fprintf(stderr, "'Keyboard' dictionnary not found\n");
                return false;
            }
            if(!CFDictionaryGetValueIfPresent(Keycodes, CFSTR("Remote"), (const void **)&code_IR)){
                fprintf(stderr, "'Remote' dictionnary not found\n");
                return false;
            } else configStruct.code_IR = DictToAssoc(code_IR);
            
        } else {
            fprintf(stderr, "'Keycodes' Dictionnary not found\n");
            return false;
        }
        
        
        
    } else {
        logIt(CFSTR("Unable to read the file %s: %@"), config_path, err);
        return false;
    }
    return true;
}

CFMutableDictionaryRef createDictionaryMatching(const long vendorID, const long productID){
    CFMutableDictionaryRef dictionary = CFDictionaryCreateMutable(kCFAllocatorDefault, 2, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
    
    CFNumberRef cfVendorId = CFNumberCreate(kCFAllocatorDefault, kCFNumberLongType, &vendorID);
    CFStringRef cfVendorSt = CFStringCreateWithCString(kCFAllocatorDefault, kIOHIDVendorIDKey, kCFStringEncodingUTF8);
    CFDictionaryAddValue(dictionary, cfVendorSt, cfVendorId);
    CFRelease(cfVendorId);
    CFRelease(cfVendorSt);
    
    CFNumberRef cfProductId = CFNumberCreate(kCFAllocatorDefault, kCFNumberLongType, &productID);
    CFStringRef cfProductSt = CFStringCreateWithCString(kCFAllocatorDefault, kIOHIDProductIDKey, kCFStringEncodingUTF8);
    CFDictionaryAddValue(dictionary, cfProductSt, cfProductId);
    CFRelease(cfProductId);
    CFRelease(cfProductSt);
    return dictionary;
}

CUSTOM_Device * getHIDinfo(IOHIDDeviceRef tIOHIDDeviceRef){
    CUSTOM_Device * myDevice = malloc(sizeof(CUSTOM_Device));
    
    long vendorID = IOHIDDevice_GetVendorID(tIOHIDDeviceRef);
    long productID = IOHIDDevice_GetProductID( tIOHIDDeviceRef );
    
    uint32_t usagePage = IOHIDDevice_GetUsagePage( tIOHIDDeviceRef );
    uint32_t usage = IOHIDDevice_GetUsage( tIOHIDDeviceRef );
    
    if ( !usagePage || !usage ) {
        usagePage = IOHIDDevice_GetPrimaryUsagePage( tIOHIDDeviceRef );
        usage = IOHIDDevice_GetPrimaryUsage( tIOHIDDeviceRef );
    }
    
    (*myDevice).vendorID = vendorID;
    (*myDevice).productID = productID;
    (*myDevice).usagePage = usagePage;
    (*myDevice).usage = usage;
    
    return myDevice;
}

bool setHook(CFRunLoopRef currentLoop){
    IOHIDManagerRef gIOHIDManagerRef;
    
    // create the manager
	gIOHIDManagerRef = IOHIDManagerCreate( kCFAllocatorDefault, kIOHIDOptionsTypeNone );
	
    if ( gIOHIDManagerRef ) {
		// open it
		IOReturn tIOReturn = IOHIDManagerOpen( gIOHIDManagerRef, kIOHIDOptionsTypeSeizeDevice);
		if ( kIOReturnSuccess != tIOReturn ) {
			fprintf( stderr, "%s: Couldn’t open IOHIDManager.", __PRETTY_FUNCTION__ );
		} else {
            CFMutableDictionaryRef dictionary = createDictionaryMatching(Sony_IR.vendorID, Sony_IR.productID);
            
			IOHIDManagerSetDeviceMatching( gIOHIDManagerRef, dictionary );
            //IOHIDManagerSetDeviceMatching( gIOHIDManagerRef, NULL );
            
            // CUSTOM : callback
            IOHIDManagerScheduleWithRunLoop(gIOHIDManagerRef, currentLoop, kCFRunLoopDefaultMode);
            // IOHIDManagerRegisterDeviceMatchingCallback( gIOHIDManagerRef, Handle_DeviceMatchingCallback, NULL );
			
            CFSetRef devCFSetRef = IOHIDManagerCopyDevices( gIOHIDManagerRef );
			if ( devCFSetRef ) {
				if ( gDeviceCFArrayRef ) {
					CFRelease( gDeviceCFArrayRef );
				}
                
				gDeviceCFArrayRef = CFArrayCreateMutable( kCFAllocatorDefault, 0, & kCFTypeArrayCallBacks );
				CFSetApplyFunction( devCFSetRef, CFSetApplierFunctionCopyToCFArray, gDeviceCFArrayRef );
				CFIndex cnt = CFArrayGetCount( gDeviceCFArrayRef );
                
                char* message = malloc(100 * sizeof(char));
                snprintf(message, 100, "there are %i detected devices\n", (int)cnt);
                MyLog(2, message);
                free(message);
                
                // loop
                CFIndex devIndex, devCount = CFArrayGetCount( gDeviceCFArrayRef );
                for ( devIndex = 0; devIndex < devCount; devIndex++ ) {
                    IOHIDDeviceRef tIOHIDDeviceRef = ( IOHIDDeviceRef ) CFArrayGetValueAtIndex( gDeviceCFArrayRef, devIndex );
                    
                    CUSTOM_Device * deviceInfoRef = getHIDinfo(tIOHIDDeviceRef);
                    CUSTOM_Device deviceInfo = *deviceInfoRef;
                    
                    //if ( !tIOHIDDeviceRef || deviceInfo.vendorID != Sony_IR.vendorID) continue;
                    //if ( !tIOHIDDeviceRef || deviceInfo.vendorID != Cherry_KB.vendorID || !((deviceInfo.usagePage == 1) && (deviceInfo.usage == 6))) continue;
                    
                    char manufacturer[256] = ""; // name of manufacturer
                    CFStringRef tCFStringRef = IOHIDDevice_GetManufacturer( tIOHIDDeviceRef );
                    
                    if ( tCFStringRef ) {
                        verify( CFStringGetCString( tCFStringRef, manufacturer, sizeof( manufacturer ), kCFStringEncodingUTF8 ) );
                    }
                    
                    char product[256] = "";      // name of product
                    tCFStringRef = IOHIDDevice_GetProduct( tIOHIDDeviceRef );
                    
                    if ( tCFStringRef ) {
                        verify( CFStringGetCString( tCFStringRef, product, sizeof( product ), kCFStringEncodingUTF8 ) );
                    }
                    
                    char* message = malloc(100 * sizeof(char));
                    snprintf(message, 100, "found ! %p \"%s - %s\" 0x%04lX 0x%04lX\n",
                             tIOHIDDeviceRef, manufacturer, product, deviceInfo.vendorID, deviceInfo.productID);
                    MyLog(2, message);
                    free(message);
                    
                    IOReturn ioReturnValue;
                    
                    // processing of a report
                    /*CFIndex reportID = 1;
                    CFIndex *pReportLength = malloc(sizeof(CFIndex));
                    *pReportLength = 1000;
                    uint8_t *report = malloc(sizeof(uint8_t)* *pReportLength);
                    ioReturnValue = IOHIDDeviceGetReport(tIOHIDDeviceRef, kIOHIDReportTypeInput, reportID, report, pReportLength);
                    printf("Report reçu %i %s %li\n", (int)reportID, report, *pReportLength);
                    */
                    
                    size_t bufferSize  = 64;
                    char *inputBuffer  = malloc(bufferSize);
                    //char *outputBuffer = malloc(bufferSize); // no output to the device
                    
                    ioReturnValue = IOHIDDeviceOpen(tIOHIDDeviceRef, kIOHIDOptionsTypeSeizeDevice);
                    if(ioReturnValue == kIOReturnSuccess){
                        printf("Device exclusively seized\n");
                        
                        IOHIDDeviceScheduleWithRunLoop(tIOHIDDeviceRef, currentLoop, kCFRunLoopDefaultMode);
                        IOHIDDeviceRegisterInputReportCallback(tIOHIDDeviceRef, (uint8_t *)inputBuffer, bufferSize, Sony_IR_Callback, NULL);
                        return true;
                        
                    } else
                        fprintf(stderr, "Device seizing failed (code %i)\n", (int)ioReturnValue);
                    free(deviceInfoRef);
                }
                
			}
		}
	} else {
		fprintf( stderr, "%s: Couldn’t create a IOHIDManager.", __PRETTY_FUNCTION__ );
	}
    
    return false;
}

// note for self
//EmitKeyEvent(kHIDUsage_KeyboardVolumeUp, true);

int main(int argc, const char * argv[])
{
    inputMode = CUSTOM_Default;
    
    keyHash = CreateKeyHash();
	    
    CFRunLoopRef currentLoop = CFRunLoopGetCurrent();
    
    int len = strlen(getenv("HOME")) + strlen(config_path) + 1;
    char * config_file = malloc(len * sizeof(char));
    config_file[0] = '\0';
    strncat(config_file, getenv("HOME"), len);
    strncat(config_file, config_path, len - strlen(config_file));
    
    // TODO : make it so the program does several trial before failing (with throttling)
    if(getConfig(config_file)){
        if(setHook(currentLoop)){
            // Run the event loop
            //RunApplicationEventLoop();
            CFRunLoopRun();
        } else{
            fprintf(stderr, "The acquisition of the device failed!\n");
            return -2;
        }
    } else {
        fprintf(stderr, "Could not read config file\n");
        return -1;
    }
    printf("End!\n");
    return 0;
}

