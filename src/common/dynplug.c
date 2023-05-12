#include "dynplug.h"

#include <stdio.h>
#include <dlfcn.h>

#include "config.h"

// Default empty functions

static void default_module_init(void) {}
static void default_module_fini(void) {}
static void default_module_set_sample_rate(float sample_rate) { (void)sample_rate; }
static void default_module_reset(void) {}
static void default_module_process(const float** x, float** y, int n_samples) { // TODO: Check channels
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
static void default_module_get_parameter_info (int index, char** name, char** shortName, char** units, char* out, char* bypass, int* steps, float* defaultValueUnmapped) {
	(void) index; (void) name; (void) shortName; (void) units; (void) out; (void) bypass; (void) steps; (void) defaultValueUnmapped;
}
static void load_default_module (dynplug *instance) {
	instance->module_handle = NULL;
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

static int load_yaaaeapa_module (dynplug *instance, const char* path) {
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
	instance->module_get_parameter_info = (void (*)(int, char**, char**, char**, char*, char*, int*, float*)) (syms[16]);

	return 0;
}


static void unload_module(dynplug *instance) {
	(*(instance->module_fini))();
	if (instance->module_handle && dlclose(instance->module_handle))
		fprintf(stderr, "dlclose error: %s\n", dlerror());
	instance->module_handle = NULL;
}

void dynplug_on_create(dynplug *instance) {
	instance->server_status = 0;
	int r = pthread_mutex_init(&(instance->mtx), NULL);
	if (r) {
		fprintf(stderr, "dynplug_on_create: error while initializing mutex\n");
		return;
	}
}

void dynplug_on_destroy(dynplug *instance) {
	int r = pthread_mutex_destroy(&(instance->mtx));
	if (r) {
		fprintf(stderr, "dynplug_on_destroy: error while destroying mutex\n");
		return;
	}
}

void dynplug_init(dynplug *instance) {
	// We can't know how many times host may call this before fini

	int r;
	r = pthread_mutex_lock(&(instance->mtx));

	if (r) {
		fprintf(stderr, "dynplug_init: error while acquiring lock\n");
		return;
	}

	if (instance->server_status == 0) {
		load_default_module(instance);
		// Let's start it;
		r = pthread_create(&(instance->server_thread), NULL, dynplug_server, (void*) instance);
		if (r != 0)
			fprintf(stderr, "dynplug_init: error while creating server thread\n");
		else {
			instance->server_status = 1;
		}
	}
	else if (instance->server_status == 1) {
		// Ok
	}
	else if (instance->server_status == 2) {
		// Cancel the stop order
		instance->server_status = 1;
	}

	r = pthread_mutex_unlock(&(instance->mtx));
	if (r) {
		fprintf(stderr, "dynplug_init: error while releasing lock\n");
		return;
	}
}

void dynplug_fini(dynplug *instance) {
	int r;
	r = pthread_mutex_lock(&(instance->mtx));
	if (r) {
		fprintf(stderr, "dynplug_server: error while acquiring lock\n");
		return;
	}

	unload_module(instance);
	load_default_module(instance); // Just in case the host calls process after fini...

	if (instance->server_status == 0)
		;
	else if (instance->server_status == 1)
		instance->server_status = 2;
	else 
		fprintf(stderr, "dynplug_fini: error in server_status\n");

	r = pthread_mutex_unlock(&(instance->mtx));
	if (r) {
		fprintf(stderr, "dynplug_fini: error while releasing lock\n");
		return;
	}

	if (pthread_join(instance->server_thread, NULL) != 0) {
		fprintf(stderr, "dynplug_fini: error while joining with server\n");
	}
}

void dynplug_set_sample_rate(dynplug *instance, float sample_rate) {
	instance->sample_rate = sample_rate;
	(*(instance->module_set_sample_rate))(sample_rate);
}

void dynplug_reset(dynplug *instance) {
	if (pthread_mutex_trylock(&(instance->mtx)) == 0) {
		(*(instance->module_reset))();
		pthread_mutex_unlock(&(instance->mtx));
	}
}

void dynplug_process(dynplug *instance, const float** x, float** y, int n_samples) {
	if (pthread_mutex_trylock(&(instance->mtx)) == 0) {
		(*(instance->module_process))(x, y, n_samples);
		pthread_mutex_unlock(&(instance->mtx));
	}
	else
		default_module_process(x, y, n_samples); // TODO: Check channels
}

void dynplug_set_parameter(dynplug *instance, int index, float value) {
	if (pthread_mutex_trylock(&(instance->mtx)) == 0) {
		(*(instance->module_set_parameter))(index, value);
		pthread_mutex_unlock(&(instance->mtx));
	}
	/* 	There should be no need to save the value for later. 
		The only case when the lock is not acquired is when the server is 
		loading another module.
	*/
}

float dynplug_get_parameter(dynplug *instance, int index) {
	float v = 0.f;
	if (pthread_mutex_trylock(&(instance->mtx)) == 0) {
		v = (*(instance->module_get_parameter))(index);
		pthread_mutex_unlock(&(instance->mtx));
	}
	return v;
}

void dynplug_note_on(dynplug *instance, char note, char velocity) {
	if (pthread_mutex_trylock(&(instance->mtx)) == 0) {
		(*(instance->module_note_on))(note, velocity);	
		pthread_mutex_unlock(&(instance->mtx));
	}
}

void dynplug_note_off(dynplug *instance, char note) {
	if (pthread_mutex_trylock(&(instance->mtx)) == 0) {
		(*(instance->module_note_off))(note);	
		pthread_mutex_unlock(&(instance->mtx));
	}
}

void dynplug_pitch_bend(dynplug *instance, int value) {
	if (pthread_mutex_trylock(&(instance->mtx)) == 0) {
		(*(instance->module_pitch_bend))(value);	
		pthread_mutex_unlock(&(instance->mtx));
	}
}

void dynplug_mod_wheel(dynplug *instance, char value) {
	if (pthread_mutex_trylock(&(instance->mtx)) == 0) {
		(*(instance->module_mod_wheel))(value);
		pthread_mutex_unlock(&(instance->mtx));
	}
}


#include <unistd.h>

int msleep(unsigned int tms) {
	return usleep(tms * 1000);
}

void* dynplug_server(void* data) {
	
	// This is just a test

	msleep(4000);

	dynplug *instance = (dynplug*) data;


	int r;
	r = pthread_mutex_lock(&(instance->mtx)); // TODO: maybe trylock is better?

	if (r) {
		fprintf(stderr, "dynplug_server: error while acquiring lock\n");
		return NULL;
	}

	// TODO: this goes in the loop
	if (instance->server_status == 2) {
		// TODO: terminate thread
	}

	unload_module(instance);
	r = load_yaaaeapa_module(instance, "/tmp/dynplug/bw_example_fx_bitcrush.so");
	if (r)
		fprintf(stderr, "dynplug_server: error while loading module\n");
	else {
		dynplug_set_parameters_info(instance);
		(*(instance->module_init))();
		(*(instance->module_set_sample_rate))(instance->sample_rate);
		(*(instance->module_reset))();
		for (int i = 0; i < instance->module_parameters_n; i++) {
			float defaultValueUnmapped;
			instance->module_get_parameter_info(i, NULL, NULL, NULL, NULL, NULL, NULL, &defaultValueUnmapped);
			instance->module_set_parameter(i, defaultValueUnmapped);
		}
	}

	r = pthread_mutex_unlock(&(instance->mtx));
	if (r) {
		fprintf(stderr, "dynplug_server: error while releasing lock\n");
		return NULL;
	}

	return NULL;
}
