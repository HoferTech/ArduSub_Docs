#ifndef THRUSTER_H
#define THRUSTER_H
void thruster_init(void);
void thruster_set(U8 index,U8 percentage,U8 direction);
void thruster_dir_set(U8 index,U8 forward);
void thruster_enable(U8 index);
void thruster_disable(U8 index);
U16 thruster_per_get(U8 index);
#endif