
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

    MAKE_SLICE(osc1);
    MAKE_SLICE(osc2);
    MAKE_SLICE(osc3);
    MAKE_SLICE(san);


    //-------------------------------------------------------------------------
    printf("test: list the registered nodes\n");

    for (int i = 0; i < node_names->count; ++i) {
        ls_StringSlice* s = &node_names->names[i];
        printf("%.*s\n", (int) (s->end - s->start), s->start);
    }

    //-------------------------------------------------------------------------
    printf("test: playing a 440 hz tone\n");

    ls_Node osc = ls->node_create(ls, osc1_s, Oscillator_s);
    ls_Node dest = ls->device_node(ls);

    ls_Pin oscOut = ls->node_indexed_output(ls, osc, 0);
    ls_Pin destIn = ls->node_indexed_input(ls, dest, 0);
    ls_Connection connection1 = ls->connect_output_to_input(ls, destIn, oscOut);

    ls->node_start(ls, osc, (ls_Seconds) { 0.f });

    demo_sleep(ls, 0.5);
    //-------------------------------------------------------------------------
    printf("test: disconnect the oscillator (silence)\n");

    ls->disconnect(ls, connection1);

    demo_sleep(ls, 0.5f);
    //-------------------------------------------------------------------------
    printf("test: reconnect the oscillator, play at 220Hz\n");

    ls_Pin frequency_osc1 = ls->node_parameter(ls, osc, frequency_s);
    ls->set_float(ls, frequency_osc1, 220.f);
    connection1 = ls->connect_output_to_input(ls, destIn, oscOut);
    ls->node_start(ls, osc, (ls_Seconds) { 0.f });

    demo_sleep(ls, 0.5f);
    //-------------------------------------------------------------------------
    printf("test: delete the oscillator while it's running (silence)\n");

    ls->node_delete(ls, osc);

    demo_sleep(ls, 0.5);
    //-------------------------------------------------------------------------
    printf("test: create and play an oscillator at 880\n");

    osc = ls->node_create(ls, osc2_s, Oscillator_s);
    frequency_osc1 = ls->node_parameter(ls, osc, frequency_s);
    ls->set_float(ls, frequency_osc1, 880.f);
    oscOut = ls->node_indexed_output(ls, osc, 0);
    connection1 = ls->connect_output_to_input(ls, destIn, oscOut);
    ls->node_start(ls, osc, (ls_Seconds) { 0.f });

    demo_sleep(ls, 0.5f);
    //-------------------------------------------------------------------------
    printf("test: modulate the oscillator at 440\n");

    ls_Node osc2 = ls->node_create(ls, osc3_s, Oscillator_s);
    ls_Pin frequency_osc2 = ls->node_parameter(ls, osc2, frequency_s);
    ls->set_float(ls, frequency_osc2, 4.f);
    ls_Pin amplitude_osc2 = ls->node_parameter(ls, osc2, amplitude_s);
    ls->set_float(ls, amplitude_osc2, 40.f);
    ls->set_float(ls, frequency_osc1, 440.f);

    ls_Pin detune_osc1 = ls->node_parameter(ls, osc, detune_s);
    ls_Pin osc2Out = ls->node_indexed_output(ls, osc2, 0);
    ls_Connection connection2 = ls->connect_output_to_input(ls, detune_osc1, osc2Out);
    ls->node_start(ls, osc2, (ls_Seconds) { 0.f });

    demo_sleep(ls, 0.5f);
    //-------------------------------------------------------------------------
    printf("test: disconnect modulated oscillator\n");

    ls->disconnect(ls, connection2);

    demo_sleep(ls, 0.5f);
    //-------------------------------------------------------------------------
    printf("test: disconnect oscillator\n");

    ls->disconnect(ls, connection1);

    demo_sleep(ls, 0.5f);
    //-------------------------------------------------------------------------
    printf("test: play a file, wait for end\n");

    char buff[1024];
    sprintf(buff, "%ssamples/mono-music-clip.wav", asset_base);
    ls_BusData musicClip = ls->bus_create_from_file(ls, buff, false);

    if (musicClip.id != ls_BusData_empty.id)
    {
        ls_Node sampledAudio = ls->node_create(ls, san_s, SampledAudio_s);
        ls_Pin src = ls->node_setting(ls, sampledAudio, sourceBus_s);
        ls_Pin sa_out = ls->node_indexed_output(ls, sampledAudio, 0);
        ls->set_bus(ls, src, musicClip);
        ls_Connection connection3 = ls->connect_output_to_input(ls, destIn, sa_out);
        ls->node_start(ls, sampledAudio, (ls_Seconds) { 0.f });
        ls->node_set_on_ended(ls, sampledAudio, san_on_ended);

        demo_sleep(ls, 20);
        printf("ended %s\n", ended_via_flag ? "properly via flag" : "improperly by timeout");
    }

    printf("test: all complete\n");
    return 0;
}
