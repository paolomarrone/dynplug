#ifndef _DYNPLUG_H
#define _DYNPLUG_H

#ifdef __cplusplus
extern "C" {
#endif


enum {
	p_1,
	p_2,
	p_3,
	p_4,
	p_5,
	p_n
};

#define BUFFER_SIZE 	32

struct _dynplug {

	void*	module_handle;
	void*   module_instance;
	void (*module_init)(void* instance);
	void (*module_set_sample_rate)(void* instance, float sample_rate);
	void (*module_reset)(void* instance);
	void (*module_process)(void *instance, const float** x, float** y, int n_samples);
	void (*module_set_parameter)(void *instance, int index, float value);
	float (*module_get_parameter)(void *instance, int index);
	void (*module_note_on)(void *instance, char note, char velocity);
	void (*module_note_off)(void *instance, char note);
	void (*module_pitch_bend)(void *instance, int value);
	void (*module_mod_wheel)(void *instance, char value);

	// pointer to so
	// TODO

	// Parameters
	float			params[p_n];

	// States
	
	// Buffers
	//float			buf[4][BUFFER_SIZE];
};
typedef struct _dynplug dynplug;

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

#ifdef __cplusplus
}
#endif

#endif
