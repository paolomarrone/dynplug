/*
 * Brickworks
 *
 * Copyright (C) 2022, 2023 Orastron Srl unipersonale
 *
 * Brickworks is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License.
 *
 * Brickworks is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Brickworks.  If not, see <http://www.gnu.org/licenses/>.
 *
 * File author: Stefano D'Angelo
 */

#include "bw_example_synth_mono.h"

#include <bw_buf.h>

void bw_example_synth_mono_init(bw_example_synth_mono *instance) {
	bw_osc_saw_init(&instance->vco_saw_coeffs);
	bw_phase_gen_init(&instance->vco1_phase_gen_coeffs);
	bw_osc_pulse_init(&instance->vco1_pulse_coeffs);
	bw_osc_tri_init(&instance->vco1_tri_coeffs);
	bw_gain_init(&instance->vco1_gain_coeffs);
	bw_phase_gen_init(&instance->vco2_phase_gen_coeffs);
	bw_osc_pulse_init(&instance->vco2_pulse_coeffs);
	bw_osc_tri_init(&instance->vco2_tri_coeffs);
	bw_gain_init(&instance->vco2_gain_coeffs);
	bw_phase_gen_init(&instance->vco3_phase_gen_coeffs);
	bw_osc_pulse_init(&instance->vco3_pulse_coeffs);
	bw_osc_tri_init(&instance->vco3_tri_coeffs);
	bw_gain_init(&instance->vco3_gain_coeffs);
	bw_noise_gen_init(&instance->noise_gen_coeffs, &instance->rand_state);
	bw_pink_filt_init(&instance->pink_filt_coeffs);
	bw_gain_init(&instance->noise_gain_coeffs);
	bw_env_gen_init(&instance->vcf_env_gen_coeffs);
	bw_svf_init(&instance->vcf_coeffs);
	bw_env_gen_init(&instance->vca_env_gen_coeffs);
	bw_phase_gen_init(&instance->a440_phase_gen_coeffs);
	bw_gain_init(&instance->gain_coeffs);
	bw_ppm_init(&instance->ppm_coeffs);
	
	bw_osc_saw_set_antialiasing(&instance->vco_saw_coeffs, 1);
	bw_osc_pulse_set_antialiasing(&instance->vco1_pulse_coeffs, 1);
	bw_osc_tri_set_antialiasing(&instance->vco1_tri_coeffs, 1);
	bw_osc_pulse_set_antialiasing(&instance->vco2_pulse_coeffs, 1);
	bw_osc_tri_set_antialiasing(&instance->vco2_tri_coeffs, 1);
	bw_gain_set_gain_lin(&instance->vco2_gain_coeffs, 0.f);
	bw_osc_pulse_set_antialiasing(&instance->vco3_pulse_coeffs, 1);
	bw_osc_tri_set_antialiasing(&instance->vco3_tri_coeffs, 1);
	bw_gain_set_gain_lin(&instance->vco3_gain_coeffs, 0.f);
	bw_gain_set_gain_lin(&instance->noise_gain_coeffs, 0.f);
	bw_phase_gen_set_frequency(&instance->a440_phase_gen_coeffs, 440.f);
	
	instance->rand_state = 0xbaddecaf600dfeed;
}

void bw_example_synth_mono_set_sample_rate(bw_example_synth_mono *instance, float sample_rate) {
	bw_phase_gen_set_sample_rate(&instance->vco1_phase_gen_coeffs, sample_rate);
	bw_osc_pulse_set_sample_rate(&instance->vco1_pulse_coeffs, sample_rate);
	bw_osc_tri_set_sample_rate(&instance->vco1_tri_coeffs, sample_rate);
	bw_gain_set_sample_rate(&instance->vco1_gain_coeffs, sample_rate);
	bw_phase_gen_set_sample_rate(&instance->vco2_phase_gen_coeffs, sample_rate);
	bw_osc_pulse_set_sample_rate(&instance->vco2_pulse_coeffs, sample_rate);
	bw_osc_tri_set_sample_rate(&instance->vco2_tri_coeffs, sample_rate);
	bw_gain_set_sample_rate(&instance->vco2_gain_coeffs, sample_rate);
	bw_phase_gen_set_sample_rate(&instance->vco3_phase_gen_coeffs, sample_rate);
	bw_osc_pulse_set_sample_rate(&instance->vco3_pulse_coeffs, sample_rate);
	bw_osc_tri_set_sample_rate(&instance->vco3_tri_coeffs, sample_rate);
	bw_gain_set_sample_rate(&instance->vco3_gain_coeffs, sample_rate);
	bw_noise_gen_set_sample_rate(&instance->noise_gen_coeffs, sample_rate);
	bw_pink_filt_set_sample_rate(&instance->pink_filt_coeffs, sample_rate);
	bw_gain_set_sample_rate(&instance->noise_gain_coeffs, sample_rate);
	bw_env_gen_set_sample_rate(&instance->vcf_env_gen_coeffs, sample_rate);
	bw_svf_set_sample_rate(&instance->vcf_coeffs, sample_rate);
	bw_env_gen_set_sample_rate(&instance->vca_env_gen_coeffs, sample_rate);
	bw_phase_gen_set_sample_rate(&instance->a440_phase_gen_coeffs, sample_rate);
	bw_gain_set_sample_rate(&instance->gain_coeffs, sample_rate);
	bw_ppm_set_sample_rate(&instance->ppm_coeffs, sample_rate);
}

void bw_example_synth_mono_reset(bw_example_synth_mono *instance) {
	const float v = instance->params[p_vcf_cutoff];
	const float cutoff = 20.f + (20e3f - 20.f) * v * v * v;
	bw_svf_set_cutoff(&instance->vcf_coeffs, bw_clipf(cutoff, 20.f, 20e3f));
	
	bw_phase_gen_reset_coeffs(&instance->vco1_phase_gen_coeffs);
	bw_phase_gen_reset_state(&instance->vco1_phase_gen_coeffs, &instance->vco1_phase_gen_state, 0.f);
	bw_osc_pulse_reset_coeffs(&instance->vco1_pulse_coeffs);
	bw_osc_tri_reset_coeffs(&instance->vco1_tri_coeffs);
	bw_gain_reset_coeffs(&instance->vco1_gain_coeffs);
	bw_phase_gen_reset_coeffs(&instance->vco2_phase_gen_coeffs);
	bw_phase_gen_reset_state(&instance->vco2_phase_gen_coeffs, &instance->vco2_phase_gen_state, 0.f);
	bw_osc_pulse_reset_coeffs(&instance->vco2_pulse_coeffs);
	bw_osc_tri_reset_coeffs(&instance->vco2_tri_coeffs);
	bw_gain_reset_coeffs(&instance->vco2_gain_coeffs);
	bw_phase_gen_reset_coeffs(&instance->vco3_phase_gen_coeffs);
	bw_phase_gen_reset_state(&instance->vco3_phase_gen_coeffs, &instance->vco3_phase_gen_state, 0.f);
	bw_osc_pulse_reset_coeffs(&instance->vco3_pulse_coeffs);
	bw_osc_tri_reset_coeffs(&instance->vco3_tri_coeffs);
	bw_gain_reset_coeffs(&instance->vco3_gain_coeffs);
	bw_osc_filt_reset_state(&instance->osc_filt_state);
	bw_pink_filt_reset_state(&instance->pink_filt_coeffs, &instance->pink_filt_state);
	bw_gain_reset_coeffs(&instance->noise_gain_coeffs);
	bw_env_gen_reset_coeffs(&instance->vcf_env_gen_coeffs);
	bw_env_gen_reset_state(&instance->vcf_env_gen_coeffs, &instance->vcf_env_gen_state);
	bw_svf_reset_coeffs(&instance->vcf_coeffs);
	bw_svf_reset_state(&instance->vcf_coeffs, &instance->vcf_state, 0.f);
	bw_env_gen_reset_coeffs(&instance->vca_env_gen_coeffs);
	bw_env_gen_reset_state(&instance->vca_env_gen_coeffs, &instance->vca_env_gen_state);
	bw_phase_gen_reset_coeffs(&instance->a440_phase_gen_coeffs);
	bw_phase_gen_reset_state(&instance->a440_phase_gen_coeffs, &instance->a440_phase_gen_state, 0.f);
	bw_gain_reset_coeffs(&instance->gain_coeffs);
	bw_ppm_reset_coeffs(&instance->ppm_coeffs);
	bw_ppm_reset_state(&instance->ppm_coeffs, &instance->ppm_state);
	instance->note = 60;
	instance->gate = 0;
	instance->pitch_bend = 0.f;
	instance->mod_wheel = 0.f;
	for (int i = 0; i < 128; i++)
		instance->notes_pressed[i] = 0;
}

void bw_example_synth_mono_process(bw_example_synth_mono *instance, const float** x, float** y, int n_samples) {
	// FIXME: control-rate modulations are asynchronous here...
	// it's all good as long as hosts gives us buffers whose length is a multiple of 32,
	// otherwise it's probably still ok but a bit "swingy"
	
	(void)x;
	
	bw_env_gen_set_gate(&instance->vcf_env_gen_coeffs, instance->gate);
	bw_env_gen_set_gate(&instance->vca_env_gen_coeffs, instance->gate);
	
	int n = instance->params[p_vco3_kbd] >= 0.5f ? instance->note : 0;
	bw_phase_gen_set_frequency(&instance->vco1_phase_gen_coeffs, 440.f *
		bw_pow2f_3(6.f * instance->params[p_vco1_coarse] - 3.f
			+ 2.f * instance->pitch_bend - 1.f
			+ 8.333333333333333e-2f * ((instance->note - 69) + 2.f * (instance->params[p_master_tune] + instance->params[p_vco1_fine]) - 2.f)));
	bw_phase_gen_set_frequency(&instance->vco2_phase_gen_coeffs, 440.f *
		bw_pow2f_3(6.f * instance->params[p_vco2_coarse] - 3.f
			+ 2.f * instance->pitch_bend - 1.f
			+ 8.333333333333333e-2f * ((instance->note - 69) + 2.f * (instance->params[p_master_tune] + instance->params[p_vco2_fine]) - 2.f)));
	bw_phase_gen_set_frequency(&instance->vco3_phase_gen_coeffs, 440.f *
		bw_pow2f_3(6.f * instance->params[p_vco3_coarse] - 3.f
			+ 2.f * instance->pitch_bend - 1.f
			+ 8.333333333333333e-2f * ((n - 69) + 2.f * (instance->params[p_master_tune] + instance->params[p_vco3_fine]) - 2.f)));
	
	for (int i = 0; i < n_samples; i += BUFFER_SIZE) {
		float *out = y[0] + i;
		int n = bw_minf(n_samples - i, BUFFER_SIZE);
		
		bw_phase_gen_process(&instance->vco3_phase_gen_coeffs, &instance->vco3_phase_gen_state, NULL, out, instance->buf[0], n);
		if (instance->params[p_vco3_waveform] >= (1.f / 4.f + 1.f / 2.f)) {
			bw_osc_tri_process(&instance->vco3_tri_coeffs, out, instance->buf[0], out, n);
			bw_osc_pulse_reset_coeffs(&instance->vco3_pulse_coeffs);
		} else if (instance->params[p_vco3_waveform] >= (1.f / 4.f)) {
			bw_osc_pulse_process(&instance->vco3_pulse_coeffs, out, instance->buf[0], out, n);
			bw_osc_tri_reset_coeffs(&instance->vco3_tri_coeffs);
		} else {
			bw_osc_saw_process(&instance->vco_saw_coeffs, out, instance->buf[0], out, n);
			bw_osc_pulse_reset_coeffs(&instance->vco3_pulse_coeffs);
			bw_osc_tri_reset_coeffs(&instance->vco3_tri_coeffs);
		}
		
		bw_noise_gen_process(&instance->noise_gen_coeffs, instance->buf[0], n);
		if (instance->params[p_noise_color] >= 0.5f)
			bw_pink_filt_process(&instance->pink_filt_coeffs, &instance->pink_filt_state, instance->buf[0], instance->buf[0], n);
		else
			bw_pink_filt_reset_state(&instance->pink_filt_coeffs, &instance->pink_filt_state); // FIXME: calling this here is sloppy coding
		bw_buf_scale(instance->buf[0], instance->buf[0], 5.f, n);
		
		for (int j = 0; j < n; j++)
			instance->buf[1][j] = instance->mod_wheel * (out[j] + instance->params[p_mod_mix] * (instance->buf[0][j] - out[j]));
		const float vcf_mod = 0.3f * instance->params[p_vcf_mod] * instance->buf[1][0];
		
		bw_buf_scale(instance->buf[2], instance->buf[1], instance->params[p_vco1_mod], n);
		bw_phase_gen_process(&instance->vco1_phase_gen_coeffs, &instance->vco1_phase_gen_state, instance->buf[2], instance->buf[2], instance->buf[3], n);
		if (instance->params[p_vco1_waveform] >= (1.f / 4.f + 1.f / 2.f)) {
			bw_osc_tri_process(&instance->vco1_tri_coeffs, instance->buf[2], instance->buf[3], instance->buf[2], n);
			bw_osc_pulse_reset_coeffs(&instance->vco1_pulse_coeffs);
		} else if (instance->params[p_vco1_waveform] >= (1.f / 4.f)) {
			bw_osc_pulse_process(&instance->vco1_pulse_coeffs, instance->buf[2], instance->buf[3], instance->buf[2], n);
			bw_osc_tri_reset_coeffs(&instance->vco1_tri_coeffs);
		} else {
			bw_osc_saw_process(&instance->vco_saw_coeffs, instance->buf[2], instance->buf[3], instance->buf[2], n);
			bw_osc_pulse_reset_coeffs(&instance->vco1_pulse_coeffs);
			bw_osc_tri_reset_coeffs(&instance->vco1_tri_coeffs);
		}
		
		bw_buf_scale(instance->buf[1], instance->buf[1], instance->params[p_vco2_mod], n);
		bw_phase_gen_process(&instance->vco2_phase_gen_coeffs, &instance->vco2_phase_gen_state, instance->buf[1], instance->buf[1], instance->buf[3], n);
		if (instance->params[p_vco2_waveform] >= (1.f / 4.f + 1.f / 2.f)) {
			bw_osc_tri_process(&instance->vco2_tri_coeffs, instance->buf[1], instance->buf[3], instance->buf[1], n);
			bw_osc_pulse_reset_coeffs(&instance->vco2_pulse_coeffs);
		} else if (instance->params[p_vco2_waveform] >= (1.f / 4.f)) {
			bw_osc_pulse_process(&instance->vco2_pulse_coeffs, instance->buf[1], instance->buf[3], instance->buf[1], n);
			bw_osc_tri_reset_coeffs(&instance->vco2_tri_coeffs);
		} else {
			bw_osc_saw_process(&instance->vco_saw_coeffs, instance->buf[1], instance->buf[3], instance->buf[1], n);
			bw_osc_pulse_reset_coeffs(&instance->vco2_pulse_coeffs);
			bw_osc_tri_reset_coeffs(&instance->vco2_tri_coeffs);
		}
		
		bw_gain_process(&instance->vco1_gain_coeffs, instance->buf[2], instance->buf[2], n);
		bw_gain_process(&instance->vco2_gain_coeffs, instance->buf[1], instance->buf[1], n);
		bw_gain_process(&instance->vco3_gain_coeffs, out, out, n);
		bw_gain_process(&instance->noise_gain_coeffs, instance->buf[0], instance->buf[0], n);
		bw_buf_mix(out, out, instance->buf[1], n);
		bw_buf_mix(out, out, instance->buf[2], n);
		
		bw_osc_filt_process(&instance->osc_filt_state, out, out, n);
		
		const float k = instance->params[p_noise_color] >= 0.5f
			? 6.f * bw_noise_gen_get_scaling_k(&instance->noise_gen_coeffs) * bw_pink_filt_get_scaling_k(&instance->pink_filt_coeffs)
			: 0.1f * bw_noise_gen_get_scaling_k(&instance->noise_gen_coeffs);
		bw_buf_scale(instance->buf[0], instance->buf[0], k, n);
		bw_buf_mix(out, out, instance->buf[0], n);
		
		bw_env_gen_process(&instance->vcf_env_gen_coeffs, &instance->vcf_env_gen_state, NULL, n);
		float v = instance->params[p_vcf_cutoff] + instance->params[p_vcf_contour] * bw_env_gen_get_y_z1(&instance->vcf_env_gen_state) + vcf_mod;
		float cutoff = 20.f + (20e3f - 20.f) * v * v * v;
		if (instance->params[p_vcf_kbd_ctrl] >= (1.f / 6.f + 2.f / 3.f))
			cutoff *= bw_pow2f_3(8.333333333333333e-2f * (instance->note - 60));
		else if (instance->params[p_vcf_kbd_ctrl] >= (1.f / 6.f + 1.f / 3.f))
			cutoff *= bw_pow2f_3((0.793700525984100f * 8.333333333333333e-2f) * (instance->note - 60));
		else if (instance->params[p_vcf_kbd_ctrl] >= (1.f / 6.f + 2.f / 3.f))
			cutoff *= bw_pow2f_3((0.629960524947437f * 8.333333333333333e-2f) * (instance->note - 60));
		// otherwise no kbd control
		bw_svf_set_cutoff(&instance->vcf_coeffs, bw_clipf(cutoff, 20.f, 20e3f));
		bw_svf_process(&instance->vcf_coeffs, &instance->vcf_state, out, out, NULL, NULL, n);
		
		bw_env_gen_process(&instance->vca_env_gen_coeffs, &instance->vca_env_gen_state, instance->buf[0], n);
		bw_buf_mul(out, out, instance->buf[0], n);
		
		bw_phase_gen_process(&instance->a440_phase_gen_coeffs, &instance->a440_phase_gen_state, NULL, instance->buf[0], NULL, n);
		bw_osc_sin_process(instance->buf[0], instance->buf[0], n);
		if (instance->params[p_a440] >= 0.5f)
			bw_buf_mix(out, out, instance->buf[0], n);
		
		bw_gain_process(&instance->gain_coeffs, out, out, n);
		bw_ppm_process(&instance->ppm_coeffs, &instance->ppm_state, out, NULL, n);
	}
}

void bw_example_synth_mono_set_parameter(bw_example_synth_mono *instance, int index, float value) {
	if (instance->params[index] == value)
		return;
	instance->params[index] = value;
	switch (index) {
	case p_volume:
		bw_gain_set_gain_lin(&instance->gain_coeffs, value * value * value);
		break;
	case p_portamento:
		bw_phase_gen_set_portamento_tau(&instance->vco1_phase_gen_coeffs, value);
		bw_phase_gen_set_portamento_tau(&instance->vco2_phase_gen_coeffs, value);
		bw_phase_gen_set_portamento_tau(&instance->vco3_phase_gen_coeffs, value);
		break;
	case p_vco1_pw_slope:
		bw_osc_pulse_set_pulse_width(&instance->vco1_pulse_coeffs, value);
		bw_osc_tri_set_slope(&instance->vco1_tri_coeffs, bw_clipf(value, 0.001f, 0.999f));
		break;
	case p_vco1_level:
		bw_gain_set_gain_lin(&instance->vco1_gain_coeffs, value * value * value);
		break;
	case p_vco2_pw_slope:
		bw_osc_pulse_set_pulse_width(&instance->vco2_pulse_coeffs, value);
		bw_osc_tri_set_slope(&instance->vco2_tri_coeffs, bw_clipf(value, 0.001f, 0.999f));
		break;
	case p_vco2_level:
		bw_gain_set_gain_lin(&instance->vco2_gain_coeffs, value * value * value);
		break;
	case p_vco3_pw_slope:
		bw_osc_pulse_set_pulse_width(&instance->vco3_pulse_coeffs, value);
		bw_osc_tri_set_slope(&instance->vco3_tri_coeffs, bw_clipf(value, 0.001f, 0.999f));
		break;
	case p_vco3_level:
		bw_gain_set_gain_lin(&instance->vco3_gain_coeffs, value * value * value);
		break;
	case p_noise_level:
		bw_gain_set_gain_lin(&instance->noise_gain_coeffs, value * value * value);
		break;
	case p_vcf_Q:
		bw_svf_set_Q(&instance->vcf_coeffs, 0.5f + 9.5f * value);
		break;
	case p_vcf_attack:
		bw_env_gen_set_attack(&instance->vcf_env_gen_coeffs, value);
		break;
	case p_vcf_decay:
		bw_env_gen_set_decay(&instance->vcf_env_gen_coeffs, value);
		break;
	case p_vcf_sustain:
		bw_env_gen_set_sustain(&instance->vcf_env_gen_coeffs, value);
		break;
	case p_vcf_release:
		bw_env_gen_set_release(&instance->vcf_env_gen_coeffs, value);
		break;
	case p_vca_attack:
		bw_env_gen_set_attack(&instance->vca_env_gen_coeffs, value);
		break;
	case p_vca_decay:
		bw_env_gen_set_decay(&instance->vca_env_gen_coeffs, value);
		break;
	case p_vca_sustain:
		bw_env_gen_set_sustain(&instance->vca_env_gen_coeffs, value);
		break;
	case p_vca_release:
		bw_env_gen_set_release(&instance->vca_env_gen_coeffs, value);
		break;
	}
}

float bw_example_synth_mono_get_parameter(bw_example_synth_mono *instance, int index) {
	if (index < p_n)
		return instance->params[index];
	const float v = bw_ppm_get_y_z1(&instance->ppm_state);
	return v < -200.f ? 0.f : bw_clipf(0.01666666666666666f * v + 1.f, 0.f, 1.f);
}

static void update_note_gate(bw_example_synth_mono *instance) {
	for (int i = 0; i < 128; i++)
		if (instance->notes_pressed[i]) {
			instance->note = i;
			instance->gate = 1;
			return;
		}
	instance->gate = 0;
}

void bw_example_synth_mono_note_on(bw_example_synth_mono *instance, char note, char velocity) {
	if (velocity == 0)
		bw_example_synth_mono_note_off(instance, note);
	else {
		instance->notes_pressed[(int)note] = 1;
		update_note_gate(instance);
	}
}

void bw_example_synth_mono_note_off(bw_example_synth_mono *instance, char note) {
	if (instance->notes_pressed[(int)note]) {
		instance->notes_pressed[(int)note] = 0;
		update_note_gate(instance);
	}
}

void bw_example_synth_mono_pitch_bend(bw_example_synth_mono *instance, int value) {
	instance->pitch_bend = (value - 0x2000) / (float)0x4000;
}

void bw_example_synth_mono_mod_wheel(bw_example_synth_mono *instance, char value) {
	instance->mod_wheel = (float)value / 0x80;
}
