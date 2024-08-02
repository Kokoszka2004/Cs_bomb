#ifndef OBRAZEK_H
#define OBRAZEK_H

#include <atomic>

// Deklaracja funkcji z obrazek.cpp
int picture_and_music(int argc, char* argv[]);

// Declare the external variable
extern std::atomic<bool> sequenceActivated;

#endif // OBRAZEK_H
