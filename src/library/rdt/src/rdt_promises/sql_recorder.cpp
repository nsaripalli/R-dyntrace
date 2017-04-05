//
// Created by nohajc on 29.3.17.
//

#include <cstring>
#include <cstdio>

#include "sql_recorder.h"
#include "tracer_conf.h"

#include "sql_generator.h"
#include "multiplexer.h"

#include <string>

using namespace std;
using namespace sql_generator;

typedef int prom_eval_t;

/* Functions for generating SQL strings. */

sql_stmt_t insert_function_statement(const call_info_t & info) {
    sql_val_t id = from_hex(info.fn_id);
    sql_val_t location = wrap_nullable_string(info.loc);
    sql_val_t definition = wrap_and_escape_nullable_string(info.fn_definition);

    return make_insert_function_statement(id, location, definition);
}

sql_stmt_t insert_arguments_statement(const call_info_t & info, bool align) {
    assert(info.arguments.size() > 0);

    vector<sql_val_cell_t> value_cells;
    sql_val_t function_id = from_hex(info.fn_id);

    int i = 0;
    for (auto argument_ref : info.arguments.all()) {
        const arg_t &argument = argument_ref.get();
        sql_val_t argument_name = wrap_nullable_string(get<0>(argument));
        sql_val_t argument_id = from_int(get<1>(argument));
        sql_val_t index = from_int(i++);

        sql_val_cell_t cell = join({argument_id, argument_name, index, function_id});
        value_cells.push_back(cell);
    }

    return make_insert_arguments_statement(value_cells, align);
}

sql_stmt_t insert_call_statement(const call_info_t & info) {
    sql_val_t id = from_int(info.call_id);
    sql_val_t pointer = from_hex(info.call_ptr); // FIXME do we really need this?
    sql_val_t name = wrap_nullable_string(info.fqfn);
    sql_val_t type = from_int(info.call_type);
    sql_val_t location = wrap_nullable_string(info.loc);
    sql_val_t function_id = from_hex(info.fn_id);

    return make_insert_function_call_statement(id, pointer, name, type, location, function_id);
}

sql_stmt_t insert_promise_statement(const prom_id_t id) {
    return make_insert_promise_statement(from_int(id));
}

sql_stmt_t insert_promise_association_statement(const call_info_t & info, bool align) {
    assert(info.arguments.size() > 0);

    vector<sql_val_cell_t> value_cells;
    sql_val_t call_id = from_int(info.call_id);

    int i = 0;
    for (auto argument_ref : info.arguments.all()) {
        const arg_t &argument = argument_ref.get();
        //sql_val_t argument_name = get<0>(argument);
        sql_val_t argument_id = from_int(get<1>(argument));
        sql_val_t promise_id = from_int(get<2>(argument));
        sql_val_t index = from_int(i++);

        sql_val_cell_t cell = join({promise_id, call_id, argument_id});
        value_cells.push_back(cell);
    }

    return make_insert_promise_associations_statement(value_cells, align);
}

sql_stmt_t insert_promise_evaluation_statement(prom_eval_t type, const prom_info_t & info) {
    //sql_val_t clock = from_int(STATE(clock_id)++);
    sql_val_t clock = next_from_sequence();
    sql_val_t event_type = from_int(type);
    sql_val_t promise_id = from_int(info.prom_id);
    sql_val_t call_id = from_int(info.from_call_id);

    // in_call_id = current call
    // from_call_id = TODO what is it

    return make_insert_promise_evaluation_statement(clock, event_type, promise_id, call_id);
}

/* Functions connecting to the outside world, create SQL and multiplex output. */

void sql_recorder_t::function_entry(const call_info_t & info) {
    bool align_statements = tracer_conf.pretty_print;
    bool need_to_insert = STATE(already_inserted_functions).count(info.fn_id) == 0;

    if (need_to_insert) {
        sql_stmt_t statement = insert_function_statement(info);
        multiplexer::output(
                multiplexer::payload_t(statement),
                tracer_conf.outputs);

        STATE(already_inserted_functions).insert(info.fn_id);
    }

    if (need_to_insert && info.arguments.size() > 0) {
        sql_stmt_t statement = insert_arguments_statement(info, align_statements);
        multiplexer::output(
                multiplexer::payload_t(statement),
                tracer_conf.outputs);
    }

    /* always */ {
        sql_stmt_t statement = insert_call_statement(info);
        multiplexer::output(
                multiplexer::payload_t(statement),
                tracer_conf.outputs);
    }


    if (info.arguments.size() > 0) {
        sql_stmt_t statement = insert_promise_association_statement(info, align_statements);
        multiplexer::output(
                multiplexer::payload_t(statement),
                tracer_conf.outputs);
    }
}

void sql_recorder_t::builtin_entry(const call_info_t & info) {
    bool need_to_insert = STATE(already_inserted_functions).count(info.fn_id) == 0;

    if (need_to_insert) {
        sql_stmt_t statement = insert_function_statement(info);
                multiplexer::output(
                multiplexer::payload_t(statement),
                tracer_conf.outputs);

        STATE(already_inserted_functions).insert(info.fn_id);
    }

    /* always */ {
        sql_stmt_t statement = insert_call_statement(info);
                multiplexer::output(
                multiplexer::payload_t(statement),
                tracer_conf.outputs);
    }

    // We do not handle arguments for built-ins.
}

void sql_recorder_t::force_promise_entry(const prom_info_t & info) {

    sql_stmt_t statement = insert_promise_evaluation_statement(RDT_SQL_FORCE_PROMISE, info);
    multiplexer::output(
            multiplexer::payload_t(statement),
            tracer_conf.outputs);
}

void sql_recorder_t::promise_created(const prom_id_t & prom_id) {
    sql_stmt_t statement = insert_promise_statement(prom_id);
    multiplexer::output(
            multiplexer::payload_t(statement),
            tracer_conf.outputs);
}

void sql_recorder_t::promise_lookup(const prom_info_t & info) {
    sql_stmt_t statement = insert_promise_evaluation_statement(RDT_SQL_LOOKUP_PROMISE, info);
    multiplexer::output(
            multiplexer::payload_t(statement),
            tracer_conf.outputs);
}

void sql_recorder_t::start_trace() { // bool output_configuration
    multiplexer::init(tracer_conf.outputs, tracer_conf.filename, tracer_conf.overwrite);

    if (tracer_conf.include_configuration)
        if (tracer_conf.overwrite) {
            sql_stmt_t statement =
                    make_pragma_statement("synchronous", "off") +
                    make_create_tables_and_views_statement();

            multiplexer::output(
                    multiplexer::payload_t(statement),
                    tracer_conf.outputs);
        }

    /* always */ {
        sql_stmt_t statement = make_begin_transaction_statement();
        multiplexer::output(
                multiplexer::payload_t(statement),
                tracer_conf.outputs);
    }
}

void sql_recorder_t::finish_trace() {
    sql_stmt_t statement = make_commit_transaction_statement();
    multiplexer::output(
            multiplexer::payload_t(statement),
            tracer_conf.outputs);

    multiplexer::close(tracer_conf.outputs);
}