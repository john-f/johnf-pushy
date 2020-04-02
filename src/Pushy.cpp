/*
 *  v v v v    base voltage
 *  a a a a    amplitude of osc
 *  f f f f    freq of osc
 *  s s s s    shape of osc
 *  o o o o    outputs
 */

#include "plugin.hpp"

static const int KNOBS = 4;

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
        KNOB9_PARAM,
        KNOB10_PARAM,
        KNOB11_PARAM,
        KNOB12_PARAM,
        KNOB13_PARAM,
        KNOB14_PARAM,
        KNOB15_PARAM,
        KNOB16_PARAM,
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
        NUM_OUTPUTS
    };
    enum LightIds {
        ENUMS(LIGHT1_LIGHT, 3),
        ENUMS(LIGHT2_LIGHT, 3),
        ENUMS(LIGHT3_LIGHT, 3),
        ENUMS(LIGHT4_LIGHT, 3),
        NUM_LIGHTS
    };
    enum Modes {
        BASE,
        AMPLITUDE,
        FREQUENCY,
        SHAPE,
        NUM_MODES
    };

    const float DEFAULTS[NUM_MODES] = {
            5, // base
            5, // amp
            2, // freq
            4  // shape
    };
    const float STEP_SIZE[NUM_MODES] = {
            0.1,  // base
            0.1,  // amp
            0.05, // freq
            0.05  // shape
    };

    dsp::ExponentialSlewLimiter slewLimiter[KNOBS][NUM_MODES];

    Pushy() {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        // base
        configParam(KNOB1_PARAM, 0.f, 10.f, DEFAULTS[BASE], "");
        configParam(KNOB2_PARAM, 0.f, 10.f, DEFAULTS[BASE], "");
        configParam(KNOB3_PARAM, 0.f, 10.f, DEFAULTS[BASE], "");
        configParam(KNOB4_PARAM, 0.f, 10.f, DEFAULTS[BASE], "");
        // amp
        configParam(KNOB5_PARAM, 0.f, 10.f, DEFAULTS[AMPLITUDE], "");
        configParam(KNOB6_PARAM, 0.f, 10.f, DEFAULTS[AMPLITUDE], "");
        configParam(KNOB7_PARAM, 0.f, 10.f, DEFAULTS[AMPLITUDE], "");
        configParam(KNOB8_PARAM, 0.f, 10.f, DEFAULTS[AMPLITUDE], "");
        // freq
        configParam(KNOB9_PARAM, 0.f, 10.f, DEFAULTS[FREQUENCY], "");
        configParam(KNOB10_PARAM, 0.f, 10.f, DEFAULTS[FREQUENCY], "");
        configParam(KNOB11_PARAM, 0.f, 10.f, DEFAULTS[FREQUENCY], "");
        configParam(KNOB12_PARAM, 0.f, 10.f, DEFAULTS[FREQUENCY], "");
        // shape
        configParam(KNOB13_PARAM, 0.f, 10.f, DEFAULTS[SHAPE], "");
        configParam(KNOB14_PARAM, 0.f, 10.f, DEFAULTS[SHAPE], "");
        configParam(KNOB15_PARAM, 0.f, 10.f, DEFAULTS[SHAPE], "");
        configParam(KNOB16_PARAM, 0.f, 10.f, DEFAULTS[SHAPE], "");
        onReset();
    }

    float phase[KNOBS];
    float oldValues[KNOBS][NUM_MODES];
    float realValues[KNOBS][NUM_MODES];
    float slewValues[KNOBS][NUM_MODES];
    bool fine;
    bool locked;
    bool active[4];

    float stepSize[NUM_MODES];

    midi::InputQueue midiInput;

    void onReset() override {
        fine = false;
        locked = false;

        for (int i = 0; i < KNOBS; i++) {
            active[i] = false;
            phase[i] = 0;
            for (int mode = 0; mode < NUM_MODES; mode++) {
                realValues[i][mode] = DEFAULTS[mode];
                slewValues[i][mode] = DEFAULTS[mode];
                slewLimiter[i][mode].setRiseFall(100, 100);
            }
        }

        midiInput.reset();
    }

    void process(const ProcessArgs &args) override {
        midi::Message msg;
        // loop through all midi messages
        while (midiInput.shift(&msg)) {
            processMessage(msg);
        }
        // check knobs for changes
        for (int i = 0; i < KNOBS; i++) {
            for (int mode = 0; mode < NUM_MODES; mode++) {
                int n = i + (4 * mode);
                float v = params[n].getValue();
                if (v != oldValues[i][mode]) {
                    setRealValue(i, mode, v);
                }
                oldValues[i][mode] = params[n].getValue();
            }
        }

        for (int i = 0; i < KNOBS; i++) {
            // do knobs -> values slew
            for (int mode = 0; mode < NUM_MODES; mode++) {
                slewValues[i][mode] = slewLimiter[i][mode].process(args.sampleTime, realValues[i][mode]);
            }

            // do oscillations
            float freq = slewValues[i][FREQUENCY];

            // accumulate the phase
            phase[i] += freq * args.sampleTime;
            if (phase[i] >= 2.f)
                phase[i] -= 2.f;

            // invert phase
            float x = 2 - phase[i];

            // todo: store when knob is turned
            int algo = int(slewValues[i][SHAPE] / 2);
            float mix = fmod(slewValues[i][SHAPE] / 2, 1);
            float invertMix = 1;
            float algo1 = 0;
            float algo2 = 0;
            float n = 0;

            switch (algo) {
                case 0:
                    // linear sweep
                    algo1 = fmax(
                            (x / -mix) + 1,
                            ((-x + 4 * mix - 2) / (mix - 1)) - 3
                    );
                    break;
                case 1:
                    // linear sweep to sine
                    invertMix = 1 - mix;
                    // linear
                    n = x * mix / 2;
                    algo1 = fmax(
                            (-x / (1 - n)) + 1,
                            ((x + 4 * n - 2) / n) - 3
                    );
                    // sine
                    algo2 = (4 * (x - 1) * (abs(x - 1) - 1));
                    break;
                case 2:
                    // sine to rounded
                    invertMix = 1 - mix;
                    // sine
                    algo1 = (4 * (x - 1) * (abs(x - 1) - 1));
                    // square
                    algo2 = cbrt(4 * (x - 1) * (abs(x - 1) - 1));
                    break;
                case 3:
                    // rounded to square
                    invertMix = 1 - mix;
                    // rounded
                    algo1 = cbrt(4 * (x - 1) * (abs(x - 1) - 1));
                    // square
                    algo2 = 2 * floor(fmod(x + 1, 2.f)) - 1;
                    break;
                case 4:
                    // square to pulse
                    algo1 = (-(x * x) + (3 * mix + 1)) > 0 ? 1 : -1;
                    break;
                case 5:
                default:
                    // always high
                    algo1 = 1;
                    break;
            }
            float y = invertMix * algo1 + mix * algo2;

            float amp = slewValues[i][AMPLITUDE];

            // output 0-10v, centered on +5v
            float osc = 5 + y * amp / 2;
            float v = slewValues[i][BASE] + osc - 5;
            v = clamp(v, 0.0f, 10.0f);

            outputs[i].setVoltage(v);
        }
    }

    void processMessage(midi::Message msg) {
        switch (msg.getStatus()) {
            case 0xb: { // cc
                processCC(msg);
                break;
            }
            case 0x9: { // note on
                uint8_t note = msg.getNote();
                if (!locked && note < KNOBS) {
                    int red = note * 3;
                    if (msg.getValue() == 0) {
                        active[note] = false;
                        lights[red].setBrightness(0.0f);
                    } else {
                        active[note] = true;
                        lights[red].setBrightness(1.0f);
                    }
                } else if (note == 9 && msg.getValue() != 0) {
                    bool anyActive = false;
                    for (bool knob : active) {
                        anyActive = anyActive || knob;
                    }
                    if (anyActive) {
                        locked = !locked;
                        for (int i = 0; i < KNOBS; i++) {
                            if (active[i]) {
                                int red = i * 3;
                                int blue = i * 3 + 2;
                                lights[blue].setBrightness(locked ? 1.0f : 0.0f);
                                lights[red].setBrightness(locked ? 0.0f : 1.0f);

                                // reset
                                // lights[red].setBrightness(0.0f);
                                //if (!locked) active[i] = false;
                            }
                        }
                    }
                } else if (note == 10) {
                    fine = msg.getValue() != 0;
                }
                break;
            }
        }
    }

    void processCC(midi::Message msg) {
        uint8_t cc = msg.getNote();

        // ignore things that aren't the knobs 1-8
        if (cc < 71 || cc > 78) return;

        int knob = cc - 71 + 1;

        float ccValue = msg.bytes[2];
        ccValue = clamp(ccValue, 0.0f, 127.0f);

        if (knob <= 4) {
            int i = knob - 1;
            handleKnobTurn(i, BASE, ccValue);
        }
        if (knob > 4) {
            // knobs: 5 = base, 6 = amplitude, 7 = frequency, 8 = shape
            // modes: 0 = base, 1 = amplitude, 2 = frequency, 3 = shape
            int mode = knob - 5;
            for (int i = 0; i < KNOBS; i++) {
                if (active[i]) {
                    handleKnobTurn(i, mode, ccValue);
                }
            }
        }
    }

    void handleKnobTurn(int i, int mode, float ccValue) {
        int n = i + (4 * mode);
        float v = params[n].getValue();
        // account for knob acceleration
        if (ccValue <= 63.0f) {
            v += ccValue * STEP_SIZE[mode] * (fine ? 0.1f : 1.0f);
        } else {
            v += (128.0f - ccValue) * -STEP_SIZE[mode] * (fine ? 0.1f : 1.0f);
        }
        v = clamp(v, 0.f, 10.f);
        params[n].setValue(v);
        oldValues[i][mode] = v;
        setRealValue(i, mode, v);
    }

    void setRealValue(int i, int mode, float v) {
        if (mode == BASE || mode == SHAPE) {
            realValues[i][mode] = v;
        } else if (mode == FREQUENCY) {
            float freq = powf(1.4f, v) - 1;
            realValues[i][FREQUENCY] = freq;
        } else if (mode == AMPLITUDE) {
            // attenuverter
            float a;
            if (v < 5) {
                a = -(powf(1.27098f, 2 * (5 - v)) - 1);
            } else {
                a = powf(1.27098f, 2 * (v - 5)) - 1;
            }
            realValues[i][AMPLITUDE] = a;
        }
    }

    json_t *dataToJson() override {
        json_t *rootJ = json_object();
        json_object_set_new(rootJ, "midi", midiInput.toJson());

        json_object_set_new(rootJ, "locked", json_boolean(locked));

        json_t* activeJ = json_array();
        for (int i = 0; i < KNOBS; i++) {
            json_array_append_new(activeJ, json_boolean(locked && active[i]));
        }
        json_object_set_new(rootJ, "active", activeJ);

        json_object_set_new(rootJ, "midi", midiInput.toJson());

        return rootJ;
    }

    void dataFromJson(json_t *rootJ) override {
        json_t *midiJ = json_object_get(rootJ, "midi");
        if (midiJ) midiInput.fromJson(midiJ);

        locked = json_boolean_value(json_object_get(rootJ, "locked"));
        if (locked) {
            json_t *activeJ = json_object_get(rootJ, "active");
            if (activeJ) {
                for (int i = 0; i < KNOBS; i++) {
                    json_t *iJ = json_array_get(activeJ, i);
                    if (iJ) {
                        active[i] = json_boolean_value(iJ);
                        if (active[i]) {
                            int blue = i * 3 + 2;
                            lights[blue].setBrightness(1.0f);
                        }
                    }
                }
            }
        }
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

        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(8.89, 30.71)), module, Pushy::KNOB1_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(21.59, 30.71)), module, Pushy::KNOB2_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(34.29, 30.71)), module, Pushy::KNOB3_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(46.99, 30.71)), module, Pushy::KNOB4_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(8.89, 43.41)), module, Pushy::KNOB5_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(21.59, 43.41)), module, Pushy::KNOB6_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(34.29, 43.41)), module, Pushy::KNOB7_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(46.99, 43.41)), module, Pushy::KNOB8_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(8.89, 56.11)), module, Pushy::KNOB9_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(21.59, 56.11)), module, Pushy::KNOB10_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(34.29, 56.11)), module, Pushy::KNOB11_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(46.99, 56.11)), module, Pushy::KNOB12_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(8.89, 68.81)), module, Pushy::KNOB13_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(21.59, 68.81)), module, Pushy::KNOB14_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(34.29, 68.81)), module, Pushy::KNOB15_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(46.99, 68.81)), module, Pushy::KNOB16_PARAM));

        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(8.89, 81.51)), module, Pushy::OUT1_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(21.59, 81.51)), module, Pushy::OUT2_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(34.29, 81.51)), module, Pushy::OUT3_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(46.99, 81.51)), module, Pushy::OUT4_OUTPUT));

        addChild(createLightCentered<MediumLight<RedGreenBlueLight>>(mm2px(Vec(8.89, 20.127)), module,
                                                                     Pushy::LIGHT1_LIGHT));
        addChild(createLightCentered<MediumLight<RedGreenBlueLight>>(mm2px(Vec(21.59, 20.127)), module,
                                                                     Pushy::LIGHT2_LIGHT));
        addChild(createLightCentered<MediumLight<RedGreenBlueLight>>(mm2px(Vec(34.29, 20.127)), module,
                                                                     Pushy::LIGHT3_LIGHT));
        addChild(createLightCentered<MediumLight<RedGreenBlueLight>>(mm2px(Vec(46.99, 20.127)), module,
                                                                     Pushy::LIGHT4_LIGHT));

        PushyMidiWidget *midiWidget = createWidget<PushyMidiWidget>(mm2px(Vec(3.41891, 128.5 - 28 - 2.54)));
        midiWidget->box.size = mm2px(Vec(49.08, 22.92));

        midiWidget->setMidiPort(module ? &module->midiInput : nullptr);
        addChild(midiWidget);
    }
};

Model *modelPushy = createModel<Pushy, PushyWidget>("Pushy");