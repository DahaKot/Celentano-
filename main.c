#include <stdio.h>
#include "treebuild.h"

#define ADD           leaf->num_data == '+'
#define SUB           leaf->num_data == '-'
#define MUL           leaf->num_data == '*'
#define DIV           leaf->num_data == '/'
#define POW           leaf->num_data == '^'
#define ASSIG         leaf->num_data == '='
#define AND           leaf->num_data == 'e'
#define ORI           leaf->num_data == 'o'
#define EQU           leaf->num_data == '~'
#define NEQ           leaf->num_data == '#'
#define LES           leaf->num_data == '<'
#define FUNCTION         leaf->type == FUNC
#define CON       !strcmp(leaf->data, "se")
#define LOOP !strcmp(leaf->data, "ripresa")
#define PIZ    !strcmp(leaf->data, "pizza")

int counter = 0;

char *File_Read(const char *filename);                          //make one string from file
char *Delete_Spaces_Tabs(const char *str);                      //delete spaces and tabs from string
void Create_ASM(struct List *prog);                             //move through list and write asm with next func
void Create_ASM_node(struct Leaf *leaf, FILE *out);             //write asm code for one node of Tree

int main() {
    char *programma = File_Read("Canzone.txt");
    printf("%s\n", programma);

    programma = Delete_Spaces_Tabs(programma);
    struct List *statement = List_Build(programma);

    FILE *funcs = fopen("Functions.txt", "w");
    fclose(funcs);

    Create_ASM(statement);

    free(programma);

    return 0;
}

char *File_Read(const char *filename) {
    char *programma = calloc(MAX_NUM_SYM, sizeof(char));

    FILE *canzone = fopen(filename, "r");                //write from file to a string
    fseek(canzone, 0 , SEEK_END);
    size_t p_size = (size_t) ftell(canzone);
    rewind(canzone);
    fread(programma, 1, p_size, canzone);
    fclose(canzone);

    return programma;
}

char *Delete_Spaces_Tabs(const char *str) {
    char *new_one = calloc(strlen(str), sizeof(char));

    int i = 0, j = 0;
    while (str[i] != '\0') {
        if (str[i] == ' ' || str[i] == '\t') {
            i++;
            continue;
        }
        new_one[j] = str[i];
        i++;
        j++;
    }

    return new_one;
}

void Create_ASM(struct List *prog) {
    FILE *source = fopen("Source.txt", "w");

    fprintf(source, "meow\n");

    struct Node *command = prog->head;
    while (command) {
        Create_ASM_node(command->data->root, source);

        command = command->next;
    }

    fprintf(source, "end\n");

    FILE *funcs = fopen("Functions.txt", "r");
    printf("functions\n");

    char c = '\0';
    fscanf(funcs, "%c", &c);
    while (c != '%') {
        fprintf(source, "%c", c);
        fscanf(funcs, "%c", &c);
        if (c == '%') {
            fscanf(funcs, "%c", &c);
            if (c == '\n') {
                continue;
            }
        }
    }


    fclose(funcs);
    fclose(source);
}

void Create_ASM_node(struct Leaf *leaf, FILE *out) {
    if (leaf == NULL) {
        return ;
    }

    if (leaf->data == "," && leaf->type == OPERATOR) {
        Create_ASM_node(leaf->left, out);
        Create_ASM_node(leaf->right, out);
    }
    else if (leaf->type == NUMBER) {
        fprintf(out, "push %lg\n", leaf->num_data);
    }
    else if (leaf->type == VARIABLE) {
        fprintf(out, "push [%d]\n", (int) leaf->num_data);
    }
    else if (SUB && leaf->left == NULL) {
        fprintf(out, "push 0\n");
        Create_ASM_node(leaf->right, out);
        fprintf(out, "sub\n");
    }
    else if (PIZ) {
        fprintf(out, "pizza\n");
        fprintf(out, "outa\n");
    }
    else if (ASSIG) {
        Create_ASM_node(leaf->right, out);
        fprintf(out, "pop [%d]\n", (int) leaf->left->num_data);
    }
    else if (CON) {
        Create_ASM_node(leaf->left, out);
        fprintf(out, "push 0\n"
                "je no%d\n", (int) leaf->num_data);

        fprintf(out, "si%d:\n", (int) leaf->num_data);
        Create_ASM_node(leaf->right->left, out);
        fprintf(out, "jmp fine%d\n", (int) leaf->num_data);

        fprintf(out, "no%d:\n", (int) leaf->num_data);
        Create_ASM_node(leaf->right->right, out);
        fprintf(out, "fine%d:\n", (int) leaf->num_data);
    }
    else if (LOOP) {
        fprintf(out, "loop%d:\n", (int) leaf->num_data);

        Create_ASM_node(leaf->left, out);
        fprintf(out, "push 0\n"
                "je fine%d\n", (int) leaf->num_data);

        Create_ASM_node(leaf->right, out);
        fprintf(out, "jmp loop%d\n", (int) leaf->num_data);
        fprintf(out, "fine%d:\n", (int) leaf->num_data);
    }
    else if (FUNCTION) {
        if (leaf->right == NULL) {
            Create_ASM_node(leaf->left, out);
            fprintf(out, "call %s\n", leaf->data);
        }
        else {
            FILE *funcs = fopen("Functions.txt", "a");
            //fseek(funcs, 0, SEEK_END);
            fprintf(funcs, "\n%s:\n", leaf->data);

            for (int i = (int) leaf->left->num_data; i > 0; i--) {
                fprintf(funcs, "pop [%d]\n", (int) leaf->num_data + i);
            }
            Create_ASM_node(leaf->right, funcs);

            fprintf(funcs, "ret\n%%");
            fclose(funcs);

            /*long position = ftell(out);
            fseek(out, 0, SEEK_END);
            printf("%s\n", leaf->data);
            fprintf(out, "     %s:\n", leaf->data);

            for (int i = (int) leaf->left->num_data; i > 0; i--) {
                fprintf(out, "pop [%d]\n", (int) leaf->num_data + i);
            }
            Create_ASM_node(leaf->right, out);

            fprintf(out, "ret\n");

            fseek(out, position, SEEK_SET);*/
        }
    }
    else {
        Create_ASM_node(leaf->left, out);
        Create_ASM_node(leaf->right, out);
        if     (ADD)    fprintf(out, "add\n");
        else if (MUL)   fprintf(out, "mul\n");
        else if (SUB)   fprintf(out, "sub\n");
        else if (DIV)   fprintf(out, "div\n");
        else if (POW)   fprintf(out, "pow\n");
        else if (AND)   fprintf(out, "mul\n");
        else if (ORI)   fprintf(out, "add\n");
        else if (EQU)   {fprintf(out, "je eq%d\n"
                                      "push 0\n"
                                      "jmp end%d\n"
                                      "eq%d:\n"
                                      "push 1\n"
                                      "end%d:\n",
                                 counter, counter, counter, counter); counter++;}
        else if (NEQ)   {fprintf(out, "jne neq%d\n"
                                         "push 0\n"
                                         "jmp end%d\n"
                                         "neq%d:\n"
                                         "push 1\n"
                                         "end%d:\n",
                                 counter, counter, counter, counter); counter++;}
        else if (LES)   {fprintf(out, "jb less%d\n"
                                         "push 0\n"
                                         "jmp end%d\n"
                                         "less%d:\n"
                                         "push 1\n"
                                         "end%d:\n",
                                 counter, counter, counter, counter); counter++;}
    }
}