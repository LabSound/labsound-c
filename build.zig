const std = @import("std");

const c_args = [_][]const u8{
    "-D_MSC_VER",
    "-std=c11",
    "-fno-sanitize=undefined",
};

const cpp_args = [_][]const u8{
    "-std=c++17",
    "-fno-sanitize=undefined",
};

pub fn build(b: *std.build.Builder) void {
    // Standard target options allows the person running `zig build` to choose
    // what target to build for. Here we do not override the defaults, which
    // means any target is allowed, and the default is native. Other options
    // for restricting supported target set are available.
    const target = b.standardTargetOptions(.{});

    // Standard release options allow the person running `zig build` to select
    // between Debug, ReleaseSafe, ReleaseFast, and ReleaseSmall.
    const mode = b.standardReleaseOptions();

    const exe = b.addExecutable("LabSound-c", "src/main.zig");
    exe.setTarget(target);
    exe.setBuildMode(mode);

    exe.addIncludeDir("./");
    exe.addIncludeDir("../LabSound/include");
    exe.addCSourceFile("labsound-c.cpp", &cpp_args);
    exe.addCSourceFile("flecs.c", &c_args);
    exe.linkLibC();
    exe.linkLibCpp();
    exe.linkSystemLibrary("c");
    exe.install();

    const run_cmd = exe.run();
    run_cmd.step.dependOn(b.getInstallStep());
    if (b.args) |args| {
        run_cmd.addArgs(args);
    }

    const run_step = b.step("run", "Run the app");
    run_step.dependOn(&run_cmd.step);

    const exe_tests = b.addTest("src/main.zig");
    exe_tests.setBuildMode(mode);

    const test_step = b.step("test", "Run unit tests");
    test_step.dependOn(&exe_tests.step);
}

