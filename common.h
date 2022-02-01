#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "table.h"
#include "types.h"
#include "sql-vm/vm.h"
#include "debug.h"
#ifndef COMMON_H
#define COMMON_H

buffer_t *new_buffer_input()
{
    buffer_t *input_buffer = (buffer_t *)malloc(sizeof(buffer_t));
    input_buffer->buffer = NULL;
    input_buffer->buffer_length = 0;
    input_buffer->input_length = 0;
    return input_buffer;
}

void print_help()
{
    printf("Meta Commands: \n");
    printf("0) .help: print help information,\n");
    printf("1) .exit: exit tinydb and save data on disk.\n");
    printf("2) .constants: print some constants for debugging.\n");
    printf("3) .btree: print b+tree of current table.\n");
    printf("\nSQL Statement: \n");
    printf("1) select (only one `select` keyword :-), and it will print all rows on table)\n");
    printf("2) insert [id:int] [name:str] [email:str]\n");
    printf("3) update [id:int] [name:str] [email:str]\n");
    printf("4) delete [id:int]\n");
}

void print_prompt()
{
    printf("tinydb > ");
}

void read_input(buffer_t *input)
{
    ssize_t bytes_read = getline(&(input->buffer), &(input->buffer_length), stdin);

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

// 执行 meta 命令: .exit, .constants, .btree, .help
meta_command_result_t do_meta_command(buffer_t *input, table_t *table)
{
    if (strcmp(input->buffer, ".exit") == 0)
    {
        db_close(table);
        exit(EXIT_SUCCESS);
    }
    else if (strcmp(input->buffer, ".constants") == 0)
    {
        printf("Constants:\n");
        print_constants();
        return META_COMMAND_SUCCESS;
    }
    else if (strcmp(input->buffer, ".btree") == 0)
    {
        printf("Tree:\n");
        print_btree(table->pager, table->root_page_num, 0);
        return META_COMMAND_SUCCESS;
    }
    else if (strcmp(input->buffer, ".help") == 0)
    {
        print_help();
        return META_COMMAND_SUCCESS;
    }
    else if (strcmp(input->buffer, ".cells") == 0)
    {
        print_max_cells();
        return META_COMMAND_SUCCESS;
    }
    else
    {
        return META_COMMAND_UNRECOGNIZED_COMMAND;
    }
}

#endif