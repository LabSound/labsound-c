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
    exe.addIncludeDir("../LabSound/src");
    exe.addIncludeDir("../LabSound/third_party");
    exe.addIncludeDir("../LabSound/third_party/libnyquist/include");
    exe.addIncludeDir("../LabSound/third_party/libnyquist/include/libnyquist");
    exe.addIncludeDir("../LabSound/third_party/libnyquist/third_party");
    exe.addIncludeDir("../LabSound/third_party/libnyquist/third_party/FLAC/src/include");
    exe.addIncludeDir("../LabSound/third_party/libnyquist/third_party/libogg/include");
    exe.addIncludeDir("../LabSound/third_party/libnyquist/third_party/libvorbis/include");
    exe.addIncludeDir("../LabSound/third_party/libnyquist/third_party/libvorbis/src");
    exe.addIncludeDir("../LabSound/third_party/libnyquist/third_party/musepack/include");
    exe.addIncludeDir("../LabSound/third_party/libnyquist/third_party/opus/celt");
    exe.addIncludeDir("../LabSound/third_party/libnyquist/third_party/opus/libopus/include");
    exe.addIncludeDir("../LabSound/third_party/libnyquist/third_party/opus/opusfile/include");
    exe.addIncludeDir("../LabSound/third_party/libnyquist/third_party/opus/opusfile/src/include");
    exe.addIncludeDir("../LabSound/third_party/libnyquist/third_party/opus/silk");
    exe.addIncludeDir("../LabSound/third_party/libnyquist/third_party/opus/silk/float");
    exe.addIncludeDir("../LabSound/third_party/libnyquist/third_party/wavpack/include");
    exe.addIncludeDir("../LabSound/third_party/libnyquist/src");
    exe.addCSourceFile("labsound-c.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/backends/RtAudio/AudioDevice_RtAudio.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/backends/RtAudio/RtAudio.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/core/_SoundPipe_FFT.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/core/AnalyserNode.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/core/AudioBasicInspectorNode.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/core/AudioBasicProcessorNode.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/core/AudioBus.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/core/AudioChannel.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/core/AudioContext.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/core/AudioDevice.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/core/AudioHardwareDeviceNode.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/core/AudioHardwareInputNode.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/core/AudioListener.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/core/AudioNode.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/core/AudioParam.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/core/AudioParamTimeline.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/core/BiquadFilterNode.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/core/ChannelMergerNode.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/core/ChannelSplitterNode.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/core/ConvolverNode.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/core/DelayNode.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/core/DynamicsCompressorNode.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/core/GainNode.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/core/NullDeviceNode.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/core/OscillatorNode.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/core/PannerNode.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/core/RealtimeAnalyser.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/core/SampledAudioNode.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/core/StereoPannerNode.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/core/WaveShaperNode.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/core/WaveTable.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/extended/ADSRNode.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/extended/AudioFileReader.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/extended/BPMDelay.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/extended/ClipNode.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/extended/DiodeNode.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/extended/FunctionNode.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/extended/GranulationNode.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/extended/LabSound.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/extended/NoiseNode.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/extended/PdNode.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/extended/PeakCompNode.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/extended/PingPongDelayNode.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/extended/PolyBLEPNode.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/extended/PowerMonitorNode.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/extended/PWMNode.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/extended/RecorderNode.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/extended/Registry.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/extended/SfxrNode.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/extended/SpatializationNode.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/extended/SpectralMonitorNode.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/extended/SuperSawNode.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/internal/src/AudioDSPKernelProcessor.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/internal/src/AudioUtilities.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/internal/src/Biquad.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/internal/src/Cone.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/internal/src/DelayDSPKernel.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/internal/src/DelayProcessor.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/internal/src/Distance.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/internal/src/DynamicsCompressor.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/internal/src/DynamicsCompressorKernel.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/internal/src/EqualPowerPanner.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/internal/src/FFTConvolver.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/internal/src/FFTFrame.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/internal/src/FFTFrameKissFFT.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/internal/src/HRTFDatabase.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/internal/src/HRTFDatabaseLoader.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/internal/src/HRTFElevation.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/internal/src/HRTFKernel.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/internal/src/HRTFPanner.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/internal/src/VectorMath.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/src/internal/src/ZeroPole.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/third_party/libnyquist/src/Common.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/third_party/libnyquist/src/Encoders.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/third_party/libnyquist/src/FlacDecoder.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/third_party/libnyquist/src/FlacDependencies.c", &c_args);
    exe.addCSourceFile("../LabSound/third_party/libnyquist/src/Mp3Decoder.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/third_party/libnyquist/src/MusepackDecoder.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/third_party/libnyquist/src/MusepackDependencies.c", &c_args);
    exe.addCSourceFile("../LabSound/third_party/libnyquist/src/OpusDecoder.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/third_party/libnyquist/src/OpusDependencies.c", &c_args);
    exe.addCSourceFile("../LabSound/third_party/libnyquist/src/VorbisDecoder.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/third_party/libnyquist/src/VorbisDependencies.c", &c_args);
    exe.addCSourceFile("../LabSound/third_party/libnyquist/src/WavDecoder.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/third_party/libnyquist/src/WavPackDecoder.cpp", &cpp_args);
    exe.addCSourceFile("../LabSound/third_party/ooura/src/fftsg.cpp", &cpp_args);
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

