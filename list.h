//
// Created by dasch on 06.11.2017.
//

#ifndef LIST_LIST_H
#define LIST_LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef struct Tree * Ldata_t;
static const int ALL_RIGHT = 1;
static const int one = 1;

struct Node {
    Ldata_t data;
    struct Node *prev;
    struct Node *next;
};

struct List {
    struct Node *head;
    struct Node *tail;
    size_t len;
};

struct Node *Node_Construct (Ldata_t value);                         //creates new node and sets data = value
struct List *List_Construct ();
void Node_Destruct (struct Node **old_one);
void List_Destruct (struct List **old_one);
void List_AddFirst (struct List *list, Ldata_t value);
void List_AddLast (struct List *list, Ldata_t value);
void List_AddAfter (struct List *list, struct Node *node, Ldata_t value);
int List_Ok (struct List *list);
void List_Dump (struct List *list, int line);
void List_Remove (struct Node **node, struct List *list);           //removes node
struct Node *List_Find (Ldata_t value, struct List *list);           //finds the first element with data == value

#endif //LIST_LIST_H
