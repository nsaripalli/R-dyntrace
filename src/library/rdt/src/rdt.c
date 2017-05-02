#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include <Defn.h>

#include "rdt.h"
#include "dyn_fn_lookup.h"
#include "filter.h"

static rdt_handler *handler = NULL;
void *p_filter = NULL;

static void internal_eval(void *data) {
    void **args = data;
    SEXP block = (SEXP) args[0];
    SEXP rho = (SEXP) args[1];

    eval(block, rho);
}

SEXP Rdt(SEXP filter, SEXP tracer, SEXP rho, SEXP options) {
    if (rdt_is_running()) {
        if (handler) free(handler);

        rdt_stop();
        return R_TrueValue;
    }

    if (!isEnvironment(rho)) {
	    error("'rho' must be an environment not %s", type2char(TYPEOF(rho)));
        return R_FalseValue;
    }

    p_filter = NULL;
    const char *filter_file = get_string(filter);
    if (strlen(filter_file))
    {
        Rprintf("Obtaining filter from '%s' file...\n", filter_file);
        p_filter = get_filter(filter_file);
    }

    const char *sys = get_string(tracer);
    tracer_setup_ptr_t setup_tracing = find_fn_by_name("setup_%s_tracing", sys);

    if (!setup_tracing) {
        error("Tracer %s not found", sys);
        return R_FalseValue;
    }

    handler = setup_tracing(options);
    if (!handler) {
        error("Unable to initialize dynamic tracing");
        return R_FalseValue;
    }

    SEXP block = findVar(install("block"), rho);
    if (block == NULL || block == R_NilValue) {
        error("Unable to find 'block' variable");
        return R_FalseValue;
    }

    rdt_start(handler, block);

    // this is to prevent long jumps return earlier than needed
    void *data[2] = {block, rho};
    R_ToplevelExec(&internal_eval, (void *)data);

    tracer_cleanup_ptr_t cleanup_tracing = find_fn_by_name("cleanup_%s_tracing", sys);
    if (cleanup_tracing) {
        cleanup_tracing(options);
    }
    // Missing cleanup_tracing function is not an error

    if (p_filter)
    {
        Rprintf("Destroying filter...\n");
        destroy_filter(p_filter);
    }

    rdt_stop();
    return R_TrueValue; // TODO Why does this return TRUE and not the return value of the expression anyway?
}
