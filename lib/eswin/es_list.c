// SPDX-License-Identifier: GPL-2.0
/*
 * ESWIN List
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


#include "eswin/es_list.h"

void es_list_init(es_list_t *list)
{
	list->head = NULL;
}

void *es_list_find(es_list_t *list, uint32_t type, uint32_t id)
{
	es_list_node_t *curr = list->head;
	while(curr) {
		if( curr->type == type && curr->id == id) {
			return curr;
		}
		curr = curr->next;
	}
	return NULL;
}

int es_list_add(es_list_t *list, uint32_t type, uint32_t id, void *data)
{
	es_list_node_t *node = malloc(sizeof(es_list_node_t));
	if(!node) return -1;

	node->type = type;
	node->id = id;
	node->data = data;
	node->next = NULL;

	if(!list->head) {
		list->head = node;
	} else {
		es_list_node_t *curr = list->head;
		while( curr->next ) curr = curr->next;
		curr->next = node;
	}
	return 0;
}


void es_list_update(es_list_t *list, uint32_t type, uint32_t id, void *new_data)
{
	es_list_node_t *node = (es_list_node_t *)es_list_find(list, type, id);
	if(!node) {
		es_list_add(list, type, id, new_data);
		return;
	}
	free(node->data);
	node->data = new_data;
	return;
}

void es_list_traverse(es_list_t *list, void(*func)(es_list_node_t *node))
{
	es_list_node_t *curr = list->head;
	while(curr) {
		func(curr);
		curr = curr->next;
	}
}

int es_list_remove(es_list_t *list, uint32_t type, uint32_t id)
{
	es_list_node_t **pp = &list->head;
	while(*pp) {
		if((*pp)->type == type && (*pp)->id == id) {
			es_list_node_t *to_free = *pp;
			*pp = to_free->next;
			if(to_free->data)
				free(to_free->data);
			free(to_free);
			return 0;
		}
		pp = &(*pp)->next;
	}
	return -1;
}


void es_list_clear(es_list_t *list)
{
	es_list_node_t *curr = list->head;
	while(curr) {
		es_list_node_t *next = curr->next;
		if(curr->data)
			free(curr->data);
		free(curr);
		curr = next;
	}
	list->head = NULL;
}
