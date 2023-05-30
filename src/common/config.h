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
 * File author: Paolo Marrone, Stefano D'Angelo
 */

#ifndef _CONFIG_H
#define _CONFIG_H

// Definitions

#define IO_MONO			1
#define IO_STEREO		(1<<1)

struct config_io_bus {
	const char	*name;
	char		 out;
	char		 aux;
	char		 cv;
	char		 configs;
};

// Data

#define COMPANY_NAME		"Orastron"
#define COMPANY_WEBSITE		"https://www.orastron.com/"
#define COMPANY_MAILTO		"mailto:info@orastron.com"

#define PLUGIN_NAME			"dynplug"
#define PLUGIN_VERSION		"0.0.1"

#define NUM_BUSES_IN		1
#define NUM_BUSES_OUT		1
#define NUM_CHANNELS_IN		2	// TODO: Should be higher and fixed
#define NUM_CHANNELS_OUT	2	// ^

static struct config_io_bus config_buses_in[NUM_BUSES_IN] = {
	{ "Audio in", 0, 0, 0, IO_STEREO }
};

static struct config_io_bus config_buses_out[NUM_BUSES_OUT] = {
	{ "Audio out", 1, 0, 0, IO_STEREO }
};

#define NUM_PARAMETERS		30 // Max number of parameters

// Internal API

#include "dynplug.h"

#define P_TYPE				dynplug
#define P_INIT				dynplug_init
#define P_FINI 				dynplug_fini
#define P_SET_SAMPLE_RATE	dynplug_set_sample_rate
#define P_RESET				dynplug_reset
#define P_PROCESS			dynplug_process
#define P_SET_PARAMETER		dynplug_set_parameter
#define P_GET_PARAMETER		dynplug_get_parameter
#define P_NOTE_ON			dynplug_note_on
#define P_NOTE_OFF			dynplug_note_off
#define P_PITCH_BEND		dynplug_pitch_bend
#define P_MOD_WHEEL			dynplug_mod_wheel

#endif
