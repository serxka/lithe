#pragma once

#include <lithe/base/defs.h>
#include <lithe/base/result.h>
#include <lithe/text/str.h>

typedef enum {
	ERR_KIND_SUCCESS,
	ERR_KIND_OTHER,
	ERR_KIND_PERMS,
	ERR_KIND_FAULT,
	ERR_KIND_INVALID,
	ERR_KIND_UNSUPPORTED,
	ERR_KIND_EXHAUSTION,

} error_kind;

typedef struct {
	error_kind kind;
	str msg;
} error;

#define BUILD_ERROR(K, MSG) ((error){(K), str$(MSG)})
#define ERR_SUCCESS BUILD_ERROR(ERR_KIND_SUCCESS, "success")
#define ERR_UNDEFINED BUILD_ERROR(ERR_KIND_OTHER, "undefined error")
#define ERR_PERMISSION BUILD_ERROR(ERR_KIND_PERMS, "lacking permission")
#define ERR_BAD_PARAMETERS BUILD_ERROR(ERR_KIND_INVALID, "bad parameters")
#define ERR_BAD_DATA BUILD_ERROR(ERR_KIND_INVALID, "bad data")
#define ERR_OUT_OF_MEMORY BUILD_ERROR(ERR_KIND_EXHAUSTION, "out of memory")

// A type for void returning results
typedef struct {
	uint8_t _pad;
} _success;

typedef result(_success, error) maybe;

#define SUCCESS OK(maybe, (_success){})
#define FAILURE(E) ERR(maybe, E)
