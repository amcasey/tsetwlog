#include "tsetwlog.h"

// GUID generated by https://blogs.msdn.microsoft.com/dcook/2015/09/08/etw-provider-names-and-guids/
TRACELOGGING_DEFINE_PROVIDER(
	g_hMyProvider,
	"tsserverEventSource",
	// {ac4e9dd1-3a7a-5022-fc37-f2394bc4f09e}
	(0xac4e9dd1, 0x3a7a, 0x5022, 0xfc, 0x37, 0xf2, 0x39, 0x4b, 0xc4, 0xf0, 0x9e));

namespace demo {
	void InitEtw() {
		TraceLoggingRegister(g_hMyProvider);
	}

	napi_value LogEvent(napi_env env, napi_callback_info args) {
		napi_status status;
		napi_value result;
		napi_valuetype valueType;
		napi_get_undefined(env, &result);

		size_t argCount = 1;
		napi_value cb_args[1];
		napi_value thisArg;
		void *pData;

		if (TraceLoggingProviderEnabled(g_hMyProvider, /* any level */0, /* any keywords */0))
		{
			// e.g. if you wanted to test before an expensive operations
		}

		status = napi_get_cb_info(env, args, &argCount, cb_args, &thisArg, &pData);
		if (status != napi_ok || argCount < 1) return result;

		status = napi_typeof(env, cb_args[0], &valueType);
		if (status != napi_ok || valueType != napi_valuetype::napi_string) return result;

		constexpr size_t msgSize = 1024;
		char16_t pMsg[msgSize];
		size_t written;

		status = napi_get_value_string_utf16(env, cb_args[0], pMsg, msgSize, &written);
		if (status != napi_ok) return result;

		TraceLoggingWrite(g_hMyProvider,
			"logEvent",
			TraceLoggingLevel(WINEVENT_LEVEL_VERBOSE), // Level is optional
			//TraceLoggingKeyword(0x10),                 // Keywords are optional
			TraceLoggingWideString((wchar_t*)pMsg, "msg")
		);

		return result;
	}
}