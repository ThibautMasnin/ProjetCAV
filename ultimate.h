#ifndef ULTIMATE_H
#define ULTIMATE_H
#include "game.h"

Box **range_standard(int x, int y, int size);
Box **range_ligne(int x, int y, int size);
Box **range_croix(int x, int y, int size);
Box **range_plus(int x, int y, int size);
Box **range_carre(int x, int y, int size);

#endif