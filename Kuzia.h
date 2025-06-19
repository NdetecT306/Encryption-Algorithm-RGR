#ifndef KUZIA_H
#define KUZIA_H

#include <vector>
#include <string>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <random>
#include <stdexcept>
#include <sstream>
#include <vector>
#include <algorithm>
const int LENGTH = 16;
using namespace std;

int Kuzia();
void KuzFileEnc(const string& filePath);
void KuzFileDec(const string& filePath);
void KuzTerEnc();
void KuzTerDec();

#endif
