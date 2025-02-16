#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "node.h"




nodeStruct* create_node(const char* name, TYPEFILE type) {
    nodeStruct* new_node = (nodeStruct*)malloc(sizeof(nodeStruct));
    new_node->name = strdup(name);
    new_node->type = type;
    new_node->parent = new_node->child = new_node->sibling = NULL;
    new_node->creation_time = time(NULL);
    return new_node;
}

void add_child(nodeStruct* parent, nodeStruct* child) {
    if (!parent->child) {
        parent->child = child;
    } else {
        nodeStruct* sibling = parent->child;
        while (sibling->sibling) {
            sibling = sibling->sibling;
        }
        sibling->sibling = child;
    }
    child->parent = parent;
}

void delete_node(nodeStruct* node) {
    if (node->parent) {
        nodeStruct* sibling = node->parent->child;
        if (sibling == node) {
            node->parent->child = node->sibling;
        } else {
            while (sibling->sibling != node) {
                sibling = sibling->sibling;
            }
            sibling->sibling = node->sibling;
        }
    }
    free(node->name);
    free(node);
}

void list_directory(nodeStruct* dir, int detailed) {
    if (dir->type != DIR) {
        printf("%s no es un directorio.\n", dir->name);
        return;
    }
    nodeStruct* child = dir->child;
    while (child) {
        if (detailed) {
            char time_str[20];
            strftime(time_str, sizeof(time_str), "%H:%M-%d/%m/%Y", localtime(&child->creation_time));
            printf("%s\t%s\t%s\n", (child->type == DIR ? "D" : "F"), time_str, child->name);
        } else {
            printf("%s\t%s\n", (child->type == DIR ? "DIR" : "FILE"), child->name);
        }
        child = child->sibling;
    }
}

void print_path(nodeStruct* node) {
    if (node->parent) {
        print_path(node->parent);
    }
    if (strlen(node->name) > 0) {
        printf("%s", node->name);
    }
}

nodeStruct* find_node(nodeStruct* parent, const char* name) {
    nodeStruct* child = parent->child;
    while (child) {
        if (strcmp(child->name, name) == 0) {
            return child;
        }
        child = child->sibling;
    }
    return NULL;
}

nodeStruct* change_directory(nodeStruct* current, const char* path) {
    if (strcmp(path, "..") == 0) {
        return current->parent ? current->parent : current;
    } else if (strcmp(path, ".") == 0) {
        return current;
    } else {
        nodeStruct* target = find_node(current, path);
        if (target && target->type == DIR) {
            return target;
        }
    }
    return NULL;
}

void write_fs(nodeStruct* root, FILE* file) {
    if (!root) return;
    char time_str[20];
    strftime(time_str, sizeof(time_str), "%H:%M-%d/%m/%Y", localtime(&root->creation_time));
    fprintf(file, "%s\t%s\t%s\n", root->name, time_str, root->type == DIR ? "D" : "F");
    write_fs(root->child, file);
    write_fs(root->sibling, file);
}
