//
// Created by nohajc on 29.3.17.
//

#ifndef R_3_3_1_SQL_RECORDER_H
#define R_3_3_1_SQL_RECORDER_H

#include "recorder.h"

class psql_recorder_t : public recorder_t<psql_recorder_t> {
public:
    void init_recorder();
    void start_trace();
    void finish_trace();
    void function_entry(const closure_info_t &);
    void function_exit(const closure_info_t &) {}
    void builtin_entry(const builtin_info_t &);
    void builtin_exit(const builtin_info_t &) {};
    void force_promise_entry(const prom_info_t &);
    void force_promise_exit(const prom_info_t &) {}
    void promise_created(const prom_basic_info_t &);
    void promise_lookup(const prom_info_t &);
    void unwind(const vector<call_id_t> &) {};
};

#endif //R_3_3_1_SQL_RECORDER_H
