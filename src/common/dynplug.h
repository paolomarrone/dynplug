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

#ifndef _DYNPLUG_H
#define _DYNPLUG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <pthread.h>

/* 
	We only support yaaaeapa for now. Maybe LADSPA in future?
*/

struct _dynplug {
	void*   data; // For example, this contains VST3's Plugin instance pointer
	void*	module_handle; // dl(m)open result
	char    module_path[300];
	pthread_mutex_t module_mtx; // For updating the module

	pthread_t module_listener_thread;
	int 	module_listener_status; // 0 = launched, 1 = on, 2 = need to stop
	pthread_mutex_t     module_listener_mtx;    // For communication between main and listener threads

	float sample_rate;

	void  	(*module_init)(void);
	void  	(*module_fini)(void);
	void 	(*module_set_sample_rate)(float sample_rate);
	void 	(*module_reset)(void);
	void 	(*module_process)(const float** x, float** y, int n_samples);
	void 	(*module_set_parameter)(int index, float value);
	float 	(*module_get_parameter)(int index);
	void 	(*module_note_on)(char note, char velocity);
	void 	(*module_note_off)(char note);
	void 	(*module_pitch_bend)(int value);
	void 	(*module_mod_wheel)(char value);
	int 	module_parameters_n;
	int 	module_buses_in_n;
	int  	module_buses_out_n;
	int  	module_channels_in_n;
	int  	module_channels_out_n;
	void 	(*module_get_parameter_info)(int index, char** name, char** shortName, char** units, char* out, char* bypass, int* steps, float* defaultValueUnmapped);
};
typedef struct _dynplug dynplug;

void dynplug_on_create(dynplug *instance);
void dynplug_on_destroy(dynplug *instance);
void dynplug_init(dynplug *instance);
void dynplug_fini(dynplug *instance);
void dynplug_set_sample_rate(dynplug *instance, float sample_rate);
void dynplug_reset(dynplug *instance);
void dynplug_process(dynplug *instance, const float** x, float** y, int n_samples);
void dynplug_set_parameter(dynplug *instance, int index, float value);
float dynplug_get_parameter(dynplug *instance, int index);
void dynplug_note_on(dynplug *instance, char note, char velocity);
void dynplug_note_off(dynplug *instance, char note);
void dynplug_pitch_bend(dynplug *instance, int value);
void dynplug_mod_wheel(dynplug *instance, char value);

/*
	Informs the DAW about the new parameters info and set default values
	Implementation specific for vst3/LADSPA/ecc..
*/
void dynplug_set_parameters_info(dynplug *instance);

void* dynplug_module_listener(void* data);

#ifdef __cplusplus
}
#endif

#endif
