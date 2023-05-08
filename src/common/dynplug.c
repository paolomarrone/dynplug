#include "dynplug.h"

#include <stdio.h>
#include <dlfcn.h>

#include "config.h"

// Default empty functions

static void default_module_init(void) {}
static void default_module_fini(void) {}
static void default_module_set_sample_rate(float sample_rate) { (void)sample_rate; }
static void default_module_reset(void) {}
static void default_module_process(const float** x, float** y, int n_samples) { 
	(void)x; (void)y; (void)n_samples;
	for (int i = 0; i < NUM_CHANNELS_OUT; i++)
		for (int s = 0; s < n_samples; s++)
			y[i][s] = 0.f;
}
static void default_module_set_parameter(int index, float value) { (void)index; (void)value; }
static float default_module_get_parameter(int index) { (void) index; return 0.f; }
static void default_module_note_on(char note, char velocity) { (void)note; (void)velocity; }
static void default_module_note_off(char note) { (void)note; }
static void default_module_pitch_bend(int bend) { (void)bend; }
static void default_module_mod_wheel(char wheel) { (void)wheel; }
static void default_module_get_parameter_info (int index, char** name, char** shortName, char* out, char* bypass, int* steps, float* defaultValueUnmapped) {
	(void) index; (void) name; (void) shortName; (void) out; (void) bypass; (void) steps; (void) defaultValueUnmapped;
}
static void load_default_module (dynplug* instance) {
	instance->module_init = &default_module_init;
	instance->module_fini = &default_module_fini;
	instance->module_set_sample_rate = &default_module_set_sample_rate;
	instance->module_reset = &default_module_reset;
	instance->module_process = &default_module_process;
	instance->module_set_parameter = &default_module_set_parameter;
	instance->module_get_parameter = &default_module_get_parameter;
	instance->module_note_on = &default_module_note_on;
	instance->module_note_off = &default_module_note_off;
	instance->module_pitch_bend = &default_module_pitch_bend;
	instance->module_mod_wheel = &default_module_mod_wheel;
	instance->module_parameters_n = 0;
	instance->module_buses_in_n = 0;
	instance->module_buses_out_n = 0;
	instance->module_channels_in_n = 0;
	instance->module_channels_out_n = 0;
	instance->module_get_parameter_info = &default_module_get_parameter_info;
}

static int load_yaaaeapa_module (dynplug* instance, const char* path) {
	instance->module_handle = dlmopen(LM_ID_NEWLM, path, RTLD_NOW | RTLD_LOCAL);

    if (!instance->module_handle) {
        fprintf(stderr, "dlmopen error: %s\n", dlerror());
        return 1;
    }
    
    const int sym_n = 17;
    const char* sym_names[sym_n] = {
		"yaaaeapa_init",
		"yaaaeapa_fini",
		"yaaaeapa_set_sample_rate",
		"yaaaeapa_reset",
		"yaaaeapa_process",
		"yaaaeapa_set_parameter",
		"yaaaeapa_get_parameter",
		"yaaaeapa_note_on",
		"yaaaeapa_note_off",
		"yaaaeapa_pitch_bend",
		"yaaaeapa_mod_wheel",
    	"yaaaeapa_parameters_n",
		"yaaaeapa_buses_in_n",
		"yaaaeapa_buses_out_n",
		"yaaaeapa_channels_in_n",
		"yaaaeapa_channels_out_n",
		"yaaaeapa_get_parameter_info"
    };

   	void* syms[sym_n];

	for (int i = 0; i < sym_n; i++) {
		syms[i] = dlsym(instance->module_handle, sym_names[i]);
		if (!syms[i]) {
			fprintf(stderr, "dlsym error reading '%s': %s\n", sym_names[i], dlerror());
        	return 1;
		}
	}

	// From now on, we're assuming that funcs are in the right format and data is correct

	instance->module_init 				= (void (*)(void)) (syms[0]);
	instance->module_fini 				= (void (*)(void)) (syms[1]);
	instance->module_set_sample_rate 	= (void (*)(float)) (syms[2]);
	instance->module_reset 				= (void (*)(void)) (syms[3]);
	instance->module_process			= (void (*)(const float**, float**, int)) (syms[4]);
	instance->module_set_parameter		= (void (*)(int, float)) (syms[5]);
	instance->module_get_parameter		= (float(*)(int)) (syms[6]);
	instance->module_note_on			= (void (*)(char, char)) (syms[7]);
	instance->module_note_off			= (void (*)(char)) (syms[8]);
	instance->module_pitch_bend			= (void (*)(int)) (syms[9]);
	instance->module_mod_wheel			= (void (*)(char)) (syms[10]);
    instance->module_parameters_n 		= *((int*) (syms[11]));
	instance->module_buses_in_n 		= *((int*) (syms[12]));
	instance->module_buses_out_n 		= *((int*) (syms[13]));
	instance->module_channels_in_n 		= *((int*) (syms[14]));
	instance->module_channels_out_n		= *((int*) (syms[15]));
	instance->module_get_parameter_info = (void (*)(int, char**, char**, char*, char*, int*, float*)) (syms[16]);

/*
	for (int i = 0; i < instance->module_parameters_n; i++) {
		//char *name, *shortName, out, bypass;
		//int steps;
		//float defaultValueUnmapped;
		//instance->module_get_parameter_info(i, &name, &shortName, &out, &bypass, &steps, &defaultValueUnmapped);
	}
*/

	return 0;
}


static void unload_module(dynplug* instance) {
	(*(instance->module_fini))();
	if (!dlclose(instance->module_handle)) {
		fprintf(stderr, "dlclose error: %s\n", dlerror());
	}
	instance->module_handle = NULL;
}

void dynplug_init(dynplug *instance) {
	
	// One day, here's the place to just init to default and to launch the server
	// All the following will be made in server's thread

	load_default_module(instance);

	// Test for now
	// TODO: check errors
	load_yaaaeapa_module(instance, "/tmp/dynplug/bw_example_fx_bitcrush.so");
	dynplug_set_parameters_info(instance);
	(*(instance->module_init))();
	
}

void dynplug_fini(dynplug *instance) {
	unload_module(instance);
	// TODO: stop the server
}

void dynplug_set_sample_rate(dynplug *instance, float sample_rate) {
	(*(instance->module_set_sample_rate))(sample_rate);
}

void dynplug_reset(dynplug *instance) {
	(*(instance->module_reset))();
}

void dynplug_process(dynplug *instance, const float** x, float** y, int n_samples) {
	(*(instance->module_process))(x, y, n_samples);
}

void dynplug_set_parameter(dynplug *instance, int index, float value) {
	(*(instance->module_set_parameter))(index, value);
}

float dynplug_get_parameter(dynplug *instance, int index) {
	return (*(instance->module_get_parameter))(index);
}

void dynplug_note_on(dynplug *instance, char note, char velocity) {
	(*(instance->module_note_on))(note, velocity);	
}

void dynplug_note_off(dynplug *instance, char note) {
	(*(instance->module_note_off))(note);	
}

void dynplug_pitch_bend(dynplug *instance, int value) {
	(*(instance->module_pitch_bend))(value);	
}

void dynplug_mod_wheel(dynplug *instance, char value) {
	(*(instance->module_mod_wheel))(value);
}

