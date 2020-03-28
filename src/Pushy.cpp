// ideas:
// only send changes to touched knobs (1-4)
// 5-8 become control knobs
// keep last touched knobs active
// can control multiple knobs
// only clear active knobs when starting a new touch
/*
 *  v v v v    base voltage
 *  a a a a    amplitude of osc
 *  f f f f    freq of osc
 *  s s s s    shape of osc
 *  o o o o    outputs
 */
// make sine default shape
// set other knobs defaults

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
        LIGHT1_LIGHT,
        LIGHT2_LIGHT,
        LIGHT3_LIGHT,
        LIGHT4_LIGHT,
        NUM_LIGHTS
    };
    enum Modes {
        BASE,
        AMPLITUDE,
        FREQUENCY,
        SHAPE,
        NUM_MODES
    };

    dsp::ExponentialSlewLimiter slewLimiter[KNOBS][NUM_MODES];

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
        configParam(KNOB9_PARAM, 0.f, 10.f, 0.f, "");
        configParam(KNOB10_PARAM, 0.f, 10.f, 0.f, "");
        configParam(KNOB11_PARAM, 0.f, 10.f, 0.f, "");
        configParam(KNOB12_PARAM, 0.f, 10.f, 0.f, "");
        configParam(KNOB13_PARAM, 0.f, 10.f, 0.f, "");
        configParam(KNOB14_PARAM, 0.f, 10.f, 0.f, "");
        configParam(KNOB15_PARAM, 0.f, 10.f, 0.f, "");
        configParam(KNOB16_PARAM, 0.f, 10.f, 0.f, "");
        onReset();
    }

    int ccs[128];
    float phase[KNOBS];
    //float frequency[KNOBS];
    //float amplitude[KNOBS];
    //float shape[KNOBS];
    float values[KNOBS][NUM_MODES];
    bool active[4];

    float stepSize[NUM_MODES];

    midi::InputQueue midiInput;

    void onReset() override {
        stepSize[BASE] = 0.1;
        stepSize[AMPLITUDE] = 0.1;
        stepSize[FREQUENCY] = 0.05;
        stepSize[SHAPE] = 0.05;

        for (int i = 0; i < KNOBS; i++) {
            active[i] = false;
            phase[i] = 0;
            for (int mode = 0; mode < NUM_MODES; mode++) {
                values[i][mode] = 0;
                slewLimiter[i][mode].setRiseFall(1000, 1000);
            }
        }

        midiInput.reset();
    }

    void process(const ProcessArgs &args) override {
//        lights[FREQ_LIGHT].setBrightness(1.0f);

        midi::Message msg;
        // loop through all midi messages
        while (midiInput.shift(&msg)) {
            processMessage(msg);
        }

        for (int i = 0; i < KNOBS; i++) {
            // do knobs -> values slew
            for (int mode = 0; mode < NUM_MODES; mode++) {
                int n = i + (4 * mode);
                values[i][mode] = slewLimiter[i][mode].process(args.sampleTime, params[n].getValue());
            }

            // do oscillations
            float freq = powf(1.36f, values[i][FREQUENCY]) - 1;

            // accumulate the phase
            phase[i] += freq * args.sampleTime;
            if (phase[i] >= 2.f)
                phase[i] -= 2.f;

            // invert phase
            float x = 2 - phase[i];

            // todo: store when knob is turned
            int algo = int(values[i][SHAPE] / 2);
            float mix = fmod(values[i][SHAPE] / 2, 1);
            float invertMix = 1;
            float algo1 = 0;
            float algo2 = 0;
            float n = 0;

            //if (i == 0) printf("algo %d, ratio %f, mode %d\n", algo, mix, mode);

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
                    algo2 = 2 * floor(fmod(x + 1, 2)) - 1;
                    break;
                case 4:
                    // square to pulse
                    algo1 = (-(x * x) + (3 * mix + 1)) > 0 ? 1 : -1;
                    break;
                case 5:
                    // always high
                    algo1 = 1;
                    break;
            }
            float y = invertMix * algo1 + mix * algo2;

            // todo: store when knob is turned, don't recalculate
            float amp = powf(1.27098f, values[i][AMPLITUDE]) - 1;
            // output 0-10v, centered on +5v
            float osc = 5.0f + y * amp / 2.0f;

            outputs[i].setVoltage(values[i][BASE]+osc-5.0f);
        }

        // reset knob values on mode change
        //if (mode != prevMode) {
        //    for (int i = 0; i < KNOBS; i++) {
        //        switch (mode) {
        //            case AMPLITUDE:
        //                params[i].setValue(amplitude[i]);
        //                break;
        //            case FREQUENCY:
        //                params[i].setValue(frequency[i]);
        //                break;
        //            case SHAPE:
        //                params[i].setValue(shape[i]);
        //                break;
        //        }
        //    }
        //}
    }

    void processMessage(midi::Message msg) {
        switch (msg.getStatus()) {
            case 0xb: { // cc
                processCC(msg);
                break;
            }
            case 0x9: { // note on
                uint8_t i = msg.getNote();
                if (i < 4) {
                    if (msg.getValue() == 0) {
                        active[i] = false;
                        lights[i].setBrightness(0.0f);
                    } else {
                        active[i] = true;
                        lights[i].setBrightness(1.0f);
                    }
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
        //printf("knob %d\n", knob);

        int8_t value = msg.bytes[2];
        value = clamp(value, 0, 127);

        if (knob <= 4) {
            float v = params[knob-1].getValue();
            // account for knob acceleration
            v += value <= uint8_t(63) ? value * stepSize[BASE] : (128 - value) * -stepSize[BASE];
            v = clamp(v, 0.f, 10.f);
            params[knob-1].setValue(v);
        }
        if (knob > 4) {
            float v;
            // knobs: 5 = base, 6 = amplitude, 7 = frequency, 8 = shape
            // modes: 0 = base, 1 = amplitude, 2 = frequency, 3 = shape
            int mode = knob - 5;
            printf("knob %d, mode %d\n", knob, mode);
            for (int i = 0; i<KNOBS; i++) {
                if (active[i]) {
                    int n = i + (4 * mode);
                    printf("setting param %d\n", n);
                    v = params[n].getValue();
                    // account for knob acceleration
                    v += value <= uint8_t(63) ? value * stepSize[mode] : (128 - value) * -stepSize[mode];
                    v = clamp(v, 0.f, 10.f);
                    params[n].setValue(v);
                }
            }
        }

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

        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(8.89, 25.63)), module, Pushy::KNOB1_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(21.59, 25.63)), module, Pushy::KNOB2_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(34.29, 25.63)), module, Pushy::KNOB3_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(46.99, 25.63)), module, Pushy::KNOB4_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(8.89, 38.33)), module, Pushy::KNOB5_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(21.59, 38.33)), module, Pushy::KNOB6_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(34.29, 38.33)), module, Pushy::KNOB7_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(46.99, 38.33)), module, Pushy::KNOB8_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(8.89, 51.03)), module, Pushy::KNOB9_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(21.59, 51.03)), module, Pushy::KNOB10_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(34.29, 51.03)), module, Pushy::KNOB11_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(46.99, 51.03)), module, Pushy::KNOB12_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(8.89, 63.73)), module, Pushy::KNOB13_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(21.59, 63.73)), module, Pushy::KNOB14_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(34.29, 63.73)), module, Pushy::KNOB15_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(46.99, 63.73)), module, Pushy::KNOB16_PARAM));

        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(8.89, 76.43)), module, Pushy::OUT1_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(21.59, 76.43)), module, Pushy::OUT2_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(34.29, 76.43)), module, Pushy::OUT3_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(46.99, 76.43)), module, Pushy::OUT4_OUTPUT));

        addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(8.89, 18.01)), module, Pushy::LIGHT1_LIGHT));
        addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(21.59, 18.01)), module, Pushy::LIGHT2_LIGHT));
        addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(34.29, 18.01)), module, Pushy::LIGHT3_LIGHT));
        addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(46.99, 18.01)), module, Pushy::LIGHT4_LIGHT));

        PushyMidiWidget *midiWidget = createWidget<PushyMidiWidget>(mm2px(Vec(3.41891, 128.5 - 28 - 2.54)));
        midiWidget->box.size = mm2px(Vec(49.08, 22.92));

        midiWidget->setMidiPort(module ? &module->midiInput : NULL);
        addChild(midiWidget);
    }
};

Model *modelPushy = createModel<Pushy, PushyWidget>("Pushy");