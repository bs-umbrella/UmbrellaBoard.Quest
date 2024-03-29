#pragma once

#define UMBRELLA_EXPORT __attribute__((visibility("default")))

#ifdef __cplusplus
#define UMBRELLA_EXPORT_FUNC extern "C" UMBRELLA_EXPORT
#else
#define UMBRELLA_EXPORT_FUNC UMBRELLA_EXPORT
#endif

#define TIMEOUT 10
#define USER_AGENT "UmbrellaBoard/" VERSION " (+https://github.com/bs-umbrella/UmbrellaBoard)"
