//
// Created by dasch on 04.12.2017.
//
#include "treebuild.h"

char *s = NULL;
size_t p = 0;
char **vars = NULL;                 //array with names of variables
int ifs_loops  = 0;                 //count of ifs and loops

int Is_Remembered (char *thing) {   //return the number of Variable or Function in vars array (and in the RAM of cpu)
    int i = 0;

    while (strcmp(vars[i], thing) != 0) {
        i++;
        if (vars[i] == NULL) {
            vars[i] = thing;
        }
    }

    return i;
}

struct List *List_Build(char *thing) {
    struct List *Programm = List_Construct();

    s = thing;
    p = 0;
    struct Tree *cmd = NULL;

    while (s[p] != '\0') {
        cmd = Tree_Construct(" ");
        cmd->root = GetG0();
        Tree_SParameters_Set(cmd);

        Human_Dump(cmd);
        //Tree_Dump(new_one, __LINE__);
        List_AddLast(Programm, cmd);
    }

    return Programm;
}

struct Leaf *GetG0() {                              //G0 := Assig || operator(if, pizza, while, func)
    struct Leaf *root = NULL;

    if ('a' <= s[p] && s[p] <= 'z') {
        root = Get_Op();
    }
    else {
        root = Get_Assign();
    }

    assert(s[p] == '!' && s[p+1] == '\n');
    p += 2;

    return root;
}

struct Leaf *Get_Op() {
    struct Leaf *root = NULL;

    if (SE) {         // "se" == "if"
        p += IF;                                                  //syntax: se ('condition')\n si: 'G0' no: 'G0' fine
        root = Leaf_Construct("se");
        root->num_data = ifs_loops;
        ifs_loops++;

        root->left = Get_L();
        root->right = Leaf_Construct("si/no");

        if (SI) {    //after si: is always no:
            p += IF + 1;
            while (s[p] != 'n') {
                struct Leaf *new_one = Leaf_Construct(",");
                new_one->left = root->right->left;
                new_one->right = GetG0();
                root->right->left = new_one;
            }
        }
        if (NO) {    //after no: is always fine\n
            p += IF + 1;
            while (s[p] != 'f') {
                struct Leaf *new_one = Leaf_Construct(",");
                new_one->left = root->right->right;
                new_one->right = GetG0();
                root->right->right = new_one;
            }
        }
        assert(FINE);
        p += CLOSE;
    }
    else if (RIPRESA) {
        p += WHILE;
        root = Leaf_Construct("ripresa");
        root->num_data = ifs_loops;
        ifs_loops++;

        root->left = Get_L();

        while (s[p] != 'f') {
            struct Leaf *new_one = Leaf_Construct(",");
            new_one->left = root->right;
            new_one->right = GetG0();
            root->right = new_one;
        }
        assert(FINE);
        p += CLOSE;
    }
    else if (PIZZA) {
        p += PRINT;

        root = Leaf_Construct("pizza");
    }
    else if (STROFA) {
        p += FUNC;
        root = Get_Id();
        root->type = FUNC;

        assert(s[p] == '(');
        p++;

        int commas = 0;
        while (s[p] != ')') {
            struct Leaf *new_one = Leaf_Construct(",");
            new_one->left = root->left;
            new_one->right = Get_Id();
            root->left = new_one;

            if (s[p] == ',') {
                p++;
                commas++;
            }
        }
        p++;

        if (s[p] == ':') {
            root->left->num_data = commas + 1;

            int num = Is_Remembered(root->data);
            root->num_data = num;

            int i = commas + 1;
            struct Leaf *varianta = root->left->right;
            varianta->parent = root->left;
            for ( ; i > 0; i--) {
                vars[num + i] = varianta->data;
                varianta->num_data = num + i;
                if (varianta->parent->left != NULL) {
                    struct Leaf *p = varianta->parent->left;
                    varianta = varianta->parent->left->right;
                    varianta->parent = p;
                }
            }

            p += 2;
            while (s[p] != 'f') {
                struct Leaf *new_one = Leaf_Construct(",");
                new_one->left = root->right;
                new_one->right = GetG0();

                root->right = new_one;
            }
            assert(FINE);
            p += CLOSE;
        }
    }

    return root;
}

struct Leaf *Get_L() {
    assert(s[p] == '(');
    p++;

    struct Leaf *left = Get_B();
    struct Leaf *root = left;
    char *c = calloc(1, sizeof(char));

    while (s[p] == 'o') {
        strncpy(c, s+p, 1);
        struct Leaf *new_one = Leaf_Construct(c);

        p++;
        new_one->left = root;
        new_one->right = Get_B();
        root = new_one;
    }

    assert(s[p] == ')' && s[p+1] == '\n');
    p += IF;

    return root;
}

struct Leaf *Get_B() {
    struct Leaf *left = Get_l();
    struct Leaf *root = left;
    char *c = calloc(1, sizeof(char));

    while (s[p] == 'e') {
        strncpy(c, s+p, 1);
        struct Leaf *new_one = Leaf_Construct(c);

        p++;
        new_one->left = root;
        new_one->right = Get_l();
        root = new_one;
    }

    return root;
}

struct Leaf *Get_l() {
    struct Leaf *left = Get_E();
    struct Leaf *root = left;
    char *c = calloc(1, sizeof(char));
    strncpy(c, s+p, 1);

    if (s[p] == '~' || s[p] == '<' || s[p] == '#') {
        p++;
        root = Leaf_Construct(c);
        root->left = left;
        root->right = Get_E();
    }

    return root;
}

struct Leaf *Get_Assign() {
    struct Leaf *left = Get_E();

    if (left && left->type == VARIABLE && s[p] == '=') {
        p++;
        struct Leaf *root = Leaf_Construct("=");
        root->right = Get_E();
        root->left = left;

        return root;
    }
    else {
        return left;
    }
}

struct Leaf *Get_E() {
    struct Leaf *left = Get_T();
    struct Leaf *root = left;

    while(s[p] == '+' || s[p] == '-') {
        int op = s[p] == '+' ? 1 : (-1);
        p++;
        struct Leaf *right = Get_T();
        struct Leaf *new = Leaf_Construct(op == 1 ? "+" : "-");
        new->left = root;
        new->right = right;
        root = new;
    }

    return root;
}

struct Leaf *Get_T() {
    struct Leaf *left = Get_S();
    struct Leaf *root = left;

    while (s[p] == '*' || s[p] == '/') {
        int op = s[p];
        p++;
        struct Leaf *right = Get_S();
        struct Leaf *new = Leaf_Construct(op == '*' ? "*" : "/");
        new->left = root;
        new->right = right;
        root = new;
    }

    return root;
}

struct Leaf *Get_S() {
    struct Leaf *left = Get_P();
    struct Leaf *root = left;

    while (s[p] == '^') {
        p++;
        struct Leaf *right = Get_S();
        struct Leaf *new = Leaf_Construct("^");
        new->left = root;
        new->right = right;
        root = new;
    }

    return root;
}

struct Leaf *Get_P() {                          //P := (E) || N || Id || F
    if (s[p] == '(') {
        p++;
        struct Leaf *root = Get_E();
        assert(s[p] == ')');
        p++;
        return root;
    }
    else if (isdigit(s[p])){
        return Get_N();
    }
    else if ('A' <= s[p] && s[p] <= 'Z') {
        return Get_Id();
    }
    else if (s[p] == 'f') {
        p++;
        return Get_F();
    }
    else {
        return NULL;
    }
}

struct Leaf *Get_N() {
    double val = 0;
    char *end_ptr = s+p;

    val = strtod(s+p, &end_ptr);
    p += (end_ptr - (s+p));

    return Leaf_Construct_N(val);
}

struct Leaf *Get_Id() {
    char *name = calloc(MAX_LEN, sizeof(char));
    for (int i = 0; 'A' <= s[p] && s[p] <= 'Z'; i++, p++) {
        name[i] = s[p];
    }

    struct Leaf *new_one = Leaf_Construct(name);

    if (vars == NULL) {
        vars = calloc(MAX_LEN, sizeof(int));
        vars[0] = name;
        new_one->num_data = 0;

        return new_one;
    }
    int num = Is_Remembered(name);
    new_one->num_data = num;

    return new_one;
}

struct Leaf *Get_F() {
    struct Leaf *root = Get_Id();
    root->type = FUNC;

    assert(s[p] == '(');
    p++;

    int commas = 0;
    while (s[p] != ')') {
        struct Leaf *new_one = Leaf_Construct(",");
        new_one->left = root->left;
        new_one->right = Get_Id();
        root->left = new_one;

        if (s[p] == ',') {
            p++;
            commas++;
        }
    }
    p++;

    return root;
}