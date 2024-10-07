const std = @import("std");
const builtin = @import("builtin");


/// build options for the wrinkles project
pub const Options = struct {
    optimize: std.builtin.Mode,
    target: std.Build.ResolvedTarget,
    test_filter: ?[]const u8 = null,

    common_build_options: *std.Build.Step.Options,

    test_step: *std.Build.Step,
    all_docs_step: *std.Build.Step,
    all_check_step: *std.Build.Step,
};

/// the minimum required zig version to build this project, manually updated
pub const MIN_ZIG_VERSION = std.SemanticVersion{
    .major = 0,
    .minor = 13,
    .patch = 0,
    .pre = "",
    // .pre = "dev.46"  <- for setting the dev version string
};

/// guarantee that the zig compiler version is more than the minimum
fn ensureZigVersion() !void {
    var installed_ver = builtin.zig_version;
    installed_ver.build = null;

    if (installed_ver.order(MIN_ZIG_VERSION) == .lt) {
        std.log.err("\n" ++
            \\---------------------------------------------------------------------------
            \\
            \\Installed Zig compiler version is too old.
            \\
            \\Min. required version: {any}
            \\Installed version: {any}
            \\
            \\Please install newer version and try again.
            \\Latest version can be found here: https://ziglang.org/download/
            \\
            \\---------------------------------------------------------------------------
            \\
        , .{ MIN_ZIG_VERSION, installed_ver });

        return error.ZigIsTooOld;
    }
}

const c_args = [_][]const u8{
   // "-D_MSC_VER",
    "-std=c11",
    "-fno-sanitize=undefined",
};

const cpp_args = [_][]const u8{
    "-std=c++17",
    "-fno-sanitize=undefined",
    "-D__MACOSX_CORE__", // select a backend for RtAudio
};

pub fn build(b: *std.Build) void {

    const test_step = b.step(
        "test",
        "step to run all unit tests",
    );

    const all_docs_step = b.step(
        "docs",
        "build the documentation for the entire library",
    );

    const all_check_step = b.step(
        "check",
        "Check if everything compiles",
    );

    const options = Options{
        .optimize = b.standardOptimizeOption(.{}),
        .target = b.standardTargetOptions(.{}),
        .test_filter = b.option(
            []const u8,
            "test-filter",
            "filter for tests to run"
        ) orelse null,

        .common_build_options = b.addOptions(),

        // steps
        .test_step = test_step,
        .all_docs_step = all_docs_step,
        .all_check_step = all_check_step,
    };

    _ = b.addModule(
        "labsound", .{
            .root_source_file = b.path("./labsound.zig"),
            .target = options.target,
            .optimize = options.optimize,
     });

    const exe = b.addExecutable(.{ 
        .name = "LabSound-c", 
        .root_source_file = b.path("./src/main.zig"), 
        .target = options.target, 
        .optimize = options.optimize });

    exe.addIncludePath(b.path("./"));
    exe.addIncludePath(b.path("build/_deps/labsound-src/include"));
    exe.addIncludePath(b.path("build/_deps/labsound-src/src"));
    exe.addIncludePath(b.path("build/_deps/labsound-src/src/internal"));
    exe.addIncludePath(b.path("build/_deps/labsound-src/third_party"));
    exe.addIncludePath(b.path("build/_deps/labsound-src/third_party/libsamplerate/include"));
    exe.addIncludePath(b.path("build/_deps/labsound-src/third_party/libnyquist/include"));
    exe.addIncludePath(b.path("build/_deps/labsound-src/third_party/libnyquist/include/libnyquist"));
    exe.addIncludePath(b.path("build/_deps/labsound-src/third_party/libnyquist/third_party"));
    exe.addIncludePath(b.path("build/_deps/labsound-src/third_party/libnyquist/third_party/FLAC/src/include"));
    exe.addIncludePath(b.path("build/_deps/labsound-src/third_party/libnyquist/third_party/libogg/include"));
    exe.addIncludePath(b.path("build/_deps/labsound-src/third_party/libnyquist/third_party/libvorbis/include"));
    exe.addIncludePath(b.path("build/_deps/labsound-src/third_party/libnyquist/third_party/libvorbis/src"));
    exe.addIncludePath(b.path("build/_deps/labsound-src/third_party/libnyquist/third_party/musepack/include"));
    exe.addIncludePath(b.path("build/_deps/labsound-src/third_party/libnyquist/third_party/opus/celt"));
    exe.addIncludePath(b.path("build/_deps/labsound-src/third_party/libnyquist/third_party/opus/libopus/include"));
    exe.addIncludePath(b.path("build/_deps/labsound-src/third_party/libnyquist/third_party/opus/opusfile/include"));
    exe.addIncludePath(b.path("build/_deps/labsound-src/third_party/libnyquist/third_party/opus/opusfile/src/include"));
    exe.addIncludePath(b.path("build/_deps/labsound-src/third_party/libnyquist/third_party/opus/silk"));
    exe.addIncludePath(b.path("build/_deps/labsound-src/third_party/libnyquist/third_party/opus/silk/float"));
    exe.addIncludePath(b.path("build/_deps/labsound-src/third_party/libnyquist/third_party/wavpack/include"));
    exe.addIncludePath(b.path("build/_deps/labsound-src/third_party/libnyquist/src"));

    exe.addCSourceFile(.{ .file = b.path("labsound-c.cpp"), .flags = &cpp_args});

    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/backends/RtAudio/AudioDevice_RtAudio.cpp"), .flags = &cpp_args} );
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/backends/RtAudio/RtAudio.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/core/_SoundPipe_FFT.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/core/AnalyserNode.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/core/AudioBasicInspectorNode.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/core/AudioBasicProcessorNode.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/core/AudioBus.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/core/AudioChannel.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/core/AudioContext.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/core/AudioDevice.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/core/AudioHardwareInputNode.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/core/AudioListener.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/core/AudioNode.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/core/AudioNodeInput.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/core/AudioNodeOutput.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/core/AudioParam.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/core/AudioParamTimeline.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/core/AudioSummingJunction.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/core/BiquadFilterNode.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/core/ChannelMergerNode.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/core/ChannelSplitterNode.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/core/ConstantSourceNode.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/core/ConvolverNode.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/core/DelayNode.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/core/DynamicsCompressorNode.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/core/GainNode.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/core/OscillatorNode.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/core/PannerNode.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/core/PeriodicWave.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/core/RealtimeAnalyser.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/core/SampledAudioNode.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/core/StereoPannerNode.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/core/WaveShaperNode.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/extended/ADSRNode.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/extended/AudioFileReader.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/extended/BPMDelay.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/extended/ClipNode.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/extended/DiodeNode.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/extended/FunctionNode.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/extended/GranulationNode.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/extended/LabSound.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/extended/NoiseNode.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/extended/PdNode.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/extended/PeakCompNode.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/extended/PingPongDelayNode.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/extended/PolyBLEPNode.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/extended/PowerMonitorNode.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/extended/PWMNode.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/extended/RecorderNode.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/extended/Registry.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/extended/SfxrNode.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/extended/SpatializationNode.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/extended/SpectralMonitorNode.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/extended/SuperSawNode.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/internal/src/AudioDSPKernelProcessor.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/internal/src/AudioUtilities.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/internal/src/Biquad.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/internal/src/Cone.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/internal/src/DelayDSPKernel.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/internal/src/DelayProcessor.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/internal/src/DirectConvolver.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/internal/src/Distance.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/internal/src/DownSampler.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/internal/src/DynamicsCompressor.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/internal/src/DynamicsCompressorKernel.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/internal/src/EqualPowerPanner.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/internal/src/FFTConvolver.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/internal/src/FFTFrame.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/internal/FFTFrameAppleAccelerate.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/internal/src/FFTFrameKissFFT.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/internal/src/HRTFPanner.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/internal/src/libSampleRate.c"), .flags = &c_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/internal/src/UpSampler.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/internal/src/VectorMath.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/src/internal/src/ZeroPole.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/third_party/libnyquist/src/Common.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/third_party/libnyquist/src/Encoders.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/third_party/libnyquist/src/FlacDecoder.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/third_party/libnyquist/src/FlacDependencies.c"), .flags = &c_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/third_party/libnyquist/src/Mp3Decoder.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/third_party/libnyquist/src/MusepackDecoder.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/third_party/libnyquist/src/MusepackDependencies.c"), .flags = &c_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/third_party/libnyquist/src/OpusDecoder.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/third_party/libnyquist/src/OpusDependencies.c"), .flags = &c_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/third_party/libnyquist/src/VorbisDecoder.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/third_party/libnyquist/src/VorbisDependencies.c"), .flags = &c_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/third_party/libnyquist/src/WavDecoder.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/third_party/libnyquist/src/WavPackDecoder.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/third_party/libnyquist/third_party/wavpack/src/common_utils.c"), .flags = &c_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/third_party/libnyquist/third_party/wavpack/src/decorr_utils.c"), .flags = &c_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/third_party/libnyquist/third_party/wavpack/src/entropy_utils.c"), .flags = &c_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/third_party/libnyquist/third_party/wavpack/src/extra1.c"), .flags = &c_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/third_party/libnyquist/third_party/wavpack/src/extra2.c"), .flags = &c_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/third_party/libnyquist/third_party/wavpack/src/open_filename.c"), .flags = &c_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/third_party/libnyquist/third_party/wavpack/src/open_utils.c"), .flags = &c_args });
    //exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/third_party/libnyquist/third_party/wavpack/src/open_legacy.c"), .flags = &c_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/third_party/libnyquist/third_party/wavpack/src/open_raw.c"), .flags = &c_args });
    //exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/third_party/libnyquist/third_party/wavpack/src/pack_dns.c"), .flags = &c_args });
    //exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/third_party/libnyquist/third_party/wavpack/src/pack_dsd.c"), .flags = &c_args });
    //exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/third_party/libnyquist/third_party/wavpack/src/pack_floats.c"), .flags = &c_args });
    //exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/third_party/libnyquist/third_party/wavpack/src/pack.c"), .flags = &c_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/third_party/libnyquist/third_party/wavpack/src/read_words.c"), .flags = &c_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/third_party/libnyquist/third_party/wavpack/src/tag_utils.c"), .flags = &c_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/third_party/libnyquist/third_party/wavpack/src/tags.c"), .flags = &c_args });
    //exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/third_party/libnyquist/third_party/wavpack/src/unpack_dsd.c"), .flags = &c_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/third_party/libnyquist/third_party/wavpack/src/unpack_floats.c"), .flags = &c_args });
    //exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/third_party/libnyquist/third_party/wavpack/src/unpack_seek.c"), .flags = &c_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/third_party/libnyquist/third_party/wavpack/src/unpack_utils.c"), .flags = &c_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/third_party/libnyquist/third_party/wavpack/src/unpack.c"), .flags = &c_args });
    //exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/third_party/libnyquist/third_party/wavpack/src/unpack3_open.c"), .flags = &c_args });
    //exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/third_party/libnyquist/third_party/wavpack/src/unpack3_seek.c"), .flags = &c_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/third_party/libnyquist/third_party/wavpack/src/write_words.c"), .flags = &c_args });
    exe.addCSourceFile(.{ .file = b.path("build/_deps/labsound-src/third_party/ooura/src/fftsg.cpp"), .flags = &cpp_args });
    exe.addCSourceFile(.{ .file = b.path("./flecs.c"), .flags = &c_args });
    exe.linkLibC();
    exe.linkLibCpp();
    exe.linkSystemLibrary("c");

    //switch (options.target.getOsTag()) {
    //    .macos => {
            exe.linkFramework("Accelerate");
            exe.linkFramework("CoreAudio");
            exe.linkFramework("Foundation");
    //    }
    //}

    // indicate that exe can be installed via `zig build install`
    const install_exe_artifact_step = &b.addInstallArtifact(exe, .{} ).step;
    // and that we're a gonna install it
    const install_exe_artifact_step_step = b.step("install-test-exe", "Install the executable");
    // and it depends on the artifact
    install_exe_artifact_step_step.dependOn(install_exe_artifact_step);

    // set up running
    var run_cmd = b.addRunArtifact(exe).step;
    run_cmd.dependOn(install_exe_artifact_step_step);
    const run_step = b.step(
        "test-run",
        "Run testy executable"
    );
    run_step.dependOn(&run_cmd);

    b.getInstallStep().dependOn(install_exe_artifact_step_step);

    const mod_unit_tests = b.addTest(
        .{
            .name = "test_labsound",
            .root_source_file = b.path("src/main.zig"),
            .target = options.target,
            .filter = options.test_filter orelse &.{},
        }
    );
    mod_unit_tests.root_module.addOptions(
        "build_options",
        options.common_build_options,
    );

    const run_unit_tests = b.addRunArtifact(mod_unit_tests);
    options.test_step.dependOn(&run_unit_tests.step);

    // also install the test binary for lldb needs
    const install_test_bin = b.addInstallArtifact(
        mod_unit_tests,
        .{}
    );

    options.test_step.dependOn(&install_test_bin.step);
}
