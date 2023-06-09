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

#ifndef _VST3_CONTROLLER_H
#define _VST3_CONTROLLER_H

#include "config.h"
#include "config_vst3.h"

#include "public.sdk/source/vst/vsteditcontroller.h"
#include "public.sdk/source/vst/vstaudioeffect.h"

#include "dynplug.h"

using namespace Steinberg;
using namespace Steinberg::Vst;

class Controller : EditController, IMidiMapping {

public:
	static FUnknown *createInstance(void *context) {
        (void) context;
		return (IEditController *) new Controller();
	}

    //bool sendMessageToProcessor(const char* tag, const void* data, int size);
    tresult PLUGIN_API notify (IMessage* message) SMTG_OVERRIDE;

    void set_parameters_info(dynplug* instance);


	tresult PLUGIN_API initialize(FUnknown *context) SMTG_OVERRIDE;
	tresult PLUGIN_API setComponentState(IBStream *state) SMTG_OVERRIDE;
	tresult PLUGIN_API getMidiControllerAssignment(int32 busIndex, int16 channel, CtrlNumber midiControllerNumber, ParamID& id) SMTG_OVERRIDE;

	OBJ_METHODS (Controller, EditController)
	DEFINE_INTERFACES
		DEF_INTERFACE (IMidiMapping)
	END_DEFINE_INTERFACES (EditController)
	REFCOUNT_METHODS (EditController)

private:

};

#endif
