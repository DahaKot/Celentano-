//
// Created by dasch on 19.11.2017.
//

#include "tree.h"

int Is_Number(const char *str) {
    int i = 0;
    if (!strcmp(str, "-")) {
        return 0;
    }
    if (str[0] == '-') {
        i++;
    }
    for ( ; str[i] != '\0'; i++) {
        if (!isdigit((int) str[i]) && str[i] != '.') {
            return 0;
        }
    }

    return 1;
}

int Is_Operator(const char *str) {
    if (!strcmp(str, "+") || !strcmp(str, "-")  || !strcmp(str, "*") || !strcmp(str, "/") || !strcmp(str, "^") ||
            !strcmp(str, "=") || !strcmp(str, "se") || !strcmp(str, ",") || !strcmp(str, "e") || !strcmp(str, "o") ||
            !strcmp(str, "~") || !strcmp(str, "<")  || !strcmp(str, "#") || !strcmp(str, "pizza") ||
            !strcmp(str, "ripresa")) {
        return (int) str[0];
    }
    return 0;
}

int Is_Func(const char *str) {
    if (!strcmp(str, "sin") || !strcmp(str, "cos") || !strcmp(str, "tg") || !strcmp(str, "ctg") ||
        !strcmp(str, "sh") || !strcmp(str, "ch") || !strcmp(str, "th") || !strcmp(str, "cth") ||
        !strcmp(str, "arcsin") || !strcmp(str, "arccos") || !strcmp(str, "arctg") ||
        !strcmp(str, "arcctg") || !strcmp(str, "ln")) {
        return 1;
    }
    return 0;
}

int Is_Variable(const char *str) {
    int i = 0;
    while (i < strlen(str) && 'A' <= str[i] && str[i] <= 'Z') {
        i++;
        if (i < strlen(str) && ('A' > str[i] || str[i] > 'Z')) {
            return 0;
        }
    }
    return 1;
}

struct Leaf *Leaf_Construct(data_t value) {
    struct Leaf *new_one = calloc(1, sizeof(struct Leaf));
    if (new_one == NULL) {
        Log_write(NOT_ENOUGH_MEMORY, __LINE__);
        return NULL;
    }

    new_one->data = calloc(DATA_LEN, sizeof(char));
    if (new_one->data == NULL) {
        Log_write(NOT_ENOUGH_MEMORY, __LINE__);
        return NULL;
    }
    strncpy(new_one->data, value, strlen(value));

    if (Is_Number(new_one->data)) {
        new_one->num_data = strtod(new_one->data, NULL);
        new_one->type = NUMBER;
    }
    else {
        new_one->num_data = POISON;
        if (Is_Operator(new_one->data)) {
            new_one->type = OPERATOR;
            new_one->num_data = Is_Operator(new_one->data);
        }
        else if (Is_Func(new_one->data)) {
            new_one->type = OPERATOR;
            func_num_set
        }
        else if (Is_Variable(new_one->data)){
            new_one->type = VARIABLE;
        }
        else {
            Log_write(WRONG_VALUE, __LINE__);
            return NULL;
        }
    }

    new_one->left = NULL;
    new_one->right = NULL;
    new_one->parent = NULL;
    new_one->parent_num = 0;
    new_one->level = 0;
    new_one->num = 0;

    return new_one;
}

struct Leaf *Leaf_Construct_N(double value) {
    struct Leaf *new_one = calloc(1, sizeof(struct Leaf));
    if (new_one == NULL) {
        Log_write(NOT_ENOUGH_MEMORY, __LINE__);
        return NULL;
    }

    new_one->num_data = value;
    new_one->type = NUMBER;
    new_one->data = "NUMBER\n";

    new_one->left = NULL;
    new_one->right = NULL;
    new_one->parent = NULL;
    new_one->parent_num = 0;
    new_one->level = 0;
    new_one->num = 0;

    return new_one;
}

void Leaf_Destruct(struct Leaf **old_one) {
    if (*old_one != NULL) {
        Leaf_Destruct(&((*old_one)->left));
        Leaf_Destruct(&((*old_one)->right));
        //Leaf_Destruct(&((*old_one)->parent));
    }
    free(*old_one);
    (*old_one) = NULL;
}

struct Tree *Tree_Construct(data_t value) {
    struct Tree *new_one = calloc(1, sizeof(struct Tree));
    if (new_one == NULL) {
        Log_write(NOT_ENOUGH_MEMORY, __LINE__);
        return (void *) NOT_ENOUGH_MEMORY;
    }

    new_one->len = 1;
    new_one->root = Leaf_Construct(value);

    return new_one;
}

void Tree_Destruct(struct Tree **old_one) {
    if (*old_one != NULL) {
        Leaf_Destruct(&((*old_one)->root));
    }
    free(*old_one);
    *old_one = NULL;
}

int Tree_Ok(struct Tree *tree) {
    if (tree == NULL || tree->root == NULL || tree->len < 1) {
        return 0;
    }

    if (tree->root->parent != NULL) {
        return 0;
    }

    struct Leaf *elem = tree->root;
    return SubTree_Ok(elem);
}

int SubTree_Ok(struct Leaf *root) {
    if (root == NULL) {
        return 0;
    }

    if (root->right == NULL && root->left == NULL) {
        return 1;
    }
    else {
        if (root->right && root->right->parent_num != root->num ||
                root->right && root->right->parent != root) {
            if (root->right && !SubTree_Ok(root->right)) {
                return 0;
            }
            return 0;
        }
        if (root->left && root->left->parent_num != root->num ||
                root->left && root->left->parent != root) {
            if (root->left && !SubTree_Ok(root->left)) {
                return 0;
            }
            return 0;
        }
    }

    return 1;
}

void Tree_Dump(struct Tree *tree, size_t line) {
    FILE *Dump_f = fopen("Dump.txt", "w");
    assert(Dump_f != NULL);

    fprintf(Dump_f, "digraph Tree {\nroot [label = \"Tree\\nTree len = %d\\nline: %d\"];\n", tree->len, line);
    fclose(Dump_f);

    Leaf_Dump(tree->root);

    Dump_f = fopen("Dump.txt", "a");
    assert(Dump_f != NULL);

    fprintf(Dump_f, "}");
    fclose(Dump_f);

    system ("dot -Tpng Dump.txt -o Dump.png");
    system("Dump.png");
}

void Leaf_Dump(struct Leaf *leaf) {
    if (leaf == NULL) {
        return ;
    }

    Leaf_Dump(leaf->left);

    FILE *Dump_f = fopen("Dump.txt", "a");
    assert(Dump_f != NULL);

    fprintf(Dump_f, "node%d [shape=record, label=\"{<name> node%d\\n%p|<parent> parent: %p\\nnumber: %d|<data> data:\\ %s|"
                    "{<type> type: %d|<num_data> %lg|<level> level: %d}|{<left> left\\n%p|<right> right\\n%p}}\"];\n",
            leaf->num, leaf->num, leaf, leaf->parent, leaf->parent_num, leaf->data,
            leaf->type, leaf->num_data, leaf->level, leaf->left, leaf->right);
    if (leaf->left != NULL) {
        if (SubTree_Ok(leaf) != 1) {
            fprintf(Dump_f, "edge [color = red];\n");
            fprintf(Dump_f, "node%d [style = \"filled\", fillcolor = \"red\"]\n", leaf->num);
        }
        else {
            fprintf(Dump_f, "edge [color = blue];\n");
        }
        fprintf(Dump_f, "\"node%d\":left -> \"node%d\":name;\n", leaf->num, leaf->left->num);
    }
    if (leaf->right != NULL) {
        if (SubTree_Ok(leaf) != 1) {
            fprintf(Dump_f, "edge [color = red];\n");
            fprintf(Dump_f, "node%d [style = \"filled\", fillcolor = \"red\"]\n", leaf->num);
        }
        else {
            fprintf(Dump_f, "edge [color = green];\n");
        }
        fprintf(Dump_f, "\"node%d\":right -> \"node%d\":name;\n", leaf->num, leaf->right->num);
    }

    fclose(Dump_f);

    Leaf_Dump(leaf->right);
}

void Tree_AddLeft(struct Tree *tree, struct Leaf *leaf, data_t value) {
    assert(tree != NULL);
    assert(leaf != NULL);
    assert(leaf->left == NULL);
    if (!Tree_Ok(tree)) {
        Tree_Dump(tree, __LINE__);
        Log_write(ERR, __LINE__);
        return ;
    }

    leaf->left = Leaf_Construct(value);

    leaf->left->level = leaf->level + 1;
    leaf->left->num = tree->len;
    tree->len++;
    leaf->left->parent_num = leaf->num;
    leaf->left->parent = leaf;

    if (!Tree_Ok(tree)) {
        Tree_Dump(tree, __LINE__);
        return ;
    }
}

void Tree_AddRight(struct Tree *tree, struct Leaf *leaf, data_t value) {
    assert(tree != NULL);
    if (!Tree_Ok(tree)) {
        Tree_Dump(tree, __LINE__);
        return ;
    }
    assert(leaf != NULL);
    assert(leaf->right == NULL);

    leaf->right = Leaf_Construct(value);

    leaf->right->level = leaf->level + 1;
    leaf->right->num = tree->len;
    tree->len++;
    leaf->right->parent_num = leaf->num;
    leaf->right->parent = leaf;

    if (!Tree_Ok(tree)) {
        Tree_Dump(tree, __LINE__);
        return ;
    }
}

void Human_Dump(struct Tree *ak) {
    FILE *Dump_f = fopen("ADump.txt", "w");
    if (Dump_f == NULL) {
        Log_write(FILE_ERR, __LINE__);
        return ;
    }

    fprintf(Dump_f, "digraph Tree {\n");
    fclose(Dump_f);

    Human_Leaf_Dump(ak->root);

    Dump_f = fopen("ADump.txt", "a");
    if (Dump_f == NULL) {
        Log_write(FILE_ERR, __LINE__);
        return ;
    }

    fprintf(Dump_f, "}");
    fclose(Dump_f);

    system ("dot -Tpng ADump.txt -o ADump.png");
    system("ADump.png");
}

void Human_Leaf_Dump (struct Leaf *leaf) { ;
    if (leaf == NULL) {
        return ;
    }

    Human_Leaf_Dump(leaf->left);

    FILE *Dump_f = fopen("ADump.txt", "a");
    if (Dump_f == NULL) {
        Log_write(FILE_ERR, __LINE__);
        return ;
    }

    if (leaf->type == NUMBER) {
        fprintf(Dump_f, "node%d [label=\"%lg\"];\n", leaf->num, leaf->num_data);
    }
    else {
        fprintf(Dump_f, "node%d [label=\"%s\"];\n", leaf->num, leaf->data);
    }

    if (leaf->left != NULL) {
        fprintf(Dump_f, "edge [color = blue];\n");
        fprintf(Dump_f, "node%d -> node%d;\n", leaf->num, leaf->left->num);
    }
    if (leaf->right != NULL) {
        fprintf(Dump_f, "edge [color = green];\n");
        fprintf(Dump_f, "node%d -> node%d;\n", leaf->num, leaf->right->num);
    }

    fclose(Dump_f);

    Human_Leaf_Dump(leaf->right);
}

void Log_write (int err_code, int line) {
    FILE *logs;
    logs = fopen("Log.txt", "a+");

    static int start = 0;
    if (start == 0) {
        time_t current_time;
        time(&current_time);

        fprintf(logs, "Program started: %s", ctime(&current_time));
        start++;
    }

    switch(err_code) {
        case (NOT_ENOUGH_MEMORY):
            fprintf(logs, "Can't alloc memory on %d\n", line);
            break;
        case (ERR):
            fprintf(logs, "Some error on %d\n", line);
            break;
        case (BAD_PTR):
            fprintf(logs, "You use invalid pointer on %d\n", line);
            break;
        case (WRONG_VALUE):
            fprintf(logs, "Trying to use wrong value . %d", line);
            break;
        case (DEL0):
            fprintf(logs, "Division by 0!");
            break;
        default:
            fprintf(logs, "Who knows, what's happening right now on %d?)\n", line);
    }

    fclose(logs);
}

void Tree_SParameters_Set(struct Tree *tree) {
    if (tree == NULL || tree->root == NULL || tree->len < 1) {
        return ;
    }

    struct Leaf *elem = tree->root;
    tree->root->parent = NULL;
    tree->root->parent_num = 0;
    tree->len = 0;

    Leaf_SParameters_Set(tree, elem, 0, NULL);
}

void Leaf_SParameters_Set(struct Tree *tree, struct Leaf *root, size_t level, struct Leaf *parent) {
    if (root == NULL) {
        return ;
    }

    root->level = level;
    root->parent_num = (parent == NULL) ? 0 : parent->num;
    root->parent = parent;
    root->num = tree->len;
    tree->len++;

    Leaf_SParameters_Set(tree, root->left, level + 1, root);
    Leaf_SParameters_Set(tree, root->right, level + 1, root);
}