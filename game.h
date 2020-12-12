#ifndef GAME_H
#define GAME_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <ctype.h>

typedef enum ship
{
    Null,
    Close,
    PorteAvion,
    Croiseur,
    Destroyer,
    SousMarin,
    Torpilleur
} Ship;

typedef enum orientation
{
    Inconnue,
    Horizontal,
    Vertical
} Orientation;

typedef struct box
{
    bool shot;
    Ship bateau;
} Box;

typedef struct state
{
    int x, y;
    Orientation sens;
} State;

typedef struct reponse
{
    int x; // the row | number, default 0
    int y; // the column | letter, default 0
} Reponse;

/*
5 ships: 1 porte-avion (5 cases),
1 croiseur (4 cases),
1 destroyer (3 cases),
1 sous-marin (3 cases),
1 torpilleur (2 cases).
*/
typedef struct player
{
    Box **grille;
    bool bot;
    bool lastShotSuccess;
    int porteAvion;
    int croiseur;
    int destroyer;
    int sousMarin;
    int torpilleur;
    int tirLigne;
    int tirCroix;
    int tirPlus;
    int tirCarre;
    Reponse *rep;
    State *etat;
} Player;

typedef Player *PlayerPtr;

void initGrille(int size, Box **grille);
int convert_letters(char *s);
int validate_reponse(char *s, PlayerPtr p, int size);
void reset_reponse(PlayerPtr p);
void printGrilles(int size, Box **grilleJoueur, Box **grilleIA);

#endif