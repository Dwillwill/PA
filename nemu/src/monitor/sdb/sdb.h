#ifndef __SDB_H__
#define __SDB_H__

#include <common.h>

word_t expr(char *e, bool success);
void new_wp(char *exp);
void free_wp(int index);
int is_wp_change();
void print_all_wp();
void print_ring_buffer();
#endif
