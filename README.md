# LabSound C bindings

These C bindings are compatible with top of tree LabSound.

The beginnings of a Python wrapper is included, but not
yet compiling. Help welcome :)

copyright (c) 2022- Nick Porcino
MIT License

## Getting Started

To build the bindings and demo with an existing LabSound installation, set
`-DCMAKE_SYSTEM_PREFIX=/path/to/installRoot`. If this path is not provided,
the build will automatically fetch LabSound.

To use the bindings in your own project, include labsound-c.cpp into your 
project directly, and also link to libnyquist and LabSound.

## Interface

Although labsound-c.cpp is implemented in C++, the interface in labsound-c.h 
exposes symbols for C.

Strings are supplied to labsound-c via string slices. These are compatible with
all languages you are likely to bind LabSound to, as there is no requirement 
that the string be zero terminated.

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

For cstd users, there is a convenience function ~ 

```c
#define ls_cstr(s) ls_StringSlice { (s), strlen(s) }
```

Time is passed in a simple struct to force a descriptive type ~

```c
typedef struct {
    float t;
} ls_Seconds;
```

There are a variety of opaque LabSound C objects that the labsound-c
interfaces will consume.

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

There are also some housekeeping routines. You'll need to create and release 
the ls_API interface object, and once per frame (eg. once a game frame at
1/60s) call `ls_idle` to give the engine a chance to do various tasks.

```c

typedef struct {
    void* (*malloc)(size_t);
    void (*free)(void*);
} ls_Alloc;
const ls_Alloc ls_default_alloc = { malloc, free };

struct ls_API* ls_create_api_1_0(ls_Alloc);
void ls_release_api_1_0(struct ls_API*);
void ls_idle(struct ls_API*);
```

The LabSound C interface differs greatly from the C++ interface. The C++ 
interface is an object oriented API meant to mimic the WebAudio specification's 
interfaces as closely as possible. The C interfaces however, are opaque, and 
not object oriented.

Nodes are created by name, and managed generically.

Inputs and outputs from a node are accessed from pins, as are the node's properties.

These are all built around generic interfaces that rely on LabSound's additional 
C++ interfaces for querying capabilities and attributes.

Please refer to the demo for usage examples.

```c
    // scheduling nodes
    ls_Seconds (*node_get_timing)(struct ls_API*, ls_Node);
    ls_Seconds (*node_get_self_timing)(struct ls_API*, ls_Node);
    void (*node_start)(struct ls_API*, ls_Node, ls_Seconds);
    void (*node_schedule)(struct ls_API*, ls_Node, ls_Seconds, int32_t);
    void (*node_stop)(struct ls_API*, ls_Node, ls_Seconds);

    // managing nodes
    const ls_NameArray* (*node_names)(struct ls_API*);
    ls_Node (*node_create)(struct ls_API*, ls_StringSlice name, ls_StringSlice type);
    void (*node_delete)(struct ls_API*, ls_Node);
    void (*create_node_output)(struct ls_API*, ls_Node, ls_StringSlice name, int channels);
    void (*node_set_on_ended)(struct ls_API*, ls_Node, void(*)());

    // getting pins from nodes
    ls_Pin (*node_named_input)(struct ls_API*, ls_Node, ls_StringSlice);
    ls_Pin (*node_indexed_input)(struct ls_API*, ls_Node, int);
    ls_Pin (*node_named_output)(struct ls_API*, ls_Node, ls_StringSlice);
    ls_Pin (*node_indexed_output)(struct ls_API*, ls_Node, int);
    ls_Pin (*node_parameter)(struct ls_API*, ls_Node, ls_StringSlice);
    ls_Pin (*node_setting)(struct ls_API*, ls_Node, ls_StringSlice);

    // information about pins
    ls_PinKind (*pin_kind)(struct ls_API*, ls_Pin);
    ls_PinDataType (*pin_data_type)(struct ls_API*, ls_Pin);

    // setting and getting pin values
    // note - these interfaces are going to be prefixed with pin_
    void (*set_float)(struct ls_API*, ls_Pin, float);
    void (*set_enum)(struct ls_API*, ls_Pin, uint32_t);
    void (*set_int)(struct ls_API*, ls_Pin, uint32_t);
    void (*set_bool)(struct ls_API*, ls_Pin, bool);
    void (*set_bus)(struct ls_API*, ls_Pin, ls_BusData);
    void (*set_bus_from_file)(struct ls_API*, ls_Pin, ls_StringSlice path);
    void (*set_named_enum)(struct ls_API*, ls_Pin, ls_StringSlice enum_name);

    // managing busses
    ls_BusData (*bus_create_from_file)(struct ls_API*, const char* path, bool mix_to_mono);

    // graph management
    // note - device_node is going to be renamed destination_node
    ls_Node(*device_node)(struct ls_API*);
    ls_Connection (*connect_output_to_input)(struct ls_API*, ls_Pin input, ls_Pin output);

    // after disconnection, ls_Connection will no longer be valid
    void (*disconnect)(struct ls_API*, ls_Connection);
```
