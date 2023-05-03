#include "dynplug.h"


void dynplug_init(dynplug *instance) {
	
	// dlopen ecc
	// dlsym and fill instance

	//dynplug_fini(instance);
	//bw_osc_saw_init(&instance->vco_saw_coeffs);
	
}

void dynplug_fini(dynplug *instance) {
	if (!instance)
		return;
	// TODO: fini...
	free(instance);
}


void dynplug_set_sample_rate(dynplug *instance, float sample_rate) {
	
	//bw_phase_gen_set_sample_rate(&instance->vco1_phase_gen_coeffs, sample_rate);
}

void dynplug_reset(dynplug *instance) {
	
	//bw_phase_gen_reset_coeffs(&instance->vco1_phase_gen_coeffs);
}

void dynplug_process(dynplug *instance, const float** x, float** y, int n_samples) {
	
	//(void)x;
	
	//TODO call module process
}

void dynplug_set_parameter(dynplug *instance, int index, float value) {

	//TODO: call module set_parameter

}

float dynplug_get_parameter(dynplug *instance, int index) {
	// TODO: return module get_parameter
}

void dynplug_note_on(dynplug *instance, char note, char velocity) {
	//TODO: call module note_on checking if exists
	
}

void dynplug_note_off(dynplug *instance, char note) {
	//TODO: call module note_off checking if exists
	
}

void dynplug_pitch_bend(dynplug *instance, int value) {
	//TODO: call module bend checking if exists
	
}

void dynplug_mod_wheel(dynplug *instance, char value) {
	//TODO: call module wheel checking if exists
	
}

size_t dynplug_mem_req(dynplug *instance) {

}

void dynplug_mem_set(dynplug *instance, void* mem) {

}
