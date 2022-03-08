#pragma once

#include <utils/base/defs.h>
#include <utils/base/result.h>
#include <utils/text/str.h>

typedef enum {
	ERR_KIND_SUCCESS,
	ERR_KIND_OTHER,
	ERR_KIND_PERMS,
	ERR_KIND_FAULT,
	ERR_KIND_INVALID,
	ERR_KIND_UNSUPPORTED,
	ERR_KIND_EXHAUSTION,
} error_kind_t;

typedef struct {
	error_kind_t kind;
	str_t msg;
} error_t;

#define BUILD_ERROR(K, MSG) ((error_t){(K), str$(MSG)})
#define ERR_SUCCESS BUILD_ERROR(ERR_KIND_SUCCESS, "success")
#define ERR_UNDEFINED BUILD_ERROR(ERR_KIND_OTHER, "undefined error")
#define ERR_PERMISSION BUILD_ERROR(ERR_KIND_PERMS, "lacking permission")
#define ERR_BAD_PARAMETERS BUILD_ERROR(ERR_KIND_INVALID, "bad parameters")
#define ERR_BAD_DATA BUILD_ERROR(ERR_KIND_INVALID, "bad data")
#define ERR_BAD_ADDRESS BUILD_ERROR(ERR_KIND_INVALID, "bad address")
#define ERR_OUT_OF_MEMORY BUILD_ERROR(ERR_KIND_EXHAUSTION, "out of memory")

// A type for void returning results
typedef struct {
	uint8_t _pad;
} _success_t;

typedef result_t(_success_t, error_t) maybe_t;

#define SUCCESS OK(maybe_t, (_success_t){})
#define FAILURE(E) ERR(maybe_t, E)
