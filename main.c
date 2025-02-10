#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "node.h"

#define MAX_INPUT 256

int main() {
    char input[MAX_INPUT];
    nodeStruct* root = create_node("", DIR);  // Directorio raíz
    nodeStruct* current = root;

    printf("Simulador de Sistema de Archivos UNIX\n");
    printf("Escribe 'help' para ver los comandos disponibles.\n");

    while (1) {
        printf("> ");
        fgets(input, MAX_INPUT, stdin);
        input[strcspn(input, "\n")] = 0;  // Eliminar el salto de línea

        if (strncmp(input, "exit", 4) == 0) {
            break;
        } else if (strncmp(input, "pwd", 3) == 0) {
            print_path(current);
            printf("\n");
        } else if (strncmp(input, "ls", 2) == 0) {
            list_directory(current);
        } else if (strncmp(input, "mkdir ", 6) == 0) {
            char* dir_name = input + 6;
            nodeStruct* new_dir = create_node(dir_name, DIR);
            add_child(current, new_dir);
        } else if (strncmp(input, "touch ", 6) == 0) {
            char* file_name = input + 6;
            nodeStruct* new_file = create_node(file_name, FILE);
            add_child(current, new_file);
        } else if (strncmp(input, "help", 4) == 0) {
            printf("Comandos disponibles:\n");
            printf("mkdir <nombre_directorio>\n");
            printf("touch <nombre_archivo>\n");
            printf("ls\n");
            printf("pwd\n");
            printf("exit\n");
        } else {
            printf("Comando no reconocido. Escribe 'help' para ver los comandos disponibles.\n");
        }
    }

    return 0;
}
