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

struct config_parameter {
	const char	*name;
	const char	*shortName;
	const char	*units;
	char		 out;
	char		 bypass;
	int		 steps;
	float		 defaultValueUnmapped;
};

// Data

#define COMPANY_NAME		"Orastron"
#define COMPANY_WEBSITE		"https://www.orastron.com/"
#define COMPANY_MAILTO		"mailto:info@orastron.com"

#define PLUGIN_NAME			"dynplug"
#define PLUGIN_VERSION		"0.0.1"

#define NUM_BUSES_IN		1
#define NUM_BUSES_OUT		1
#define NUM_CHANNELS_IN		1	// Should be higher and fixed
#define NUM_CHANNELS_OUT	1	// ^

static struct config_io_bus config_buses_in[NUM_BUSES_IN] = {
	{ "Audio in", 0, 0, 0, IO_MONO }
};

static struct config_io_bus config_buses_out[NUM_BUSES_OUT] = {
	{ "Audio out", 1, 0, 0, IO_MONO }
};

#define NUM_PARAMETERS		100 // Max number of parameters

/*
static struct config_parameter config_parameters[NUM_PARAMETERS] = {
	{ "p1", "p1", "", 0, 0, 0, 0.5f },
	{ "p2", "p2", "", 0, 0, 0, 0.5f },
	{ "p3", "p3", "", 0, 0, 0, 0.5f },
	{ "p4", "p4", "", 0, 0, 0, 0.5f },
	{ "p5", "p5", "", 0, 0, 0, 0.5f }
	// TODO: some output parameters
};
*/

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
//#define P_MEM_REQ			dynplug_mem_req
//#define P_MEM_SET 			dynplug_mem_set

#endif
