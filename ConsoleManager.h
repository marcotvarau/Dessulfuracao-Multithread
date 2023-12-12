#pragma once
#ifndef CONSOLEMANAGER_H
#define CONSOLEMANAGER_H

#include <string>
#include <windows.h>

// Declara��o das fun��es
void ConsoleProcesso();
void PrintProcesso(const std::string& message);
void ConsoleAlarmes();
void PrintAlarme(const std::string& message);

extern HANDLE hNamedPipe;
extern HANDLE hNamedPipe2;

#endif // CONSOLEMANAGER_H
