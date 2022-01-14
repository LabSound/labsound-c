
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
    char const* start;
    char const* end;
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
} ls_Pin;
const ls_Pin ls_Pin_empty = { 0 };

typedef struct {
    uint64_t id;
} ls_Node;
const ls_Node ls_Node_empty = { 0 };

typedef struct {
    uint64_t id;
} ls_Connection;
const ls_Connection ls_Connection_empty = { 0 };

typedef enum {
    ls_PinInvalid = 0, 
    ls_PinInput, ls_PinOutput, ls_PinParam, ls_PinSetting
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

    void (*node_start)(struct LabSoundAPI_1_0*,
        ls_Node, ls_Seconds);

    void (*node_stop)(struct LabSoundAPI_1_0*,
        ls_Node, ls_Seconds);

    // getting pins from nodes
    //
    ls_Pin (*node_named_input)(struct LabSoundAPI_1_0*,
        ls_Node, ls_StringSlice);

    ls_Pin (*node_indexed_input)(struct LabSoundAPI_1_0*,
        ls_Node, int);

    ls_Pin (*node_named_output)(struct LabSoundAPI_1_0*,
        ls_Node, ls_StringSlice);

    ls_Pin (*node_indexed_output)(struct LabSoundAPI_1_0*,
        ls_Node, int);

    ls_Pin (*node_parameter)(struct LabSoundAPI_1_0*,
        ls_Node, ls_StringSlice);

    ls_Pin (*node_setting)(struct LabSoundAPI_1_0*,
        ls_Node, ls_StringSlice);

    ls_PinKind (*pin_kind)(struct LabSoundAPI_1_0*,
        ls_Pin);
    
    ls_PinDataType (*pin_data_type)(struct LabSoundAPI_1_0*,
        ls_Pin);

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
        ls_Pin, float);

    void (*set_enum)(struct LabSoundAPI_1_0*,
        ls_Pin, uint32_t);

    void (*set_int)(struct LabSoundAPI_1_0*,
        ls_Pin, uint32_t);

    void (*set_bool)(struct LabSoundAPI_1_0*,
        ls_Pin, bool);
    
    void (*set_bus_from_file)(struct LabSoundAPI_1_0*,
        ls_Pin, ls_StringSlice path);

    void (*set_named_enum)(struct LabSoundAPI_1_0*,
        ls_Pin, ls_StringSlice enum_name);

    // graph management
    //
    ls_Node(*device_node)(struct LabSoundAPI_1_0*);

    ls_Connection (*connect_output_to_input)(struct LabSoundAPI_1_0*,
        ls_Pin input, ls_Pin output);

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
