#define _CRT_SECURE_NO_WARNINGS

#ifndef	MESSAGEUTILITY_H
#define MESSAGEUTILITY_H

#include <string>

// Declara��o das fun��es
std::string generateRandomReal();
std::string getCurrentTime();
std::string getAlarmText(const std::string& id);

#endif // MESSAGEUTILITY_H