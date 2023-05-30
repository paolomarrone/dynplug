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
 * File author: Paolo Marrone
 */

#include "dynplug.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#ifdef _WIN32
#include "windows.h"
#endif

#include <sys/types.h> // For windows?
#include <sys/stat.h>
#include <errno.h>

#include <fcntl.h>
#include <unistd.h>

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

static int load_yaaaeapa_module (dynplug *instance) {
	instance->module_handle = dlmopen(LM_ID_NEWLM, instance->module_path, RTLD_NOW | RTLD_LOCAL);

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
			dlclose(instance->module_handle);
			instance->module_handle = NULL;
        	return 2;
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
	if (instance->module_handle && dlclose(instance->module_handle)) {
		fprintf(stderr, "dlclose error: %s\n", dlerror());
		// TODO: delete file from disk.
		// TODO: think better about this: it breaks when multiple dynplug instances are running and loading the same module...
	}
	instance->module_handle = NULL;
}

void dynplug_on_create(dynplug *instance) {
	instance->module_listener_status = 0;
	int r = pthread_mutex_init(&(instance->module_mtx), NULL)
		||  pthread_mutex_init(&(instance->module_listener_mtx), NULL);
	if (r) {
		fprintf(stderr, "dynplug_on_create: error while initializing mutex\n");
		return;
	}

}

void dynplug_on_destroy(dynplug *instance) {
	int r = pthread_mutex_destroy(&(instance->module_mtx))
		||  pthread_mutex_destroy(&(instance->module_listener_mtx));
	if (r) {
		fprintf(stderr, "dynplug_on_destroy: error while destroying mutex\n");
		return;
	}
}

void dynplug_init(dynplug *instance) {
	// We can't know how many times host may call this before fini

	pthread_mutex_lock(&(instance->module_mtx));
	pthread_mutex_lock(&(instance->module_listener_mtx));

	if (instance->module_listener_status == 0) {
		load_default_module(instance);
		int r = pthread_create(&(instance->module_listener_thread), NULL, dynplug_module_listener, (void*) instance);
		if (r != 0)
			fprintf(stderr, "dynplug_init: error while creating module_listener thread\n");
		else {
			instance->module_listener_status = 1;
		}
	}
	else if (instance->module_listener_status == 1) {
		// Ok
	}
	else if (instance->module_listener_status == 2) {
		// Cancel the stop order
		instance->module_listener_status = 1;
	}

	pthread_mutex_unlock(&(instance->module_mtx));
	pthread_mutex_unlock(&(instance->module_listener_mtx));
}

void dynplug_fini(dynplug *instance) {
	pthread_mutex_lock(&(instance->module_mtx));
	pthread_mutex_lock(&(instance->module_listener_mtx));

	unload_module(instance);
	load_default_module(instance); // Just in case the host calls process after fini...

	if (instance->module_listener_status == 0)
		;
	else if (instance->module_listener_status == 1)
		instance->module_listener_status = 2;
	else 
		fprintf(stderr, "dynplug_fini: error in module_listener_status\n");

	pthread_mutex_unlock(&(instance->module_mtx));
	pthread_mutex_unlock(&(instance->module_listener_mtx));

	if (pthread_join(instance->module_listener_thread, NULL) != 0) {
		fprintf(stderr, "dynplug_fini: error while joining with module_listener\n");
	}
}

void dynplug_set_sample_rate(dynplug *instance, float sample_rate) {
	instance->sample_rate = sample_rate;
	(*(instance->module_set_sample_rate))(sample_rate);
}

void dynplug_reset(dynplug *instance) {
	if (pthread_mutex_trylock(&(instance->module_mtx)) == 0) {
		(*(instance->module_reset))();
		pthread_mutex_unlock(&(instance->module_mtx));
	}
}

void dynplug_process(dynplug *instance, const float** x, float** y, int n_samples) {
	if (pthread_mutex_trylock(&(instance->module_mtx)) == 0) {
		(*(instance->module_process))(x, y, n_samples);
		pthread_mutex_unlock(&(instance->module_mtx));
	}
	else
		default_module_process(x, y, n_samples); // TODO: Check channels
}

void dynplug_set_parameter(dynplug *instance, int index, float value) {
	if (pthread_mutex_trylock(&(instance->module_mtx)) == 0) {
		(*(instance->module_set_parameter))(index, value);
		pthread_mutex_unlock(&(instance->module_mtx));
	}
	/* 	There should be no need to save the value for later. 
		The only case when the lock is not acquired is when the module_listener is 
		loading another module.
	*/
}

float dynplug_get_parameter(dynplug *instance, int index) {
	float v = 0.f;
	if (pthread_mutex_trylock(&(instance->module_mtx)) == 0) {
		v = (*(instance->module_get_parameter))(index);
		pthread_mutex_unlock(&(instance->module_mtx));
	}
	return v;
}

void dynplug_note_on(dynplug *instance, char note, char velocity) {
	if (pthread_mutex_trylock(&(instance->module_mtx)) == 0) {
		(*(instance->module_note_on))(note, velocity);	
		pthread_mutex_unlock(&(instance->module_mtx));
	}
}

void dynplug_note_off(dynplug *instance, char note) {
	if (pthread_mutex_trylock(&(instance->module_mtx)) == 0) {
		(*(instance->module_note_off))(note);	
		pthread_mutex_unlock(&(instance->module_mtx));
	}
}

void dynplug_pitch_bend(dynplug *instance, int value) {
	if (pthread_mutex_trylock(&(instance->module_mtx)) == 0) {
		(*(instance->module_pitch_bend))(value);	
		pthread_mutex_unlock(&(instance->module_mtx));
	}
}

void dynplug_mod_wheel(dynplug *instance, char value) {
	if (pthread_mutex_trylock(&(instance->module_mtx)) == 0) {
		(*(instance->module_mod_wheel))(value);
		pthread_mutex_unlock(&(instance->module_mtx));
	}
}



int msleep(unsigned int tms) {
	return usleep(tms * 1000);
}

void* dynplug_module_listener(void* data) {
	dynplug *instance = (dynplug*) data;

	char pipepath[261];
#ifdef _WIN32
	char tmpdir[247];
	GetTempPath(247, tmpdir);
#else
	char* tmpdir =  getenv("TMPDIR");
	if (tmpdir == NULL)
		tmpdir = "/tmp/";
#endif
	snprintf(pipepath, 261, "%sdynplug_magicpipe", tmpdir);
	
	int r;
	r = mkfifo(pipepath, 0666);
	if (r != 0) {
		if (errno == EEXIST) {
			printf("named pipe already exists: ok\n");
		}
		else {
			fprintf(stderr, "dynplug_module_listener: error creating named pipe - %d\n", errno);
			return NULL;
		}
	}

	int fd = open(pipepath, O_RDONLY | O_NONBLOCK);
	if (fd == -1) {
		fprintf(stderr, "dynplug_module_listener: error opening named pipe - %d\n", errno);
		return NULL;
	}

	while (1) {
		char module_path[300];
		int bytesread;

		if((bytesread = read(fd, module_path, 299)) <= 0) {
	    	msleep(1000);
	    	pthread_mutex_lock(&(instance->module_listener_mtx));
	    	if (instance->module_listener_status == 2) {
	    		instance->module_listener_status = 0;
	    		pthread_mutex_unlock(&(instance->module_listener_mtx));
	    		return NULL;
	    	}
	    	pthread_mutex_unlock(&(instance->module_listener_mtx));
	    	continue;
	    }
		
	    if (module_path[bytesread - 1] == '\n')
	    	module_path[bytesread - 1] = '\0';
	    else
        	module_path[bytesread] = '\0';
        printf("Received: %s\n", module_path);

        pthread_mutex_lock(&(instance->module_mtx));
		
		unload_module(instance);
		load_default_module(instance);
		
		strncpy(instance->module_path, module_path, 300);
		instance->module_path[299] = '\0';
		r = load_yaaaeapa_module(instance);
		if (r > 0) {
			fprintf(stderr, "dynplug_module_listener: error while loading module: %d\n", r);
		}
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

		pthread_mutex_unlock(&(instance->module_mtx));
	}

	if (close(fd) < 0) {
        fprintf(stderr, "dynplug_module_listener: error closing named pipe - %d\n", errno);
    }

	return NULL;
}
