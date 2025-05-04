#ifndef CRASHHANDLER_H
#define CRASHHANDLER_H

#include <windows.h>
#include <dbghelp.h>
#include <tchar.h>

LONG WINAPI UnhandledExcHandler(struct _EXCEPTION_POINTERS* aExceptionInfoPtr);

#endif