#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "node.h"

struct nodeStruct {
    TYPEFILE type;
    struct nodeStruct* parent;
    struct nodeStruct* child;
    struct nodeStruct* sibling;
    char* name;
};

// Crear un nuevo nodo
nodeStruct* create_node(const char* name, TYPEFILE type) {
    nodeStruct* new_node = (nodeStruct*)malloc(sizeof(nodeStruct));
    new_node->name = strdup(name);
    new_node->type = type;
    new_node->parent = new_node->child = new_node->sibling = NULL;
    return new_node;
}

// Agregar un hijo al nodo
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

// Eliminar un nodo
void delete_node(nodeStruct* node) {
    if (node->child) {
        printf("No se puede eliminar un directorio no vacío: %s\n", node->name);
        return;
    }
    free(node->name);
    free(node);
}

// Listar los contenidos de un directorio
void list_directory(nodeStruct* dir) {
    if (dir->type != DIR) {
        printf("%s no es un directorio.\n", dir->name);
        return;
    }
    nodeStruct* child = dir->child;
    while (child) {
        printf("%s\t%s\n", (child->type == DIR ? "DIR" : "FILE"), child->name);
        child = child->sibling;
    }
}

// Imprimir la ruta completa del nodo
void print_path(nodeStruct* node) {
    if (node->parent) {
        print_path(node->parent);
    }
    printf("/%s", node->name);
}
