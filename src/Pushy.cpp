#include "plugin.hpp"

static const int KNOBS = 8;
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
        NUM_LIGHTS
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
    }

    int8_t values[128];
    int ccs[128];

    midi::InputQueue midiInput;

	void process(const ProcessArgs& args) override {
        midi::Message msg;
        // loop through all midi messages
        while (midiInput.shift(&msg)) {
            processMessage(msg);
        }
        for (int i=0; i < KNOBS; i++) {
            outputs[i].setVoltage(params[i].getValue());
        }
	}

    void processMessage(midi::Message msg) {
        switch (msg.getStatus()) {
            case 0xb: { // cc
                processCC(msg);
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

        float v = outputs[knob].getVoltage();

        v += value <= uint8_t(63) ? value * 0.1f : (128 - value) * -0.1f;
        v = clamp(v, 0.f, 10.f);
        params[knob].setValue(v);

    }
};


struct PushyMidiWidget : MidiWidget {
    void setMidiPort(midi::Port* port) {
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
	PushyWidget(Pushy* module) {
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

        PushyMidiWidget* midiWidget = createWidget<PushyMidiWidget>(mm2px(Vec(3.41891, 128.5 - 28 - 2.54)));
        midiWidget->box.size = mm2px(Vec(49.08, 22.92));

        midiWidget->setMidiPort(module ? &module->midiInput : NULL);
        addChild(midiWidget);
	}
};

Model* modelPushy = createModel<Pushy, PushyWidget>("Pushy");