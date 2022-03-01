#pragma once

#include <lithe/assert/panic.h>
#include <lithe/base/defs.h>

#define result(Ok, Err)          \
	struct {                 \
		bool fail;       \
		union {          \
			Ok ok;   \
			Err err; \
		};               \
	}

#define OK(T, v) ((T){.fail = false, .ok = (v)})
#define ERR(T, e) ((T){.fail = true, .err = (e)})

#define try$(T, EXPR)                             \
	({                                        \
		typeof(EXPR) _expr = (EXPR);      \
		if (_expr.fail)                   \
			return ERR(T, _expr.err); \
		_expr.ok;                         \
	})

#define expect$(EXPR, msg)                                                    \
	({                                                                    \
		typeof(EXPR) _expr = (EXPR);                                  \
		if (_expr.fail)                                               \
			panic("unwrap on " #EXPR " failed: (\"%s\")", (msg)); \
		_expr.ok;                                                     \
	})

#define unwrap$(EXPR) expect$(EXPR, "no message")
#define unwrap_or$(EXPR, v)                  \
	({                                   \
		typeof(EXPR) _expr = (EXPR); \
		typeof(_expr.ok) _res;       \
		if (_expr.fail)              \
			_res = (v);          \
		else                         \
			_res = _expr.ok;     \
		_res;                        \
	})
