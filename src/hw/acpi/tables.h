#pragma once

#include <utils/base/attributes.h>
#include <utils/base/defs.h>
#include <utils/text/str.h>

#define ACPI_RSDP_SIG str_n$(8, "RSD PTR ")

typedef struct {
	char signature[8];
	uint8_t checksum;
	char oemid[6];
	uint8_t revision;
	uint32_t rsdt_address;
	// Version 2.0 extended fields
	uint32_t length;
	uint64_t xsdt_address;
	uint8_t extended_checksum;
	uint8_t _reserved[3];
} PACKED acpi_rsdp_t;

typedef struct {
	char signature[4];
	uint32_t length;
	uint8_t revision;
	uint8_t checksum;
	char oemid[6];
	char oem_table_id[8];
	uint32_t oem_revision;
	uint32_t creator_id;
	uint32_t creator_revision;
} PACKED acpi_stdh_t;

typedef struct {
	acpi_stdh_t header;
	uint64_t children[];
} PACKED acpi_xsdt_t;

static INLINE size_t acpi_xsdt_children_size(acpi_xsdt_t* xsdt) {
	return (xsdt->header.length - sizeof(acpi_stdh_t)) / 8;
}

bool acpi_rsdp_checksum(acpi_rsdp_t* table);
bool acpi_stdh_checksum(acpi_stdh_t* table);
void* acpi_find_table(acpi_xsdt_t* root, str_t table);
