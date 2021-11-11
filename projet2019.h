#ifndef PROJET2019_H
#define PROJET2019_H

#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

#define NTRANCHES 1025

typedef union{
	intmax_t a;
	void* adr;
	long double c;
} align_data;

typedef struct{
	ptrdiff_t next;
	ptrdiff_t previous;
	size_t len;
	align_data data[];
} node;

typedef struct {
	ptrdiff_t decalage;
	size_t nb_blocs;
} tranche;

typedef struct{
	void* memory;
	node* first;
	node* last;
	tranche* libre;
	size_t size;
} head;

void* ld_create(size_t nboctets);
void* ld_first(void* liste);
void* ld_last(void* liste);
void* ld_next(void* liste, void* current);
void* ld_previous(void* liste, void* current);
void ld_destroy (void* liste);
size_t ld_get(void* liste, void* current, size_t len, void* val);
void* ld_insert_first (void* liste, size_t len, void* p_data);
void* ld_insert_last (void* liste, size_t len, void* p_data);
void* ld_insert_before (void* liste, void* n, size_t len, void* p_data);
void* ld_insert_after (void* liste, void* n, size_t len, void* p_data);
void* ld_delete_node (void* liste, void* n);
size_t ld_total_free_memory(void* liste);
size_t ld_total_useful_memory (void* liste);
void* ld_add_memory(void* liste, size_t nboctets);
void* ld_compactify(void* liste);

void print_list_int(void* liste, int rev);
void* ld_get_at(void* liste, int i);
int ld_number_of_free_blocs(void* liste);

#endif
