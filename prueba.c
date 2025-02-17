#include <stdio.h>
#include <string.h>

void recursivePrint(char *path) {
    // Encontrar la primera ocurrencia del separador '/'
    char *separator = strchr(path, '/');
    
    if (separator != NULL) {
        // Terminar el primer segmento con '\0'
        *separator = '\0';
        
        // Imprimir el primer segmento
        printf("%s\n", path);
        
        // Llamar recursivamente para el resto del string
        recursivePrint(separator + 1);
    } else {
        // Imprimir el último segmento
        printf("%s\n", path);
    }
}

int main() {
    char path[] = "/home";
    recursivePrint(path);
    return 0;
}