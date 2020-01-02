set BINDIR=bin
set OBJDIR=obj

set INCDIRS=..\include
set LIBDIR=lib
set LIBS=

set SOURCE=main.cpp
set BINARY=example.exe

set CXX=cl /nologo /std:c++17 /D_CRT_SECURE_NO_WARNINGS /MT /EHsc

if "%mode%"=="debug" goto debug
if "%mode%"=="release" goto release
if "%mode%"=="nolog" goto nolog
goto :eof

:debug
set COMPFLAGS=/Zi /bigobj /O2
set LINKFLAGS=/SUBSYSTEM:CONSOLE /DEBUG
goto :eof

:release
set COMPFLAGS=/DNDEBUG /bigobj /O2
set LINKFLAGS=/SUBSYSTEM:CONSOLE
goto :eof

:nolog
set COMPFLAGS=/DNOLOG /DNDEBUG /bigobj /O2
set LINKFLAGS=/SUBSYSTEM:CONSOLE
