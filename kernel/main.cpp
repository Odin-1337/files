#include <includes.h>

void unload(
	driver_object_t* driver_object
) {
	
}

nt_status_t driver_entry(
	driver_object_t* driver_object,
	unicode_string_t* driver_path
) {
	// if null everything will break.
	if (!ntoskrnl_base()) {
		return nt_status_t::failed_driver_entry;
	}

	// not nessesary as driver works being loaded signed or not.
	if (!mm_is_address_valid(driver_object)) {
		dbg_print("[!] Fatal error, invalid driver object. object ptr: 0x%x\n", driver_object);
		return nt_status_t::invalid_parameter;
	}
	driver_object->m_driver_unload = &unload; // comment out if kdmapped :skid:

	unicode_string_t altitude;
	rtl_init_unicode_string(&altitude, L"450000");

	large_integer_t cookie;
	auto result = cm_register_callback_ex(
		client::registry_callback,
		altitude,
		&cookie
	);

	dbg_print("CmRegisterCallbackEx: 0x%x\n", result);

	return result;
}