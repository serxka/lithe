#include <hw/acpi/tables.h>
#include <utils/base/attributes.h>
#include <utils/base/defs.h>
#include <utils/text/str.h>

bool acpi_rsdp_checksum(acpi_rsdp_t* table) {
	uint8_t sum = 0;
	for (size_t i = 0; i < table->length; ++i)
		sum += ((uint8_t*)table)[i];
	return sum == 0;
}

bool acpi_stdh_checksum(acpi_stdh_t* table) {
	uint8_t sum = 0;
	for (size_t i = 0; i < table->length; ++i)
		sum += ((uint8_t*)table)[i];
	return sum == 0;
}

void* acpi_find_table(acpi_xsdt_t* root, str_t table) {
	size_t entries = acpi_xsdt_children_size(root);
	for (size_t i = 0; i < entries; ++i) {
		acpi_stdh_t* header = (acpi_stdh_t*)root->children[i];
		if (str_equ(str_n$(4, header->signature), table))
			return (void*)header;
	}
	return null;
}
