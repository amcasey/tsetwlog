#include <windows.h>
#include "node_api.h"

/*
 * This code is needed as the module builds against the node.exe library, but at runtime this is
 * not guaranteed to be the binary name. For example, Electron uses an "electron.exe" host process,
 * are loads Node.js as "node.dll". There are also Node.js forks which may have a different name (e.g. io.js).
 *
 * This means node.exe must be delay loaded, and the indirection below used to return the right module to bind to
 * should it not be named "node.exe".
 *
 */

// Declare an 'impl_' prefixed pointer for every NAPI function we want to use
#define DECL_NAPI_IMPL(fn_name) decltype(&fn_name) impl_##fn_name

DECL_NAPI_IMPL(napi_module_register);
DECL_NAPI_IMPL(napi_create_function);
DECL_NAPI_IMPL(napi_set_named_property);
DECL_NAPI_IMPL(napi_add_env_cleanup_hook);
DECL_NAPI_IMPL(napi_get_cb_info);
DECL_NAPI_IMPL(napi_typeof);
DECL_NAPI_IMPL(napi_get_value_string_utf16);

bool LoadNapiFunctions()
{
	// Handle to the .exe for the current process (e.g. node.exe in the mainline scenario)
	HMODULE nodeModule = GetModuleHandle(NULL);
	// See if this contains the APIs we want to use.
	FARPROC fn_addr = GetProcAddress(nodeModule, "napi_module_register");

	// If not, see if node.dll is present and contains the exports (e.g. Electron scenario).
	if (fn_addr == NULL) {
		nodeModule = GetModuleHandle("node.dll");
		if (nodeModule == NULL) return false;
		fn_addr = GetProcAddress(nodeModule, "napi_module_register");
		if (fn_addr == NULL) {
			// Couldn't find the module with the NAPI exports
			return false;
		}
	}

#define GET_NAPI_IMPL(fn_name) \
fn_addr = GetProcAddress(nodeModule, #fn_name); \
if (fn_addr == NULL) return false;              \
impl_##fn_name = (decltype(impl_##fn_name))fn_addr;

	// Assign the addresses of the needed functions to the "impl_*" pointers.
	GET_NAPI_IMPL(napi_module_register);
	GET_NAPI_IMPL(napi_create_function);
	GET_NAPI_IMPL(napi_set_named_property);
	GET_NAPI_IMPL(napi_add_env_cleanup_hook);
	GET_NAPI_IMPL(napi_get_cb_info);
	GET_NAPI_IMPL(napi_typeof);
	GET_NAPI_IMPL(napi_get_value_string_utf16);

	return true;
}
