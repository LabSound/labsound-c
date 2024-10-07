const std = @import("std");

const ls = @cImport({
    @cInclude("labsound-c.h");
});

const labsound = @import("labsound");

pub fn main() anyerror!void {
    std.log.info("All your codebase are belong to us.", .{});

    const oscillator_str = "Oscillator";
    const Oscillator_s =  ls.ls_StringSlice {
        .start = oscillator_str, .end = &oscillator_str[10],}; 
    const osc1_str = "osc1";
    const osc1_s = ls.ls_StringSlice {
        .start = osc1_str, .end = &osc1_str[4], };

    const api = ls.ls_create_api_1_0(ls.ls_default_alloc);
    const osc = api.*.node_create.?(api, osc1_s, Oscillator_s);
    const dest = api.*.destination_node.?(api);

    const oscOut = api.*.node_indexed_output.?(api, osc, 0);
    const destIn = api.*.node_indexed_input.?(api, dest, 0);
    //const connection1 
    _ = api.*.connect.?(api, destIn, oscOut);

    api.*.node_start.?(api, osc, ls.ls_Seconds { .t = 0 });
}

test "basic test" {
    try std.testing.expectEqual(10, 3 + 7);
}
