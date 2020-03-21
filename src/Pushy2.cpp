//#include "plugin.hpp"
//
//
//struct Pushy : Module {
//	enum ParamIds {
//		KNOB1_PARAM,
//		KNOB2_PARAM,
//		KNOB3_PARAM,
//		KNOB4_PARAM,
//		KNOB5_PARAM,
//		KNOB6_PARAM,
//		KNOB7_PARAM,
//		KNOB8_PARAM,
//		NUM_PARAMS
//	};
//	enum InputIds {
//		NUM_INPUTS
//	};
//	enum OutputIds {
//		OUT1_OUTPUT,
//		OUT2_OUTPUT,
//		OUT3_OUTPUT,
//		OUT4_OUTPUT,
//		OUT5_OUTPUT,
//		OUT6_OUTPUT,
//		OUT7_OUTPUT,
//		OUT8_OUTPUT,
//		NUM_OUTPUTS
//	};
//	enum LightIds {
//		NUM_LIGHTS
//	};
//
//	Pushy() {
//		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
//		configParam(KNOB1_PARAM, 0.f, 1.f, 0.f, "");
//		configParam(KNOB2_PARAM, 0.f, 1.f, 0.f, "");
//		configParam(KNOB3_PARAM, 0.f, 1.f, 0.f, "");
//		configParam(KNOB4_PARAM, 0.f, 1.f, 0.f, "");
//		configParam(KNOB5_PARAM, 0.f, 1.f, 0.f, "");
//		configParam(KNOB6_PARAM, 0.f, 1.f, 0.f, "");
//		configParam(KNOB7_PARAM, 0.f, 1.f, 0.f, "");
//		configParam(KNOB8_PARAM, 0.f, 1.f, 0.f, "");
//	}
//
//	void process(const ProcessArgs& args) override {
//	}
//};
//
//
//struct PushyWidget : ModuleWidget {
//	PushyWidget(Pushy* module) {
//		setModule(module);
//		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Pushy.svg")));
//
//		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
//		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
//		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
//		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
//
//		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(8.89, 29.44)), module, Pushy::KNOB1_PARAM));
//		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(21.59, 29.44)), module, Pushy::KNOB2_PARAM));
//		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(34.29, 29.44)), module, Pushy::KNOB3_PARAM));
//		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(46.99, 29.44)), module, Pushy::KNOB4_PARAM));
//		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(8.89, 42.14)), module, Pushy::KNOB5_PARAM));
//		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(21.59, 42.14)), module, Pushy::KNOB6_PARAM));
//		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(34.29, 42.14)), module, Pushy::KNOB7_PARAM));
//		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(46.99, 42.14)), module, Pushy::KNOB8_PARAM));
//
//		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(8.89, 54.84)), module, Pushy::OUT1_OUTPUT));
//		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(21.59, 54.84)), module, Pushy::OUT2_OUTPUT));
//		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(34.29, 54.84)), module, Pushy::OUT3_OUTPUT));
//		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(46.99, 54.84)), module, Pushy::OUT4_OUTPUT));
//		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(8.89, 67.54)), module, Pushy::OUT5_OUTPUT));
//		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(21.59, 67.54)), module, Pushy::OUT6_OUTPUT));
//		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(34.29, 67.54)), module, Pushy::OUT7_OUTPUT));
//		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(46.99, 67.54)), module, Pushy::OUT8_OUTPUT));
//	}
//};
//
//
//Model* modelPushy = createModel<Pushy, PushyWidget>("Pushy");