#pragma once
#include <string>

void loadConfigToSdCard();
void saveConfigFromSdCard();

void loadConfigFromString(std::string json);
std::string saveConfigToString();
