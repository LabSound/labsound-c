const labsound = @cImport(@embedFile("labsound-c.h"));
const c = @cImport({
    @cInclude("labsound-c.h");
});
const std = @import("std");

const BusData = struct {
    api: *LabSoundAPI,
    bus: ls_BusData,

    pub fn isValid(self: *BusData) bool {
        return self.bus.id != ls_BusData_empty.id;
    }

    pub fn createFromFile(api: *LabSoundAPI, path: []const u8, mixToMono: bool) !BusData {
        var bus = api.bus_create_from_file(api, path.ptr, mixToMono);
        if (bus.id == ls_BusData_empty.id) {
            return error.BusCreationFailed;
        }
        return BusData{ .api = api, .bus = bus };
    }
};

pub const InputPin = struct {
    api: *c.LabSoundAPI,
    pin: c.ls_InputPin,

    pub fn init(api: *c.LabSoundAPI, pin: c.ls_InputPin) InputPin {
        return InputPin{
            .api = api,
            .pin = pin,
        };
    }

    pub fn isValid(self: InputPin) bool {
        return self.pin.id != c.ls_InputPin_empty.id;
    }

    pub fn getKind(self: InputPin) c.ls_PinKind {
        return self.api.pin_kind(self.api, self.pin);
    }

    pub fn getDataType(self: InputPin) c.ls_PinDataType {
        return self.api.pin_data_type(self.api, self.pin);
    }

    pub fn setFloat(self: InputPin, value: f32) void {
        self.api.set_float(self.api, self.pin, value);
    }

    pub fn setEnum(self: InputPin, value: u32) void {
        self.api.set_enum(self.api, self.pin, value);
    }

    pub fn setInt(self: InputPin, value: u32) void {
        self.api.set_int(self.api, self.pin, value);
    }

    pub fn setBool(self: InputPin, value: bool) void {
        self.api.set_bool(self.api, self.pin, value);
    }

    pub fn setBus(self: InputPin, bus: BusData) void {
        self.api.set_bus(self.api, self.pin, bus.bus);
    }

    pub fn setBusFromFile(self: InputPin, file: []const u8) void {
        var slice: c.ls_StringSlice = c.ls_StringSlice{
            .start = file.ptr,
            .end = file.ptr + file.len,
        };
        self.api.set_bus_from_file(self.api, self.pin, slice);
    }

    pub fn setNamedEnum(self: InputPin, enumName: []const u8) void {
        var slice: c.ls_StringSlice = c.ls_StringSlice{
            .start = enumName.ptr,
            .end = enumName.ptr + enumName.len,
        };
        self.api.set_named_enum(self.api, self.pin, slice);
    }
};

pub const Connection = struct {
    api: *c.LabSoundAPI,
    connection: c.ls_Connection,

    pub fn init(api: *c.LabSoundAPI, connection: c.ls_Connection) Connection {
        return Connection{
            .api = api,
            .connection = connection,
        };
    }

    pub fn isValid(self: Connection) bool {
        return self.connection.id != c.ls_Connection_empty.id;
    }

    pub fn disconnect(self: *Connection) void {
        if (self.isValid()) {
            self.api.disconnect(self.api, self.connection);
            self.connection = c.ls_Connection_empty;
        }
    }
};


pub const Node = struct {
    api: *c.LabSoundAPI,
    node: c.ls_Node,

    pub fn init(api: *c.LabSoundAPI, node: c.ls_Node) Node {
        return Node{
            .api = api,
            .node = node,
        };
    }

    pub fn deinit(self: *Node) void {
        self.api.node_delete(self.api, self.node);
    }

    pub fn getTiming(self: Node) c.ls_Seconds {
        return self.api.node_get_timing(self.api, self.node);
    }

    pub fn getSelfTiming(self: Node) c.ls_Seconds {
        return self.api.node_get_self_timing(self.api, self.node);
    }

    pub fn diagnose(self: Node) void {
        self.api.node_diagnose(self.api, self.node);
    }

    pub fn start(self: Node, time: c.ls_Seconds) void {
        self.api.node_start(self.api, self.node, time);
    }

    pub fn schedule(self: Node, time: c.ls_Seconds, count: i32) void {
        self.api.node_schedule(self.api, self.node, time, count);
    }

    pub fn scheduledStateName(self: Node) ?[]const u8 {
        const state_name = self.api.node_scheduled_state_name(self.api, self.node);
        if (state_name == null) return null;
        return std.mem.sliceToString(state_name);
    }

    pub fn stop(self: Node, time: c.ls_Seconds) void {
        self.api.node_stop(self.api, self.node, time);
    }

    pub fn namedInput(self: Node, name: []const u8) InputPin {
        var name_slice = c.ls_StringSlice{
            .start = name.ptr,
            .end = name.ptr + name.len,
        };
        const input_pin = self.api.node_named_input(self.api, self.node, name_slice);
        return InputPin.init(self.api, input_pin);
    }

    pub fn indexedInput(self: Node, index: i32) InputPin {
        const input_pin = self.api.node_indexed_input(self.api, self.node, index);
        return InputPin.init(self.api, input_pin);
    }

    pub fn namedOutput(self: Node, name: []const u8) OutputPin {
        var name_slice = c.ls_StringSlice{
            .start = name.ptr,
            .end = name.ptr + name.len,
        };
        const output_pin = self.api.node_named_output(self.api, self.node, name_slice);
        return OutputPin.init(self.api, output_pin);
    }

    pub fn indexedOutput(self: Node, index: i32) OutputPin {
        const output_pin = self.api.node_indexed_output(self.api, self.node, index);
        return OutputPin.init(self.api, output_pin);
    }

    pub fn parameter(self: Node, name: []const u8) InputPin {
        var name_slice = c.ls_StringSlice{
            .start = name.ptr,
            .end = name.ptr + name.len,
        };
        const input_pin = self.api.node_parameter(self.api, self.node, name_slice);
        return InputPin.init(self.api, input_pin);
    }

    pub fn setting(self: Node, name: []const u8) InputPin {
        var name_slice = c.ls_StringSlice{
            .start = name.ptr,
            .end = name.ptr + name.len,
        };
        const input_pin = self.api.node_setting(self.api, self.node, name_slice);
        return InputPin.init(self.api, input_pin);
    }

    pub fn createOutput(self: Node, name: []const u8, channels: i32) void {
        var name_slice = c.ls_StringSlice{
            .start = name.ptr,
            .end = name.ptr + name.len,
        };
        self.api.create_node_output(self.api, self.node, name_slice, channels);
    }
};

pub const LabSoundAPIWrapper = struct {
    api: *c.LabSoundAPI,

    pub fn init() LabSoundAPIWrapper {
        return LabSoundAPIWrapper{
            .api = c.ls_create_api_1_0(c.ls_default_alloc),
        };
    }

    pub fn deinit(self: *LabSoundAPIWrapper) void {
        c.ls_release_api_1_0(self.api);
    }

    // Factory method to create Node
    pub fn createNode(self: *LabSoundAPIWrapper, name: []const u8, type_: []const u8) Node {
        var name_slice = c.ls_StringSlice{
            .start = name.ptr,
            .end = name.ptr + name.len,
        };
        var type_slice = c.ls_StringSlice{
            .start = type_.ptr,
            .end = type_.ptr + type_.len,
        };
        const node = self.api.node_create(self.api, name_slice, type_slice);
        return Node.init(self.api, node);
    }

    pub fn nodeNames(self: *LabSoundAPIWrapper, allocator: *std.heap.Allocator) ![]std.mem.AllocatorSlice([]const u8) {
        const name_array = self.api.node_names(self.api);
        var names = try allocator.alloc(std.mem.AllocatorSlice([]const u8), name_array.count);
        defer allocator.free(names);

        for (name_idx: usize = 0; name_idx < name_array.count; name_idx += 1) {
            const name = name_array.names[name_idx];
            names[name_idx] = std.mem.sliceToString(name.start, name.end - name.start);
        }

        return names;
    }

    pub fn getApi(self: *LabSoundAPIWrapper) *c.LabSoundAPI {
        return self.api;
    }

    pub fn destinationNode(self: *LabSoundAPIWrapper) Node {
        const node = self.api.destination_node(self.api);
        return Node.init(self.api, node);
    }

    pub fn connect(self: *LabSoundAPIWrapper, input: InputPin, output: OutputPin) Connection {
        const connection = self.api.connect(self.api, input.pin, output.pin);
        return Connection.init(self.api, connection);
    }

    pub fn disconnect(self: *LabSoundAPIWrapper, connection: *Connection) void {
        connection.disconnect();
    }
};

test "LabSoundAPIWrapper oscillator test" {
    var allocator = std.testing.allocator;

    // Initialize API
    var api = LabSoundAPIWrapper.init();
    defer api.deinit();

    // Create Oscillator Node (with frequency 440Hz)
    const osc = api.createNode("Oscillator", "Sine");
    const freq_input = osc.parameter("frequency");
    freq_input.set_float(440.0);

    // Create Gain Node (with gain set to 0.75)
    const gain = api.createNode("Gain", "Gain");
    const gain_input = gain.parameter("gain");
    gain_input.set_float(0.75);

    // Connect Oscillator -> Gain -> Destination
    const destination = api.destinationNode();
    const osc_output = osc.indexed_output(0);
    const gain_input_pin = gain.indexed_input(0);
    const gain_output = gain.indexed_output(0);
    const dest_input = destination.indexed_input(0);

    _ = api.connect(osc_output, gain_input_pin);
    _ = api.connect(gain_output, dest_input);

    // Start the oscillator and play for 1/2 second
    const start_time = 0.0;
    osc.start(start_time);

    // Wait for 1/2 second before stopping the nodes
    std.time.sleep(500 * std.time.millisecond);

    // Stop the oscillator
    osc.stop(start_time + 0.5);

    // Test cleanup will happen automatically with `defer`
}
