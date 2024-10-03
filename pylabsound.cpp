#include <nanobind/nanobind.h>
#include "labsound-c.h"
#include <string>
#include <vector>

namespace nb = nanobind;
using namespace LabSoundNS;


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
    //.def("set_on_ended", &Node::set_on_ended)
    .def("create_output", &Node::create_output);

nb::class_<Connection>(m, "Connection")
    .def(nb::init<LabSoundAPI*, ls_Connection>())
    .def("is_valid", &Connection::is_valid)
    .def("disconnect", &Connection::disconnect);

nb::class_<InputPin>(m, "InputPin")
    .def(nb::init<LabSoundAPI*, ls_InputPin>())
    .def("is_valid", &InputPin::is_valid)
    .def("get_kind", &InputPin::get_kind)
    .def("get_data_type", &InputPin::get_data_type)
    .def("set_float", &InputPin::set_float)
    .def("set_enum", &InputPin::set_enum)
    .def("set_int", &InputPin::set_int)
    .def("set_bool", &InputPin::set_bool)
    .def("set_bus", &InputPin::set_bus)
    .def("set_bus_from_file", &InputPin::set_bus_from_file)
    .def("set_named_enum", &InputPin::set_named_enum);

nb::class_<OutputPin>(m, "OutputPin")
    .def(nb::init<LabSoundAPI*, ls_OutputPin>())
    .def("is_valid", &OutputPin::is_valid);

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
