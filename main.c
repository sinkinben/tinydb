#include "common.h"
#include "dummyparser.h"
#include "transaction.h"
int main(int argc, char *argv[])
{
    buffer_t *input_buffer = new_buffer_input();
    if (argc < 2)
    {
        printf("Must supply a database filename.\n");
        exit(EXIT_FAILURE);
    }
    char *filename = argv[1];
    table_t *table = db_open(filename);
    // print_help();
    while (true)
    {
        print_prompt();
        read_input(input_buffer);

        if (input_buffer->buffer[0] == '.')
        {
            switch (do_meta_command(input_buffer, table))
            {
            case META_COMMAND_SUCCESS:
                continue;
            case META_COMMAND_UNRECOGNIZED_COMMAND:
                printf("Unrecognized meta command '%s'\n", input_buffer->buffer);
                continue;
            }
        }

        statement_t statement = {
            .conditions = NULL,
            .schemas = NULL,
            .table_name = filename
        };
        switch (parse_statement(input_buffer, &statement))
        {
        case PARSE_SUCCESS:
            break;
        case PARSE_SYNTAX_ERROR:
            printf("Syntax error. Could not parse '%s' sql statement.\n", input_buffer->buffer);
            continue;
        case PARSE_STRING_TOO_LONG:
            printf("String is too long.\n");
            continue;
        case PARSE_UNIMPLEMENTED:
            printf("SQL statement '%s' is not implemented.\n", strtok(input_buffer->buffer, " "));
            continue;
        case PARSE_UNRECOGNIZED_STATEMENT:
            printf("Unrecognized keyword at start of '%s'.\n", input_buffer->buffer);
            continue;
        }

        execute_result_t result = vm_executor(&statement, table);
        vm_logger(result, &statement, input_buffer);
    }
}
