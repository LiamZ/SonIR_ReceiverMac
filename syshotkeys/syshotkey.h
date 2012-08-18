//
//  syshotkey.h
//  SonIR
//
//  Created by LiamZ on 18/05/12.
//

#ifndef SonIR_syshotkey_h
#define SonIR_syshotkey_h

#include <stdio.h>
#include <stdlib.h>
#include <Carbon/Carbon.h>

enum
{
    //UI Features
    sysMissionControl,
    sysLaunchPad,
    sysAppWindow,
    sysDesktop,
    sysDashboard,
    // Consumer Keys
    sysMediaPlay,
    sysMediaPrev,
    sysMediaNext,
    sysMediaVolUp,
    sysMediaVolDown,
    sysMediaVolMute
};

enum
{
    kVK_missionControl = 0xa0,
    kVK_dashboard = 0x82
};

void readConfig(long);

void execSysFunction(long);

#endif
