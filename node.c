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
        if(node->name[0] != '/'){
            printf("/%s", node->name);
        }else{
            printf("%s", node->name);
        }
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


// void decodePath(char *path) {
//     // Encontrar la primera ocurrencia del separador '/'
//     char *separator = strchr(path, '/');
    
//     if (separator != NULL) {
//         // Terminar el primer segmento con '\0'
//         *separator = '\0';
        
//         // Imprimir el primer segmento
//         printf("%s\n", path);
        
//         // Llamar recursivamente para el resto del string
//         decodePath(separator + 1);
//     } else {
//         // Imprimir el último segmento
//         printf("%s\n", path);
//     }
// }


nodeStruct* change_single_directory(nodeStruct* current, const char* name) {
    // printf("Cambiando a %s\n", name);
    if (strcmp(name, "..") == 0) {
        return current->parent ? current->parent : current;
    } else if (strcmp(name, ".") == 0) {
        return current;
    } else {
        nodeStruct* target = find_node(current, name);
        if (target && target->type == DIR) {
            // printf("Cambio a %s\n", target->name);
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
        // Terminar el primer segmento con '\0'
        *separator = '\0';
        
        // Imprimir el primer segmento
        // printf("HOLA %s\n", path);
        // printf("%s %ld\n", path, strlen(path));
        nodeStruct* new_current;
        if (strlen(path) == 0){
            new_current = root;
        }else{
            // printf("HOLA 1%s\n", path);
            new_current = change_single_directory(current, path);
        }
        
        return change_complex_directory(new_current, root, separator + 1);
    } else {
        // printf("Else %s\n", path);
        return change_single_directory(current, path);
    }
}

void write_fs(nodeStruct* root, FILE* file) {
    if (!root) return;
    char time_str[20];
    strftime(time_str, sizeof(time_str), "%H:%M-%d/%m/%Y", localtime(&root->creation_time));
    fprintf(file, "%s\t%s\t%s\n", root->name, time_str, root->type == DIR ? "D" : "F");
    write_fs(root->child, file);
    write_fs(root->sibling, file);
}
