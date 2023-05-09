/*
 * Brickworks
 *
 * Copyright (C) 2022 Orastron Srl unipersonale
 *
 * Brickworks is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License.
 *
 * Brickworks is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Brickworks.  If not, see <http://www.gnu.org/licenses/>.
 *
 * File author: Stefano D'Angelo
 */

#include "controller.h"

#include "pluginterfaces/base/conststringtable.h"
#include "base/source/fstreamer.h"
#include "pluginterfaces/vst/ivstmidicontrollers.h"
#include "common.h"

#include <stdio.h> // tmp


// TODO: fix
bool Controller::sendMessageToProcessor(const char* tag, const void* data, int size) {
	auto message = allocateMessage();
	if (!message)
		return false;
	FReleaser msgReleaser(message);
	message->setMessageID("BinaryMessage");

	message->getAttributes()->setBinary(tag, &data, size); // Fix this hack
	sendMessage(message);
	return true;
}

tresult PLUGIN_API Controller::notify(IMessage* message) {
	printf("controller notify \n");
	if(!message)
		return kInvalidArgument;

	if(!strcmp( message->getMessageID(), "BinaryMessage")) {
		const void* data;
		uint32 size;
		if( message->getAttributes()->getBinary( "hi", data, size ) == kResultOk ) {
			printf("hi= %p\n", (dynplug*) (*((void**) data)));
			dynplug *inst = (dynplug*) (*((void**) data));
			this->set_parameters_info(inst);
			return kResultOk;
		}
	}
	return EditController::notify( message );
}

// To be called from UI thread
void Controller::set_parameters_info(dynplug* instance) {
	printf("Controller set_parameters_info A \n"); fflush(stdout);
	int n = instance->module_parameters_n;
printf("Controller set_parameters_info B %d \n", n); fflush(stdout);
	
	parameters.removeAll();

printf("Controller set_parameters_info C \n"); fflush(stdout);
	
	for (int p = 0; p < n; p++) {
		printf("Controller set_parameters_info D %d \n", p); fflush(stdout);
	
		char *name, *shortName, *units, out, bypass;
		int steps;
		float defaultValueUnmapped;
		instance->module_get_parameter_info(p, &name, &shortName, &units, &out, &bypass, &steps, &defaultValueUnmapped);

		parameters.addParameter(
			ConstStringTable::instance()->getString(name),
			units ? ConstStringTable::instance()->getString(units) : nullptr,
			steps,
			defaultValueUnmapped,
			(out ? ParameterInfo::kIsReadOnly | ParameterInfo::kIsHidden : ParameterInfo::kCanAutomate)
			| (bypass ? ParameterInfo::kIsBypass : 0),
			p,
			0,
			shortName ? ConstStringTable::instance()->getString(shortName) : nullptr
		);
	}

printf("Controller set_parameters_info E \n"); fflush(stdout);
	componentHandler->restartComponent(kReloadComponent);
printf("Controller set_parameters_info F \n"); fflush(stdout);

}

tresult PLUGIN_API Controller::initialize(FUnknown *context) {
	tresult r = EditController::initialize(context);
	if (r != kResultTrue)
		return r;

	// add parameters
	for (int i = 0; i < NUM_PARAMETERS; i++)
		parameters.addParameter(
			ConstStringTable::instance()->getString(config_parameters[i].name),
			config_parameters[i].units ? ConstStringTable::instance()->getString(config_parameters[i].units) : nullptr,
			config_parameters[i].steps,
			config_parameters[i].defaultValueUnmapped,
			(config_parameters[i].out ? ParameterInfo::kIsReadOnly | ParameterInfo::kIsHidden : ParameterInfo::kCanAutomate)
			| (config_parameters[i].bypass ? ParameterInfo::kIsBypass : 0),
			i,
			0,
			config_parameters[i].shortName ? ConstStringTable::instance()->getString(config_parameters[i].shortName) : nullptr
		);

#ifdef P_PITCH_BEND
	parameters.addParameter(
			ConstStringTable::instance()->getString("MIDI Pitch Bend"),
			nullptr,
			0,
			0.5f,
			ParameterInfo::kCanAutomate,
			TAG_PITCH_BEND,
			0,
			nullptr
		);
#endif

#ifdef P_MOD_WHEEL
	parameters.addParameter(
			ConstStringTable::instance()->getString("MIDI Mod Wheel"),
			nullptr,
			0,
			0.5f,
			ParameterInfo::kCanAutomate,
			TAG_MOD_WHEEL,
			0,
			nullptr
		);
#endif

	return kResultTrue;
}

tresult PLUGIN_API Controller::setComponentState(IBStream *state) {
	/*
	Maybe we should find a better place for this, 
	but while instantiating messages don't arrive...
	probably the system isn't fully ready... thank you vst3
	*/
	sendMessageToProcessor("procAddr", (const void*) this, sizeof(void*));

	if (!state)
		return kResultFalse;

	IBStreamer streamer(state, kLittleEndian);

	float f;
	for (int i = 0; i < NUM_PARAMETERS; i++) {
		if (config_parameters[i].out)
			continue;
		if (streamer.readFloat(f) == false)
			return kResultFalse;
		setParamNormalized(i, f);
	}

	return kResultTrue;
}

#if defined(P_PITCH_BEND) || defined(P_MOD_WHEEL)
tresult PLUGIN_API Controller::getMidiControllerAssignment(int32 busIndex, int16 channel, CtrlNumber midiControllerNumber, ParamID& id) {
	if (busIndex != 0)
		return kResultFalse;

#ifdef P_PITCH_BEND
	if (midiControllerNumber == Vst::kPitchBend) {
		id = TAG_PITCH_BEND;
		return kResultTrue;
	}
#endif

#ifdef P_MOD_WHEEL
	if (midiControllerNumber == Vst::kCtrlModWheel) {
		id = TAG_MOD_WHEEL;
		return kResultTrue;
	}
#endif

	return kResultFalse;
}
#endif
