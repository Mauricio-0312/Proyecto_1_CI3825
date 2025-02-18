// Proyecto 1: Simulando la Administracion de un Sistema de Archivos UNIX
// Mauricio Fragachan 20-10265
// Jesus Gutierrez 20-10332
// Definicion de la estructura NodeStruct y funciones auxiliares


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "node.h"

typedef struct nodeStruct nodeStruct;

struct nodeStruct {
    TYPEFILE type;
    struct nodeStruct* parent;
    struct nodeStruct* child;
    struct nodeStruct* sibling;
    char* name;
    time_t creation_time;
};

nodeStruct* create_node(const char* name, TYPEFILE type) {
    nodeStruct* new_node = (nodeStruct*)malloc(sizeof(nodeStruct));
    new_node->name = strdup(name);
    new_node->type = type;
    new_node->parent = new_node->child = new_node->sibling = NULL;
    new_node->creation_time = time(NULL);
    return new_node;
}

void add_child(nodeStruct* parent, nodeStruct* child) {
    if (parent == NULL) {
        // Manejo del error: tal vez loguear, retornar, o alguna acción adecuada
        printf("Error: parent es NULL.\n");
        return;  // O algún otro comportamiento de manejo de error
    }
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
            printf("\033[0;32m%s\033[0m\t%s\t%s\n", (child->type == DIR ? "D" : "F"), time_str, child->name);
        } else {
            printf("%s\t%s\n", (child->type == DIR ? "DIR" : "FILE"), child->name);
        }
        child = child->sibling;
    }
}
char* get_path(nodeStruct* node) {
    if (node->parent) {
        char* parent_path = get_path(node->parent);
        char* full_path = (char*)malloc(strlen(parent_path) + strlen(node->name) + 2);
        sprintf(full_path, "%s/%s", parent_path, node->name);
        free(parent_path);
        return full_path;
    } else {
        return strdup("");
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


nodeStruct* change_single_directory(nodeStruct* current, const char* name, TYPEFILE targetType ){ 
    if (strcmp(name, "..") == 0) {
        return current->parent ? current->parent : current;
    } else if (strcmp(name, ".") == 0) {
        return current;
    } else {
        if(current == NULL){
            return NULL;
        }
        nodeStruct* target = find_node(current, name);
        if (target && target->type == targetType){ 
            return target;
        }
    }
    return NULL;
}

nodeStruct* change_complex_directory(nodeStruct* current, nodeStruct* root, const char* path) {
    // Encontrar la primera ocurrencia del separador '/'
    if (strlen(path) == 0){
        return current;
    }

    char *separator = strchr(path, '/');
    
    if (separator != NULL) {
       
        *separator = '\0';
        
        nodeStruct* new_current;
        if (strlen(path) == 0){
            new_current = root;
        }else{
            new_current = change_single_directory(current, path, DIR);
        }
        
        return change_complex_directory(new_current, root, separator + 1);
    } else {
        return change_single_directory(current, path, DIR);
    }
}

nodeStruct* find_nested_node(nodeStruct* current, nodeStruct* root, const char* path, TYPEFILE targetType) {
    // Encontrar la primera ocurrencia del separador '/'
    if (strlen(path) == 0){
        return current;
    }

    char *separator = strchr(path, '/');
    
    if (separator != NULL) {
        // Terminar el primer segmento con '\0'
        *separator = '\0';
        
        
        nodeStruct* new_current;
        if (strlen(path) == 0){
            new_current = root;
        }else{
            new_current = change_single_directory(current, path, DIR);
        }
        
        return find_nested_node(new_current, root, separator + 1, targetType);
    } else {
        
        return change_single_directory(current, path, targetType);
    }
}

nodeStruct* create_nested_node(nodeStruct* current, nodeStruct* root, const char* path, TYPEFILE targetType) {
    // Encontrar la primera ocurrencia del separador '/'
    if (strlen(path) == 0){
        return current;
    }

    char *separator = strchr(path, '/');
    
    if (separator != NULL) {
        // Terminar el primer segmento con '\0'
        *separator = '\0';
        
        
        nodeStruct* new_current;
        if (strlen(path) == 0){
            new_current = root;
        }else{
            new_current = change_single_directory(current, path, DIR);
        }
        
        return create_nested_node(new_current, root, separator + 1, targetType);
    } else {
        nodeStruct* node = change_single_directory(current, path, targetType);
        if (node) {
            if (targetType == FIL){

                printf("El archivo ya existe.\n");
            }else{
                printf("El directorio ya existe.\n");
            }
        } else {
            nodeStruct* new_node = create_node(path, targetType);
            add_child(current, new_node);
            node = new_node;
        }

        return node;
    }
}

// Funcion para esc
void write_fs(nodeStruct* root, FILE* file) {
    if (!root) return;
    
    char time_str[20];
    char *new_path = root->parent != NULL ? get_path(root) : strdup("/");
    
    strftime(time_str, sizeof(time_str), "%H:%M-%d/%m/%Y", localtime(&root->creation_time));
    fprintf(file, "%s\t%s\t%s\t%s\n", root->name, time_str, root->type == DIR ? "D" : "F", new_path);

    // Liberar memoria
    free(new_path);

    write_fs(root->child, file);
    write_fs(root->sibling, file);
}

// Funcion para limpiar el arbol de nodos
void clean(nodeStruct *node) { 
    
    if(node != NULL){

        clean(node->child);
        clean(node->sibling);
        
        // Liberar memoria
        free(node->name);
        free(node);
    }
}

// Funcion para eliminar un Archivo
void removeFIL(nodeStruct *node){
    
    delete_node(node);
}

// Funcion para eliminar un Directorio
void removeDIR(nodeStruct* node){
    
    // Si el directorio no esta vacio, se elimina
    if(!node->child){
        delete_node(node);

    }else{
        printf("Error: Directorio no está vacío.\n");
    }
    
}
