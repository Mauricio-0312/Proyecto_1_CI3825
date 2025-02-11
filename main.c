#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "node.h"

#define MAX_INPUT 256

int main() {
    char input[MAX_INPUT];
    nodeStruct* root = create_node("", DIR);  // Directorio raíz
    nodeStruct* current = root;

    printf("\n");
    printf("\033[1mSimulador de Sistema de Archivos UNIX\033[0m\n");
    printf("\n");
    printf("Escribe 'help' para ver los comandos disponibles.\n");
    

    while (1) {
        printf("\033[1m>\033[0m");
        fgets(input, MAX_INPUT, stdin);
        input[strcspn(input, "\n")] = 0;  // Eliminar el salto de línea

        if (strncmp(input, "exit", 4) == 0) {
            break;
        } else if (strncmp(input, "pwd", 3) == 0) {
            print_path(current);
            printf("\n");
        } else if (strncmp(input, "ls", 2) == 0) {
            int detailed = strstr(input, "-l") != NULL;
            list_directory(current, detailed);
        } else if (strncmp(input, "mkdir ", 6) == 0) {
            char* dir_name = input + 6;
            if (find_node(current, dir_name)) {
                printf("El directorio ya existe.\n");
            } else {
                nodeStruct* new_dir = create_node(dir_name, DIR);
                add_child(current, new_dir);
            }
        } else if (strncmp(input, "touch ", 6) == 0) {
            char* file_name = input + 6;
            if (find_node(current, file_name)) {
                printf("El archivo ya existe.\n");
            } else {
                nodeStruct* new_file = create_node(file_name, FIL);
                add_child(current, new_file);
            }
        } else if (strncmp(input, "cd ", 3) == 0) {
            char* path = input + 3;
            nodeStruct* new_dir = change_directory(current, path);
            if (new_dir) {
                current = new_dir;
            } else {
                printf("Directorio no encontrado.\n");
            }
        } else if (strncmp(input, "rm ", 3) == 0) {
            char* file_name = input + 3;
            nodeStruct* file = find_node(current, file_name);
            if (file && file->type == FIL) {
                delete_node(file);
            } else {
                printf("Archivo no encontrado o es un directorio.\n");
            }
        } else if (strncmp(input, "rmdir ", 6) == 0) {
            char* dir_name = input + 6;
            nodeStruct* dir = find_node(current, dir_name);
            if (dir && dir->type == DIR && !dir->child) {
                delete_node(dir);
            } else {
                printf("Directorio no encontrado o no está vacío.\n");
            }
        } else if (strncmp(input, "wrts ", 5) == 0) {
            char* filename = input + 5;
            FILE* file = fopen(filename, "w");
            if (file) {
                write_fs(root, file);
                fclose(file);
                printf("Sistema de archivos guardado en %s.\n", filename);
            } else {
                printf("No se pudo abrir el archivo para escritura.\n");
            }
        } else if (strncmp(input, "help", 4) == 0) {
            printf("Comandos disponibles:\n");
            printf("mkdir <nombre_directorio>\n");
            printf("touch <nombre_archivo>\n");
            printf("ls [-l]\n");
            printf("pwd\n");
            printf("cd <nombre_directorio>\n");
            printf("rm <nombre_archivo>\n");
            printf("rmdir <nombre_directorio>\n");
            printf("wrts <nombre_archivo>\n");
            printf("exit\n");
        } else {
            printf("Comando no reconocido. Escribe 'help' para ver los comandos disponibles.\n");
        }
    }

    return 0;
}
