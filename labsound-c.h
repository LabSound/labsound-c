
#define LS_IMPL

#ifndef LABSOUNDC_H
#define LABSOUNDC_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

/*
LabSound C
Copyright 2022 Nick Porcino & the LabSound Authors

LabSound C provides a uniform functional interface to the
LabSound library. LabSound itself is written in C++, and has
a highly polymorphic API meant to emulate close the WebAudio
interfaces. That's very handy for transliterating WebAudio code,
but less friendly for native prograaming. This API exposes
all of the functionality of LabSound, but reduces the interface
surface to a few dozen function calls.

Under the hood it maintains a full database of LabSound objects
however in order to avoid the need to expose object life cycles
outside of the implementation, all the objects are tracked using
FLECS, and managed transparently to the user. Since the 
LabSound C interface itself is pass by object, all of the complexity
of object ownership is bypassed since a user of the interface
will never have a copy of anything owned by LabSound.

The only exception is the API object itself, the user is responsible
for its lifetime, and the interface is only usable after the
api object is created, and before it is destroyed.

The WebAudio interface involves a certain amount of set up,
occasionally value setting, and tear down. Other than that, it's
normal to simply let a graph run. As such, the fact that the
LabSound C interface goes through a FLECS hash to find the
backing objects for API application is not particularly of
concern, as it won't be a run time overhead, only a set up
overhead.
 */


// LSNAMESPACE is allows the introduction of a namespace to the symbols so that 
// multiple libraries can include the labsound C library without symbol 
// conflicts. The default is ls_1_0_ to indicate the 1.0 version of LabSound.
//
#ifndef LSNAMESPACE
#define LSNAMESPACE ls_1_0_
#endif

// The LSCONCAT macro is used to apply a namespace to the symbols in the public
// interface.
#define LSCONCAT1(a, b) a ## b
#define LSCONCAT(a, b) LSCONCAT1(a, b)

// LSAPI may be overridden externally to control symbol visibility.
#ifndef LSAPI
#define LSAPI
#endif

#ifdef __cplusplus
#define LSEXTERNC extern "C" LSAPI
#else
#define LSEXTERNC extern LSAPI
#endif


#ifdef __cplusplus
extern "C" {
#endif

// forward declare the LabSound C types in a namespace
#define ls_StringSlice LSCONCAT(LSNAMESPACE, StringSlice)
#define ls_NameArray LSCONCAT(LSNAMESPACE, NameArray)
#define ls_Seconds LSCONCAT(LSNAMESPACE, Seconds)
#define ls_InputPin LSCONCAT(LSNAMESPACE, InputPin)
#define ls_OutputPin LSCONCAT(LSNAMESPACE, OutputPin)
#define ls_Node LSCONCAT(LSNAMESPACE, Node)
#define ls_Connection LSCONCAT(LSNAMESPACE, Connection)
#define ls_BusData LSCONCAT(LSNAMESPACE, BusData)
#define ls_PinKind LSCONCAT(LSNAMESPACE, PinKind)
#define ls_PinDataType LSCONCAT(LSNAMESPACE, PinDataType)


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

#define ls_APIDetail LSCONCAT(LSNAMESPACE, APIDetail)
struct ls_APIDetail;
#define ls_API LSCONCAT(LSNAMESPACE, API)

LSAPI
struct ls_API {
    struct ls_APIDetail* _detail;

    // operations on nodes
    //
    ls_Seconds (*node_get_timing)(struct ls_API*,
        ls_Node);

    ls_Seconds (*node_get_self_timing)(struct ls_API*,
        ls_Node);

    void (*node_diagnose)(struct ls_API*,
                          ls_Node);
    
    void (*node_start)(struct ls_API*,
        ls_Node, ls_Seconds);

    void (*node_schedule)(struct ls_API*,
        ls_Node, ls_Seconds, int32_t);
    
    const char* (*node_scheduled_state_name)(struct ls_API*,
        ls_Node);

    void (*node_stop)(struct ls_API*,
        ls_Node, ls_Seconds);

    // getting pins from nodes
    //
    ls_InputPin (*node_named_input)(struct ls_API*,
        ls_Node, ls_StringSlice);

    ls_InputPin (*node_indexed_input)(struct ls_API*,
        ls_Node, int);

    ls_OutputPin (*node_named_output)(struct ls_API*,
        ls_Node, ls_StringSlice);

    ls_OutputPin (*node_indexed_output)(struct ls_API*,
        ls_Node, int);

    ls_InputPin (*node_parameter)(struct ls_API*,
        ls_Node, ls_StringSlice);

    ls_InputPin (*node_setting)(struct ls_API*,
        ls_Node, ls_StringSlice);

    void (*node_set_on_ended)(struct ls_API*,
        ls_Node, void(*)());

    ls_PinKind (*pin_kind)(struct ls_API*,
        ls_InputPin);

    ls_PinDataType (*pin_data_type)(struct ls_API*,
        ls_InputPin);

    // managing nodes
    //
    const ls_NameArray* (*node_names)(struct ls_API*);

    ls_Node (*node_create)(struct ls_API*, 
        ls_StringSlice name, ls_StringSlice type);

    void (*node_delete)(struct ls_API*,
        ls_Node);

    void (*create_node_output)(struct ls_API*,
        ls_Node, ls_StringSlice name, int channels);
    
    // setting and getting pin values
    //
    void (*set_float)(struct ls_API*,
        ls_InputPin, float);

    void (*set_enum)(struct ls_API*,
        ls_InputPin, uint32_t);

    void (*set_int)(struct ls_API*,
        ls_InputPin, uint32_t);

    void (*set_bool)(struct ls_API*,
        ls_InputPin, bool);

    void (*set_bus)(struct ls_API*,
        ls_InputPin, ls_BusData);
   
    void (*set_bus_from_file)(struct ls_API*,
        ls_InputPin, ls_StringSlice path);

    void (*set_named_enum)(struct ls_API*,
        ls_InputPin, ls_StringSlice enum_name);

    // managing busses
    ls_BusData (*bus_create_from_file)(struct ls_API*,
        const char* path, bool mix_to_mono);

    // graph management
    //
    ls_Node(*destination_node)(struct ls_API*);

    ls_Connection (*connect)(struct ls_API*,
        ls_InputPin input, ls_OutputPin output);

    // after disconnection, ls_Connection will no longer be valid
    void (*disconnect)(struct ls_API*,
        ls_Connection);
};

typedef struct {
    void* (*malloc)(size_t);
    void (*free)(void*);
} ls_Alloc;
const ls_Alloc ls_default_alloc = { malloc, free };

struct ls_API* ls_create_api_1_0(ls_Alloc);
void ls_release_api_1_0(struct ls_API*);
void ls_idle(struct ls_API*);

typedef struct ls_API LabSoundAPI;

#ifdef __cplusplus
} // extern "C"

/*

This is a cpp wrapper on the labsound c interface.

Of course LabSound itself is written in C++, but it's a highly polymorphic
interface meant to emulate the WebAudio interface. The labsound-c interfaces
eschew polymorphism for a simple functional interface. This interface lends
itself well to binding by other languages, by design. Some languages wrap
very well on C style interfaces, and others wrap very well on C++ style
interfaces. The C++ interface here is meant to be that interface.

 */

#include <string>
#include <vector>

#define LabSoundNS LSCONCAT(LSNAMESPACE, NameSpace)

namespace LabSoundNS {
class Node;

LSAPI
class BusData {
public:
    BusData(LabSoundAPI* api, ls_BusData bus)
        : api(api), bus(bus) {}

    BusData(LabSoundAPI* api, const std::string& path, bool mix_to_mono=false)
        : api(api) {
        bus = api->bus_create_from_file(api, path.c_str(), mix_to_mono);
    }

    bool is_valid() const {
        return bus.id != ls_BusData_empty.id;
    }

private:
    friend class InputPin;
    LabSoundAPI* api;
    ls_BusData bus;
};

class InputPin {
public:
    InputPin(LabSoundAPI* api, ls_InputPin pin)
        : api(api), pin(pin) {}

    bool is_valid() const {
        return pin.id != ls_InputPin_empty.id;
    }

    ls_PinKind get_kind() const {
        return api->pin_kind(api, pin);
    }

    ls_PinDataType get_data_type() const {
        return api->pin_data_type(api, pin);
    }

    void set_float(float v) const {
        api->set_float(api, pin, v);
    }

    void set_enum(uint32_t v) const {
        api->set_enum(api, pin, v);
    }

    void set_int(uint32_t v) const {
        api->set_int(api, pin, v);
    }

    void set_bool(bool v) const {
        api->set_bool(api, pin, v);
    }

    void set_bus(BusData v) const {
        api->set_bus(api, pin, v.bus);
    }

    void set_bus_from_file(const std::string& v) {
        ls_StringSlice slice = { v.c_str(), v.c_str() + v.size() };
        api->set_bus_from_file(api, pin, slice);
    }

    void set_named_enum(const std::string& v) {
        ls_StringSlice slice = { v.c_str(), v.c_str() + v.size() };
        api->set_bus_from_file(api, pin, slice);
    }

private:
    friend class LabSoundAPIWrapper;
    LabSoundAPI* api;
    ls_InputPin pin;
};

class OutputPin {
public:
    OutputPin(LabSoundAPI* api, ls_OutputPin pin)
        : api(api), pin(pin) {}

    bool is_valid() const {
        return pin.id != ls_OutputPin_empty.id;
    }

private:
    friend class LabSoundAPIWrapper;
    LabSoundAPI* api;
    ls_OutputPin pin;
};

class Connection {
public:
    Connection(LabSoundAPI* api, ls_Connection connection)
        : api(api), connection(connection) {}

    bool is_valid() const {
        return connection.id != ls_Connection_empty.id;
    }

    void disconnect() {
        if (is_valid()) {
            api->disconnect(api, connection);
            connection = ls_Connection_empty;
        }
    }

private:
    LabSoundAPI* api;
    ls_Connection connection;
};

class Node {
public:
    Node(LabSoundAPI* api, ls_Node node) 
        : api(api), node(node) {}

    ~Node() {
        api->node_delete(api, node);
    }

    ls_Seconds get_timing() const {
        return api->node_get_timing(api, node);
    }

    ls_Seconds get_self_timing() const {
        return api->node_get_self_timing(api, node);
    }

    void diagnose() {
        api->node_diagnose(api, node);
    }

    void start(ls_Seconds time) {
        api->node_start(api, node, time);
    }

    void schedule(ls_Seconds time, int32_t count) {
        api->node_schedule(api, node, time, count);
    }

    const char* scheduled_state_name() {
        return api->node_scheduled_state_name(api, node);
    }

    void stop(ls_Seconds time) {
        api->node_stop(api, node, time);
    }

    InputPin named_input(const std::string& name) const {
        ls_StringSlice name_slice = { name.c_str(), name.c_str() + name.size() };
        ls_InputPin input_pin = api->node_named_input(api, node, name_slice);
        return InputPin(api, input_pin);
    }

    InputPin indexed_input(int index) const {
        ls_InputPin input_pin = api->node_indexed_input(api, node, index);
        return InputPin(api, input_pin);
    }

    OutputPin named_output(const std::string& name) const {
        ls_StringSlice name_slice = { name.c_str(), name.c_str() + name.size() };
        ls_OutputPin output_pin = api->node_named_output(api, node, name_slice);
        return OutputPin(api, output_pin);
    }

    OutputPin indexed_output(int index) const {
        ls_OutputPin output_pin = api->node_indexed_output(api, node, index);
        return OutputPin(api, output_pin);
    }

    InputPin parameter(const std::string& name) const {
        ls_StringSlice name_slice = { name.c_str(), name.c_str() + name.size() };
        ls_InputPin input_pin = api->node_parameter(api, node, name_slice);
        return InputPin(api, input_pin);
    }

    InputPin setting(const std::string& name) const {
        ls_StringSlice name_slice = { name.c_str(), name.c_str() + name.size() };
        ls_InputPin input_pin = api->node_setting(api, node, name_slice);
        return InputPin(api, input_pin);
    }
/*
    void set_on_ended(nb::callable callback) {
        api->node_set_on_ended(api, node, [callback]() {
            // Call the Python function inside the C callback
            nb::gil_scoped_acquire acquire;
            callback();
        });
    }
*/
    void create_output(const std::string& name, int channels) {
        ls_StringSlice name_slice = { name.c_str(), name.c_str() + name.size() };
        api->create_node_output(api, node, name_slice, channels);
    }

private:
    LabSoundAPI* api;
    ls_Node node;
};

class LabSoundAPIWrapper {
public:
    LabSoundAPIWrapper() 
    : api(ls_create_api_1_0(ls_default_alloc)) {
    }

    ~LabSoundAPIWrapper() {
        ls_release_api_1_0(api);
    }

    // Factory method to create Node
    Node create_node(const std::string& name, const std::string& type) {
        ls_StringSlice name_slice = { name.c_str(), name.c_str() + name.size() };
        ls_StringSlice type_slice = { type.c_str(), type.c_str() + type.size() };
        ls_Node node = api->node_create(api, name_slice, type_slice);
        return Node(api, node);
    }

    std::vector<std::string> node_names() {
        static std::vector<std::string> names;
        if (!names.size()) {
            const ls_NameArray* name_array = api->node_names(api);
            for (size_t i = 0; i < name_array->count; i++) {
                names.push_back(std::string(name_array->names[i].start, name_array->names[i].end));
            }
        }
        return names;
    }

    LabSoundAPI* get() const { return api; }

    Node destination_node() {
        ls_Node node = api->destination_node(api);
        return Node(api, node);
    }
    
    Connection connect(InputPin input, OutputPin output) {
        ls_Connection connection = api->connect(api, input.pin, output.pin);
        return Connection(api, connection);
    }

    void disconnect(Connection connection) {
        connection.disconnect();
    }

private:
    LabSoundAPI* api;
};

} // namespace LabSoundNS

#endif

#endif // LABSOUNDC_H
