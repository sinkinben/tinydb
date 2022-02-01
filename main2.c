/**
 * @brief This is the REPL program of tinydb, which is based on a sql parser.
 */
#include "sql-parser/sqlparser.h"
#include "common.h"
int main(int argc, char *argv[])
{
    buffer_t *input = new_buffer_input();

    if (argc < 2)
    {
        printf("Must supply a database filename.\n");
        exit(EXIT_FAILURE);
    }
    char *filename = argv[1];
    table_t *table = db_open(filename);
    statement_t *stm = (statement_t *)malloc(sizeof(statement_t));
    execute_result_t result;

    while (1)
    {
        print_prompt();
        read_input(input);

        /* If `input` is a meta command, then it will start with '.' */
        if (input->buffer[0] == '.')
        {
            switch (do_meta_command(input, table))
            {
            case META_COMMAND_SUCCESS:
                continue;
            case META_COMMAND_UNRECOGNIZED_COMMAND:
                printf("Unrecognized meta command '%s'\n", input->buffer);
                continue;
            }
        }

        /* Parse the SQL, and put the result in statement_t `stm` */
        statement_init(stm);
        sql_parser(input->buffer, stm);
        result = vm_executor(stm, table);
        vm_logger(result, stm, input);
        statement_free(stm);
    }
}