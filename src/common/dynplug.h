#ifndef _DYNPLUG_H
#define _DYNPLUG_H

#ifdef __cplusplus
extern "C" {
#endif

// We only support yaaaeapa for now. Maybe LADSPA in future?

// Fix
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
	void*   data; // For example, this contains VST3's controller pointer
	void*	module_handle; // dl(m)open result

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


//void dynplug_set_data(dynplug *instance, void* data);
//void* dynplug_get_data(dynplug *instance);

/*
	Informs the DAW about the new parameters
	Implementation specific for vst3/LADSPA/ecc..
*/
void dynplug_set_parameters_info(dynplug *instance);

#ifdef __cplusplus
}
#endif

#endif
