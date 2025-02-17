#ifndef NODE_H
#define NODE_H

typedef enum { DIR, FIL } TYPEFILE;

typedef struct nodeStruct {
    TYPEFILE type;
    struct nodeStruct* parent;
    struct nodeStruct* child;
    struct nodeStruct* sibling;
    char* name;
    time_t creation_time;
} nodeStruct;

// Declaración de funciones
nodeStruct* create_node(const char* name, TYPEFILE type);
void add_child(nodeStruct* parent, nodeStruct* child);
void delete_node(nodeStruct* node);
void list_directory(nodeStruct* dir, int detailed);
void print_path(nodeStruct* node);
nodeStruct* change_single_directory(nodeStruct* current, const char* name);
nodeStruct* find_node(nodeStruct* parent, const char* name);
void write_fs(nodeStruct* root, FILE* file);
void decodePath(char *path);
nodeStruct* change_complex_directory(nodeStruct* current, nodeStruct* root, const char* path);



#endif // NODE_H
