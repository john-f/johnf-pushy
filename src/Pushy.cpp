#include "plugin.hpp"

static const int KNOBS = 8;
static const float STEPSIZE = 0.1f;
//static const double U = 5.08f;

struct Pushy : Module {
    enum ParamIds {
        KNOB1_PARAM,
        KNOB2_PARAM,
        KNOB3_PARAM,
        KNOB4_PARAM,
        KNOB5_PARAM,
        KNOB6_PARAM,
        KNOB7_PARAM,
        KNOB8_PARAM,
        NUM_PARAMS
    };
    enum InputIds {
        NUM_INPUTS
    };
    enum OutputIds {
        OUT1_OUTPUT,
        OUT2_OUTPUT,
        OUT3_OUTPUT,
        OUT4_OUTPUT,
        OUT5_OUTPUT,
        OUT6_OUTPUT,
        OUT7_OUTPUT,
        OUT8_OUTPUT,
        NUM_OUTPUTS
    };
    enum LightIds {
        FREQ_LIGHT,
        NUM_LIGHTS
    };
    enum Modes {
        AMPLITUDE,
        FREQUENCY
    };

    Pushy() {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        configParam(KNOB1_PARAM, 0.f, 10.f, 0.f, "");
        configParam(KNOB2_PARAM, 0.f, 10.f, 0.f, "");
        configParam(KNOB3_PARAM, 0.f, 10.f, 0.f, "");
        configParam(KNOB4_PARAM, 0.f, 10.f, 0.f, "");
        configParam(KNOB5_PARAM, 0.f, 10.f, 0.f, "");
        configParam(KNOB6_PARAM, 0.f, 10.f, 0.f, "");
        configParam(KNOB7_PARAM, 0.f, 10.f, 0.f, "");
        configParam(KNOB8_PARAM, 0.f, 10.f, 0.f, "");
        onReset();
    }

    int8_t values[128];
    int ccs[128];
    float phase[KNOBS];
    float frequency[KNOBS];
    float amplitude[KNOBS];
    u_int8_t mode = AMPLITUDE;
    u_int8_t prevMode = FREQUENCY;

    midi::InputQueue midiInput;

    void onReset() override {
        for (int i = 0; i < KNOBS; i++) {
            phase[i] = 0;
            frequency[i] = 0;
            amplitude[i] = 0;
        }
        midiInput.reset();
    }

    void process(const ProcessArgs &args) override {
        prevMode = mode;
//        lights[FREQ_LIGHT].setBrightness(1.0f);

        midi::Message msg;
        // loop through all midi messages
        while (midiInput.shift(&msg)) {
            processMessage(msg);
        }

        for (int i = 0; i < KNOBS; i++) {
            float freq = powf(1.36f, frequency[i]) - 1;

            // Accumulate the phase
            phase[i] += freq * args.sampleTime;
            if (phase[i] >= 2.f)
                phase[i] -= 2.f;

            // Compute the sine output
            float x = phase[i];
            float y = abs((2 * x - 1) - 1) - 1;
            //if (i == 0) printf("phase %f, y %f\n", x, y);

            // todo: store when knob is turned, don't recalculate
            float amp = powf(1.27098f, amplitude[i]) - 1;
            // output 0-10v, centered on +5v
            outputs[i].setVoltage(5.0f + y * amp / 2.0f);
        }


        // reset knob values on mode change
        if (mode != prevMode) {
            for (int i = 0; i < KNOBS; i++) {
                mode == AMPLITUDE ?
                params[i].setValue(amplitude[i])
                                  : params[i].setValue(frequency[i]);
            }
        }
    }

    void processMessage(midi::Message msg) {
        switch (msg.getStatus()) {
            case 0xb: { // cc
                processCC(msg);
                break;
            }
            case 0x9: { // note on
                if (msg.getNote() == 10) {
                    if (msg.getValue() == 0) {
                        mode = AMPLITUDE;
                        lights[FREQ_LIGHT].setBrightness(0.0f);
                    } else {
                        mode = FREQUENCY;
                        lights[FREQ_LIGHT].setBrightness(1.0f);
                    }
                }
                break;
            }
        }
    }

    void processCC(midi::Message msg) {
        uint8_t cc = msg.getNote();

        // ignore things that aren't the knobs
        if (cc < 71 || cc > 78) return;

        int knob = cc - 71;

        int8_t value = msg.bytes[2];
        value = clamp(value, 0, 127);

        params[71 - cc].getValue();

//        float v = outputs[knob].getVoltage();
        float v;
        switch (mode) {
            case AMPLITUDE: {
                v = amplitude[knob];
                v += value <= uint8_t(63) ? value * STEPSIZE : (128 - value) * -STEPSIZE;
                v = clamp(v, 0.f, 10.f);
                amplitude[knob] = v;
                break;
            }
            case FREQUENCY: {
                v = frequency[knob];
                v += value <= uint8_t(63) ? value * STEPSIZE * 0.5f : (128 - value) * -STEPSIZE * 0.5f;
                v = clamp(v, 0.f, 10.f);
                frequency[knob] = v;
                break;
            }
        }

        // account for knob acceleration values
//        v += value <= uint8_t(63) ? value * STEPSIZE : (128 - value) * -STEPSIZE;
//        v = clamp(v, 0.f, 10.f);

        params[knob].setValue(v);

    }

    json_t *dataToJson() override {
        json_t *rootJ = json_object();
        json_object_set_new(rootJ, "midi", midiInput.toJson());
        return rootJ;
    }

    void dataFromJson(json_t *rootJ) override {
        json_t *midiJ = json_object_get(rootJ, "midi");
        if (midiJ) midiInput.fromJson(midiJ);
    }

};


struct PushyMidiWidget : MidiWidget {
    void setMidiPort(midi::Port *port) {
        MidiWidget::setMidiPort(port);

        driverChoice->textOffset = Vec(6.f, 14.7f);
        driverChoice->box.size = mm2px(Vec(driverChoice->box.size.x, 7.5f));
        driverChoice->color = nvgRGB(0xf0, 0xf0, 0xf0);

        driverSeparator->box.pos = driverChoice->box.getBottomLeft();

        deviceChoice->textOffset = Vec(6.f, 14.7f);
        deviceChoice->box.size = mm2px(Vec(deviceChoice->box.size.x, 7.5f));
        deviceChoice->box.pos = driverChoice->box.getBottomLeft();
        deviceChoice->color = nvgRGB(0xf0, 0xf0, 0xf0);

        deviceSeparator->box.pos = deviceChoice->box.getBottomLeft();

        channelChoice->textOffset = Vec(6.f, 14.7f);
        channelChoice->box.size = mm2px(Vec(channelChoice->box.size.x, 7.5f));
        channelChoice->box.pos = deviceChoice->box.getBottomLeft();
        channelChoice->color = nvgRGB(0xf0, 0xf0, 0xf0);
    }
};

struct PushyWidget : ModuleWidget {
    PushyWidget(Pushy *module) {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Pushy2.svg")));

        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(8.89, 29.44)), module, Pushy::KNOB1_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(21.59, 29.44)), module, Pushy::KNOB2_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(34.29, 29.44)), module, Pushy::KNOB3_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(46.99, 29.44)), module, Pushy::KNOB4_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(8.89, 42.14)), module, Pushy::KNOB5_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(21.59, 42.14)), module, Pushy::KNOB6_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(34.29, 42.14)), module, Pushy::KNOB7_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(46.99, 42.14)), module, Pushy::KNOB8_PARAM));

        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(8.89, 54.84)), module, Pushy::OUT1_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(21.59, 54.84)), module, Pushy::OUT2_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(34.29, 54.84)), module, Pushy::OUT3_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(46.99, 54.84)), module, Pushy::OUT4_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(8.89, 67.54)), module, Pushy::OUT5_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(21.59, 67.54)), module, Pushy::OUT6_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(34.29, 67.54)), module, Pushy::OUT7_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(46.99, 67.54)), module, Pushy::OUT8_OUTPUT));

        addChild(createLightCentered<SmallLight<GreenLight>>(mm2px(Vec(13.97 + 1.27, 54.84)), module,
                                                             Pushy::FREQ_LIGHT));

        PushyMidiWidget *midiWidget = createWidget<PushyMidiWidget>(mm2px(Vec(3.41891, 128.5 - 28 - 2.54)));
        midiWidget->box.size = mm2px(Vec(49.08, 22.92));

        midiWidget->setMidiPort(module ? &module->midiInput : NULL);
        addChild(midiWidget);
    }
};

Model *modelPushy = createModel<Pushy, PushyWidget>("Pushy");