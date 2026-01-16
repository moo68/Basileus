#include "basileus/shader.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


char* read_shader_file(char* filepath) {
    char *shader_source = NULL;
    size_t total_length = 0;

    FILE* fp = fopen(filepath, "r");
    if (fp == NULL) {
        printf("ERROR! Could not open shader file: %s\n", filepath);
        return NULL;
    }

    char line[512];
    while (fgets(line, sizeof(line), fp) != NULL) {
        size_t line_length = strlen(line);

        char *new_buffer = realloc(shader_source, total_length + line_length + 1);
        if (!new_buffer) {
            printf("ERROR! Could not reallocate shader file buffer!");
            free(shader_source);
            fclose(fp);
            return NULL;
        }

        shader_source = new_buffer;
        memcpy(shader_source + total_length, line, line_length);

        total_length += line_length;
        shader_source[total_length] = '\0';
    }

    fclose(fp);
    return shader_source;
}

