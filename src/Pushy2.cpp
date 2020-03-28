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
//		KNOB9_PARAM,
//		KNOB10_PARAM,
//		KNOB11_PARAM,
//		KNOB12_PARAM,
//		KNOB13_PARAM,
//		KNOB14_PARAM,
//		KNOB15_PARAM,
//		KNOB16_PARAM,
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
//		NUM_OUTPUTS
//	};
//	enum LightIds {
//		LIGHT1_LIGHT,
//		LIGHT2_LIGHT,
//		LIGHT3_LIGHT,
//		LIGHT4_LIGHT,
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
//		configParam(KNOB9_PARAM, 0.f, 1.f, 0.f, "");
//		configParam(KNOB10_PARAM, 0.f, 1.f, 0.f, "");
//		configParam(KNOB11_PARAM, 0.f, 1.f, 0.f, "");
//		configParam(KNOB12_PARAM, 0.f, 1.f, 0.f, "");
//		configParam(KNOB13_PARAM, 0.f, 1.f, 0.f, "");
//		configParam(KNOB14_PARAM, 0.f, 1.f, 0.f, "");
//		configParam(KNOB15_PARAM, 0.f, 1.f, 0.f, "");
//		configParam(KNOB16_PARAM, 0.f, 1.f, 0.f, "");
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
//		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(8.89, 25.63)), module, Pushy::KNOB1_PARAM));
//		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(21.59, 25.63)), module, Pushy::KNOB2_PARAM));
//		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(34.29, 25.63)), module, Pushy::KNOB3_PARAM));
//		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(46.99, 25.63)), module, Pushy::KNOB4_PARAM));
//		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(8.89, 38.33)), module, Pushy::KNOB5_PARAM));
//		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(21.59, 38.33)), module, Pushy::KNOB6_PARAM));
//		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(34.29, 38.33)), module, Pushy::KNOB7_PARAM));
//		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(46.99, 38.33)), module, Pushy::KNOB8_PARAM));
//		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(8.89, 51.03)), module, Pushy::KNOB9_PARAM));
//		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(21.59, 51.03)), module, Pushy::KNOB10_PARAM));
//		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(34.29, 51.03)), module, Pushy::KNOB11_PARAM));
//		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(46.99, 51.03)), module, Pushy::KNOB12_PARAM));
//		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(8.89, 63.73)), module, Pushy::KNOB13_PARAM));
//		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(21.59, 63.73)), module, Pushy::KNOB14_PARAM));
//		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(34.29, 63.73)), module, Pushy::KNOB15_PARAM));
//		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(46.99, 63.73)), module, Pushy::KNOB16_PARAM));
//
//		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(8.89, 76.43)), module, Pushy::OUT1_OUTPUT));
//		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(21.59, 76.43)), module, Pushy::OUT2_OUTPUT));
//		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(34.29, 76.43)), module, Pushy::OUT3_OUTPUT));
//		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(46.99, 76.43)), module, Pushy::OUT4_OUTPUT));
//
//		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(8.89, 18.01)), module, Pushy::LIGHT1_LIGHT));
//		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(21.59, 18.01)), module, Pushy::LIGHT2_LIGHT));
//		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(34.29, 18.01)), module, Pushy::LIGHT3_LIGHT));
//		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(46.99, 18.01)), module, Pushy::LIGHT4_LIGHT));
//	}
//};
//
//
//Model* modelPushy = createModel<Pushy, PushyWidget>("Pushy");