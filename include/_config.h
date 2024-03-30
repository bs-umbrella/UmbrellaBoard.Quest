#pragma once

#define UMBRELLA_EXPORT __attribute__((visibility("default")))

#ifdef __cplusplus
#define UMBRELLA_EXPORT_FUNC extern "C" UMBRELLA_EXPORT
#else
#define UMBRELLA_EXPORT_FUNC UMBRELLA_EXPORT
#endif

#ifndef TIMEOUT
#define TIMEOUT 10
#endif

#ifndef USER_AGENT
#define USER_AGENT "UmbrellaBoard/" VERSION " (+https://github.com/bs-umbrella/UmbrellaBoard.Quest)"
#endif

#ifndef COMMUNITIES_URL
#define COMMUNITIES_URL "file:///sdcard/ModData/com.beatgames.beatsaber/Mods/UmbrellaBoard/communities.json"
#endif
