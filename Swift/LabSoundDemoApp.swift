

import SwiftUI

class LabSoundController: ObservableObject {
    
    var api = LabSoundWrapper()
        
    @Published var value: Float = 0 {
        didSet {
            print("value \(value)")
        }
    }
    @Published var position: Float = 0.5 {
        didSet {
            print("position \(position)")
        }
    }
    
    @Published var sfxr_attack: Float = 0 {
        didSet {
            api.sfxr_attack = sfxr_attack
        }
    }
    @Published var sfxr_decay: Float = 0 {
        didSet {
            api.sfxr_decay = sfxr_decay
        }
    }
    @Published var sfxr_sustain: Float = 0 {
        didSet {
            api.sfxr_sustain = sfxr_sustain
        }
    }
    @Published var sfxr_sustain_punch: Float = 0 {
        didSet {
            api.sfxr_sustain_punch = sfxr_sustain_punch
        }
    }
    @Published var sfxr_vibratoSpeed: Float = 0 {
        didSet {
            api.sfxr_vibratoSpeed = sfxr_vibratoSpeed
        }
    }
    @Published var sfxr_vibratoDepth: Float = 0 {
        didSet {
            api.sfxr_vibratoDepth = sfxr_vibratoDepth
        }
    }
    @Published var sfxr_minFrequency: Float = 0 {
        didSet {
            api.sfxr_minFrequency = sfxr_minFrequency
        }
    }
    @Published var sfxr_startFrequency: Float = 0 {
        didSet {
            api.sfxr_startFrequency = sfxr_startFrequency
        }
    }
    @Published var sfxr_slide: Float = 0 {
        didSet {
            api.sfxr_slide = sfxr_slide
        }
    }
    @Published var sfxr_deltaSlide: Float = 0 {
        didSet {
            api.sfxr_deltaSlide = sfxr_deltaSlide
        }
    }
    @Published var sfxr_lpFilterCutoff: Float = 0 {
        didSet {
            api.sfxr_lpFilterCutoff = sfxr_lpFilterCutoff
        }
    }
    @Published var sfxr_lpFilterCutoffSweep: Float = 0 {
        didSet {
            api.sfxr_lpFilterCutoffSweep = sfxr_lpFilterCutoffSweep
        }
    }
    @Published var sfxr_lpFilterResonance: Float = 0 {
        didSet {
            api.sfxr_lpFilterResonance = sfxr_lpFilterResonance
        }
    }
    @Published var sfxr_hpFilterCutoff: Float = 0 {
        didSet {
            api.sfxr_hpFilterCutoff = sfxr_hpFilterCutoff
        }
    }
    @Published var sfxr_hpFilterCutoffSweep: Float = 0 {
        didSet {
            api.sfxr_hpFilterCutoffSweep = sfxr_hpFilterCutoffSweep
        }
    }
    @Published var sfxr_changeAmount: Float = 0 {
        didSet {
            api.sfxr_changeAmount = sfxr_changeAmount
        }
    }
    @Published var sfxr_changeSpeed: Float = 0 {
        didSet {
            api.sfxr_changeSpeed = sfxr_changeSpeed
        }
    }
    @Published var sfxr_squareDuty: Float = 0 {
        didSet {
            api.sfxr_squareDuty = sfxr_squareDuty
        }
    }
    @Published var sfxr_dutySweep: Float = 0 {
        didSet {
            api.sfxr_dutySweep = sfxr_dutySweep
        }
    }
    @Published var sfxr_repeatSpeed: Float = 0 {
        didSet {
            api.sfxr_repeatSpeed = sfxr_repeatSpeed
        }
    }
    @Published var sfxr_phaserOffset: Float = 0 {
        didSet {
            api.sfxr_phaserOffset = sfxr_phaserOffset
        }
    }
    @Published var sfxr_phaserSweep: Float = 0 {
        didSet {
            api.sfxr_phaserSweep = sfxr_phaserSweep
        }
    }
    
    
}


@main
struct LabSoundDemoApp: App {
    var ls = LabSoundController()
    
    var body: some Scene {
        WindowGroup {
            ContentView(ls: ls)
        }
    }
}


struct ContentView: View {
    @ObservedObject var ls: LabSoundController
    
    var body: some View {
        HStack {
            VStack {
                Button {
                    
                } label: {
                    Text("Default")
                        .frame(maxWidth: .infinity)
                }
                .buttonStyle(.borderedProminent)
                
                Button {
                    
                } label: {
                    Text("Coin")
                        .frame(maxWidth: .infinity)
                }
                .buttonStyle(.borderedProminent)
                
                Button {
                    
                } label: {
                    Text("Laser")
                        .frame(maxWidth: .infinity)
                }
                .buttonStyle(.borderedProminent)
                
                Button {
                    
                } label: {
                    Text("Explosion")
                        .frame(maxWidth: .infinity)
                }
                .buttonStyle(.borderedProminent)
                
                Button {
                    
                } label: {
                    Text("Power Up")
                        .frame(maxWidth: .infinity)
                }
                .buttonStyle(.borderedProminent)
                
                Button {
                    
                } label: {
                    Text("Hit")
                        .frame(maxWidth: .infinity)
                }
                .buttonStyle(.borderedProminent)
                
                Button {
                    
                } label: {
                    Text("Jump")
                        .frame(maxWidth: .infinity)
                }
                .buttonStyle(.borderedProminent)
                
                Button {
                    
                } label: {
                    Text("Select")
                        .frame(maxWidth: .infinity)
                }
                .buttonStyle(.borderedProminent)
                
                Button {
                    
                } label: {
                    Text("Mutate")
                        .frame(maxWidth: .infinity)
                }
                .buttonStyle(.borderedProminent)
                
                Button {
                    
                } label: {
                    Text("Random")
                        .frame(maxWidth: .infinity)
                }
                .buttonStyle(.borderedProminent)
            }
            .padding()
            .frame(maxWidth: 200)
            .background(Color(.lightGray))
            .clipShape(RoundedRectangle(cornerRadius: 8, style: .continuous))
            
            VStack {
                
                /*
                 {"attack",              "ATCK", 0.0,  0, 1},
                 {"decay",               "DECY", 0.4,  0, 1},
                 {"sustain",             "SUS ", 0.3,  0, 1},
                 {"sustainPunch",        "SUSP", 0.0,  0, 1},
                 {"vibratoDepth",        "VIBD", 0.0,  0, 1},
                 {"vibratoSpeed",        "VIBS", 0.0,  0, 1},
                 */
                HStack {
                    VStack {
                        SmallKnob(value: $ls.sfxr_attack)
                            .backgroundColor(.yellow)
                            .foregroundColor(.blue)
                        Text("ATCK")
                    }
                    VStack {
                        SmallKnob(value: $ls.sfxr_decay)
                            .backgroundColor(.yellow)
                            .foregroundColor(.blue)
                        Text("DECY")
                    }
                    VStack {
                        SmallKnob(value: $ls.sfxr_sustain)
                            .backgroundColor(.yellow)
                            .foregroundColor(.blue)
                        Text("SUST")
                    }
                    VStack {
                        SmallKnob(value: $ls.sfxr_sustain_punch)
                            .backgroundColor(.yellow)
                            .foregroundColor(.blue)
                        Text("SUSP")
                    }
                    VStack {
                        SmallKnob(value: $ls.sfxr_vibratoDepth)
                            .backgroundColor(.yellow)
                            .foregroundColor(.blue)
                        Text("VIBD")
                    }
                    VStack {
                        SmallKnob(value: $ls.sfxr_vibratoSpeed)
                            .backgroundColor(.yellow)
                            .foregroundColor(.blue)
                        Text("VIBS")
                    }
                }
                .padding()
                .background(Color(.lightGray))
                .clipShape(RoundedRectangle(cornerRadius: 8, style: .continuous))
                
                /*
                 {"startFrequency",      "SFRQ", 0.3,  0, 1},
                 {"minFrequency",        "MFRQ", 0.0,  0, 1},
                 {"slide",               "SLID", 0.0, -1, 1},
                 {"deltaSlide",          "DSLD", 0.0,  0, 1},
                 */
                
                HStack {
                    VStack {
                        SmallKnob(value: $ls.sfxr_startFrequency)
                            .backgroundColor(.yellow)
                            .foregroundColor(.blue)
                        Text("SFRQ")
                    }
                    VStack {
                        SmallKnob(value: $ls.sfxr_minFrequency)
                            .backgroundColor(.yellow)
                            .foregroundColor(.blue)
                        Text("MFRQ")
                    }
                    VStack {
                        SmallKnob(value: $ls.sfxr_slide)
                            .backgroundColor(.yellow)
                            .foregroundColor(.blue)
                        Text("SLID")
                    }
                    VStack {
                        SmallKnob(value: $ls.sfxr_deltaSlide)
                            .backgroundColor(.yellow)
                            .foregroundColor(.blue)
                        Text("DSLD")
                    }
                }
                .padding()
                .background(Color(.lightGray))
                .clipShape(RoundedRectangle(cornerRadius: 8, style: .continuous))
                
                /*
                 {"lpFilterCutoff",      "LPFC", 1.0,  0, 1},
                 {"lpFilterCutoffSweep", "LPFS", 0.0, -1, 1},
                 {"lpFilterResonance",   "LPFR", 0.0,  0, 1},
                 {"hpFilterCutoff",      "HPFC", 0.0,  0, 1},
                 {"hpFilterCutoffSweep", "HPFS", 0.0, -1, 1},
                 */
                HStack {
                    VStack {
                        SmallKnob(value: $ls.sfxr_lpFilterCutoff)
                            .backgroundColor(.yellow)
                            .foregroundColor(.blue)
                        Text("LPFC")
                    }
                    VStack {
                        SmallKnob(value: $ls.sfxr_lpFilterCutoffSweep)
                            .backgroundColor(.yellow)
                            .foregroundColor(.blue)
                        Text("LPFS")
                    }
                    VStack {
                        SmallKnob(value: $ls.sfxr_lpFilterResonance)
                            .backgroundColor(.yellow)
                            .foregroundColor(.blue)
                        Text("LPFR")
                    }
                    VStack {
                        SmallKnob(value: $ls.sfxr_hpFilterCutoff)
                            .backgroundColor(.yellow)
                            .foregroundColor(.blue)
                        Text("HPFC")
                    }
                    VStack {
                        SmallKnob(value: $ls.sfxr_hpFilterCutoffSweep)
                            .backgroundColor(.yellow)
                            .foregroundColor(.blue)
                        Text("HPFS")
                    }
                }
                .padding()
                .background(Color(.lightGray))
                .clipShape(RoundedRectangle(cornerRadius: 8, style: .continuous))
            }

            /*
            static AudioParamDescriptor s_sfxParams[] = {
               {"changeAmount",        "CHGA", 0.0, -1, 1},
               {"changeSpeed",         "CHGS", 0.0,  0, 1},
               {"squareDuty",          "DUTY", 0.0,  0, 1},
               {"dutySweep",           "DSWP", 0.0, -1, 1},
               {"repeatSpeed",         "REPS", 0.0,  0, 1},
               {"phaserOffset",        "PHSO", 0.0, -1, 1},
               {"phaserSweep",         "PHSS", 0.0, -1, 1},
                nullptr };
            */

            HStack {
                VStack {
                    VStack {
                        SmallKnob(value: $ls.sfxr_changeAmount)
                            .backgroundColor(.yellow)
                            .foregroundColor(.blue)
                        Text("CHGA")
                    }
                    VStack {
                        SmallKnob(value: $ls.sfxr_changeSpeed)
                            .backgroundColor(.yellow)
                            .foregroundColor(.blue)
                        Text("CHGS")
                    }
                    VStack {
                        SmallKnob(value: $ls.sfxr_squareDuty)
                            .backgroundColor(.yellow)
                            .foregroundColor(.blue)
                        Text("DUTY")
                    }
                    VStack {
                        SmallKnob(value: $ls.sfxr_dutySweep)
                            .backgroundColor(.yellow)
                            .foregroundColor(.blue)
                        Text("DSWP")
                    }
                }
                .padding()
                .background(Color(.lightGray))
                .clipShape(RoundedRectangle(cornerRadius: 8, style: .continuous))

                VStack {
                    VStack {
                        SmallKnob(value: $ls.sfxr_repeatSpeed)
                            .backgroundColor(.yellow)
                            .foregroundColor(.blue)
                        Text("REPS")
                    }
                    VStack {
                        SmallKnob(value: $ls.sfxr_phaserOffset)
                            .backgroundColor(.yellow)
                            .foregroundColor(.blue)
                        Text("PHSO")
                    }
                    VStack {
                        SmallKnob(value: $ls.sfxr_phaserSweep)
                            .backgroundColor(.yellow)
                            .foregroundColor(.blue)
                        Text("PHSS")
                    }
                }
                .padding()
                .background(Color(.lightGray))
                .clipShape(RoundedRectangle(cornerRadius: 8, style: .continuous))
            }

        }

        /*
 char const * const s_waveTypes[5] = {"Square", "Sawtooth", "Sine", "Noise", nullptr};

 */
        
        GeometryReader { proxy in
            VStack(alignment: .center) {
                Ribbon(position: $ls.position)
                    .backgroundColor(.yellow)
                    .foregroundColor(.blue)
                    .cornerRadius(10)
                    .frame(width: proxy.size.width / 3,
                           height: 20)
                Ribbon(position: $ls.position)
                    .backgroundColor(.orange)
                    .foregroundColor(.red)
                    .cornerRadius(20)
                    .indicatorWidth(60)
                    .frame(width: proxy.size.width / 2,
                           height: 20)
                Ribbon(position: $ls.position)
                    .foregroundColor(.white.opacity(0.5))
                    .cornerRadius(100)
                    .indicatorWidth(200)
                    .frame(height: 20)
            }
        }
    }
}

