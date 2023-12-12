#ifndef MESSAGEPROCESSING_H
#define MESSAGEPROCESSING_H

#include <string>

// Declaração das funções
std::string processaMensagemProcesso(const std::string& message);
std::string processMessage(const std::string& inputMessage);
std::string processHardwareFailureMessage(const std::string& inputMessage);

#endif // MESSAGEGENERATOR_H
