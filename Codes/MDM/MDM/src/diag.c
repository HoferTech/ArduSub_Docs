#include "adc.h"
#include "thruster.h"
#include "util.h"
#define ADC_PERF    ADCA

float board_pres_read(void);

U16 moving_average = 0;
typedef struct
{
	U16 data_start;
	U16 data_ID;
	U16 data_size;
	U8 thrust_enable;
	U8 thrust_dir;
	float board_temp;
	float board_pres;
	U16 thrust_1_set;
	U16 thrust_2_set;
	U16 thrust_3_set;
	U16 thrust_4_set;
	U16 thrust_5_set;
	U16 thrust_6_set;
	U16 spare1;
	U16 flags;
	float cal_pres;
	U16 checksum;	
}t_diag;

t_diag diag;

#define PRES_SENS ADC_CH0
#define TEMP_SENS ADC_CH0
struct adc_channel_config adcch_conf;
void adc_init(void)
{
	struct adc_config adc_conf_temp;
	struct adc_config adc_conf_pres;

	U16 temp_cal;
	adc_read_configuration(&ADC_PERF, &adc_conf_temp);
	//adcch_read_configuration(&ADC_PERF, PRES_SENS, &adcch_conf);
	adcch_read_configuration(&ADC_PERF, TEMP_SENS, &adcch_conf);

	adc_set_conversion_parameters(&adc_conf_temp, ADC_SIGN_ON, ADC_RES_12, ADC_REF_BANDGAP);
	adc_set_conversion_trigger(&adc_conf_temp, ADC_TRIG_MANUAL, 1, 0);
	adc_enable_internal_input(&adc_conf_temp, ADC_INT_TEMPSENSE);
	adc_set_clock_rate(&adc_conf_temp, 6250L);

	adcch_set_input(&adcch_conf, ADCCH_POS_PIN0, ADCCH_NEG_NONE, 1);
	//adcch_set_input(&adcch_conf, ADCCH_POS_PIN2, ADCCH_NEG_PIN0, 1);

	adc_write_configuration(&ADC_PERF, &adc_conf_temp);
//	adcch_write_configuration(&ADC_PERF, PRES_SENS, &adcch_conf);
	adcch_write_configuration(&ADC_PERF, TEMP_SENS, &adcch_conf);
	
	 adc_enable(&ADC_PERF);
	temp_cal = adc_get_calibration_data(ADC_CAL_ADCA);
	
	
}


float board_temp_read(void)
{
	U16 result;
	 adc_start_conversion(&ADC_PERF, PRES_SENS);
	 adc_wait_for_interrupt_flag(&ADC_PERF, PRES_SENS);

	 result = adc_get_result(&ADC_PERF, PRES_SENS);
	 //diag.board_temp = result;
	 diag.board_temp = result;
	 return result;
}

float ext_temp_read(void)
{
	U16 result;
	adcch_set_input(&adcch_conf, ADCCH_POS_PIN0, ADCCH_NEG_NONE, 1);
	adcch_write_configuration(&ADC_PERF, TEMP_SENS, &adcch_conf);
	adc_start_conversion(&ADC_PERF, TEMP_SENS);
	adc_wait_for_interrupt_flag(&ADC_PERF, TEMP_SENS);

	result = adc_get_result(&ADC_PERF, TEMP_SENS);
	diag.board_temp = (float)((result * 100.0) / 2048.0);
	//diag.board_temp = result;
	return 0;
}

float pres_cal_fact = 1.0;
void calibrate_pressure(float *cal_pres)
{
	float pres;
	pres = board_pres_read();
	pres_cal_fact = *cal_pres / pres;
	diag.cal_pres = *cal_pres;
}

float board_pres_read(void)
{
	U16 result;
	float voltage;
	adcch_set_input(&adcch_conf, ADCCH_POS_PIN1, ADCCH_NEG_NONE, 1);
	adcch_write_configuration(&ADC_PERF, TEMP_SENS, &adcch_conf);
	adc_start_conversion(&ADC_PERF, TEMP_SENS);
	adc_wait_for_interrupt_flag(&ADC_PERF, TEMP_SENS);

	result = adc_get_result(&ADC_PERF, TEMP_SENS);
	moving_average = moving_average + result - moving_average/16;
	voltage = ((moving_average/16.0) / 2047.0) * 1.65;
	//diag.board_temp = voltage; 
	//voltage = (float)((result * 1.65) / 2047.0);
	//diag.board_pres = ((voltage / .109) * 150.0) * pres_cal_fact; 
	diag.board_pres = result; 
	return diag.board_pres;
}

void diag_update(void)
{
	diag.thrust_1_set = thruster_per_get(0);
	diag.thrust_2_set = thruster_per_get(1);
	diag.thrust_3_set = thruster_per_get(2);
	diag.thrust_4_set = thruster_per_get(3);
	diag.thrust_5_set = thruster_per_get(4);
	diag.thrust_6_set = thruster_per_get(5);
	diag.spare1++;

}

t_block diag_struct_ptr_get(void)
{
	t_block block;
	block.data_ptr = &diag;
	block.data_size_bytes = sizeof(diag);
	return block;
}
