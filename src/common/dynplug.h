#ifndef _DYNPLUG_H
#define _DYNPLUG_H

#ifdef __cplusplus
extern "C" {
#endif

// We only support yaaaeapa for now. Maybe LADSPA in future?

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
	void*	module_handle; // dlopen result

	void  	(*module_init)();
	void  	(*module_fini)();
	void 	(*module_set_sample_rate)(float sample_rate);
	void 	(*module_reset)();
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
	void* 	module_data;
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
size_t dynplug_mem_req(dynplug *instance);
void dynplug_mem_set(dynplug *instance, void* mem);

#ifdef __cplusplus
}
#endif

#endif
