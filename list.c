//
// Created by dasch on 06.11.2017.
//
#include "list.h"

struct Node *Node_Construct(Ldata_t value) {
    struct Node *new_one = calloc(1, sizeof(struct Node));
    assert(new_one != NULL);

    new_one->data = value;
    new_one->prev = NULL;
    new_one->next = NULL;

    return new_one;
}

struct List *List_Construct() {
    struct List *new_one = calloc(one, sizeof(struct List));
    assert(new_one != NULL);

    new_one->head = NULL;
    new_one->tail = NULL;
    new_one->len = 0;

    return new_one;
}

void Node_Destruct(struct Node **old_one) {
    if (*old_one != NULL) {
        free((*old_one)->next);
        free((*old_one)->prev);
        (*old_one)->next = NULL;
        (*old_one)->next = NULL;
    }
    free(*old_one);
    *old_one = NULL;
}

void List_Destruct(struct List **old_one) {
    if (*old_one != NULL) {
        Node_Destruct(&((*old_one)->tail));
        Node_Destruct(&((*old_one)->head));
    }
    free(*old_one);
    *old_one = NULL;
}

int List_Ok (struct List *list) {
    if (list == NULL || list->len < 0 ||
        (list->len != 0 && (list->head == NULL || list->tail == NULL))) {
        return 0;
    }

    if (list->len == 1 && (list->head != list->tail)) {
        return 0;
    }

    struct Node *list_elem = list->head;
    while (list_elem) {
        if (list_elem == list->head && list_elem->prev != NULL
            || list_elem == list->tail && list_elem->next != NULL
            || (list_elem != list->head && list_elem != list->tail
                && (list_elem->prev == NULL || list_elem->next == NULL
                    || list_elem->prev->next != list_elem->next->prev
                    || list_elem->prev->next != list_elem || list_elem->next->prev != list_elem))) {
            return 0;
        }

        list_elem = list_elem->next;
    }

    list_elem = list->head;
    struct Node *point1 = list_elem;
    struct Node *point2 = list_elem;
    while (point1 && point2) {
        if (point1 == point2 && point1 != list->head) {
            return 0;
        }

        point1 = point1->next;
        point2 = point2->next;
        if (point2 == NULL || point1 == NULL) {
            break;
        }
        else {
            point2 = point2->next;
        }
    }

    return 1;
}

void List_Dump (struct List *list, int line) {
    FILE *Dump_f = fopen("Dump.txt", "w");
    fprintf(Dump_f, "digraph List {\nhead [label = \"List's err on %d\\nList len = %d\"];\n", line, list->len);

    struct Node *elem = list->head;
    int i = 2;

    if (elem == NULL) {
        fprintf(Dump_f, "}");
        fclose(Dump_f);

        system ("dot -Tpng Dump.txt -o Dump.png");
        system("Dump.png");
        return ;
    }

    fprintf(Dump_f, "node1 [shape=record, label=\"{<name> node1\\n%p|"
            "<data> data:\\ %p|{<prev> left\\n%p|<next> right\\n%p}}\"];\n", elem, elem->data, elem->prev, elem->next);
    while (elem != NULL) {
        elem = elem->next;
        if (elem == NULL) {
            break;
        }
        fprintf(Dump_f, "node%d [shape=record, label=\"{<name> node%d\\n%p|"
                "<data> data:\\ %p|{<prev> left\\n%p|<next> right\\n%p}}\"];\n", i, i, elem, elem->data, elem->prev, elem->next);
        i++;
    }

    i = 1;
    fprintf(Dump_f, "edge [color = limegreen];\n");
    //fprintf(Dump_f, "node [style = filled color = red;\n");
    while (i <= list->len && list->len != 1) {
        if (i == list->len) {
            fprintf(Dump_f, "\"node%d\":prev -> \"node%d\":name;\n", i, i-1);
            break;
        }
        if (i > 1) {
            fprintf(Dump_f, "\"node%d\":prev -> \"node%d\":name;\n", i, i-1);
        }
        i++;
    }
    fprintf(Dump_f, "edge [color = deepskyblue];\n");
    i = 1;
    while (i < list->len && list->len != 1) {
        fprintf(Dump_f, "\"node%d\":next -> \"node%d\":name;\n", i, i+1);
        i++;
    }

    fprintf(Dump_f, "}");
    fclose(Dump_f);

    system ("dot -Tpng Dump.txt -o Dump.png");
    system("Dump.png");
}

void List_AddFirst(struct List *list, Ldata_t value) {
    if (List_Ok(list) != ALL_RIGHT) {
        List_Dump(list, __LINE__);
        return ;
    }

    struct Node *node = Node_Construct(value);

    if (list->len == 0) {
        list->head = node;
        list->tail = node;
    }
    else {
        node->next = list->head;
        list->head->prev = node;
        list->head = node;
    }

    list->len++;

    if (List_Ok(list) != ALL_RIGHT) {
        List_Dump(list, __LINE__);
        return ;
    }
}

void List_AddLast(struct List *list, Ldata_t value) {
    if (List_Ok(list) != ALL_RIGHT) {
        List_Dump(list, __LINE__);
        return ;
    }

    struct Node *node = Node_Construct(value);

    if (list->len == 0) {
        list->head = node;
        list->tail = node;
    }
    else {
        node->prev = list->tail;
        list->tail->next = node;
        list->tail = node;
    }

    list->len++;

    if (List_Ok(list) != ALL_RIGHT) {
        List_Dump(list, __LINE__);
        return ;
    }
}

void List_AddAfter (struct List *list, struct Node *node, Ldata_t value) {
    if (List_Ok(list) != ALL_RIGHT) {
        List_Dump(list, __LINE__);
        return ;
    }

    if (node == list->tail) {
        List_AddLast(list, value);
    }
    else {
        struct Node *new = Node_Construct(value);

        new->prev = node;
        new->next = node->next;

        node->next->prev = new;
        node->next = new;
    }

    list->len++;

    if (List_Ok(list) != ALL_RIGHT) {
        List_Dump(list, __LINE__);
        return ;
    }
}

void List_Remove (struct Node **node, struct List *list) {
    assert(*node != NULL);
    if (list->len == 0) {
        return ;
    }
    if (List_Ok(list) != ALL_RIGHT) {
        List_Dump(list, __LINE__);
        return ;
    }

    if (list->len == 1) {
        Node_Destruct(node);
        list->tail = NULL;
        list->head = NULL;
    }
    else if (*node == list->head) {
        list->head = list->head->next;
        list->head->prev = NULL;
    }
    else if (*node == list->tail) {
        list->tail = list->tail->prev;
        list->tail->next = NULL;
    }
    else {
        (*node)->next->prev = (*node)->prev;
        (*node)->prev->next = (*node)->next;
        Node_Destruct(node);
    }

    list->len--;

    if (List_Ok(list) != ALL_RIGHT) {
        List_Dump(list, __LINE__);
        return ;
    }
}

struct Node *List_Find (Ldata_t value, struct List *list) {
    if (List_Ok(list) != ALL_RIGHT) {
        List_Dump(list, __LINE__);
        return NULL;
    }

    struct Node *list_elem = list->head;

    while (list_elem != NULL) {
        if (value == list_elem->data) {
            return list_elem;
        }
        list_elem = list_elem->next;
    }

    if (List_Ok(list) != ALL_RIGHT) {
        List_Dump(list, __LINE__);
        return NULL;
    }

    return NULL;
}