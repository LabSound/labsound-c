
#ifndef LABSOUNDC_H
#include "labsound-c.h"
#endif

#include <LabSound/LabSound.h>
#include <LabSound/backends/AudioDevice_RtAudio.h>

#include "flecs.h"

#include <memory>
#include <string>
#include <unordered_map>
using std::shared_ptr;
using std::unique_ptr;
using std::string;
using std::unordered_map;

using namespace lab;

struct e_Node {
    uint32_t hash_index;
};

struct e_Input {
    ls_Node node;
    int index;
};

struct e_Output {
    ls_Node node;
    int index;
};

struct e_Param {
    ls_Node node;
    int index;
};

struct e_Setting {
    ls_Node node;
    int index;
};

struct e_Connection {
    ls_OutputPin from_pin;
    ls_InputPin to_pin;
    ls_Node from_node, to_node;
    int from_output_index, to_index;
    bool to_is_param;
};

struct e_BusData {
    uint32_t hash_index;
};

ECS_COMPONENT_DECLARE(e_Node);
ECS_COMPONENT_DECLARE(e_Input);
ECS_COMPONENT_DECLARE(e_Output);
ECS_COMPONENT_DECLARE(e_Param);
ECS_COMPONENT_DECLARE(e_Setting);
ECS_COMPONENT_DECLARE(e_Connection);
ECS_COMPONENT_DECLARE(e_BusData);

// Returns input, output
inline std::pair<AudioStreamConfig, AudioStreamConfig>
    GetDefaultAudioDeviceConfiguration(const bool with_input = true)
{    
    const std::vector<AudioDeviceInfo> audioDevices = lab::AudioDevice_RtAudio::MakeAudioDeviceList();
    AudioDeviceInfo defaultOutputInfo, defaultInputInfo;
    for (auto & info : audioDevices) {
        if (info.is_default_output)
            defaultOutputInfo = info;
        if (info.is_default_input)
            defaultInputInfo = info;
    }
    
    AudioStreamConfig outputConfig;
    if (defaultOutputInfo.index != -1) {
        outputConfig.device_index = defaultOutputInfo.index;
        outputConfig.desired_channels = std::min(uint32_t(2), defaultOutputInfo.num_output_channels);
        outputConfig.desired_samplerate = defaultOutputInfo.nominal_samplerate;
    }
    
    AudioStreamConfig inputConfig;
    if (with_input) {
        if (defaultInputInfo.index != -1) {
            inputConfig.device_index = defaultInputInfo.index;
            inputConfig.desired_channels = std::min(uint32_t(1), defaultInputInfo.num_input_channels);
            inputConfig.desired_samplerate = defaultInputInfo.nominal_samplerate;
        }
        else {
            throw std::invalid_argument("the default audio input device was requested but none were found");
        }
    }
    
    // RtAudio doesn't support mismatched input and output rates.
    // this may be a pecularity of RtAudio, but for now, force an RtAudio
    // compatible configuration
    if (defaultOutputInfo.nominal_samplerate != defaultInputInfo.nominal_samplerate) {
        float min_rate = std::min(defaultOutputInfo.nominal_samplerate, defaultInputInfo.nominal_samplerate);
        inputConfig.desired_samplerate = min_rate;
        outputConfig.desired_samplerate = min_rate;
        printf("Warning ~ input and output sample rates don't match, attempting to set minimum");
    }
    return {inputConfig, outputConfig};
}

struct LabSoundAPI_1_0_Detail {
    shared_ptr<lab::AudioContext> ac;
    ls_Alloc alloc;
    ls_Node destination_node;
    uint32_t next_id;
    unordered_map<uint32_t, shared_ptr<lab::AudioNode>> nodes;
    unordered_map<uint32_t, shared_ptr<lab::AudioBus>> busses;
    ecs_world_t *ecs;
    ecs_entity_t has_input_rel;
    ecs_entity_t has_output_rel;
    ecs_entity_t has_param_rel;
    ecs_entity_t has_setting_rel;
    ecs_entity_t has_connection_rel;
    ecs_app_desc_t desc;
};

namespace ls_1_0 {

shared_ptr<lab::AudioNode> ls_node(struct LabSoundAPI_1_0* ls, const e_Node* id)
{
    if (!id)
        return {};

    auto it = ls->_detail->nodes.find(id->hash_index);
    if (it == ls->_detail->nodes.end())
        return {};
    return it->second;
};

shared_ptr<lab::AudioNode> ls_node(struct LabSoundAPI_1_0* ls, const ls_Node n)
{
    if (!ecs_is_alive(ls->_detail->ecs, n.id))
        return {};

    const e_Node* node = ecs_get(ls->_detail->ecs, n.id, e_Node);
    return ls_node(ls, node);
};

const e_Connection* ls_connection(struct LabSoundAPI_1_0* ls, const ls_Connection c)
{
    auto w = ls->_detail->ecs;
    if (!ecs_is_alive(w, c.id))
        return nullptr;

    const e_Connection* ec = ecs_get(w, c.id, e_Connection);
    if (!ec)
        return nullptr;

    auto node_from = ls_node(ls, ec->from_node);
    if (!node_from)
        return nullptr;

    auto node_to = ls_node(ls, ec->to_node);
    if (!node_to)
        return nullptr;

    return ec;
}


// operations on nodes
//
ls_Seconds node_get_timing(struct LabSoundAPI_1_0* ls,
    ls_Node n) 
{
   auto ln = ls_node(ls, n);
   if (!ln)
       return ls_Seconds { -1.f };

   return ls_Seconds{ ln->graphTime().microseconds.count() * 1.e-6f };
}

ls_Seconds node_get_self_timing(struct LabSoundAPI_1_0* ls,
    ls_Node n) 
{
    auto ln = ls_node(ls, n);
    if (!ln)
        return ls_Seconds{ -1.f };

   return ls_Seconds{ ln->totalTime().microseconds.count() -
                      ln->graphTime().microseconds.count() * 1.e-6f };
}

void node_start(struct LabSoundAPI_1_0* ls,
    ls_Node n, ls_Seconds when) 
{
    auto ln = ls_node(ls, n);
    if (!ln)
        return;

   lab::SampledAudioNode* san =
       dynamic_cast<lab::SampledAudioNode*>(ln.get());
   if (san) {
       san->schedule(when.t);
       return;
   }

   lab::AudioScheduledSourceNode* asn =
       dynamic_cast<lab::AudioScheduledSourceNode*>(ln.get());
   if (!asn)
       return;

   asn->start(when.t);
}

void node_diagnose(struct LabSoundAPI_1_0* ls, ls_Node n)
{
    auto ln = ls_node(ls, n);
    if (!ln)
        return;

    lab::AudioContext& ac = *ls->_detail->ac.get();
    ac.diagnose(ln);
}

void node_schedule(struct LabSoundAPI_1_0* ls,
    ls_Node n, ls_Seconds when, int count)
{
    if (!count)
        return;

    auto ln = ls_node(ls, n);
    if (!ln)
        return;

    lab::SampledAudioNode* san =
        dynamic_cast<lab::SampledAudioNode*>(ln.get());
    if (san) {
        san->schedule(when.t, count);
        return;
    }

    lab::AudioScheduledSourceNode* asn =
        dynamic_cast<lab::AudioScheduledSourceNode*>(ln.get());
    if (!asn)
        return;

    asn->start(when.t); // disregarding count.
}

const char* node_scheduled_state_name(struct LabSoundAPI_1_0* ls,
                                         ls_Node n)
{
    auto ln = ls_node(ls, n);
    if (!ln)
        return "Nonexistent node";
    
    return schedulingStateName(ln->schedulingState());
}


void node_stop(struct LabSoundAPI_1_0* ls,
    ls_Node n, ls_Seconds when) 
{
    auto ln = ls_node(ls, n);
    if (!ln)
        return;

    lab::SampledAudioNode* san =
        dynamic_cast<lab::SampledAudioNode*>(ln.get());
    if (san) {
        san->stop(when.t);
        return;
    }

    lab::AudioScheduledSourceNode* asn =
        dynamic_cast<lab::AudioScheduledSourceNode*>(ln.get());
    if (!asn)
        return;

    asn->stop(when.t);
}
 

// getting pins from nodes
//
ls_InputPin node_named_input(
        struct LabSoundAPI_1_0* ls,
        ls_Node n, ls_StringSlice str) 
{
    auto ln = ls_node(ls, n);
    if (!ln)
        return ls_InputPin_empty;

    string name(str.start, str.end - str.start);
    string path = string(ln->name()) + "(" + name;

    // return the pin if it's pin looked up already
    ecs_entity_t e = ecs_lookup(ls->_detail->ecs, path.c_str());
    if (e)
        return ls_InputPin { e };

    int ic = ln->numberOfInputs();
    for (int idx = 0; idx < ic; ++idx) {
        auto i = ln->input(idx);
        if (i->name() == name) {
            // create the input pin on the fly

            e_Input eIn { n, idx };
            ecs_entity_desc_t desc = { 0 };
            desc.name = path.c_str();
            e = ecs_entity_init(ls->_detail->ecs, &desc);
            ecs_add(ls->_detail->ecs, e, e_Input);
            ecs_set_id(ls->_detail->ecs, e, 
                       ecs_id(e_Input), sizeof(eIn), &eIn);

            // associate the pin with the node
            ecs_add_pair(ls->_detail->ecs, n.id, ls->_detail->has_input_rel, e);
            ecs_add_pair(ls->_detail->ecs, e, EcsChildOf, n.id); // autodelete

            return ls_InputPin { e };
        }
    }

    return ls_InputPin_empty;
}

ls_InputPin node_indexed_input(
        struct LabSoundAPI_1_0* ls,
        ls_Node n, int index)
{
    auto ln = ls_node(ls, n);
    if (!ln)
        return ls_InputPin_empty;

    if (ln->numberOfInputs() <= index)
        return ls_InputPin_empty;

    string name = ln->input(index)->name();
    string path = string(ln->name()) + "(" + name;

    // return the pin if it's pin looked up already
    ecs_entity_t e = ecs_lookup(ls->_detail->ecs, path.c_str());
    if (e)
        return ls_InputPin { e };

    // create the input pin on the fly
    e_Input eIn { n, index };
    ecs_entity_desc_t desc = { 0 };
    desc.name = path.c_str();
    e = ecs_entity_init(ls->_detail->ecs, &desc);
    ecs_add(ls->_detail->ecs, e, e_Input);
    ecs_set_id(ls->_detail->ecs, e, 
            ecs_id(e_Input), sizeof(eIn), &eIn);

    // associate the pin with the node
    ecs_add_pair(ls->_detail->ecs, n.id, ls->_detail->has_input_rel, e);
    ecs_add_pair(ls->_detail->ecs, e, EcsChildOf, n.id); // autodelete

    return ls_InputPin { e };
}

ls_OutputPin node_named_output(struct LabSoundAPI_1_0* ls,
    ls_Node n, ls_StringSlice str) 
{
    auto ln = ls_node(ls, n);
    if (!ln)
        return ls_OutputPin_empty;

    string name(str.start, str.end - str.start);
    string path = string(ln->name()) + ")" + name;

    // return the pin if it's pin looked up already
    ecs_entity_t e = ecs_lookup(ls->_detail->ecs, path.c_str());
    if (e)
        return ls_OutputPin { e };

    // create the output pin on the fly
    int ic = ln->numberOfOutputs();
    for (int index = 0; index < ic; ++index) {
        auto output = ln->output(index);
        if (output->name() == name) {
            e_Output eOutput { n, index };
            ecs_entity_desc_t desc = { 0 };
            desc.name = path.c_str();
            e = ecs_entity_init(ls->_detail->ecs, &desc);
            ecs_add(ls->_detail->ecs, e, e_Output);
            ecs_set_id(ls->_detail->ecs, e, 
                    ecs_id(e_Input), sizeof(eOutput), &eOutput);

            // associate the pin with the node
            ecs_add_pair(ls->_detail->ecs, n.id, ls->_detail->has_output_rel, e);
            ecs_add_pair(ls->_detail->ecs, e, EcsChildOf, n.id); // autodelete

            return ls_OutputPin { e };
        }
        ++index;
    }
    return ls_OutputPin_empty;
}

ls_OutputPin node_indexed_output(struct LabSoundAPI_1_0* ls,
    ls_Node n, int index) 
{
    auto ln = ls_node(ls, n);
    if (!ln)
        return ls_OutputPin_empty;

    if (index >= ln->numberOfOutputs())
        return ls_OutputPin_empty;

    string name = ln->output(index)->name();
    string path = string(ln->name()) + ")" + name;

    // return the pin if it's pin looked up already
    ecs_entity_t e = ecs_lookup(ls->_detail->ecs, path.c_str());
    if (e)
        return ls_OutputPin { e };

    // create the input pin on the fly
    e_Output eOut { n, index };
    ecs_entity_desc_t desc = { 0 };
    desc.name = path.c_str();
    e = ecs_entity_init(ls->_detail->ecs, &desc);
    ecs_add(ls->_detail->ecs, e, e_Output);
    ecs_set_id(ls->_detail->ecs, e, 
            ecs_id(e_Output), sizeof(eOut), &eOut);

    // associate the pin with the node
    ecs_add_pair(ls->_detail->ecs, n.id, ls->_detail->has_output_rel, e);
    ecs_add_pair(ls->_detail->ecs, e, EcsChildOf, n.id); // autodelete

    return ls_OutputPin { e };
}

ls_InputPin node_parameter(struct LabSoundAPI_1_0* ls,
    ls_Node n, ls_StringSlice name)
{
    auto ln = ls_node(ls, n);
    if (!ln)
        return ls_InputPin_empty;

    string nameStr(name.start, name.end - name.start);
    string path = string(ln->name()) + "*" + nameStr;

    // return the pin if it's pin looked up already
    ecs_entity_t e = ecs_lookup(ls->_detail->ecs, path.c_str());
    if (e)
        return ls_InputPin { e };

    int param = ln->param_index(nameStr.c_str());
    if (param < 0)
        return ls_InputPin_empty;

    e_Param eParam { n, param };
    ecs_entity_desc_t desc = { 0 };
    desc.name = path.c_str();
    e = ecs_entity_init(ls->_detail->ecs, &desc);
    ecs_add(ls->_detail->ecs, e, e_Param);
    ecs_set_id(ls->_detail->ecs, e, 
            ecs_id(e_Param), sizeof(eParam), &eParam);

    // associate the pin with the node
    ecs_add_pair(ls->_detail->ecs, n.id, ls->_detail->has_param_rel, e);
    ecs_add_pair(ls->_detail->ecs, e, EcsChildOf, n.id); // autodelete

    return ls_InputPin { e };
}

ls_InputPin node_setting(struct LabSoundAPI_1_0* ls,
    ls_Node n, ls_StringSlice name)
{
    auto ln = ls_node(ls, n);
    if (!ln)
        return ls_InputPin_empty;

    string nameStr(name.start, name.end - name.start); 
    string path = string(ln->name()) + string("@") + nameStr;

    // return the pin if it's pin looked up already
    auto w = ls->_detail->ecs;
    ecs_entity_t e = ecs_lookup(w, path.c_str());
    if (e)
        return ls_InputPin { e };

    int setting = ln->setting_index(nameStr.c_str());
    if (setting < 0)
        return ls_InputPin_empty;

    e_Setting eSetting { n, setting };
    ecs_entity_desc_t desc = { 0 };
    desc.name = path.c_str();
    e = ecs_entity_init(w, &desc);
    ecs_add(ls->_detail->ecs, e, e_Setting);
    ecs_set_id(ls->_detail->ecs, e, 
            ecs_id(e_Setting), sizeof(eSetting), &eSetting);

    // associate the pin with the node
    ecs_add_pair(w, n.id, ls->_detail->has_setting_rel, e);
    ecs_add_pair(w, e, EcsChildOf, n.id); // autodelete

    return ls_InputPin { e };
}

void node_set_on_ended(struct LabSoundAPI_1_0* ls,
    ls_Node n, void(*fn)())
{
    auto ln = ls_node(ls, n);
    if (!ln)
        return;

    auto san = dynamic_cast<lab::AudioScheduledSourceNode*>(ln.get());
    if (!san)
        return;

    san->setOnEnded([fn]() { fn(); });
}


ls_PinKind pin_kind(struct LabSoundAPI_1_0* ls,
    ls_InputPin p)
{
    auto w = ls->_detail->ecs;
    if (!ecs_is_alive(w, p.id))
        return ls_PinKindInvalid;

    const e_Input* ei = ecs_get(w, p.id, e_Input);
    if (ei) return ls_PinKindInput;
    const e_Output* eo = ecs_get(w, p.id, e_Output);
    if (eo) return ls_PinKindOutput;
    const e_Param* ep = ecs_get(w, p.id, e_Param);
    if (eo) return ls_PinKindParam;
    const e_Setting* es = ecs_get(w, p.id, e_Setting);
    if (es) return ls_PinKindSetting;
    return ls_PinKindInvalid;
}

ls_PinDataType pin_data_type(struct LabSoundAPI_1_0* ls,
    ls_InputPin p)
{
    auto w = ls->_detail->ecs;
    if (!ecs_is_alive(w, p.id))
        return ls_Invalid; 

    const e_Input* ei = ecs_get(w, p.id, e_Input);
    if (ei) return ls_Bus;
    const e_Output* eo = ecs_get(w, p.id, e_Output);
    if (eo) return ls_Bus;
    const e_Param* ep = ecs_get(w, p.id, e_Param);
    if (eo) return ls_Float;
    const e_Setting* es = ecs_get(w, p.id, e_Setting);
    if (es) {
        auto ln = ls_node(ls, es->node);
        if (!ln)
            return ls_Invalid;
        switch (ln->settings()[es->index]->type()) {
            case lab::SettingType::None:
                return ls_Invalid;
            case lab::SettingType::Bool:
                return ls_Bool;
            case lab::SettingType::Integer:
                return ls_Int;
            case lab::SettingType::Float:
                return ls_Float;
            case lab::SettingType::Enum:
                return ls_Enum;
            case lab::SettingType::Bus:
                return ls_Bus;
        }
    }
    return ls_Invalid;
}

// managing nodes
//
const ls_NameArray* node_names(struct LabSoundAPI_1_0* ls)
{
    static auto src_names = lab::NodeRegistry::Instance().Names();
    static ls_NameArray names;
    static bool init = true;
    if (init) {
        names.names = (ls_StringSlice*) ls->_detail->alloc.malloc(
                    sizeof(ls_StringSlice) * src_names.size());
        names.count = (int) src_names.size();
        for (int i = 0; i < names.count; ++i) {
            names.names[i].start = src_names[i].c_str();
            names.names[i].end = names.names[i].start + src_names[i].size();
        }
    }
    return &names;
}

ls_Node node_create(struct LabSoundAPI_1_0* ls, 
    ls_StringSlice name, ls_StringSlice type)
{
    string n(type.start, type.end - type.start);
    lab::AudioContext& ac = *ls->_detail->ac.get();
    lab::AudioNode* node = lab::NodeRegistry::Instance().Create(n, ac);
    if (!node)
        return ls_Node_empty;

    string path(name.start, name.end - name.start);
    uint32_t id = ls->_detail->next_id++;
    ls->_detail->nodes.insert({ id , shared_ptr<lab::AudioNode>(node) });
    e_Node en = { id };
    ecs_entity_desc_t desc = { 0 };
    desc.name = path.c_str();
    auto e = ecs_entity_init(ls->_detail->ecs, &desc);
    ecs_add(ls->_detail->ecs, e, e_Node);
    ecs_set_id(ls->_detail->ecs, e, 
            ecs_id(e_Node), sizeof(en), &en);

    return ls_Node { e };
}

void node_delete(struct LabSoundAPI_1_0* ls,
    ls_Node n)
{
    auto w = ls->_detail->ecs;
    if (!ecs_is_alive(w, n.id))
        return;

    auto ln = ls_node(ls, n);
    if (!ln)
        return;

    ls->_detail->ac->disconnect(ln);

    ecs_query_desc_t desc = {0};
    desc.filter.terms[0].id = ecs_pair(ls->_detail->has_connection_rel, n.id);
    ecs_query_t *q = ecs_query_init(w, &desc);
    ecs_iter_t it = ecs_query_iter(w, q);
    while (ecs_query_next(&it)) {
/*        e_Node* eTo = ecs_term(&it, e_Node, 1);
        if (!eTo)
            continue;
        ls_Connection* c = ecs_term(&it, ls_Connection, 2);
        if (!c)
            continue;
        const e_Connection* ec = ecs_get(w, c->id, e_Connection);
        if (!ec)
            continue;
        const e_Param* ep = ecs_get(w, ec->to.id, e_Param);
        if (ep) {
            const e_Node* paramNode = ecs_get(w, ep->node.id, e_Node);
            if (!paramNode)
                continue;

            ls->_detail->ac->disconnectParam(paramNode->node->param(ep->index), eTo->node, ep->index);
        }
        else {
            //ls->_detail->disconnectNode(c->node, eTo->node);
        }*/
    }

    const e_Node* en = ecs_get(w, n.id, e_Node);
    auto hash_it = ls->_detail->nodes.find(en->hash_index);
    if (hash_it != ls->_detail->nodes.end())
        ls->_detail->nodes.erase(hash_it);

    ecs_remove_all(w, n.id);    // remove components
    ecs_delete(w, n.id);           // and delete the node
}

void create_node_output(
        struct LabSoundAPI_1_0* ls,
        ls_Node n, ls_StringSlice name, int channels)
{
    auto w = ls->_detail->ecs;
    auto ln = ls_node(ls, n);
    if (!ln)
        return;

    string name_str(name.start, name.end - name.start);
    auto existing_output = ln->output(name_str.c_str());
    if (!existing_output) {
        auto new_output = std::unique_ptr<AudioNodeOutput>(new AudioNodeOutput(ln.get(), name_str.c_str(), channels));
        lab::ContextGraphLock r(ls->_detail->ac.get(), "create_node_output");
        ln->addOutput(r, std::move(new_output));
    }
    else {
        lab::ContextRenderLock r(ls->_detail->ac.get(), "create_node_output");
        if (existing_output->bus(r)->numberOfChannels() != channels) {
            existing_output->bus(r)->setNumberOfChannels(r, channels);
        }
    }
}

// setting and getting pin values
//
void set_float(struct LabSoundAPI_1_0* ls,
    ls_InputPin p, float val)
{
    auto w = ls->_detail->ecs;

    const e_Param* ep = ecs_get(w, p.id, e_Param);
    if (ep) {
        auto ln = ls_node(ls, ep->node);
        if (ln && ln->param(ep->index))
            ln->param(ep->index)->setValue(val);
        return;
    }
    
    const e_Setting* es = ecs_get(w, p.id, e_Setting);
    if (es) {
        auto ln = ls_node(ls, ep->node);
        if (ln && ln->setting(es->index))
            ln->setting(es->index)->setFloat(val);
    }
}

void set_int(struct LabSoundAPI_1_0* ls,
    ls_InputPin p, uint32_t i)
{
    auto w = ls->_detail->ecs;
    if (!ecs_is_alive(w, p.id))
        return;
    
    const e_Setting* es = ecs_get(w, p.id, e_Setting);
    if (es) {
        auto ln = ls_node(ls, es->node);
        if (ln && ln->setting(es->index))
            ln->setting(es->index)->setUint32(i);
    }
}

void set_enum(struct LabSoundAPI_1_0* ls,
    ls_InputPin p, uint32_t i)
{
    auto w = ls->_detail->ecs;
    if (!ecs_is_alive(w, p.id))
        return;
    
    const e_Setting* es = ecs_get(w, p.id, e_Setting);
    if (es) {
        auto ln = ls_node(ls, es->node);
        if (ln && ln->setting(es->index))
            ln->setting(es->index)->setEnumeration(i);
    }
}

void set_named_enum(struct LabSoundAPI_1_0* ls,
    ls_InputPin p, ls_StringSlice enum_name)
{
    auto w = ls->_detail->ecs;
    if (!ecs_is_alive(w, p.id))
        return;
    
    const e_Setting* es = ecs_get(w, p.id, e_Setting);
    if (es) {
        auto ln = ls_node(ls, es->node);
        if (ln && ln->setting(es->index)) {
            string name(enum_name.start, enum_name.end - enum_name.start);
            ln->setting(es->index)->setEnumeration(name.c_str());
        }
    }
}

void set_bool(struct LabSoundAPI_1_0* ls,
    ls_InputPin p, bool b)
{
    auto w = ls->_detail->ecs;
    if (!ecs_is_alive(w, p.id))
        return;
    
    const e_Setting* es = ecs_get(w, p.id, e_Setting);
    if (es) {
        auto ln = ls_node(ls, es->node);
        if (ln && ln->setting(es->index))
            ln->setting(es->index)->setBool(b);
     }
}

void set_bus(struct LabSoundAPI_1_0* ls,
    ls_InputPin p, ls_BusData d)
{
    auto w = ls->_detail->ecs;
    if (!ecs_is_alive(w, p.id) || !ecs_is_alive(w, d.id))
        return;

    const e_Setting* es = ecs_get(w, p.id, e_Setting);
    if (!es)
        return;

    auto ln = ls_node(ls, es->node);
    if (ln && ln->setting(es->index)) {
        const e_BusData* bd = ecs_get(w, d.id, e_BusData);
        auto it = ls->_detail->busses.find(bd->hash_index);
        if (it != ls->_detail->busses.end() && it->second.get()) {
            ln->setting(es->index)->setBus(it->second.get());
        }
    }
}

void set_bus_from_file(struct LabSoundAPI_1_0* ls,
    ls_InputPin p, ls_StringSlice path)
{
    auto w = ls->_detail->ecs;
    if (!ecs_is_alive(w, p.id))
        return;

    const e_Setting* es = ecs_get(w, p.id, e_Setting);
    if (!es)
        return;

    auto ln = ls_node(ls, es->node);
    if (ln && ln->setting(es->index)) {
        string pathStr(path.start, path.end - path.start);
        auto soundClip = lab::MakeBusFromFile(pathStr.c_str(), false);
        ln->setting(es->index)->setBus(soundClip.get());
    }
}

ls_Node destination_node(struct LabSoundAPI_1_0* ls)
{
    return ls->_detail->destination_node;
}


// graph management
//
ls_Connection connect(struct LabSoundAPI_1_0* ls,
    ls_InputPin input, ls_OutputPin output)
{
    auto w = ls->_detail->ecs;
    if (!ecs_is_alive(w, input.id) || !ecs_is_alive(w, output.id))
        return ls_Connection_empty;

    const e_Output* source_output = ecs_get(w, output.id, e_Output);
    if (!source_output || !ecs_is_alive(w, source_output->node.id))
        return ls_Connection_empty;

    auto source_node = ls_node(ls, source_output->node);
    if (!source_node)
        return ls_Connection_empty;

    const e_Input* dest_input = ecs_get(w, input.id, e_Input);
    if (dest_input) {
        auto dst_node = ls_node(ls, dest_input->node);
        if (!dst_node)
            return ls_Connection_empty;

        // connect node output to node input
        ls->_detail->ac->connect(dst_node, source_node, 
                dest_input->index, source_output->index);

        ecs_entity_desc_t desc = { 0 };
        auto e = ecs_entity_init(w, &desc);
        ls_Connection lc { e };
        e_Connection ec { output, input, 
            source_output->node, dest_input->node, source_output->index, dest_input->index, false };
        ecs_add(w, e, e_Connection);
        ecs_set_id(ls->_detail->ecs, e, 
                ecs_id(e_Connection), sizeof(ec), &ec);
        ecs_add_pair(w, source_output->node.id, ls->_detail->has_connection_rel, e);
        ecs_add_pair(w, e, EcsChildOf, source_output->node.id); // autodelete
        ecs_add_pair(w, dest_input->node.id, ls->_detail->has_connection_rel, e);
        ecs_add_pair(w, e, EcsChildOf, dest_input->node.id); // autodelete
        return lc;
    }

    const e_Param* p = ecs_get(w, input.id, e_Param);
    if (!p || !ecs_is_alive(w, p->node.id))
        return ls_Connection_empty;

    auto param_node = ls_node(ls, p->node);
    if (!param_node)
        return ls_Connection_empty;

    // connect node output to param
    ls->_detail->ac->connectParam(param_node->param(p->index), source_node, source_output->index);
    
    ecs_entity_desc_t desc = { 0 };
    auto connection_e = ecs_entity_init(w, &desc);
    ls_Connection lc { connection_e };
    e_Connection ec{ output, input, source_output->node, p->node, source_output->index, p->index, true };
    ecs_add(w, connection_e, e_Connection);
    ecs_set_id(ls->_detail->ecs, connection_e, 
                ecs_id(e_Connection), sizeof(ec), &ec);
    ecs_add_pair(w, source_output->node.id, ls->_detail->has_connection_rel, connection_e);
    ecs_add_pair(w, connection_e, EcsChildOf, source_output->node.id); // autodelete
    ecs_add_pair(w, p->node.id, ls->_detail->has_connection_rel, connection_e);
    ecs_add_pair(w, connection_e, EcsChildOf, p->node.id); // autodelete

    return lc;
}

void disconnect(struct LabSoundAPI_1_0* ls,
    ls_Connection c)
{
    const e_Connection* ec = ls_connection(ls, c);
    if (!ec)
        return;

    auto from_node = ls_node(ls, ec->from_node);
    auto to_node = ls_node(ls, ec->to_node);
    if (!from_node || !to_node)
        return;

    if (ec->to_is_param) {
        ls->_detail->ac->disconnectParam(
            to_node->param(ec->to_index), from_node, ec->from_output_index);
    }
    else {
        ls->_detail->ac->disconnect(
            to_node, from_node, ec->to_index, ec->from_output_index);
    }
    ecs_delete(ls->_detail->ecs, c.id);
}

static
int ecs_run_action(
    ecs_world_t* world,
    ecs_app_desc_t* desc)
{
    if (desc->init) {
        desc->init(world);
    }

    auto api = (LabSoundAPI_1_0*)desc->ctx;
    api->_detail->desc = *desc;

    return ecs_app_run_frame(world, desc) == 0;
}


static
ls_BusData bus_create_from_file(struct LabSoundAPI_1_0* ls, 
    const char* name, bool mix_to_mono)
{
    const std::string path(name);
    std::shared_ptr<lab::AudioBus> bus;
    try {
        bus = lab::MakeBusFromFile(path, mix_to_mono);
    }
    catch (...) {
        return ls_BusData_empty;
    }
    if (!bus)
        return ls_BusData_empty;

    uint32_t id = ls->_detail->next_id++;
    ls->_detail->busses.insert({ id , bus });
    e_BusData en = { id };
    ecs_entity_desc_t desc = { 0 };
    desc.name = path.c_str();
    auto e = ecs_entity_init(ls->_detail->ecs, &desc);
    ecs_add(ls->_detail->ecs, e, e_BusData);
    ecs_set_id(ls->_detail->ecs, e,
        ecs_id(e_BusData), sizeof(en), &en);

    return ls_BusData{ e };
}


} // namespace ls_1_0

extern "C"
void ls_idle(struct LabSoundAPI_1_0* ls) {
    if (!ls || !ls->_detail->ecs)
        return;

    /* int result = */ ecs_app_run_frame(ls->_detail->ecs, &ls->_detail->desc) /* == 0 */;
}


extern "C"
struct LabSoundAPI_1_0* ls_create_api_1_0(ls_Alloc alloc) {

    LabSoundAPI_1_0* api = (LabSoundAPI_1_0*) 
        alloc.malloc(sizeof(LabSoundAPI_1_0));
    if (!api)
        return nullptr;

    memset(api, 0, sizeof(LabSoundAPI_1_0));
    api->_detail = (LabSoundAPI_1_0_Detail*)
        alloc.malloc(sizeof(LabSoundAPI_1_0_Detail));
    if (!api->_detail)
        return nullptr;
    memset(api->_detail, 0, sizeof(LabSoundAPI_1_0_Detail));
    new(api->_detail) LabSoundAPI_1_0_Detail();
    api->_detail->alloc = alloc;

    AudioStreamConfig _inputConfig;
    AudioStreamConfig _outputConfig;
    auto config = GetDefaultAudioDeviceConfiguration(true);
    _inputConfig = config.first;
    _outputConfig = config.second;
    std::shared_ptr<lab::AudioDevice_RtAudio> device(new lab::AudioDevice_RtAudio(_inputConfig, _outputConfig));
    auto context = std::make_shared<lab::AudioContext>(false, true);
    auto destinationNode = std::make_shared<lab::AudioDestinationNode>(*context.get(), device);
    device->setDestinationNode(destinationNode);
    context->setDestinationNode(destinationNode);
    api->_detail->ac = context;

    api->_detail->ecs = ecs_init();
    ECS_COMPONENT_DEFINE(api->_detail->ecs, e_Node);
    ECS_COMPONENT_DEFINE(api->_detail->ecs, e_Input);
    ECS_COMPONENT_DEFINE(api->_detail->ecs, e_Output);
    ECS_COMPONENT_DEFINE(api->_detail->ecs, e_Param);
    ECS_COMPONENT_DEFINE(api->_detail->ecs, e_Setting);
    ECS_COMPONENT_DEFINE(api->_detail->ecs, e_Connection);
    ECS_COMPONENT_DEFINE(api->_detail->ecs, e_BusData);
    api->_detail->has_input_rel = ecs_new_id(api->_detail->ecs);
    api->_detail->has_output_rel = ecs_new_id(api->_detail->ecs);
    api->_detail->has_param_rel = ecs_new_id(api->_detail->ecs);
    api->_detail->has_setting_rel = ecs_new_id(api->_detail->ecs);
    api->_detail->has_connection_rel = ecs_new_id(api->_detail->ecs);

    EcsRest ecs_rest{ 0 };
    ecs_set_id(api->_detail->ecs, FLECS__EEcsRest, FLECS__EEcsRest, sizeof(EcsRest), &ecs_rest);
    //ecs_singleton_set(api->_detail->ecs, EcsRest, ecs_rest);

    ecs_app_set_run_action(ls_1_0::ecs_run_action);

    ecs_app_desc_t app{ 0 };
    app.target_fps = 60.f;
    app.delta_time = 0.f;
    app.threads = 0;
    app.enable_rest = true;
    app.ctx = (void*)api;

    app.enable_rest = true;
    ecs_app_run(api->_detail->ecs, &app);

    api->_detail->next_id = 1;
    e_Node en = { api->_detail->next_id };
    ecs_entity_desc_t desc = { 0 };
    desc.name = "Device Node";
    auto e = ecs_entity_init(api->_detail->ecs, &desc);
    ecs_add(api->_detail->ecs, e, e_Node);
    ecs_set_id(api->_detail->ecs, e,
        ecs_id(e_Node), sizeof(en), &en);

    api->_detail->nodes.insert({ api->_detail->next_id, api->_detail->ac->destinationNode() });
    api->_detail->destination_node = { e };
    api->_detail->next_id++;

    using namespace ls_1_0;

    // operations on nodes
    //
    api->node_diagnose = node_diagnose;
    api->node_get_timing = node_get_timing;
    api->node_get_self_timing = node_get_self_timing;
    api->node_start = node_start;
    api->node_schedule = node_schedule;
    api->node_scheduled_state_name = node_scheduled_state_name;
    api->node_stop = node_stop;

    // getting pins from nodes
    //
    api->node_named_input = node_named_input;
    api->node_indexed_input = node_indexed_input;
    api->node_named_output = node_named_output;
    api->node_indexed_output = node_indexed_output;
    api->node_parameter = node_parameter;
    api->node_setting = node_setting;
    api->pin_kind = pin_kind;
    api->pin_data_type = pin_data_type;
    api->node_set_on_ended = node_set_on_ended;


    // managing nodes
    //
    api->node_names = node_names;
    api->node_create = node_create;
    api->node_delete = node_delete;
    api->create_node_output = create_node_output;

    // setting and getting pin values
    //
    api->set_float = set_float;
    api->set_int = set_int;
    api->set_bool = set_bool;
    api->set_bus = set_bus;
    api->set_bus_from_file = set_bus_from_file;
    api->set_enum = set_enum;
    api->set_named_enum = set_named_enum;

    // managing busses
    api->bus_create_from_file = bus_create_from_file;

    // graph management
    //
    api->destination_node = destination_node;
    api->connect = connect;
    api->disconnect = disconnect;
    return api;
}    

