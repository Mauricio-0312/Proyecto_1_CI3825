#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "node.h"

#define MAX_INPUT 256

// Struct para almacenar palabras
typedef struct {
    char* path;
    char type[2];
} file_properties;


// Funcion que lee el archivo de texto
file_properties get_file_properties(const char* str) {
    char* copy = strdup(str);
    char* space_pos = strchr(copy, ' ');
    char* path;
    char* type;
    file_properties properties = {NULL, ""};

    if (space_pos != NULL) {
        int index = space_pos - copy;
        path = strndup(copy, index);
        type = strdup(copy + index + 3);
        properties.path = path;
        strncpy(properties.type, type, sizeof(properties.type) - 1);
        free(type);
    } else {
        path = strdup(copy);
        properties.path = path;
        strncpy(properties.type, "", sizeof(properties.type) - 1);
        
    }
    
    free(copy);
    return properties;
}


void split(const char* str, char **prefix, char **last_token){
    char delimiter[] = "/";
    char *token;
    char *str_copy = strdup(str);  // Crear una copia de la cadena de entrada para no modificar el original
    *prefix = (char *)malloc(strlen(str) + 1);
    *last_token = NULL;

    (*prefix)[0] = '\0';

    // Si la cadena empieza con el delimitador, añadirlo al prefijo
    //  if (str[0] == '/') {
    //      strcat(*prefix, "/");
    //  }


    // Obtener el primer token
    token = strtok(str_copy, delimiter);

    // Continuar obteniendo tokens hasta que no haya más, guardando el último token
    while (token != NULL) {
        *last_token = strdup(token);
        token = strtok(NULL, delimiter);
        if (token != NULL) {
            strcat(*prefix, *last_token);
            strcat(*prefix, "/");
            free(*last_token);
        }
    }

    // Remover el último '/' si no hay tokens adicionales
    if ((*prefix)[strlen(*prefix) - 1] == '/' && strlen(*prefix) != 1) {
        (*prefix)[strlen(*prefix) - 1] = '\0';
    }
   

    free(str_copy);

    // // Imprimir el prefijo y el último token
    // printf("Prefijo: %s\n", prefix);
    // printf("Último token: %s\n", last_token);
}


int main(int argc, char *argv[]) {
    char input[MAX_INPUT];

    nodeStruct* root = create_node("/", DIR);  // Directorio raíz
    nodeStruct* current = root;

    // Lectura del archivo
    FILE* file1 = fopen(argv[1], "r");

     if(file1 != NULL) {
        char line[256];
        
        int line_number = 0;
        while (fgets(line, sizeof(line), file1)) {
            line_number++;
            if (line_number == 1) {
            continue;  // Saltar la primera línea
            }
            
            // Extraemos el path y tipo
            file_properties line_properties = get_file_properties(line);
            char *prefix;
            char *last_token;
            split(line_properties.path, &prefix, &last_token);

           
             printf("Prefijo: %s\n", prefix);
             printf("Ultimo: %s\n", last_token);

            if (strcmp(prefix, "") != 0){
                printf("Entramos al if\n");
                nodeStruct* new_current = change_directory(current, prefix);
                if (new_current) {
                    current = new_current;
                }
                else{
                    printf("Directorio no encontrado\n");
                }

            }

            printf("Directorio actual: ");
            print_path(current);
            printf("\n");

            if (current == NULL) {
                fprintf(stderr, "Error: No se pudo cambiar al directorio %s\n", prefix);
                free(prefix);
                free(last_token);
                continue;
            }

            if (line_properties.type[0] == 'F'){
                nodeStruct* new_file = create_node(last_token, FIL);
                add_child(current, new_file);
            }
            
            if (line_properties.type[0] == 'D') {
                nodeStruct* new_dir = create_node(last_token, DIR);
                add_child(current, new_dir);
            }

            // Touch 
            // if (strncmp(input, "touch ", 6) == 0) {
            // char* file_name = input + 6;
            // if (find_node(current, file_name)) {
            //     printf("El archivo ya existe.\n");
            // } else {
            //     nodeStruct* new_file = create_node(file_name, FIL);
            //     add_child(current, new_file);
            // }
            // }

            printf("\n");
            printf("LISTAMOS EL DIRECTORIO\n");
            printf("\n");
            list_directory(current, 0);
            printf("\n");

            free(prefix);
            free(last_token);

            current = root;

            // Mkdir
            
            // if (strncmp(input, "mkdir ", 6) == 0) {
            // char* dir_name = input + 6;
            // if (find_node(current, dir_name)) {
            //     printf("El directorio ya existe.\n");
            // } else {
            //     nodeStruct* new_dir = create_node(dir_name, DIR);
            //     add_child(current, new_dir);
            // }
            // }

            
        }

        // Cierra el archivo
        fclose(file1);
    }

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
