#ifndef	_RDT_H
#define	_RDT_H

#include <rdtrace.h>

#ifdef __cplusplus
extern "C" {
#endif

SEXP Rdt(SEXP tracer, SEXP rho, SEXP options);
SEXP Rdt_deparse(SEXP call);

rdt_handler *setup_default_tracing(SEXP options);
rdt_handler *setup_noop_tracing(SEXP options);
rdt_handler *setup_promise_tracing(SEXP options);
rdt_handler *setup_debug_tracing(SEXP options);

const char *get_string(SEXP sexp);

#ifdef __cplusplus
}
#endif

#endif /* _RDT_H */