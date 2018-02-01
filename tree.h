//
// Created by dasch on 19.11.2017.
//

#ifndef DIFF_TREE_H
#define DIFF_TREE_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <mem.h>
#include <ctype.h>
#include <sys/types.h>
#include <time.h>

enum FUNC {
    SIN = 1,
    COS,
    TG,
    CTG,
    SH,
    CH,
    TH,
    CTH,
    ARCSIN,
    ARCCOS,
    ARCTG,
    ARCCTG,
    LN
};

#define func_num_set     if (!strcmp(new_one->data, "sin")) {new_one->num_data = SIN;}\
                    else if (!strcmp(new_one->data, "cos")) {new_one->num_data = COS;}\
                    else if (!strcmp(new_one->data, "tg")) {new_one->num_data = TG;}\
                    else if (!strcmp(new_one->data, "ctg")) {new_one->num_data = CTG;}\
                    else if (!strcmp(new_one->data, "sh")) {new_one->num_data = SH;}\
                    else if (!strcmp(new_one->data, "ch")) {new_one->num_data = CH;}\
                    else if (!strcmp(new_one->data, "th")) {new_one->num_data = TH;}\
                    else if (!strcmp(new_one->data, "cth")) {new_one->num_data = CTH;}\
                    else if (!strcmp(new_one->data, "arcsin")) {new_one->num_data = ARCSIN;}\
                    else if (!strcmp(new_one->data, "arccos")) {new_one->num_data = ARCCOS;}\
                    else if (!strcmp(new_one->data, "arctg")) {new_one->num_data = ARCTG;}\
                    else if (!strcmp(new_one->data, "arcctg")) {new_one->num_data = ARCCTG;}\
                    else if (!strcmp(new_one->data, "ln")) {new_one->num_data = LN;}

enum TYPES {
    OPERATOR = 1,
    NUMBER = 2,
    VARIABLE = 3,
    STRING = 4
};

enum ERRs {
    NOT_ENOUGH_MEMORY = 1,
    ERR,
    BAD_PTR,
    WRONG_VALUE,
    FILE_ERR,
    DEL0
};

typedef char * data_t;

static const size_t DATA_LEN = 50;
static const size_t MAX_LEN = 10;
static const size_t MAX_NUM_SYM = 1024*1024;
static const size_t POISON = 700077;
static const size_t INVALID_NODE = 0;
static int lefts = 2;
static int rights = 1000;
static size_t changes = 0;
static double e = 2.7182;

struct Leaf {
    int type;                       //on of the types: operator, number or variable
    data_t data;                    //node's data in string format
    double num_data;                //node's data in double format in case of type == number
    struct Leaf *left;
    struct Leaf *right;

    size_t parent_num;
    struct Leaf *parent;            //pointer on parent
    size_t level;
    size_t num;
};

struct Tree {
    size_t len;
    struct Leaf *root;
};

struct Leaf *Leaf_Construct(data_t value);
void Leaf_Destruct(struct Leaf **old_one);
struct Tree *Tree_Construct(data_t value);
void Tree_Destruct(struct Tree **old_one);
int Tree_Ok(struct Tree *tree);
int SubTree_Ok(struct Leaf *root);
void Tree_Dump(struct Tree *tree, size_t line);
void Leaf_Dump(struct Leaf *leaf);
void Tree_AddLeft(struct Tree *tree, struct Leaf *leaf, data_t value);
void Tree_AddRight(struct Tree *tree, struct Leaf *leaf, data_t value);
void Tree_Print(struct Tree *tree);
void Leaf_Print(struct Leaf *leaf, FILE *tree_file);
void Human_Dump(struct Tree *ak);
void Human_Leaf_Dump(struct Leaf *leaf);
void Log_write (int err_code, int line);

struct Leaf *Leaf_Construct_N(double value);
void Tree_SParameters_Set(struct Tree *tree);
void Leaf_SParameters_Set(struct Tree *tree, struct Leaf *root, size_t level, struct Leaf *parent);

int Is_Number(const char *str);
int Is_Operator(const char *str);
int Is_Func(const char *str);
int Is_Variable(const char *str);

#endif //DIFF_TREE_H