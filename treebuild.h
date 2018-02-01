//
// Created by dasch on 04.12.2017.
//

#ifndef CALCULATOR_TREEBUILD_H
#define CALCULATOR_TREEBUILD_H

#include <assert.h>
#include <math.h>
#include <ctype.h>
#include "tree.h"
#include "list.h"

#define SE s[p] == 's' && s[p+1] == 'e'
#define SI s[p] == 's' && s[p+1] == 'i' && s[p+2] == ':'
#define NO s[p] == 'n' && s[p+1] == 'o' && s[p+2] == ':'
#define FINE s[p] == 'f' && s[p+1] == 'i' && s[p+2] == 'n' && s[p+3] == 'e'
#define RIPRESA s[p] == 'r' && s[p+1] == 'i' && s[p+2] == 'p' && s[p+3] == 'r' && s[p+4] == 'e' && s[p+5] == 's' && s[p+6] == 'a'
#define PIZZA s[p] == 'p' && s[p+1] == 'i' && s[p+2] == 'z' && s[p+3] == 'z' && s[p+4] == 'a'
#define STROFA s[p] == 's' && s[p+1] == 't' && s[p+2] == 'r' && s[p+3] == 'o' && s[p+4] == 'f' && s[p+5] == 'a'

enum OPs {
    IF = 2,
    CLOSE = 4,
    PRINT = 5,
    FUNC = 6,
    WHILE = 7
};

int Is_Remembered (char *thing);
struct List *List_Build(char *thing);
struct Leaf *GetG0();
struct Leaf *Get_Op();
struct Leaf *Get_L();
struct Leaf *Get_B();
struct Leaf *Get_l();
struct Leaf *Get_Assign();
struct Leaf *Get_E();
struct Leaf *Get_T();
struct Leaf *Get_S();
struct Leaf *Get_P();
struct Leaf *Get_N();
struct Leaf *Get_Id();
struct Leaf *Get_F();

#endif //CALCULATOR_TREEBUILD_H
