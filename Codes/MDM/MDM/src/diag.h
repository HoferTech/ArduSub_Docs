
 #ifndef DIAG_H
 #define DIAG_H
 #include "util.h"
 
 void adc_init(void);
 void diag_update(void);
 float board_temp_read(void);
 float board_pres_read(void);
 float ext_temp_read(void);
 t_block diag_struct_ptr_get(void);
 void calibrate_pressure(float *cal_pres);
 #endif