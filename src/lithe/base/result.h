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

#define OK(T, v)                         \
	(T) {                            \
		.fail = false, .ok = (v) \
	}
#define ERR(T, e)                        \
	(T) {                            \
		.fail = true, .err = (e) \
	}

#define try$(T, expr)                             \
	({                                        \
		typeof(expr) _expr = (expr);      \
		if (expr.fail)                    \
			return ERR(T, _expr.err); \
		_expr.ok;                         \
	})

#define expect$(expr, msg)                                                     \
	({                                                                     \
		typeof(expr) _expr = (expr);                                   \
		if (_expr.fail)                                                \
			panic$("unwrap on " #expr " failed: (\"%s\")", (msg)); \
		_expr.ok;                                                      \
	})
#define unwrap$(expr)
#define unwrap_or$(expr, v)                  \
	({                                   \
		typeof(expr) _expr = (expr); \
		typeof(_expr.ok) _res;       \
		if (_expr.fail)              \
			_res = (v);          \
		else                         \
			_res = _expr.ok;     \
		_res;                        \
	})
