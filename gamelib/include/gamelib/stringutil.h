#pragma once

#include <stdarg.h>

char* DuplicateString(const char* in);
void CopyString(char* dest, size_t destSize, const char* source);

void FormatStringBuffer(char* dest, size_t destSize, const char* format, ...);
void FormatStringBufferV(char* dest, size_t destSize, const char* format, va_list args);

const char* FirstNonWhitespace(const char* str);
