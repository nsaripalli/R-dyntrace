#ifndef __DYNTRACE_H__
#define __DYNTRACE_H__

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <R.h>

//#include <Rinterface.h>
#define HAVE_DECL_SIZE_MAX 1
#define R_USE_SIGNALS 1
#include <Defn.h>

#include <Rinternals.h>
#include <config.h>
#include <libintl.h>

#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>

#ifdef ENABLE_DYNTRACE

#define DYNTRACE_PROBE_HEADER(probe_name)                                      \
    if (dyntrace_active_dyntracer != NULL &&                                   \
        dyntrace_active_dyntracer->probe_name != NULL &&                       \
        !dyntrace_is_priviliged_mode()) {                                      \
        dyntrace_active_dyntrace_context->dyntracing_context->execution_time   \
            .expression += dyntrace_reset_stopwatch();                         \
        dyntrace_active_dyntrace_context->dyntracing_context->execution_count  \
            .probe_name++;                                                     \
        CHECK_REENTRANCY(probe_name);                                          \
        dyntrace_active_dyntracer_probe_name = #probe_name;                    \
        dyntrace_disable_garbage_collector();

#define DYNTRACE_PROBE_FOOTER(probe_name)                                      \
    dyntrace_reinstate_garbage_collector();                                    \
    dyntrace_active_dyntracer_probe_name = NULL;                               \
    dyntrace_active_dyntrace_context->dyntracing_context->execution_time       \
        .probe_name += dyntrace_reset_stopwatch();                             \
    }

#define CHECK_REENTRANCY(probe_name)                                           \
    if (dyntrace_check_reentrancy) {                                           \
        if (dyntrace_active_dyntracer_probe_name != NULL) {                    \
            dyntrace_log_error("[NESTED HOOK EXECUTION] - %s triggers %s",     \
                               dyntrace_active_dyntracer_probe_name,           \
                               #probe_name);                                   \
        }                                                                      \
    }

#define DYNTRACE_PROBE_BEGIN(prom)                                             \
    DYNTRACE_PROBE_HEADER(probe_begin);                                        \
    PROTECT(prom);                                                             \
    dyntrace_active_dyntracer->probe_begin(dyntrace_active_dyntrace_context,   \
                                           prom);                              \
    UNPROTECT(1);                                                              \
    DYNTRACE_PROBE_FOOTER(probe_begin);

#define DYNTRACE_PROBE_END()                                                   \
    DYNTRACE_PROBE_HEADER(probe_end);                                          \
    dyntrace_active_dyntracer->probe_end(dyntrace_active_dyntrace_context);    \
    DYNTRACE_PROBE_FOOTER(probe_end);

#define DYNTRACE_SHOULD_PROBE(probe_name)                                      \
    (dyntrace_active_dyntracer->probe_name != NULL)

#define DYNTRACE_PROBE_CLOSURE_ENTRY(call, op, rho)                            \
    DYNTRACE_PROBE_HEADER(probe_closure_entry);                                \
    PROTECT(call);                                                             \
    PROTECT(op);                                                               \
    PROTECT(rho);                                                              \
    dyntrace_active_dyntracer->probe_closure_entry(                            \
        dyntrace_active_dyntrace_context, call, op, rho);                      \
    UNPROTECT(3);                                                              \
    DYNTRACE_PROBE_FOOTER(probe_closure_entry);

#define DYNTRACE_PROBE_CLOSURE_EXIT(call, op, rho, retval)                     \
    DYNTRACE_PROBE_HEADER(probe_closure_exit);                                 \
    PROTECT(call);                                                             \
    PROTECT(op);                                                               \
    PROTECT(rho);                                                              \
    PROTECT(retval);                                                           \
    dyntrace_active_dyntracer->probe_closure_exit(                             \
        dyntrace_active_dyntrace_context, call, op, rho, retval);              \
    UNPROTECT(4);                                                              \
    DYNTRACE_PROBE_FOOTER(probe_closure_exit);

#define DYNTRACE_PROBE_BUILTIN_ENTRY(call, op, rho)                            \
    DYNTRACE_PROBE_HEADER(probe_builtin_entry);                                \
    PROTECT(call);                                                             \
    PROTECT(op);                                                               \
    PROTECT(rho);                                                              \
    dyntrace_active_dyntracer->probe_builtin_entry(                            \
        dyntrace_active_dyntrace_context, call, op, rho);                      \
    UNPROTECT(3);                                                              \
    DYNTRACE_PROBE_FOOTER(probe_builtin_entry);

#define DYNTRACE_PROBE_BUILTIN_EXIT(call, op, rho, retval)                     \
    DYNTRACE_PROBE_HEADER(probe_builtin_exit);                                 \
    PROTECT(call);                                                             \
    PROTECT(op);                                                               \
    PROTECT(rho);                                                              \
    PROTECT(retval);                                                           \
    dyntrace_active_dyntracer->probe_builtin_exit(                             \
        dyntrace_active_dyntrace_context, call, op, rho, retval);              \
    UNPROTECT(4);                                                              \
    DYNTRACE_PROBE_FOOTER(probe_builtin_exit);

#define DYNTRACE_PROBE_SPECIAL_ENTRY(call, op, rho)                            \
    DYNTRACE_PROBE_HEADER(probe_special_entry);                                \
    PROTECT(call);                                                             \
    PROTECT(op);                                                               \
    PROTECT(rho);                                                              \
    dyntrace_active_dyntracer->probe_special_entry(                            \
        dyntrace_active_dyntrace_context, call, op, rho);                      \
    UNPROTECT(3);                                                              \
    DYNTRACE_PROBE_FOOTER(probe_special_entry);

#define DYNTRACE_PROBE_SPECIAL_EXIT(call, op, rho, retval)                     \
    DYNTRACE_PROBE_HEADER(probe_special_exit);                                 \
    PROTECT(call);                                                             \
    PROTECT(op);                                                               \
    PROTECT(rho);                                                              \
    PROTECT(retval);                                                           \
    dyntrace_active_dyntracer->probe_special_exit(                             \
        dyntrace_active_dyntrace_context, call, op, rho, retval);              \
    UNPROTECT(4);                                                              \
    DYNTRACE_PROBE_FOOTER(probe_special_exit);

#define DYNTRACE_PROBE_PROMISE_CREATED(prom, rho)                              \
    DYNTRACE_PROBE_HEADER(probe_promise_created);                              \
    PROTECT(prom);                                                             \
    PROTECT(rho);                                                              \
    dyntrace_active_dyntracer->probe_promise_created(                          \
        dyntrace_active_dyntrace_context, prom, rho);                          \
    UNPROTECT(2);                                                              \
    DYNTRACE_PROBE_FOOTER(probe_promise_created);

#define DYNTRACE_PROBE_PROMISE_FORCE_ENTRY(promise)                            \
    DYNTRACE_PROBE_HEADER(probe_promise_force_entry);                          \
    PROTECT(promise);                                                          \
    dyntrace_active_dyntracer->probe_promise_force_entry(                      \
        dyntrace_active_dyntrace_context, promise);                            \
    UNPROTECT(1);                                                              \
    DYNTRACE_PROBE_FOOTER(probe_promise_force_entry);

#define DYNTRACE_PROBE_PROMISE_FORCE_EXIT(promise)                             \
    DYNTRACE_PROBE_HEADER(probe_promise_force_exit);                           \
    PROTECT(promise);                                                          \
    dyntrace_active_dyntracer->probe_promise_force_exit(                       \
        dyntrace_active_dyntrace_context, promise);                            \
    UNPROTECT(1);                                                              \
    DYNTRACE_PROBE_FOOTER(probe_promise_force_exit);

#define DYNTRACE_PROBE_PROMISE_VALUE_LOOKUP(promise, in_force)                 \
    DYNTRACE_PROBE_HEADER(probe_promise_value_lookup);                         \
    PROTECT(promise);                                                          \
    dyntrace_active_dyntracer->probe_promise_value_lookup(                     \
        dyntrace_active_dyntrace_context, promise, in_force);                  \
    UNPROTECT(1);                                                              \
    DYNTRACE_PROBE_FOOTER(probe_promise_value_lookup);

#define DYNTRACE_PROBE_PROMISE_EXPRESSION_LOOKUP(promise, in_force)            \
    DYNTRACE_PROBE_HEADER(probe_promise_expression_lookup);                    \
    PROTECT(promise);                                                          \
    dyntrace_active_dyntracer->probe_promise_expression_lookup(                \
        dyntrace_active_dyntrace_context, promise, in_force);                  \
    UNPROTECT(1);                                                              \
    DYNTRACE_PROBE_FOOTER(probe_promise_expression_lookup);

#define DYNTRACE_PROBE_PROMISE_ENVIRONMENT_LOOKUP(promise, in_force)           \
    DYNTRACE_PROBE_HEADER(probe_promise_environment_lookup);                   \
    PROTECT(promise);                                                          \
    dyntrace_active_dyntracer->probe_promise_environment_lookup(               \
        dyntrace_active_dyntrace_context, promise, in_force);                  \
    UNPROTECT(1);                                                              \
    DYNTRACE_PROBE_FOOTER(probe_promise_environment_lookup);

#define DYNTRACE_PROBE_PROMISE_VALUE_SET(promise, in_force)                    \
    DYNTRACE_PROBE_HEADER(probe_promise_value_lookup);                         \
    PROTECT(promise);                                                          \
    dyntrace_active_dyntracer->probe_promise_value_set(                        \
        dyntrace_active_dyntrace_context, promise, in_force);                  \
    UNPROTECT(1);                                                              \
    DYNTRACE_PROBE_FOOTER(probe_promise_value_lookup);

#define DYNTRACE_PROBE_PROMISE_EXPRESSION_SET(promise, in_force)               \
    DYNTRACE_PROBE_HEADER(probe_promise_expression_lookup);                    \
    PROTECT(promise);                                                          \
    dyntrace_active_dyntracer->probe_promise_expression_set(                   \
        dyntrace_active_dyntrace_context, promise, in_force);                  \
    UNPROTECT(1);                                                              \
    DYNTRACE_PROBE_FOOTER(probe_promise_expression_lookup);

#define DYNTRACE_PROBE_PROMISE_ENVIRONMENT_SET(promise, in_force)              \
    DYNTRACE_PROBE_HEADER(probe_promise_environment_lookup);                   \
    PROTECT(promise);                                                          \
    dyntrace_active_dyntracer->probe_promise_environment_set(                  \
        dyntrace_active_dyntrace_context, promise, in_force);                  \
    UNPROTECT(1);                                                              \
    DYNTRACE_PROBE_FOOTER(probe_promise_environment_lookup);

#define DYNTRACE_PROBE_ERROR(call, message)                                    \
    DYNTRACE_PROBE_HEADER(probe_error);                                        \
    PROTECT(call);                                                             \
    dyntrace_active_dyntracer->probe_error(dyntrace_active_dyntrace_context,   \
                                           call, message);                     \
    UNPROTECT(1);                                                              \
    DYNTRACE_PROBE_FOOTER(probe_error);

#define DYNTRACE_PROBE_VECTOR_ALLOC(sexptype, length, bytes, srcref)           \
    DYNTRACE_PROBE_HEADER(probe_vector_alloc);                                 \
    dyntrace_active_dyntracer->probe_vector_alloc(                             \
        dyntrace_active_dyntrace_context, sexptype, length, bytes, srcref);    \
    DYNTRACE_PROBE_FOOTER(probe_vector_alloc);

#define DYNTRACE_PROBE_EVAL_ENTRY(e, rho)                                      \
    DYNTRACE_PROBE_HEADER(probe_eval_entry);                                   \
    PROTECT(e);                                                                \
    PROTECT(rho);                                                              \
    dyntrace_active_dyntracer->probe_eval_entry(                               \
        dyntrace_active_dyntrace_context, e, rho);                             \
    UNPROTECT(2);                                                              \
    DYNTRACE_PROBE_FOOTER(probe_eval_entry);

#define DYNTRACE_PROBE_EVAL_EXIT(e, rho, retval)                               \
    DYNTRACE_PROBE_HEADER(probe_eval_exit);                                    \
    PROTECT(e);                                                                \
    PROTECT(rho);                                                              \
    PROTECT(retval);                                                           \
    dyntrace_active_dyntracer->probe_eval_exit(                                \
        dyntrace_active_dyntrace_context, e, rho, retval);                     \
    UNPROTECT(3);                                                              \
    DYNTRACE_PROBE_FOOTER(probe_eval_exit);

#define DYNTRACE_PROBE_GC_ENTRY(size_needed)                                   \
    DYNTRACE_PROBE_HEADER(probe_gc_entry);                                     \
    dyntrace_active_dyntracer->probe_gc_entry(                                 \
        dyntrace_active_dyntrace_context, size_needed);                        \
    DYNTRACE_PROBE_FOOTER(probe_gc_entry);

#define DYNTRACE_PROBE_GC_EXIT(gc_count)                                       \
    DYNTRACE_PROBE_HEADER(probe_gc_exit);                                      \
    dyntrace_active_dyntracer->probe_gc_exit(dyntrace_active_dyntrace_context, \
                                             gc_count);                        \
    DYNTRACE_PROBE_FOOTER(probe_gc_exit);

#define DYNTRACE_PROBE_ALLOCATE(object)                                        \
    DYNTRACE_PROBE_HEADER(probe_allocate);                                     \
    PROTECT(object);                                                           \
    dyntrace_active_dyntracer->probe_allocate(                                 \
        dyntrace_active_dyntrace_context, object);                             \
    UNPROTECT(1);                                                              \
    DYNTRACE_PROBE_FOOTER(probe_allocate);

#define DYNTRACE_PROBE_GC_UNMARK(object)                                       \
    DYNTRACE_PROBE_HEADER(probe_gc_unmark);                                    \
    PROTECT(object);                                                           \
    dyntrace_active_dyntracer->probe_gc_unmark(                                \
        dyntrace_active_dyntrace_context, object);                             \
    UNPROTECT(1);                                                              \
    DYNTRACE_PROBE_FOOTER(probe_gc_unmark);

#define DYNTRACE_PROBE_CONTEXT_BEGIN(cptr)                                     \
    DYNTRACE_PROBE_HEADER(probe_context_begin);                                \
    dyntrace_active_dyntracer->probe_context_begin(                            \
        dyntrace_active_dyntrace_context, cptr);                               \
    DYNTRACE_PROBE_FOOTER(probe_context_begin);

#define DYNTRACE_PROBE_CONTEXT_JUMP(cptr, val, restart)                        \
    DYNTRACE_PROBE_HEADER(probe_context_jump);                                 \
    PROTECT(val);                                                              \
    dyntrace_active_dyntracer->probe_context_jump(                             \
        dyntrace_active_dyntrace_context, cptr, val, restart);                 \
    UNPROTECT(1);                                                              \
    DYNTRACE_PROBE_FOOTER(probe_context_jump);

#define DYNTRACE_PROBE_CONTEXT_END(cptr)                                       \
    DYNTRACE_PROBE_HEADER(probe_context_end);                                  \
    dyntrace_active_dyntracer->probe_context_end(                              \
        dyntrace_active_dyntrace_context, cptr);                               \
    DYNTRACE_PROBE_FOOTER(probe_context_end);

#define DYNTRACE_PROBE_NEW_ENVIRONMENT(rho)                                    \
    DYNTRACE_PROBE_HEADER(probe_new_environment);                              \
    PROTECT(rho);                                                              \
    dyntrace_active_dyntracer->probe_new_environment(                          \
        dyntrace_active_dyntrace_context, rho);                                \
    UNPROTECT(1);                                                              \
    DYNTRACE_PROBE_FOOTER(probe_new_environment);

#define DYNTRACE_PROBE_S3_GENERIC_ENTRY(generic, object)                       \
    DYNTRACE_PROBE_HEADER(probe_S3_generic_entry);                             \
    PROTECT(object);                                                           \
    dyntrace_active_dyntracer->probe_S3_generic_entry(                         \
        dyntrace_active_dyntrace_context, generic, object);                    \
    UNPROTECT(1);                                                              \
    DYNTRACE_PROBE_FOOTER(probe_S3_generic_entry);

#define DYNTRACE_PROBE_S3_GENERIC_EXIT(generic, object, retval)                \
    DYNTRACE_PROBE_HEADER(probe_S3_generic_exit);                              \
    PROTECT(object);                                                           \
    PROTECT(retval);                                                           \
    dyntrace_active_dyntracer->probe_S3_generic_exit(                          \
        dyntrace_active_dyntrace_context, generic, object, retval);            \
    UNPROTECT(2);                                                              \
    DYNTRACE_PROBE_FOOTER(probe_S3_generic_exit);

#define DYNTRACE_PROBE_S3_DISPATCH_ENTRY(generic, clazz, method, object)       \
    DYNTRACE_PROBE_HEADER(probe_S3_dispatch_entry);                            \
    PROTECT(method);                                                           \
    PROTECT(object);                                                           \
    dyntrace_active_dyntracer->probe_S3_dispatch_entry(                        \
        dyntrace_active_dyntrace_context, generic, clazz, method, object);     \
    UNPROTECT(2);                                                              \
    DYNTRACE_PROBE_FOOTER(probe_S3_dispatch_entry);

#define DYNTRACE_PROBE_S3_DISPATCH_EXIT(generic, clazz, method, object,        \
                                        retval)                                \
    DYNTRACE_PROBE_HEADER(probe_S3_dispatch_exit);                             \
    PROTECT(method);                                                           \
    PROTECT(object);                                                           \
    PROTECT(retval);                                                           \
    dyntrace_active_dyntracer->probe_S3_dispatch_exit(                         \
        dyntrace_active_dyntrace_context, generic, clazz, method, object,      \
        retval);                                                               \
    UNPROTECT(3);                                                              \
    DYNTRACE_PROBE_FOOTER(probe_S3_dispatch_exit);

#define DYNTRACE_PROBE_ENVIRONMENT_DEFINE_VAR(symbol, value, rho)              \
    DYNTRACE_PROBE_HEADER(probe_environment_define_var);                       \
    PROTECT(symbol);                                                           \
    PROTECT(value);                                                            \
    PROTECT(rho);                                                              \
    dyntrace_active_dyntracer->probe_environment_define_var(                   \
        dyntrace_active_dyntrace_context, symbol, value, rho);                 \
    UNPROTECT(3);                                                              \
    DYNTRACE_PROBE_FOOTER(probe_environment_define_var);

#define DYNTRACE_PROBE_ENVIRONMENT_ASSIGN_VAR(symbol, value, rho)              \
    DYNTRACE_PROBE_HEADER(probe_environment_assign_var);                       \
    PROTECT(symbol);                                                           \
    PROTECT(value);                                                            \
    PROTECT(rho);                                                              \
    dyntrace_active_dyntracer->probe_environment_assign_var(                   \
        dyntrace_active_dyntrace_context, symbol, value, rho);                 \
    UNPROTECT(3);                                                              \
    DYNTRACE_PROBE_FOOTER(probe_environment_assign_var);

#define DYNTRACE_PROBE_ENVIRONMENT_REMOVE_VAR(symbol, rho)                     \
    DYNTRACE_PROBE_HEADER(probe_environment_remove_var);                       \
    PROTECT(symbol);                                                           \
    PROTECT(rho);                                                              \
    dyntrace_active_dyntracer->probe_environment_remove_var(                   \
        dyntrace_active_dyntrace_context, symbol, rho);                        \
    UNPROTECT(2);                                                              \
    DYNTRACE_PROBE_FOOTER(probe_environment_remove_var);

#define DYNTRACE_PROBE_ENVIRONMENT_LOOKUP_VAR(symbol, value, rho)              \
    DYNTRACE_PROBE_HEADER(probe_environment_lookup_var);                       \
    PROTECT(symbol);                                                           \
    PROTECT(value);                                                            \
    PROTECT(rho);                                                              \
    dyntrace_active_dyntracer->probe_environment_lookup_var(                   \
        dyntrace_active_dyntrace_context, symbol, value, rho);                 \
    UNPROTECT(3);                                                              \
    DYNTRACE_PROBE_FOOTER(probe_environment_lookup_var);

#else
#define DYNTRACE_PROBE_BEGIN(prom)
#define DYNTRACE_PROBE_END()
#define DYNTRACE_PROBE_CLOSURE_ENTRY(call, op, rho)
#define DYNTRACE_PROBE_CLOSURE_EXIT(call, op, rho, retval)
#define DYNTRACE_PROBE_BUILTIN_ENTRY(call, op, rho)
#define DYNTRACE_PROBE_BUILTIN_EXIT(call, op, rho, retval)
#define DYNTRACE_PROBE_SPECIAL_ENTRY(call, op, rho)
#define DYNTRACE_PROBE_SPECIAL_EXIT(call, op, rho, retval)
#define DYNTRACE_PROBE_PROMISE_CREATED(prom, rho)
#define DYNTRACE_PROBE_PROMISE_FORCE_ENTRY(promise)
#define DYNTRACE_PROBE_PROMISE_FORCE_EXIT(promise)
#define DYNTRACE_PROBE_PROMISE_VALUE_LOOKUP(promise)
#define DYNTRACE_PROBE_PROMISE_EXPRESSION_LOOKUP(promise)
#define DYNTRACE_PROBE_PROMISE_ENVIRONMENT_LOOKUP(promise)
#define DYNTRACE_PROBE_ERROR(call, message)
#define DYNTRACE_PROBE_VECTOR_ALLOC(sexptype, length, bytes, scref)
#define DYNTRACE_PROBE_EVAL_ENTRY(e, rho)
#define DYNTRACE_PROBE_EVAL_EXIT(e, rho, retval)
#define DYNTRACE_PROBE_GC_ENTRY(size_needed)
#define DYNTRACE_PROBE_GC_EXIT(gc_count)
#define DYNTRACE_PROBE_GC_UNMARK(object)
#define DYNTRACE_PROBE_GC_ALLOCATE(object)
#define DYNTRACE_PROBE_CONTEXT_BEGIN(cptr)
#define DYNTRACE_PROBE_CONTEXT_END(cptr)
#define DYNTRACE_PROBE_CONTEXT_JUMP(cptr, val, restart)
#define DYNTRACE_PROBE_NEW_ENVIRONMENT(rho)
#define DYNTRACE_PROBE_S3_GENERIC_ENTRY(generic, object)
#define DYNTRACE_PROBE_S3_GENERIC_EXIT(generic, object, retval)
#define DYNTRACE_PROBE_S3_DISPATCH_ENTRY(generic, clazz, method, object)
#define DYNTRACE_PROBE_S3_DISPATCH_EXIT(generic, clazz, method, object, retval)
#define DYNTRACE_PROBE_ENVIRONMENT_DEFINE_VAR(symbol, value, rho)
#define DYNTRACE_PROBE_ENVIRONMENT_ASSIGN_VAR(symbol, value, rho)
#define DYNTRACE_PROBE_ENVIRONMENT_REMOVE_VAR(symbol, rho)
#define DYNTRACE_PROBE_ENVIRONMENT_LOOKUP_VAR(symbol, value, rho)
#endif

/* ----------------------------------------------------------------------------
   DYNTRACE TYPE DEFINITIONS
---------------------------------------------------------------------------- */

typedef struct {
    clock_t probe_begin;
    clock_t probe_end;
    clock_t probe_closure_entry;
    clock_t probe_closure_exit;
    clock_t probe_builtin_entry;
    clock_t probe_builtin_exit;
    clock_t probe_special_entry;
    clock_t probe_special_exit;
    clock_t probe_promise_created;
    clock_t probe_promise_force_entry;
    clock_t probe_promise_force_exit;
    clock_t probe_promise_value_lookup;
    clock_t probe_promise_expression_lookup;
    clock_t probe_promise_environment_lookup;
    clock_t probe_error;
    clock_t probe_vector_alloc;
    clock_t probe_eval_entry;
    clock_t probe_eval_exit;
    clock_t probe_gc_entry;
    clock_t probe_gc_exit;
    clock_t probe_gc_promise_unmarked;
    clock_t probe_context_jump;
    clock_t probe_context_begin;
    clock_t probe_context_end;
    clock_t probe_new_environment;
    clock_t probe_S3_generic_entry;
    clock_t probe_S3_generic_exit;
    clock_t probe_S3_dispatch_entry;
    clock_t probe_S3_dispatch_exit;
    clock_t probe_environment_define_var;
    clock_t probe_environment_assign_var;
    clock_t probe_environment_remove_var;
    clock_t probe_environment_lookup_var;
    clock_t expression;
} execution_time_t;

typedef struct {
    unsigned int probe_begin;
    unsigned int probe_end;
    unsigned int probe_closure_entry;
    unsigned int probe_closure_exit;
    unsigned int probe_builtin_entry;
    unsigned int probe_builtin_exit;
    unsigned int probe_special_entry;
    unsigned int probe_special_exit;
    unsigned int probe_promise_created;
    unsigned int probe_promise_force_entry;
    unsigned int probe_promise_force_exit;
    unsigned int probe_promise_value_lookup;
    unsigned int probe_promise_expression_lookup;
    unsigned int probe_promise_environment_lookup;
    unsigned int probe_error;
    unsigned int probe_vector_alloc;
    unsigned int probe_eval_entry;
    unsigned int probe_eval_exit;
    unsigned int probe_gc_entry;
    unsigned int probe_gc_exit;
    unsigned int probe_gc_promise_unmarked;
    unsigned int probe_context_jump;
    unsigned int probe_context_begin;
    unsigned int probe_context_end;
    unsigned int probe_new_environment;
    unsigned int probe_S3_generic_entry;
    unsigned int probe_S3_generic_exit;
    unsigned int probe_S3_dispatch_entry;
    unsigned int probe_S3_dispatch_exit;
    unsigned int probe_environment_define_var;
    unsigned int probe_environment_assign_var;
    unsigned int probe_environment_remove_var;
    unsigned int probe_environment_lookup_var;
    unsigned int expression;
} execution_count_t;

typedef struct {
    const char *r_compile_pkgs;
    const char *r_disable_bytecode;
    const char *r_enable_jit;
    const char *r_keep_pkg_source;
} environment_variables_t;

typedef struct {
    execution_time_t execution_time;
    execution_count_t execution_count;
    environment_variables_t environment_variables;
    const char *begin_datetime;
    const char *end_datetime;
} dyntracing_context_t;

typedef struct {
    void *dyntracer_context;
    dyntracing_context_t *dyntracing_context;
} dyntrace_context_t;

typedef struct {

    /***************************************************************************
    Fires when the tracer starts. Not an interpreter hook.
    Look for DYNTRACE_PROBE_BEGIN(...) in
    - src/main/dyntrace.c
    ***************************************************************************/
    void (*probe_begin)(dyntrace_context_t *dyntrace_context, const SEXP prom);

    /***************************************************************************
    Fires when the tracer ends (after the traced call returns).
    Not an interpreter hook.
    Look for DYNTRACE_PROBE_END(...) in
    - src/main/dyntrace.c
    ***************************************************************************/
    void (*probe_end)(dyntrace_context_t *dyntrace_context);

    /***************************************************************************
    Fires on every R function call.
    Look for DYNTRACE_PROBE_CLOSURE_ENTRY(...) in
    - src/main/eval.c
    ***************************************************************************/
    void (*probe_closure_entry)(dyntrace_context_t *dyntrace_context,
                                const SEXP call, const SEXP op,
                                const SEXP rho);

    /***************************************************************************
    Fires after every R function call.
    Look for DYNTRACE_PROBE_CLOSURE_EXIT(...) in
    - src/main/eval.c
    ***************************************************************************/
    void (*probe_closure_exit)(dyntrace_context_t *dyntrace_context,
                               const SEXP call, const SEXP op, const SEXP rho,
                               const SEXP retval);

    /***************************************************************************
    Fires on every BUILTINSXP call.
    Look for DYNTRACE_PROBE_BUILTIN_ENTRY(...) in
    - src/main/eval.c
    ***************************************************************************/
    void (*probe_builtin_entry)(dyntrace_context_t *dyntrace_context,
                                const SEXP call, const SEXP op, const SEXP rho);

    /***************************************************************************
    Fires after every BUILTINSXP call.
    Look for DYNTRACE_PROBE_BUILTIN_EXIT(...) in
    - src/main/eval.c
    ***************************************************************************/
    void (*probe_builtin_exit)(dyntrace_context_t *dyntrace_context,
                               const SEXP call, const SEXP op, const SEXP rho,
                               const SEXP retval);

    /***************************************************************************
    Fires on every SPECIALSXP call.
    Look for DYNTRACE_PROBE_SPECIAL_ENTRY(...) in
    - src/main/eval.c
    ***************************************************************************/
    void (*probe_special_entry)(dyntrace_context_t *dyntrace_context,
                                const SEXP call, const SEXP op,
                                const SEXP rho);

    /***************************************************************************
    Fires after every SPECIALSXP call.
    Look for DYNTRACE_PROBE_SPECIAL_EXIT(...) in
    - in src/main/eval.c
    ***************************************************************************/
    void (*probe_special_exit)(dyntrace_context_t *dyntrace_context,
                               const SEXP call, const SEXP op,
                               const SEXP rho, const SEXP retval);

    /***************************************************************************
    Fires on promise allocation.
    Look for DYNTRACE_PROBE_PROMISE_CREATED(...) in
    - src/main/memory.c
    ***************************************************************************/
    void (*probe_promise_created)(dyntrace_context_t *dyntrace_context,
                                  const SEXP prom, const SEXP rho);

    /***************************************************************************
    Fires when a promise is forced (accessed for the first time).
    Look for DYNTRACE_PROBE_PROMISE_FORCE_ENTRY(...) in
    - src/main/eval.c
    ***************************************************************************/
    void (*probe_promise_force_entry)(dyntrace_context_t *dyntrace_context,
                                      const SEXP promise);

    /***************************************************************************
    Fires right after a promise is forced.
    Look for DYNTRACE_PROBE_PROMISE_FORCE_EXIT(...) in
    - src/main/eval.c
    ***************************************************************************/
    void (*probe_promise_force_exit)(dyntrace_context_t *dyntrace_context,
                                     const SEXP promise);

    /***************************************************************************
    Fires when a promise is accessed but already forced.
    Look for DYNTRACE_PROBE_PROMISE_VALUE_LOOKUP(...) in
    - src/main/eval.c
    ***************************************************************************/
    void (*probe_promise_value_lookup)(dyntrace_context_t *dyntrace_context,
                                       const SEXP promise, int in_force);

    /***************************************************************************
    Fires when the expression inside a promise is accessed.
    Look for DYNTRACE_PROBE_PROMISE_EXPRESSION_LOOKUP(...) in
    - src/main/coerce.c
    ***************************************************************************/
    void (*probe_promise_expression_lookup)(
        dyntrace_context_t *dyntrace_context, const SEXP promise, int in_force);

    /***************************************************************************
      Fires when the environment inside a promise is accessed.
      Look for DYNTRACE_PROBE_PROMISE_ENVIRONMENT_LOOKUP(...) in
      - src/main/eval.c
    ***************************************************************************/
    void (*probe_promise_environment_lookup)(
        dyntrace_context_t *dyntrace_context, const SEXP promise, int in_force);

    void (*probe_promise_value_set)(dyntrace_context_t *dyntrace_context,
                                       const SEXP promise, int in_force);
    void (*probe_promise_expression_set)(
            dyntrace_context_t *dyntrace_context, const SEXP promise, int in_force);
    void (*probe_promise_environment_set)(
            dyntrace_context_t *dyntrace_context, const SEXP promise, int in_force);

    /***************************************************************************
    Look for DYNTRACE_PROBE_ERROR(...) in
    - src/main/errors.c
    ***************************************************************************/
    void (*probe_error)(dyntrace_context_t *dyntrace_context, const SEXP call,
                        const char *message);

    /***************************************************************************
    Look for DYNTRACE_PROBE_VECTOR_ALLOC(...) in
    - src/main/memory.c
    ***************************************************************************/
    void (*probe_vector_alloc)(dyntrace_context_t *dyntrace_context,
                               int sexptype, long length, long bytes,
                               const char *srcref);

    /***************************************************************************
    Look for DYNTRACE_PROBE_EVAL_ENTRY(...) in
    - src/main/eval.c
    ***************************************************************************/
    void (*probe_eval_entry)(dyntrace_context_t *dyntrace_context, SEXP e,
                             SEXP rho);

    /***************************************************************************
    Look for DYNTRACE_PROBE_EVAL_EXIT(...) in
    - src/main/eval.c
    ***************************************************************************/
    void (*probe_eval_exit)(dyntrace_context_t *dyntrace_context, SEXP e,
                            SEXP rho, SEXP retval);

    /***************************************************************************
    Look for DYNTRACE_PROBE_GC_ENTRY(...) in
    - src/main/memory.c
    ***************************************************************************/
    void (*probe_gc_entry)(dyntrace_context_t *dyntrace_context,
                           R_size_t size_needed);

    /***************************************************************************
    Look for DYNTRACE_PROBE_GC_EXIT(...) in
    - src/main/memory.c
    ***************************************************************************/
    void (*probe_gc_exit)(dyntrace_context_t *dyntrace_context, int gc_count);

    /***************************************************************************
    Fires when an object gets garbage collected
    Look for DYNTRACE_PROBE_GC_UNMARK(...) in
    - src/main/memory.c
    ***************************************************************************/
    void (*probe_gc_unmark)(dyntrace_context_t *dyntrace_context, const SEXP object);

    /***************************************************************************
    Fires when an object gets allocated
    Look for DYNTRACE_PROBE_ALLOCATE(...) in
    - src/main/memory.c
    ***************************************************************************/
    void (*probe_allocate)(dyntrace_context_t *dyntrace_context, const SEXP object);


    /***************************************************************************
    Fires when the interpreter is about to longjump into a different context.
    Parameter rho is the target environment.
    Look for DYNTRACE_PROBE_CONTEXT_JUMP(...) in
    - src/main/context.c
    ***************************************************************************/
    void (*probe_context_jump)(dyntrace_context_t *dyntrace_context,
                               const RCNTXT *context, const SEXP return_value,
                               int restart);

    /***************************************************************************
    Fires when the interpreter creates a new context.
    Look for DYNTRACE_PROBE_CONTEXT_BEGIN(...) in
    - src/main/context.c
    ***************************************************************************/
    void (*probe_context_begin)(dyntrace_context_t *dyntrace_context,
                                const RCNTXT *context);

    /***************************************************************************
    Fires when the interpreter discards a context.
    Look for DYNTRACE_PROBE_CONTEXT_END(...) in
    - src/main/context.c
    ***************************************************************************/
    void (*probe_context_end)(dyntrace_context_t *dyntrace_context,
                              const RCNTXT *context);

    /***************************************************************************
    Fires when the interpreter creates a new environment.
    Parameter rho is the newly created environment.
    Look for DYNTRACE_PROBE_NEW_ENVIRONMENT(..) in
    - src/main/memory.c
    ***************************************************************************/
    void (*probe_new_environment)(dyntrace_context_t *dyntrace_context,
                                  const SEXP rho);

    /***************************************************************************
    Look for DYNTRACE_PROBE_S3_GENERIC_ENTRY(...) in
    - src/main/objects.c
    ***************************************************************************/
    void (*probe_S3_generic_entry)(dyntrace_context_t *dyntrace_context,
                                   const char *generic, const SEXP object);

    /***************************************************************************
    Look for DYNTRACE_PROBE_S3_GENERIC_EXIT(...) in
    - src/main/objects.c
    ***************************************************************************/
    void (*probe_S3_generic_exit)(dyntrace_context_t *dyntrace_context,
                                  const char *generic, const SEXP object,
                                  const SEXP retval);

    /***************************************************************************
    Look for DYNTRACE_PROBE_S3_DISPATCH_ENTRY(...) in
    - src/main/objects.c
    ***************************************************************************/
    void (*probe_S3_dispatch_entry)(dyntrace_context_t *dyntrace_context,
                                    const char *generic, const char *clazz,
                                    const SEXP method, const SEXP object);

    /***************************************************************************
    Look for DYNTRACE_PROBE_S3_DISPATCH_EXIT(...) in
    - src/main/objects.c
    ***************************************************************************/
    void (*probe_S3_dispatch_exit)(dyntrace_context_t *dyntrace_context,
                                   const char *generic, const char *clazz,
                                   const SEXP method, const SEXP object,
                                   const SEXP retval);

    /***************************************************************************
    Look for DYNTRACE_PROBE_ENVIRONMENT_DEFINE_VAR(...) in
    - src/main/envir.c
    ***************************************************************************/
    void (*probe_environment_define_var)(dyntrace_context_t *dyntrace_context,
                                         SEXP symbol, SEXP value, SEXP rho);

    /***************************************************************************
    Look for DYNTRACE_PROBE_ENVIRONMENT_ASSIGN_VAR(...) in
    - src/main/envir.c
    ***************************************************************************/
    void (*probe_environment_assign_var)(dyntrace_context_t *dyntrace_context,
                                         SEXP symbol, SEXP value, SEXP rho);

    /***************************************************************************
    Look for DYNTRACE_PROBE_ENVIRONMENT_REMOVE_VAR(...) in
    - src/main/envir.c
    ***************************************************************************/
    void (*probe_environment_remove_var)(dyntrace_context_t *dyntrace_context,
                                         SEXP symbol, SEXP rho);

    /***************************************************************************
    Look for DYNTRACE_PROBE_ENVIRONMENT_LOOKUP_VAR(...) in
    - src/main/envir.c
    ***************************************************************************/
    void (*probe_environment_lookup_var)(dyntrace_context_t *dyntrace_context,
                                         SEXP symbol, SEXP value, SEXP rho);
    void *context;
} dyntracer_t;

// ----------------------------------------------------------------------------
// STATE VARIABLES - For Internal Use Only
// ----------------------------------------------------------------------------

// the current dyntracer
extern dyntracer_t *dyntrace_active_dyntracer;
// flag to disable reentrancy check
extern int dyntrace_check_reentrancy;
// name of currently executing probe
extern const char *dyntrace_active_dyntracer_probe_name;
// state of garbage collector before the hook is triggered
extern int dyntrace_garbage_collector_state;
// context of dyntrace
extern dyntrace_context_t *dyntrace_active_dyntrace_context;
// stopwatch for measuring execution time
extern clock_t dyntrace_stopwatch;
// flag for checking if we are in privileged mode
extern int dyntrace_privileged_mode_flag;

SEXP do_dyntrace(SEXP call, SEXP op, SEXP args, SEXP rho);
int dyntrace_is_active();
int dyntrace_should_probe();
int dyntrace_dyntracer_is_active();
int dyntrace_dyntracer_probe_is_active();
void dyntrace_disable_garbage_collector();
void dyntrace_reinstate_garbage_collector();
void dyntrace_enable_privileged_mode();
void dyntrace_disable_privileged_mode();
int dyntrace_is_priviliged_mode();
clock_t dyntrace_reset_stopwatch();
dyntracer_t *dyntracer_from_sexp(SEXP dyntracer_sexp);
SEXP dyntracer_to_sexp(dyntracer_t *dyntracer, const char *classname);
dyntracer_t *dyntracer_replace_sexp(SEXP dyntracer_sexp,
                                    dyntracer_t *new_dyntracer);
SEXP dyntracer_destroy_sexp(SEXP dyntracer_sexp,
                            void (*destroy_dyntracer)(dyntracer_t *dyntracer));
int findOp(void *addr);
// ----------------------------------------------------------------------------
// helpers
// ----------------------------------------------------------------------------

#define CHKSTR(s) ((s) == NULL ? "<unknown>" : (s))
SEXP get_named_list_element(const SEXP list, const char *name);
char *serialize_sexp(SEXP s);
const char *get_string(SEXP sexp);
int newhashpjw(const char *s);
void NORET jump_to_top_ex(Rboolean, Rboolean, Rboolean, Rboolean, Rboolean);

#define dyntrace_log_error(error, ...)                                         \
    do { Rprintf("DYNTRACE LOG - ERROR - %s · %s(...) · %d - " error "\n", __FILE__, __func__, __LINE__, ##__VA_ARGS__); \
        dyntrace_active_dyntracer_probe_name = NULL;                           \
        jump_to_top_ex(TRUE, TRUE, TRUE, TRUE, FALSE);                         \
    } while (0);

#define dyntrace_log_warning(warning, ...)                                     \
    Rprintf("DYNTRACE LOG - WARNING - %s · %s(...) · %d - " warning "\n", __FILE__, __func__, __LINE__, ##__VA_ARGS__);

#define dyntrace_log_info(info, ...)                                           \
    Rprintf("DYNTRACE LOG - INFO - %s · %s(...) · %d - " info "\n", __FILE__, __func__, __LINE__, ##__VA_ARGS__);

#ifdef __cplusplus
}
#endif

#endif /* __DYNTRACE_H__ */
