// Proyecto 1: Simulando la Administracion de un Sistema de Archivos UNIX
// Mauricio Fragachan 20-10265
// Jesus Gutierrez 20-10332
// Main

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "node.h"

#define MAX_INPUT 256
// Funcion que divide un path:
// Si un path tiene n elementos, devuelve el subpath de los primeros n-1 elementos
// y el nesimo elemento.
void split(const char* str, char **prefix, char **last_token){
    char delimiter[] = "/";
    char *token;
    char *str_copy = strdup(str);  
    // Crear una copia de la cadena de entrada para no modificar el original
    *prefix = (char *)malloc(strlen(str) + 1);
    *last_token = NULL;

    (*prefix)[0] = '\0';

    // Obtener el primer token
    token = strtok(str_copy, delimiter);

    size_t prefix_size = strlen(str) + 1;

    // Continuar obteniendo tokens hasta que no haya mas, guardando el ultimo token
    while (token != NULL) {
        *last_token = strdup(token);
        token = strtok(NULL, delimiter);

        // Comprobamos si necesitamos más espacio en *prefix
        size_t current_len = strlen(*prefix);
        size_t new_len = current_len + strlen(*last_token) + 1;
        if (new_len >= prefix_size) {
            // Realocamos *prefix si es necesario
            prefix_size = new_len + 1;  // +1 por el '\0'
            *prefix = (char *)realloc(*prefix, prefix_size);
        }

        if (token != NULL) {
            strcat(*prefix, *last_token);
            strcat(*prefix, "/");
            free(*last_token);
        }
    }

    // Remover el ultimo '/' si no hay tokens adicionales
    if (strlen(*prefix) > 0 && (*prefix)[strlen(*prefix) - 1] == '/') {
        (*prefix)[strlen(*prefix) - 1] = '\0';
    }
    free(str_copy);
}

// Funcion main donde se maneja la entrada del usuario
int main(int argc, char *argv[]) {
    char input[MAX_INPUT];

    // Definimos e inicializamos el directorio raiz
    nodeStruct* root = create_node("/", DIR);  
    nodeStruct* current = root;

    // Lectura del archivo
    FILE* file1 = fopen(argv[1], "r");

    // Si el archivo existe entonces hacemos la lectura y creamos el sistema de archivoss
     if(file1 != NULL) {
        char line[999];
        
        int line_number = 0;
        // Iteramos sobre cada linea del archivo
        while (fgets(line, sizeof(line), file1) != NULL) {
            // Se salta la primera linea ya que el root ya esta creado (suponiendo que la entrada del archivo es correcta)
            line_number++;
            if (line_number == 1) {
            continue;  
            }
            
            int k = 0;
            while (line[k] != '\t' && line[k] != ' ') {
                k++;
            }

            char *copy = (char*)malloc(k + 1);
            if (copy == NULL) {
                perror("Error al asignar memoria");
                fclose(file1);
                return 1;
            }

            int j = 0;

            // Copiamos el path
            while (j < k) {
                copy[j] = line[j];
                j++;
            }
            copy[j] = '\0';
            
            int k2 = k;
            while (line[k2] == '\t' || line[k2] == ' ') {
                k2++;
            }
            // Copiamos el tipo de archivo
            char tipo = line[k2];
            
            // Separamos el path extraido de la linea
            char *prefix;
            char *last_token;
            split(copy, &prefix, &last_token);

            // Buscamos el directorio donde vamos a crear el nuevo nodo
            if (strcmp(prefix, "") != 0){
                nodeStruct* new_current = change_complex_directory(current, root, prefix);
                if (new_current) {
                    current = new_current;
                }
                else{
                    printf("Error: Directorio no encontrado\n");
                }
            }
            
            if (current == NULL) {         
                printf("Error: No se pudo cambiar al directorio %s\n", prefix);
                free(prefix);
                free(last_token);
                free(copy);
                continue;
            }

            if (tipo == 'F'){
                nodeStruct* new_file = create_node(last_token, FIL);
                add_child(current, new_file);
            }
            
            if (tipo == 'D') {
                nodeStruct* new_dir = create_node(last_token, DIR);
                add_child(current, new_dir);
            }

            // Luego de crear el nuevo nodo, regresamos al root
            current = root;

            // Liberar memoria
            free(prefix);
            free(last_token);
            free(copy);     
        }

        // Cerrar archivo
        fclose(file1);
    }

    printf("\n");
    printf("\033[1mSimulador de Sistema de Archivos UNIX\033[0m\n");
    printf("\n");
    printf("Escribe \033[1m'help'\033[0m para ver los comandos disponibles.\n");
    
    // Ciclo para manejar la entrada de comandos
    while (1) {
        printf("\n");
        printf("\033[1m> \033[0m");
        fgets(input, MAX_INPUT, stdin);
        input[strcspn(input, "\n")] = 0;  // Eliminar el salto de línea

        // Comando exit
        if (strncmp(input, "exit", 4) == 0) {
            // Limpiamos el arbol
            clean(root);
            break;
        // Comando pwd
        } else if (strncmp(input, "pwd", 3) == 0) {
            // Obtenemos el path y luego se imprime
            char *path = current != root ? get_path(current) : strdup("/");
            printf("%s\n", path);
            // Liberar memoria
            free(path);

        // Comando ls
        } else if (strncmp(input, "ls", 2) == 0) {
            // Verificamos si se ingreso el flag '-l'
            int detailed = strstr(input, "-l") != NULL;
            char* path;
            if (detailed) {
                path = input + 6;
            }else {
                path = input + 3;
            }
            // Buscamos el directorio y luego imprimimos su contenido
            nodeStruct *dir = find_nested_node(current, root, path, DIR);
            if (dir) {
                list_directory(dir, detailed);
            } else {
                printf("Error: Directorio no encontrado.\n");
            }

        // Comando mkdir
        } else if (strncmp(input, "mkdir ", 6) == 0) {
            char* dir_path = input + 6;
            create_nested_node(current, root, dir_path, DIR);
            
        // Comando touch
        } else if (strncmp(input, "touch ", 6) == 0) {
            char* file_path = input + 6;
            create_nested_node(current, root, file_path, FIL);
            
        // Comando cd
        } else if (strncmp(input, "cd ", 3) == 0) {
            char* path = input + 3;
            nodeStruct* new_dir = find_nested_node(current, root, path, DIR);
            if (new_dir) {
                current = new_dir;
            } else {
                printf("Error: Directorio no encontrado.\n");
            }

        // Comando rm
        } else if (strncmp(input, "rm ", 3) == 0) {
            char* file_path = input + 3;

            // Verificamos que no exista un directorio en el path con el mismo nombre
            nodeStruct* dir = find_nested_node(current, root, file_path, DIR);
            if (dir) {
                printf("Error: No se puede eliminar un directorio con 'rm'.\n");
                continue;
            }

            // Buscamos el archivo y en caso que exista, se elimina
            nodeStruct* file = find_nested_node(current, root, file_path, FIL);
            if (file && file != root){
                removeFIL(file);
            }else{
                printf("Error: Archivo no encontrado\n");
            }

        // Comando rmdir
        } else if (strncmp(input, "rmdir ", 6) == 0) {
            char* dir_path = input + 6;
            // Verificamos que no exista un archivo en el path con el mismo nombre
            nodeStruct* file = find_nested_node(current, root, dir_path, FIL);
            if (file) {
                printf("Error: No se puede eliminar un archivo con 'rmdir'.\n");
                continue;
            }

            // Buscamos el directorio y en caso que exista, se elimina
            nodeStruct* dir = find_nested_node(current, root, dir_path, DIR);
            if (dir && dir != root){
                if (dir == current)
                {
                    printf("Error: No se puede eliminar el directorio actual\n");
                }
                
                removeDIR(dir);
            }else{
                printf("Error: Directorio no encontrado\n");
            }
                     
        // Comando wrts
        } else if (strncmp(input, "wrts ", 5) == 0) {
            char* filename = input + 5;
            // Escribimos sobre el archivo
            FILE* file = fopen(filename, "w");
            if (file) {
                write_fs(root, file); 
                fclose(file);
                printf("Sistema de archivos guardado en %s.\n", filename);
            } else {
                printf("Error: No se pudo abrir el archivo para escritura.\n");
            }

        // Comando help
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
            printf("Error: Comando no reconocido. Escribe \033[1m'help'\033[0m para ver los comandos disponibles.\n");
        }
    }

    return 0;
}
