#pragma once

typedef unsigned int uint, u32;
typedef unsigned char ubyte, u8;

//#define MAX_BONE_INFLUENCE 4
//const int MAX_BONE_INF = 4; ’ранитс€ в пам€ти, если не используетс€ в compile-time выражени€х.
constexpr int MAX_BONE_INF = 4; // «начение известно на этапе компил€ции (compile-time). √арантированно не создаЄт объект в пам€ти Ч компил€тор подставит значение напр€мую.
