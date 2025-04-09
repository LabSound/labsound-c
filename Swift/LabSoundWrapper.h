#ifndef LabSoundWrapper_h
#define LabSoundWrapper_h

#import <Foundation/Foundation.h>
#include "labsound-c.h"

// This is a wrapper Objective-C++ class around the C++ class
@interface LabSoundWrapper : NSObject
@property (readonly) struct LabSoundAPI_1_0* api;

@property (readwrite) float sfxr_attack; // 0-1
@property (readwrite) float sfxr_decay; // 0-1
@property (readwrite) float sfxr_sustain; // 0-1
@property (readwrite) float sfxr_sustain_punch; // 0-1

@property (readwrite) float sfxr_startFrequency; // 0-1
@property (readwrite) float sfxr_minFrequency; // 0-1
@property (readwrite) float sfxr_slide; // -1, -1
@property (readwrite) float sfxr_deltaSlide; // 0-1
@property (readwrite) float sfxr_vibratoSpeed;
@property (readwrite) float sfxr_vibratoDepth;

@property (readwrite) float sfxr_lpFilterCutoff;
@property (readwrite) float sfxr_lpFilterCutoffSweep;
@property (readwrite) float sfxr_lpFilterResonance;
@property (readwrite) float sfxr_hpFilterCutoff;
@property (readwrite) float sfxr_hpFilterCutoffSweep;

@property (readwrite) float sfxr_changeAmount;
@property (readwrite) float sfxr_changeSpeed;
@property (readwrite) float sfxr_squareDuty;
@property (readwrite) float sfxr_dutySweep;
@property (readwrite) float sfxr_repeatSpeed;
@property (readwrite) float sfxr_phaserOffset;
@property (readwrite) float sfxr_phaserSweep;


@end

#endif /* MSCalculatorWrapper_h */
