#pragma once

#include <utils/assert/panic.h>
#include <utils/base/defs.h>

#define result_t(OK, ERR)        \
	struct {                 \
		bool fail;       \
		union {          \
			OK ok;   \
			ERR err; \
		};               \
	}

#define OK(T, V) ((T){.fail = false, .ok = (V)})
#define ERR(T, E) ((T){.fail = true, .err = (E)})

#define try$(T, EXPR)                             \
	({                                        \
		typeof(EXPR) _expr = (EXPR);      \
		if (_expr.fail)                   \
			return ERR(T, _expr.err); \
		_expr.ok;                         \
	})

#define expect$(EXPR, MSG)                                                     \
	({                                                                     \
		typeof(EXPR) _expr = (EXPR);                                   \
		if (_expr.fail)                                                \
			panic$("unwrap on " #EXPR " failed: (\"%s\")", (MSG)); \
		_expr.ok;                                                      \
	})

#define unwrap$(EXPR) expect$(EXPR, "no message")
#define unwrap_or$(EXPR, V)                  \
	({                                   \
		typeof(EXPR) _expr = (EXPR); \
		typeof(_expr.ok) _res;       \
		if (_expr.fail)              \
			_res = (V);          \
		else                         \
			_res = _expr.ok;     \
		_res;                        \
	})
