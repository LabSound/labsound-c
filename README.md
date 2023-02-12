# LabSound C bindings

These bindings are compatible with top of tree LabSound. They will also work with upcoming release 1.3.0.

copyright (c) 2022- Nick Porcino
MIT License

## Getting Started

To build the demo, install LabSound. Installing LabSound will also install libnyquist. Use cmake to configure this project, with a CMAKE_SYSTEM_PREFIX set to the install prefix used for LabSound.

To use the bindings in your own project, include labsound-c.cpp into your project directly, and also link to libnyquist and LabSound.

## Interface

Strings are supplied to labsound-c via string slices. These are compatible with all languages you are likely to bind LabSound to, as there is no requirement that the string be zero terminated.

Although labsound-c.cpp is implemented in C++, the interface in labsound-c.h exposes symbols for C.

```c
typedef struct {
    const char* start;
    const char* end;
} ls_StringSlice;
```

There's an array of string slices ~

```c
typedef struct {
    ls_StringSlice* names;
    int count;
} ls_NameArray;
```

For C users, there is a convenience function ~ 
```c
#define ls_cstr(s) ls_StringSlice { (s), strlen(s) }
```

Time is passed in a simple struct ~

```c
typedef struct {
    float t;
} ls_Seconds;
```

There are a variety of opaque lsc objects that the labsound-c interfaces will consume.

```c
struct ls_Pin, ls_Node, ls_Connection, ls_BusData;
```

There are a few enumerations ~

```c
typedef enum {
    ls_PinInvalid = 0, 
    ls_PinInput, ls_PinOutput, ls_PinParam, ls_PinSetting
} ls_PinKind;

typedef enum {
    ls_Invalid = 0, 
    ls_String, ls_Path, ls_Bool, ls_Int, ls_Float, ls_Bus, ls_Enum
} ls_PinDataType;
```

There are some housekeeping routines. You'll need to create and release the LabSoundAPI_1_0 interface object, and once per frame call `ls_idle` to give the engine a chance to do various tasks.

```c

typedef struct {
    void* (*malloc)(size_t);
    void (*free)(void*);
} ls_Alloc;
const ls_Alloc ls_default_alloc = { malloc, free };

struct LabSoundAPI_1_0* ls_create_api_1_0(ls_Alloc);
void ls_release_api_1_0(struct LabSoundAPI_1_0*);
void ls_idle(struct LabSoundAPI_1_0*);
```

The interface differs from the C++ interface. The C++ interface is an object oriented API meant to mimic the WebAudio specification's interfaces as closely as possible. The C interfaces however, are opaque, and not object oriented.

Inputs and outputs from a node are accessed from pins, as are the node's properties.

Instead, they are built around generic interfaces that rely on LabSounds additional C++ interfaces for querying capabilities and attributes.

Please refer to the demo for usage examples.

```c
    // scheduling nodes
    ls_Seconds (*node_get_timing)(struct LabSoundAPI_1_0*, ls_Node);
    ls_Seconds (*node_get_self_timing)(struct LabSoundAPI_1_0*, ls_Node);
    void (*node_start)(struct LabSoundAPI_1_0*, ls_Node, ls_Seconds);
    void (*node_schedule)(struct LabSoundAPI_1_0*, ls_Node, ls_Seconds, int32_t);
    void (*node_stop)(struct LabSoundAPI_1_0*, ls_Node, ls_Seconds);

    // managing nodes
    const ls_NameArray* (*node_names)(struct LabSoundAPI_1_0*);
    ls_Node (*node_create)(struct LabSoundAPI_1_0*, ls_StringSlice name, ls_StringSlice type);
    void (*node_delete)(struct LabSoundAPI_1_0*, ls_Node);
    void (*create_node_output)(struct LabSoundAPI_1_0*, ls_Node, ls_StringSlice name, int channels);
    void (*node_set_on_ended)(struct LabSoundAPI_1_0*, ls_Node, void(*)());

    // getting pins from nodes
    ls_Pin (*node_named_input)(struct LabSoundAPI_1_0*, ls_Node, ls_StringSlice);
    ls_Pin (*node_indexed_input)(struct LabSoundAPI_1_0*, ls_Node, int);
    ls_Pin (*node_named_output)(struct LabSoundAPI_1_0*, ls_Node, ls_StringSlice);
    ls_Pin (*node_indexed_output)(struct LabSoundAPI_1_0*, ls_Node, int);
    ls_Pin (*node_parameter)(struct LabSoundAPI_1_0*, ls_Node, ls_StringSlice);
    ls_Pin (*node_setting)(struct LabSoundAPI_1_0*, ls_Node, ls_StringSlice);

    // information about pins
    ls_PinKind (*pin_kind)(struct LabSoundAPI_1_0*, ls_Pin);
    ls_PinDataType (*pin_data_type)(struct LabSoundAPI_1_0*, ls_Pin);

    // setting and getting pin values
    // note - these interfaces are going to be prefixed with pin_
    void (*set_float)(struct LabSoundAPI_1_0*, ls_Pin, float);
    void (*set_enum)(struct LabSoundAPI_1_0*, ls_Pin, uint32_t);
    void (*set_int)(struct LabSoundAPI_1_0*, ls_Pin, uint32_t);
    void (*set_bool)(struct LabSoundAPI_1_0*, ls_Pin, bool);
    void (*set_bus)(struct LabSoundAPI_1_0*, ls_Pin, ls_BusData);
    void (*set_bus_from_file)(struct LabSoundAPI_1_0*, ls_Pin, ls_StringSlice path);
    void (*set_named_enum)(struct LabSoundAPI_1_0*, ls_Pin, ls_StringSlice enum_name);

    // managing busses
    ls_BusData (*bus_create_from_file)(struct LabSoundAPI_1_0*, const char* path, bool mix_to_mono);

    // graph management
    // note - device_node is going to be renamed destination_node
    ls_Node(*device_node)(struct LabSoundAPI_1_0*);
    ls_Connection (*connect_output_to_input)(struct LabSoundAPI_1_0*, ls_Pin input, ls_Pin output);

    // after disconnection, ls_Connection will no longer be valid
    void (*disconnect)(struct LabSoundAPI_1_0*, ls_Connection);
```
