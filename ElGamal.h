#ifndef ELGAMAL_H
#define ELGAMAL_H
#ifdef __cplusplus

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <cstdlib>
#include <cctype>
#include <limits>
#include <stdexcept>
#include <filesystem>
#include <gmpxx.h>
#include <gmp.h>
#include <random>
#include <sys/stat.h>

extern "C" {
#endif

int ElGamal_gmp();
int ElGamDec(bool useFile);
int ElGamEnc(bool useFile);

#ifdef __cplusplus
}
#endif

#endif
