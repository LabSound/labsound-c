
#include "LabSoundDemo.h"
#include "labsound-c.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "tinycthread.h"

static bool end_sleep = false;
static bool ended_via_flag = false;

void san_on_ended()
{
    printf("\n audio ended\n");
    end_sleep = true;
}


void demo_sleep(LabSoundAPI* ls, float seconds) {
    while (seconds > 1.f/60.f) {
        ls_idle(ls);
        if (end_sleep) {
            end_sleep = false;
            ended_via_flag = true;
            return;
        }

        thrd_sleep(&(struct timespec) {
            .tv_sec = 0,
            .tv_nsec = 1000000000 / 60 }, NULL);

        seconds -= 1.f / 60.f;
    }
    if (seconds > 0.f) {
        thrd_sleep(&(struct timespec) {
            .tv_sec = 0,
            .tv_nsec = (long)(1000000000.f * seconds)
        }, NULL);
    }
    ls_idle(ls);
    ended_via_flag = false;
}




#define MAKE_SLICE(STR) \
    static const char* STR##_slicestr = #STR;\
    ls_StringSlice STR##_s = { STR##_slicestr, STR##_slicestr + sizeof(#STR) - 1 };

int main(int argc, char** argcv)
{
    LabSoundAPI* ls = ls_create_api_1_0(ls_default_alloc);
    const ls_NameArray* node_names = ls->node_names(ls);
    if (!node_names)
        return 1;

    MAKE_SLICE(amplitude);
    MAKE_SLICE(frequency);
    MAKE_SLICE(detune);
    MAKE_SLICE(Oscillator);
    MAKE_SLICE(SampledAudio);
    MAKE_SLICE(sourceBus);
    MAKE_SLICE(StereoPanner);
    MAKE_SLICE(pan);

    MAKE_SLICE(osc1);
    MAKE_SLICE(osc2);
    MAKE_SLICE(osc3);
    MAKE_SLICE(san);
    MAKE_SLICE(stpan);

    //-------------------------------------------------------------------------
    printf("test: list the registered nodes\n");

    for (int i = 0; i < node_names->count; ++i) {
        ls_StringSlice* s = &node_names->names[i];
        printf("%.*s\n", (int) (s->end - s->start), s->start);
    }

    ls_Node dest = ls->destination_node(ls);
    ls_InputPin dest_in = ls->node_indexed_input(ls, dest, 0);

    //-------------------------------------------------------------------------
    if (false)
    {
        printf("test: playing a 440 hz tone\n");
        ls_Node osc = ls->node_create(ls, osc1_s, Oscillator_s);
        ls_OutputPin osc_out = ls->node_indexed_output(ls, osc, 0);
        ls_InputPin osc_freq = ls->node_parameter(ls, osc, frequency_s);
        ls_InputPin osc_detune = ls->node_parameter(ls, osc, detune_s);
        
        ls_Connection osc_to_dest;
        
        osc_to_dest = ls->connect(ls, dest_in, osc_out);
        ls->node_start(ls, osc, (ls_Seconds) { 0.f });
        demo_sleep(ls, 0.5);
        
        printf("test: disconnect the oscillator (silence)\n");
        
        ls->disconnect(ls, osc_to_dest);
        
        demo_sleep(ls, 0.5f);
        
        printf("test: reconnect the oscillator, play at 220Hz\n");
        
        ls->set_float(ls, osc_freq, 220.f);
        osc_to_dest = ls->connect(ls, dest_in, osc_out);
        ls->node_start(ls, osc, (ls_Seconds) { 0.f });
        
        demo_sleep(ls, 0.5f);
        
        printf("test: delete the oscillator while it's running (silence)\n");
        
        ls->node_delete(ls, osc);
        
        demo_sleep(ls, 0.5);
    }
    //-------------------------------------------------------------------------
    if (false)
    {
        printf("test: create and play an oscillator at 880\n");
        
        ls_Node osc = ls->node_create(ls, osc2_s, Oscillator_s);
        ls_OutputPin osc_out = ls->node_indexed_output(ls, osc, 0);
        ls_InputPin osc_freq = ls->node_parameter(ls, osc, frequency_s);
        ls_InputPin osc_detune = ls->node_parameter(ls, osc, detune_s);
        
        ls->set_float(ls, osc_freq, 880.f);
        ls_Connection osc_to_dest = ls->connect(ls, dest_in, osc_out);
        ls->node_start(ls, osc, (ls_Seconds) { 0.f });
        
        demo_sleep(ls, 0.5f);
        //-------------------------------------------------------------------------
        printf("test: modulate the oscillator at 440\n");
        
        ls_Node lfo = ls->node_create(ls, osc3_s, Oscillator_s);
        ls_InputPin lfo_freq = ls->node_parameter(ls, lfo, frequency_s);
        ls_InputPin lfo_ampl = ls->node_parameter(ls, lfo, amplitude_s);
        ls_OutputPin lfo_out = ls->node_indexed_output(ls, lfo, 0);
        
        ls->set_float(ls, lfo_freq, 2.f);
        ls->set_float(ls, lfo_ampl, 20.f);
        ls->set_float(ls, osc_freq, 440.f);
        
        ls_Connection lfo_to_osc = ls->connect(ls, osc_detune, lfo_out);
        ls->node_start(ls, lfo, (ls_Seconds) { 0.f });
        
        demo_sleep(ls, 2.f);
        //-------------------------------------------------------------------------
        printf("test: disconnect modulated oscillator\n");
        
        ls->disconnect(ls, lfo_to_osc);
        
        demo_sleep(ls, 0.5f);
        //-------------------------------------------------------------------------
        printf("test: disconnect oscillator\n");
        
        ls->disconnect(ls, osc_to_dest);
        
        demo_sleep(ls, 0.5f);
    }
    //-------------------------------------------------------------------------

    if (true)
    {
        printf("test: play a file, wait for end\n");
        char buff[1024];
        snprintf(buff, sizeof(buff), "%ssamples/mono-music-clip.wav", asset_base);
        ls_BusData musicClip = ls->bus_create_from_file(ls, buff, false);
        if (musicClip.id != ls_BusData_empty.id)
        {
            ls_Node sampledAudio;
            ls_InputPin sampledAudio_srcBus;
            ls_OutputPin sa_out;
            ls_Connection connection3;
            
            sampledAudio        = ls->node_create(ls,  san_s, SampledAudio_s);
            sampledAudio_srcBus = ls->node_setting(ls, sampledAudio, sourceBus_s);
            sa_out              = ls->node_indexed_output(ls, sampledAudio, 0);
            connection3         = ls->connect(ls, dest_in, sa_out);
            
            if (connection3.id != ls_Connection_empty.id) {
                ls->node_diagnose(ls,     sampledAudio);
                ls->set_bus(ls,           sampledAudio_srcBus, musicClip);
                ls->node_start(ls,        sampledAudio, (ls_Seconds) { 0.f });
                ls->node_set_on_ended(ls, sampledAudio, san_on_ended);

                demo_sleep(ls, 1);
                printf("scheduled state %s\n", ls->node_scheduled_state_name(ls, sampledAudio));
                demo_sleep(ls, 5);
                ls->disconnect(ls, connection3);

                printf("  ended %s\n", ended_via_flag ? "properly via flag" : "improperly by timeout");
            }
            else {
                printf("  Couldn't connect the sampled audio to the destination\n");
            }
        }
    }

    //-------------------------------------------------------------------------
    if (true)  
    {
        printf("test: stereo panning\n");
        char buff[1024];
        sprintf(buff, "%ssamples/trainrolling.wav", asset_base);
        ls_BusData trainClip = ls->bus_create_from_file(ls, buff, false);
        if (trainClip.id != ls_BusData_empty.id)
        {
            ls_Node sampledAudio = ls->node_create(ls, san_s, SampledAudio_s);
            ls_InputPin src = ls->node_setting(ls, sampledAudio, sourceBus_s);
            ls_OutputPin sa_out = ls->node_indexed_output(ls, sampledAudio, 0);
            ls->set_bus(ls, src, trainClip);
            
            ls_Node stPanner = ls->node_create(ls, stpan_s, StereoPanner_s);
            ls_OutputPin stPanner_out = ls->node_indexed_output(ls, stPanner, 0);
            ls_InputPin stPanner_in = ls->node_indexed_input(ls, stPanner, 0);
            ls_Connection connection3 = ls->connect(ls, dest_in, stPanner_out);
            ls_Connection connection4 = ls->connect(ls, stPanner_in, sa_out);
            ls->node_schedule(ls, sampledAudio, (ls_Seconds) { 0.f }, -1); // -1 to loop forever
            ls_InputPin pan_param = ls->node_parameter(ls, stPanner, pan_s);
            
            float seconds = 4.f;
            float half = seconds * 0.5f;
            for (float i = 0; i < seconds; i += 0.01f) {
                float x = (i - half) / half;
                ls->set_float(ls, pan_param, x);
                thrd_sleep(&(struct timespec) {
                    .tv_sec = 0,
                    .tv_nsec = (long)(1000000000.f * 0.01f)
                }, NULL);
            }
        }
    }

    printf("test: all complete\n");
    return 0;
}
