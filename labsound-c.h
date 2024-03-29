
#define LS_IMPL

#ifndef LABSOUNDC_H
#define LABSOUNDC_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    const char* start;
    const char* end;
} ls_StringSlice;

#define ls_cstr(s) ls_StringSlice { (s), strlen(s) }

typedef struct {
    ls_StringSlice* names;
    int count;
} ls_NameArray;

typedef struct {
    float t;
} ls_Seconds;

typedef struct {
    uint64_t id;
} ls_InputPin;
const ls_InputPin ls_InputPin_empty = { 0 };
typedef struct {
    uint64_t id;
} ls_OutputPin;
const ls_OutputPin ls_OutputPin_empty = { 0 };

typedef struct {
    uint64_t id;
} ls_Node;
const ls_Node ls_Node_empty = { 0 };

typedef struct {
    uint64_t id;
} ls_Connection;
const ls_Connection ls_Connection_empty = { 0 };

typedef struct {
    uint64_t id;
} ls_BusData;
const ls_BusData ls_BusData_empty = { 0 };

typedef enum {
    ls_PinKindInvalid = 0,
    ls_PinKindInput, ls_PinKindOutput, ls_PinKindParam, ls_PinKindSetting
} ls_PinKind;

typedef enum {
    ls_Invalid = 0, 
    ls_String, ls_Path, ls_Bool, ls_Int, ls_Float, ls_Bus, ls_Enum
} ls_PinDataType;

struct LabSoundAPI_1_0_Detail;
struct LabSoundAPI_1_0 {
    struct LabSoundAPI_1_0_Detail* _detail;

    // operations on nodes
    //
    ls_Seconds (*node_get_timing)(struct LabSoundAPI_1_0*,
        ls_Node);

    ls_Seconds (*node_get_self_timing)(struct LabSoundAPI_1_0*,
        ls_Node);

    void (*node_diagnose)(struct LabSoundAPI_1_0*,
                          ls_Node);
    
    void (*node_start)(struct LabSoundAPI_1_0*,
        ls_Node, ls_Seconds);

    void (*node_schedule)(struct LabSoundAPI_1_0*,
        ls_Node, ls_Seconds, int32_t);
    
    const char* (*node_scheduled_state_name)(struct LabSoundAPI_1_0*,
        ls_Node);

    void (*node_stop)(struct LabSoundAPI_1_0*,
        ls_Node, ls_Seconds);

    // getting pins from nodes
    //
    ls_InputPin (*node_named_input)(struct LabSoundAPI_1_0*,
        ls_Node, ls_StringSlice);

    ls_InputPin (*node_indexed_input)(struct LabSoundAPI_1_0*,
        ls_Node, int);

    ls_OutputPin (*node_named_output)(struct LabSoundAPI_1_0*,
        ls_Node, ls_StringSlice);

    ls_OutputPin (*node_indexed_output)(struct LabSoundAPI_1_0*,
        ls_Node, int);

    ls_InputPin (*node_parameter)(struct LabSoundAPI_1_0*,
        ls_Node, ls_StringSlice);

    ls_InputPin (*node_setting)(struct LabSoundAPI_1_0*,
        ls_Node, ls_StringSlice);

    void (*node_set_on_ended)(struct LabSoundAPI_1_0*,
        ls_Node, void(*)());

    ls_PinKind (*pin_kind)(struct LabSoundAPI_1_0*,
        ls_InputPin);

    ls_PinDataType (*pin_data_type)(struct LabSoundAPI_1_0*,
        ls_InputPin);

    // managing nodes
    //
    const ls_NameArray* (*node_names)(struct LabSoundAPI_1_0*);

    ls_Node (*node_create)(struct LabSoundAPI_1_0*, 
        ls_StringSlice name, ls_StringSlice type);

    void (*node_delete)(struct LabSoundAPI_1_0*,
        ls_Node);

    void (*create_node_output)(struct LabSoundAPI_1_0*,
        ls_Node, ls_StringSlice name, int channels);
    
    // setting and getting pin values
    //
    void (*set_float)(struct LabSoundAPI_1_0*,
        ls_InputPin, float);

    void (*set_enum)(struct LabSoundAPI_1_0*,
        ls_InputPin, uint32_t);

    void (*set_int)(struct LabSoundAPI_1_0*,
        ls_InputPin, uint32_t);

    void (*set_bool)(struct LabSoundAPI_1_0*,
        ls_InputPin, bool);

    void (*set_bus)(struct LabSoundAPI_1_0*,
        ls_InputPin, ls_BusData);
   
    void (*set_bus_from_file)(struct LabSoundAPI_1_0*,
        ls_InputPin, ls_StringSlice path);

    void (*set_named_enum)(struct LabSoundAPI_1_0*,
        ls_InputPin, ls_StringSlice enum_name);

    // managing busses
    ls_BusData (*bus_create_from_file)(struct LabSoundAPI_1_0*,
        const char* path, bool mix_to_mono);

    // graph management
    //
    ls_Node(*destination_node)(struct LabSoundAPI_1_0*);

    ls_Connection (*connect)(struct LabSoundAPI_1_0*,
        ls_InputPin input, ls_OutputPin output);

    // after disconnection, ls_Connection will no longer be valid
    void (*disconnect)(struct LabSoundAPI_1_0*,
        ls_Connection);
};

typedef struct {
    void* (*malloc)(size_t);
    void (*free)(void*);
} ls_Alloc;
const ls_Alloc ls_default_alloc = { malloc, free };

struct LabSoundAPI_1_0* ls_create_api_1_0(ls_Alloc);
void ls_release_api_1_0(struct LabSoundAPI_1_0*);
void ls_idle(struct LabSoundAPI_1_0*);

typedef struct LabSoundAPI_1_0 LabSoundAPI;

#ifdef __cplusplus
} // extern "C"
#endif

#endif // LABSOUNDC_H
