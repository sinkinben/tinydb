#include "common.h"
int main(int argc, char *argv[])
{
    buffer_t *input_buffer = new_buffer_input();
    table_t *table = new_table();
    while (true)
    {
        print_prompt();
        read_input(input_buffer);

        if (input_buffer->buffer[0] == '.')
        {
            switch (do_meta_command(input_buffer))
            {
            case META_COMMAND_SUCCESS:
                continue;
            case META_COMMAND_UNRECOGNIZED_COMMAND:
                printf("Unrecognized meta command '%s'\n", input_buffer->buffer);
                continue;
            }
        }

        statement_t statement;
        switch (prepare_statement(input_buffer, &statement))
        {
        case PREPARE_SUCCESS:
            break;
        case PREPARE_SYNTAX_ERROR:
            printf("Syntax error. Could not parse sql statement.\n");
            continue;
        case PREPARE_UNRECOGNIZED_STATEMENT:
            printf("Unrecognized keyword at start of '%s'.\n", input_buffer->buffer);
            continue;
        }

        switch (execute_statement(&statement, table))
        {
        case EXECUTE_SUCCESS:
            printf("Executed. \n");
            break;
        case EXECUTE_TABLE_FULL:
            printf("Exectue Error: Table full.\n");
            break;
        case EXECUTE_UNKNOWN_STATEMENT:
            printf("Exectue Error: Unknown sql statement '%s' \n", input_buffer->buffer);
            break;
        }
    }
}
