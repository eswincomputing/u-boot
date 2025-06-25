// SPDX-License-Identifier: GPL-2.0
/*
 *
 * Copyright 2024, Beijing ESWIN Computing Technology Co., Ltd.. All rights reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Authors: Xiang Xu <xuxiang@eswincomputing.com>
 */

#ifndef ES_LIST_H
#define ES_LIST_H

#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <linux/compiler.h>

typedef struct __attribute__((packed)) es_list_node{
	uint8_t type;
	uint32_t id;
	void *data;
	struct es_list_node *next;
}es_list_node_t;

typedef struct {
	es_list_node_t *head;
} es_list_t;

#define  es_list_foreach(list_ptr, node_ptr) \
	for (es_list_node_t *node_ptr = (list_ptr)->head; \
		 node_ptr != NULL; \
		 node_ptr = node_ptr->next)

void es_list_init(es_list_t *list);
void *es_list_find(es_list_t *list, uint32_t type, uint32_t id);
int es_list_add(es_list_t *list, uint32_t type, uint32_t id, void *data);
void es_list_update(es_list_t *list, uint32_t type, uint32_t id, void *new_data);
void es_list_traverse(es_list_t *list, void(*func)(es_list_node_t *node));
int es_list_remove(es_list_t *list, uint32_t type, uint32_t id);
void es_list_clear(es_list_t *list);

#endif

