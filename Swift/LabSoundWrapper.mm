#import "LabSoundWrapper.h"
#include "labsound-c.h"

@implementation LabSoundWrapper
{
    LabSoundAPI_1_0* api;
    float _sfxr_attack;
    float _sfxr_decay;
    float _sfxr_sustain;
    float _sfxr_sustain_punch;
    float _sfxr_vibratoSpeed;
    float _sfxr_vibratoDepth;
    
    float _sfxr_startFrequency;
    float _sfxr_minFrequency;
    float _sfxr_slide;
    float _sfxr_deltaSlide;

    float _sfxr_lpFilterCutoff;
    float _sfxr_lpFilterCutoffSweep;
    float _sfxr_lpFilterResonance;
    float _sfxr_hpFilterCutoff;
    float _sfxr_hpFilterCutoffSweep;
    
    float _sfxr_changeAmount;
    float _sfxr_changeSpeed;
    float _sfxr_squareDuty;
    float _sfxr_dutySweep;
    float _sfxr_repeatSpeed;
    float _sfxr_phaserOffset;
    float _sfxr_phaserSweep;
    
    
    ls_InputPin _pin_sfxr_attack;
    ls_InputPin _pin_sfxr_decay;
    ls_InputPin _pin_sfxr_sustain;
    ls_InputPin _pin_sfxr_sustain_punch;
    ls_InputPin _pin_sfxr_vibratoSpeed;
    ls_InputPin _pin_sfxr_vibratoDepth;
    
    ls_InputPin _pin_sfxr_startFrequency;
    ls_InputPin _pin_sfxr_minFrequency;
    ls_InputPin _pin_sfxr_slide;
    ls_InputPin _pin_sfxr_deltaSlide;

    ls_InputPin _pin_sfxr_lpFilterCutoff;
    ls_InputPin _pin_sfxr_lpFilterCutoffSweep;
    ls_InputPin _pin_sfxr_lpFilterResonance;
    ls_InputPin _pin_sfxr_hpFilterCutoff;
    ls_InputPin _pin_sfxr_hpFilterCutoffSweep;
    
    ls_InputPin _pin_sfxr_changeAmount;
    ls_InputPin _pin_sfxr_changeSpeed;
    ls_InputPin _pin_sfxr_squareDuty;
    ls_InputPin _pin_sfxr_dutySweep;
    ls_InputPin _pin_sfxr_repeatSpeed;
    ls_InputPin _pin_sfxr_phaserOffset;
    ls_InputPin _pin_sfxr_phaserSweep;

    ls_Node dest;
    ls_InputPin dest_in;
    ls_Node sfxr_node;
    
    char sfxr_name[4];
}

-(instancetype)init {
    self = [super init];
    if (!self)
        return nil;
    
    self->api = ls_create_api_1_0(ls_default_alloc);
    auto ls = self->api;
    
    self->_sfxr_attack = 0.f;
    self->_sfxr_decay = 0.3f;
    self->_sfxr_sustain = 0.4f;
    self->_sfxr_sustain_punch = 0.f;
    self->_sfxr_vibratoSpeed = 0.f;
    self->_sfxr_vibratoDepth = 0.f;
    
    self->_sfxr_startFrequency = 3.f;
    self->_sfxr_minFrequency = 0.f;
    self->_sfxr_slide = 0.f;
    self->_sfxr_deltaSlide = 0.f;
    
    self->dest = ls->destination_node(ls);
    self->dest_in = ls->node_indexed_input(ls, dest, 0);

    #define MAKE_SLICE(STR) \
        static const char* STR##_slicestr = #STR;\
        ls_StringSlice STR##_s = { STR##_slicestr, STR##_slicestr + sizeof(#STR) - 1 };
    MAKE_SLICE(SFXR);   // node kind
    MAKE_SLICE(sfxr);   // a unique identifier
    
    self->sfxr_node = ls->node_create(ls, sfxr_s, SFXR_s);
    
    make a sfxrNodeController instantiated with a pointer to ls
    and wrap all this accessor stuff up into it then it will be comfortably available from
    swift, and also all the utility copying around & duplication can be hidden in the wrapper

    self->_pin_sfxr_attack = ls->node_setting(ls, self->sfxr_node, sourceBus_s);
    self->_pin_sfxr_decay = ls->node_setting(ls, self->sfxr_node, sourceBus_s);
    self->_pin_sfxr_sustain = ls->node_setting(ls, self->sfxr_node, sourceBus_s);
    self->_pin_sfxr_sustain_punch = ls->node_setting(ls, self->sfxr_node, sourceBus_s);
    self->_pin_sfxr_vibratoSpeed = ls->node_setting(ls, self->sfxr_node, sourceBus_s);
    self->_pin_sfxr_vibratoDepth = ls->node_setting(ls, self->sfxr_node, sourceBus_s);
    
    self->_pin_sfxr_startFrequency = ls->node_setting(ls, self->sfxr_node, sourceBus_s);
    self->_pin_sfxr_minFrequency = ls->node_setting(ls, self->sfxr_node, sourceBus_s);
    self->_pin_sfxr_slide = ls->node_setting(ls, self->sfxr_node, sourceBus_s);
    self->_pin_sfxr_deltaSlide = ls->node_setting(ls, self->sfxr_node, sourceBus_s);

    self->_pin_sfxr_lpFilterCutoff = ls->node_setting(ls, self->sfxr_node, sourceBus_s);
    self->_pin_sfxr_lpFilterCutoffSweep = ls->node_setting(ls, self->sfxr_node, sourceBus_s);
    self->_pin_sfxr_lpFilterResonance = ls->node_setting(ls, self->sfxr_node, sourceBus_s);
    self->_pin_sfxr_hpFilterCutoff = ls->node_setting(ls, self->sfxr_node, sourceBus_s);
    self->_pin_sfxr_hpFilterCutoffSweep = ls->node_setting(ls, self->sfxr_node, sourceBus_s);
    
    self->_pin_sfxr_changeAmount = ls->node_setting(ls, self->sfxr_node, sourceBus_s);
    self->_pin_sfxr_changeSpeed = ls->node_setting(ls, self->sfxr_node, sourceBus_s);
    self->_pin_sfxr_squareDuty = ls->node_setting(ls, self->sfxr_node, sourceBus_s);
    self->_pin_sfxr_dutySweep = ls->node_setting(ls, self->sfxr_node, sourceBus_s);
    self->_pin_sfxr_repeatSpeed = ls->node_setting(ls, self->sfxr_node, sourceBus_s);
    self->_pin_sfxr_phaserOffset = ls->node_setting(ls, self->sfxr_node, sourceBus_s);
    self->_pin_sfxr_phaserSweep = ls->node_setting(ls, self->sfxr_node, sourceBus_s);
    
    return self;
}

-(void) dealloc {
    ls_release_api_1_0(api);
    [super dealloc];
}

// properties

-(LabSoundAPI_1_0*) api {
    return self->api;
}

-(float) sfxr_attack {
    return self->_sfxr_attack;
}
-(void) setSfxr_attack:(float)sfxr_attack {
    self->_sfxr_attack = sfxr_attack;
}

-(float) sfxr_decay {
    return self->_sfxr_decay;
}
-(void) setSfxr_decay:(float)_sfxr_decay {
    self->_sfxr_decay = _sfxr_decay;
}

-(float) sfxr_sustain {
    return self->_sfxr_sustain;
}
-(void) setSfxr_sustain:(float)sfxr_sustain {
    self->_sfxr_sustain = sfxr_sustain;
}

-(float) sfxr_sustain_punch {
    return self->_sfxr_sustain_punch;
}
-(void) setSfxr_sustain_punch:(float)sfxr_sustain_punch {
    self->_sfxr_sustain_punch = sfxr_sustain_punch;
}

-(float) sfxr_vibratoSpeed {
    return self->_sfxr_vibratoSpeed;
}
-(void) setSfxr_vibratoSpeed:(float)sfxr_vibratoSpeed {
    self->_sfxr_vibratoSpeed = sfxr_vibratoSpeed;
}

-(float) sfxr_vibratoDepth {
    return self->_sfxr_vibratoDepth;
}
-(void) setSfxr_vibratoDepth:(float)sfxr_vibratoDepth {
    self->_sfxr_vibratoDepth = sfxr_vibratoDepth;
}

-(float) sfxr_startFrequency {
    return self->_sfxr_startFrequency;
}
-(void) setSfxr_startFrequency:(float)sfxr_startFrequency {
    self->_sfxr_startFrequency = sfxr_startFrequency;
}

-(float) sfxr_minFrequency {
    return self->_sfxr_minFrequency;
}
-(void) setSfxr_minFrequency:(float)sfxr_minFrequency {
    self->_sfxr_minFrequency = sfxr_minFrequency;
}

-(float) sfxr_slide {
    return self->_sfxr_slide;
}
-(void) setSfxr_slide:(float)sfxr_slide {
    self->_sfxr_slide = sfxr_slide;
}

-(float) sfxr_deltaSlide {
    return self->_sfxr_deltaSlide;
}
-(void) setSfxr_deltaSlide:(float)sfxr_deltaSlide {
    self->_sfxr_deltaSlide = sfxr_deltaSlide;
}

-(float) sfxr_lpFilterCutoff {
    return self->_sfxr_lpFilterCutoff;
}
-(void) setSfxr_lpFilterCutoff:(float)sfxr_lpFilterCutoff {
    self->_sfxr_lpFilterCutoff = sfxr_lpFilterCutoff;
}

-(float) sfxr_lpFilterCutoffSweep {
    return self->_sfxr_lpFilterCutoffSweep;
}
-(void) setSfxr_lpFilterCutoffSweep:(float)sfxr_lpFilterCutoffSweep {
    self->_sfxr_lpFilterCutoffSweep = sfxr_lpFilterCutoffSweep;
}

-(float) sfxr_lpFilterResonance {
    return self->_sfxr_lpFilterResonance;
}
-(void) setSfxr_lpFilterResonance:(float)sfxr_lpFilterResonance {
    self->_sfxr_lpFilterResonance = sfxr_lpFilterResonance;
}

-(float) sfxr_hpFilterCutoff {
    return self->_sfxr_hpFilterCutoff;
}
-(void) setSfxr_hpFilterCutoff:(float)sfxr_hpFilterCutoff {
    self->_sfxr_hpFilterCutoff = sfxr_hpFilterCutoff;
}

-(float) sfxr_hpFilterCutoffSweep {
    return self->_sfxr_hpFilterCutoffSweep;
}
-(void) setSfxr_hpFilterCutoffSweep:(float)sfxr_hpFilterCutoffSweep {
    self->_sfxr_hpFilterCutoffSweep = sfxr_hpFilterCutoffSweep;
}

-(float) sfxr_changeAmount {
    return self->_sfxr_changeAmount;
}
-(void)setSfxr_changeAmount:(float)sfxr_changeAmount {
    self->_sfxr_changeAmount = sfxr_changeAmount;
}

-(float) sfxr_changeSpeed {
    return self->_sfxr_changeSpeed;
}
-(void)setSfxr_changeSpeed:(float)sfxr_changeSpeed {
    self->_sfxr_changeSpeed = sfxr_changeSpeed;
}

-(float) sfxr_squareDuty {
    return self->_sfxr_squareDuty;
}
-(void)setSfxr_squareDuty:(float)sfxr_squareDuty {
    self->_sfxr_squareDuty = sfxr_squareDuty;
}

-(float) sfxr_dutySweep {
    return self->_sfxr_dutySweep;
}
-(void)setSfxr_dutySweep:(float)sfxr_dutySweep {
    self->_sfxr_dutySweep = sfxr_dutySweep;
}

-(float) sfxr_repeatSpeed {
    return self->_sfxr_repeatSpeed;
}
-(void)setSfxr_repeatSpeed:(float)sfxr_repeatSpeed {
    self->_sfxr_repeatSpeed = sfxr_repeatSpeed;
}

-(float) sfxr_phaserOffset {
    return self->_sfxr_phaserOffset;
}
-(void)setSfxr_phaserOffset:(float)sfxr_phaserOffset {
    self->_sfxr_phaserOffset = sfxr_phaserOffset;
}

-(float) sfxr_phaserSweep {
    return self->_sfxr_phaserSweep;
}
-(void)setSfxr_phaserSweep:(float)sfxr_phaserSweep {
    self->_sfxr_phaserSweep = sfxr_phaserSweep;
}

@end
