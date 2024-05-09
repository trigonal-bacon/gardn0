#pragma once

#include <assert.h>

#ifdef CLIENT
#define CLIENT_ONLY(...) __VA_ARGS__
#else
#define CLIENT_ONLY(...)
#endif

#ifdef SERVER
#define SERVER_ONLY(...) __VA_ARGS__
#else
#define SERVER_ONLY(...)
#endif