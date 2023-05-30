/*
 * Dynplug
 *
 * Copyright (C) 2022 Orastron Srl unipersonale
 *
 * Copyright is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License.
 *
 * Copyright is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Copyright.  If not, see <http://www.gnu.org/licenses/>.
 *
 * File authors: Paolo Marrone, Stefano D'Angelo
 */

#include "controller.h"

#include "pluginterfaces/base/conststringtable.h"
#include "base/source/fstreamer.h"
#include "pluginterfaces/vst/ivstmidicontrollers.h"
#include "common.h"

#include <stdio.h>

/*
bool Controller::sendMessageToProcessor(const char* tag, const void* data, int size) {
	auto message = allocateMessage();
	if (!message)
		return false;
	FReleaser msgReleaser(message);
	message->setMessageID("BinaryMessage");

	message->getAttributes()->setBinary(tag, data, size);
	sendMessage(message);
	return true;
}
*/
tresult PLUGIN_API Controller::notify(IMessage* message) {
	if(!message)
		return kInvalidArgument;

	if(!strcmp( message->getMessageID(), "BinaryMessage")) {
		const void* data;
		uint32 size;
		if( message->getAttributes()->getBinary( "set_parameters_info", data, size ) == kResultOk ) {
			dynplug *inst = (dynplug*) (*((void**) data));
			this->set_parameters_info(inst);
			return kResultOk;
		}
	}
	return EditController::notify( message );
}

static void str16ncpy(char16_t* dest, char* src, int n) {
    int i = 0;
    while((i++ < n) && (*dest++ = *src++) != 0) ;
}


// To be called from UI thread
void Controller::set_parameters_info(dynplug* instance) {
	int n = instance->module_parameters_n;

	for (int i = 0; i < NUM_PARAMETERS; i++) {

		Parameter* p = parameters.getParameterByIndex(i);
		ParameterInfo& pi = p->getInfo();

		if (i < n) {	
			char *name, *shortName, *units, out, bypass;
			int steps;
			float defaultValueUnmapped;
			instance->module_get_parameter_info(i, &name, &shortName, &units, &out, &bypass, &steps, &defaultValueUnmapped);

			str16ncpy(pi.title, name, 127);
			pi.title[127] = '\0';
			str16ncpy(pi.shortTitle, shortName, 127);
			pi.shortTitle[127] = '\0';
			str16ncpy(pi.units, units, 127);
			pi.units[127] = '\0';
			pi.stepCount = static_cast<int32>(steps);
			pi.defaultNormalizedValue = static_cast<ParamValue>(defaultValueUnmapped);
			pi.flags = (out ? ParameterInfo::kIsReadOnly | ParameterInfo::kIsHidden : ParameterInfo::kCanAutomate)
				| (bypass ? ParameterInfo::kIsBypass : 0);
		}
		else {
			pi.flags = ParameterInfo::kIsHidden;
		}
	}
	// Not every host cares about this, but still
	componentHandler->restartComponent(kParamTitlesChanged | kParamValuesChanged);

	// Not every host cares about this, but still
	for (int i = 0; i < n; i++) {
		float defaultValueUnmapped;
		instance->module_get_parameter_info(i, NULL, NULL, NULL, NULL, NULL, NULL, &defaultValueUnmapped);
		beginEdit(i);
		performEdit(i, defaultValueUnmapped);
		endEdit(i);
	}
}

tresult PLUGIN_API Controller::initialize(FUnknown *context) {

	tresult r = EditController::initialize(context);
	if (r != kResultTrue)
		return r;

	// add parameters
	for (int i = 0; i < NUM_PARAMETERS; i++) {
		char s[7]; 
		sprintf(s,"p %d", i);
		char16_t ss[7];
		str16ncpy(ss, s, 7);
		parameters.addParameter(
			ss,
			nullptr,
			0,
			0.f,
			ParameterInfo::kCanAutomate,
			i,
			0,
			ss
		);
	}

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

	return kResultTrue;
}

tresult PLUGIN_API Controller::setComponentState(IBStream *state) {
	(void) state;
	// Let's avoid this for now
	/*
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
	*/
	return kResultTrue;
}

tresult PLUGIN_API Controller::getMidiControllerAssignment(int32 busIndex, int16 channel, CtrlNumber midiControllerNumber, ParamID& id) {
	(void) channel;
	if (busIndex != 0)
		return kResultFalse;

	if (midiControllerNumber == Vst::kPitchBend) {
		id = TAG_PITCH_BEND;
		return kResultTrue;
	}

	if (midiControllerNumber == Vst::kCtrlModWheel) {
		id = TAG_MOD_WHEEL;
		return kResultTrue;
	}

	return kResultFalse;
}
