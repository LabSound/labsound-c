#include <nanobind/nanobind.h>
#include "labsound_c_api.h"

namespace nb = nanobind;

class Node;

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


class LabSoundAPIWrapper {
public:
    LabSoundAPIWrapper() : api(ls_create_api_1_0(ls_default_alloc)) {}

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
        std::vector<std::string> names;
        ls_StringSliceArray name_array = api->node_names(api);
        for (size_t i = 0; i < name_array.size; i++) {
            names.push_back(std::string(name_array.data[i].begin, name_array.data[i].end));
        }
        return names;
    }

    LabSoundAPI* get() const { return api; }

    Node destination_node() {
        ls_Node node = api->destination_node(api);
        return Node(api, node);
    }
    
    Connection connect(InputPin input, OutputPin output) {
        ls_Connection connection = api->connect(api, input, output);
        return Connection(api, connection);
    }

    void disconnect(Connection connection) {
        connection.disconnect();
    }

private:
    LabSoundAPI* api;
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

    // Additional methods as needed for InputPin...

private:
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

    ls_PinKind get_kind() const {
        return api->pin_kind(api, pin);
    }

    ls_PinDataType get_data_type() const {
        return api->pin_data_type(api, pin);
    }

    void set_float(float value) {
        api->set_float(api, pin, value);
    }

    void set_enum(uint32_t value) {
        api->set_enum(api, pin, value);
    }

    void set_int(uint32_t value) {
        api->set_int(api, pin, value);
    }

    void set_bool(bool value) {
        api->set_bool(api, pin, value);
    }

    void set_bus(ls_BusData value) {
        api->set_bus(api, pin, value);
    }

    void set_bus_from_file(const std::string& path) {
        ls_StringSlice path_slice = { path.c_str(), path.c_str() + path.size() };
        api->set_bus_from_file(api, pin, path_slice);
    }

    void set_named_enum(const std::string& name, uint32_t value) {
        ls_StringSlice name_slice = { name.c_str(), name.c_str() + name.size() };
        api->set_named_enum(api, pin, name_slice, value);
    }

private:
    LabSoundAPI* api;
    ls_OutputPin pin;
};

class BusData {
public:
    BusData(LabSoundAPI* api, ls_BusData bus)
        : api(api), bus(bus) {}

    BusData(LabSoundAPI* api, const std::string& path, bool mix_to_mono=false)
        : api(api) {
        ls_StringSlice path_slice = { path.c_str(), path.c_str() + path.size() };
        bus = api->bus_create_from_file(api, path_slice, mix_to_mono);
    }

    bool is_valid() const {
        return bus.id != ls_BusData_empty.id;
    }

    // Additional methods can be added to interact with the bus

private:
    LabSoundAPI* api;
    ls_BusData bus;
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

    void set_on_ended(nb::callable callback) {
        api->node_set_on_ended(api, node, []() {
            // Call the Python function inside the C callback
            nb::gil_scoped_acquire acquire;
            callback();
        });
    }

    void create_output(const std::string& name, int channels) {
        ls_StringSlice name_slice = { name.c_str(), name.c_str() + name.size() };
        api->create_node_output(api, node, name_slice, channels);
    }

private:
    LabSoundAPI* api;
    ls_Node node;
};

NB_MODULE(LabSound, m) {

nb::class_<LabSoundAPIWrapper>(m, "LabSoundAPI")
    .def(nb::init<>())
    .def("create_node", &LabSoundAPIWrapper::create_node)
    .def("node_names", &LabSoundAPIWrapper::node_names)
    .def("destination_node", &LabSoundAPIWrapper::destination_node)
    .def("connect", &LabSoundAPIWrapper::connect)
    .def("disconnect", &LabSoundAPIWrapper::disconnect);

nb::class_<Node>(m, "Node")
    .def(nb::init<LabSoundAPI*, ls_Node>())
    .def("get_timing", &Node::get_timing)
    .def("get_self_timing", &Node::get_self_timing)
    .def("diagnose", &Node::diagnose)
    .def("start", &Node::start)
    .def("schedule", &Node::schedule)
    .def("scheduled_state_name", &Node::scheduled_state_name)
    .def("stop", &Node::stop)
    .def("named_input", &Node::named_input)
    .def("indexed_input", &Node::indexed_input)
    .def("named_output", &Node::named_output)
    .def("indexed_output", &Node::indexed_output)
    .def("parameter", &Node::parameter)
    .def("setting", &Node::setting)
    .def("set_on_ended", &Node::set_on_ended)
    .def("create_output", &Node::create_output);

nb::class_<Connection>(m, "Connection")
    .def(nb::init<LabSoundAPI*, ls_Connection>())
    .def("is_valid", &Connection::is_valid)
    .def("disconnect", &Connection::disconnect);

nb::class_<InputPin>(m, "InputPin")
    .def(nb::init<LabSoundAPI*, ls_InputPin>())
    .def("is_valid", &InputPin::is_valid)
    .def("get_kind", &InputPin::get_kind)
    .def("get_data_type", &InputPin::get_data_type);

nb::class_<OutputPin>(m, "OutputPin")
    .def(nb::init<LabSoundAPI*, ls_OutputPin>())
    .def("is_valid", &OutputPin::is_valid)
    .def("get_kind", &OutputPin::get_kind)
    .def("get_data_type", &OutputPin::get_data_type)
    .def("set_float", &OutputPin::set_float)
    .def("set_enum", &OutputPin::set_enum)
    .def("set_int", &OutputPin::set_int)
    .def("set_bool", &OutputPin::set_bool)
    .def("set_bus", &OutputPin::set_bus)
    .def("set_bus_from_file", &OutputPin::set_bus_from_file)
    .def("set_named_enum", &OutputPin::set_named_enum);

nb::enum_<ls_PinKind>(m, "PinKind")
    .value("Invalid", ls_PinKindInvalid)
    .value("Input", ls_PinKindInput)
    .value("Output", ls_PinKindOutput)
    .value("Param", ls_PinKindParam)
    .value("Setting", ls_PinKindSetting);

nb::enum_<ls_PinDataType>(m, "PinDataType")
    .value("Invalid", ls_Invalid)
    .value("String", ls_String)
    .value("Path", ls_Path)
    .value("Bool", ls_Bool)
    .value("Int", ls_Int)
    .value("Float", ls_Float)
    .value("Bus", ls_Bus)
    .value("Enum", ls_Enum);
}
