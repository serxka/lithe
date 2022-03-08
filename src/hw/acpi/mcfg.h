#pragma once

#include <hw/acpi/tables.h>
#include <utils/base/attributes.h>
#include <utils/base/defs.h>
#include <utils/text/str.h>

#define ACPI_MCFG_SIG str_n$(4, "MCFG")

typedef struct {
	uint64_t address;
	uint16_t segment_group;
	uint8_t bus_start;
	uint8_t bus_end;
	uint32_t _reserved;
} PACKED mcfg_record_t;

typedef struct {
	acpi_stdh_t header;
	uint64_t _reserved;
	mcfg_record_t records[];
} acpi_mcfg_t;

static INLINE size_t acpi_mcfg_record_size(acpi_mcfg_t* mcfg) {
	return (mcfg->header.length - sizeof(acpi_mcfg_t))
	       / sizeof(mcfg_record_t);
}
