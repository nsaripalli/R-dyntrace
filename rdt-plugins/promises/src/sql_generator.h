#ifndef R_3_3_1_SQL_GENERATOR_H
#define R_3_3_1_SQL_GENERATOR_H

#define RDT_SQL_LOOKUP_PROMISE 0x0
#define RDT_SQL_FORCE_PROMISE 0xF

#include <string>
#include <vector>
//#include <stdarg.h>
#include <initializer_list>

#define RDT_SQL_SCHEMA "rdt-plugins/promises/SQL/schema.sql"

namespace sql_generator {

    typedef std::string sql_stmt_t;
    typedef std::string sql_val_t;
    typedef std::string sql_val_cell_t;

    sql_stmt_t make_insert_function_statement(sql_val_t id, sql_val_t location, sql_val_t definition, sql_val_t type, sql_val_t compiled);
    sql_stmt_t make_insert_function_call_statement(sql_val_t id, sql_val_t ptr, sql_val_t name, sql_val_t location,  sql_val_t function_id, sql_val_t parent_call_id);
    sql_stmt_t make_insert_arguments_statement(std::vector<sql_val_cell_t> & arguments, bool align);

    sql_stmt_t make_insert_promise_statement(sql_val_t id, sql_val_t type);
    sql_stmt_t make_insert_promise_evaluation_statement(sql_val_t clock, sql_val_t event_type, sql_val_t promise_id, sql_val_t from_call_id, sql_val_t in_call_id);
    sql_stmt_t make_insert_promise_associations_statement(std::vector<sql_val_cell_t> & associations, bool align);

    sql_stmt_t make_select_max_argument_id_statement();
    sql_stmt_t make_select_max_promise_id_statement();
    sql_stmt_t make_select_min_promise_id_statement();
    sql_stmt_t make_select_max_promise_evaluation_clock_statement();
    sql_stmt_t make_select_max_call_id_statement();
    sql_stmt_t make_select_max_function_id_statement();
    sql_stmt_t make_select_all_function_ids_and_definitions_statement();
    sql_stmt_t make_select_all_argument_ids_names_and_function_allegiances_statement();
    sql_stmt_t make_select_all_function_ids_statement();
    sql_stmt_t make_select_all_negative_promise_ids_statement();

    sql_stmt_t make_begin_transaction_statement();
    sql_stmt_t make_abort_transaction_statement();
    sql_stmt_t make_commit_transaction_statement();

    sql_stmt_t make_pragma_statement(sql_val_t option, sql_val_t value);

    sql_stmt_t make_create_tables_and_views_statement();
    std::vector<sql_stmt_t> split_into_individual_statements(sql_stmt_t statements);

        sql_val_cell_t join(std::initializer_list<sql_val_t>);

    sql_val_t from_int(int i);
    sql_val_t from_hex(int h);
    sql_val_t from_nullable_string(std::string s);
    sql_val_t wrap_nullable_string(std::string s);
    sql_val_t wrap_and_escape_nullable_string(std::string s);
    sql_val_t from_nullable_cstring(const char * s);

    sql_val_t next_from_sequence();
}

#endif //R_3_3_1_SQL_GENERATOR_H
