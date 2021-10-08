#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#ifndef DATA_STRUCTURE_H
#define DATA_STRUCTURE_H

typedef struct
{
    char *buffer;
    size_t buffer_length;
    ssize_t input_length;
} buffer_t;

buffer_t *new_buffer_input()
{
    buffer_t *input_buffer = (buffer_t *)malloc(sizeof(buffer_t));
    input_buffer->buffer = NULL;
    input_buffer->buffer_length = 0;
    input_buffer->input_length = 0;
    return input_buffer;
}

void print_prompt()
{
    printf("tinydb > ");
}

void read_input(buffer_t *input)
{
    size_t bytes_read = getline(&(input->buffer), &(input->buffer_length), stdin);

    if (bytes_read <= 0)
    {
        printf("Error reading input\n");
        exit(EXIT_FAILURE);
    }
    // ignore '\n'
    input->input_length = bytes_read - 1;
    input->buffer[bytes_read - 1] = 0;
}

void close_input_buffer(buffer_t *input)
{
    free(input->buffer);
    free(input);
}
#endif