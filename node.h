#ifndef NODE_H
#define NODE_H

typedef struct nodeStruct nodeStruct;

typedef enum { DIR, FILE } TYPEFILE;

// Declaración de funciones
nodeStruct* create_node(const char* name, TYPEFILE type);
void add_child(nodeStruct* parent, nodeStruct* child);
void delete_node(nodeStruct* node);
void list_directory(nodeStruct* dir);
void print_path(nodeStruct* node);

#endif // NODE_H
