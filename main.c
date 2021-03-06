#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include "regex.h"
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

int convert_letters(char *s);
PlayerPtr play(int size, PlayerPtr joueur, PlayerPtr adversaire);

void initGrille(int size, Box **grille)
{
    int i, j;
    for (i = 0; i < size; i++)
    {
        for (j = 0; j < size; j++)
        {
            if(i&&j) {
                grille[i][j].shot = false;
            }
            else {
                grille[i][j].shot = true;
            }
            grille[i][j].bateau = Null;
        }
    }
}

int addShip(int size, Box **grille, Ship bateau)
{
    int x, y, length, tmp, attempts = 50;
    size--;
    switch (bateau)
    {
    case PorteAvion:
        length = 5;
        break;
    case Croiseur:
        length = 4;
        break;
    case Destroyer:
        length = 3;
        break;
    case SousMarin:
        length = 3;
        break;
    case Torpilleur:
        length = 2;
        break;
    default:
        length = size;
        break;
    }
    if (length > size)
    {
        return 1;
    }
    while (attempts--)
    {
        tmp = 1;
        if (rand() % 2)
        {
            x = rand() % (size - length + 1) + 1;
            y = rand() % (size) + 1;
            if (!(grille[x][y].bateau))
            {
                while (x + tmp <= size && tmp < length && !(grille[x + tmp][y].bateau))
                {
                    tmp++;
                }
                if (tmp >= length)
                {
                    grille[x - 1][y].bateau = Close;
                    while (length--)
                    {
                        grille[x][y].bateau = bateau;
                        grille[x][y - 1].bateau = Close;
                        if (y + 1 <= size)
                        {
                            grille[x][y + 1].bateau = Close;
                        }
                        x++;
                    }
                    if (x <= size)
                    {
                        grille[x][y].bateau = Close;
                    }
                    return 0;
                }
            }
        }
        else
        {
            x = rand() % (size) + 1;
            y = rand() % (size - length + 1) + 1;
            if (!(grille[x][y].bateau))
            {
                while (!(grille[x][y + tmp].bateau) && y + tmp <= size && tmp < length)
                {
                    tmp++;
                }
                if (tmp >= length)
                {
                    grille[x][y - 1].bateau = Close;
                    while (length--)
                    {
                        grille[x][y].bateau = bateau;
                        grille[x - 1][y].bateau = Close;
                        if (x + 1 <= size)
                        {
                            grille[x + 1][y].bateau = Close;
                        }
                        y++;
                    }
                    if (y <= size)
                    {
                        grille[x][y].bateau = Close;
                    }
                    return 0;
                }
            }
        }
    }
    return 1;
}

int initPlayer(int size, PlayerPtr joueur, bool bot)
{
    joueur->grille = malloc(size * sizeof(Box *));
    for (int i = 0; i < size; i++)
    {
        joueur->grille[i] = malloc(size * sizeof(Box));
    }
    initGrille(size, joueur->grille);
    joueur->bot = bot;
    joueur->lastShotSuccess = false;
    joueur->porteAvion = 5;
    joueur->croiseur = 4;
    joueur->destroyer = 3;
    joueur->sousMarin = 3;
    joueur->torpilleur = 2;
    joueur->tirLigne = 1;
    joueur->tirCroix = 1;
    joueur->tirPlus = 1;
    joueur->tirCarre = 1;
    joueur->rep = malloc(sizeof(Reponse));
    joueur->rep->x = 0;
    joueur->rep->y = 0;
    if (bot)
    {
        joueur->etat = malloc(sizeof(State));
        joueur->etat->x = 0;
        joueur->etat->y = 0;
        joueur->etat->sens = Inconnue;
    }
    else
    {
        joueur->etat = NULL;
    }
    if (addShip(size, joueur->grille, PorteAvion))
    {
        return 1;
    }
    else if (addShip(size, joueur->grille, Croiseur))
    {
        return 1;
    }
    else if (addShip(size, joueur->grille, Destroyer))
    {
        return 1;
    }
    else if (addShip(size, joueur->grille, SousMarin))
    {
        return 1;
    }
    else if (addShip(size, joueur->grille, Torpilleur))
    {
        return 1;
    }
    return 0;
}

void printGrilles(int size, Box **grilleJoueur, Box **grilleIA)
{
    int i, j;
    printf("\nJoueur :  ");
    for (j = 2; j < size; j++)
    {
        printf("    ");
    }
    printf("\t\t\tOrdinateur :\n +");
    for (j = 1; j < size; j++)
    {
        for (j = 0; j < 2 * size; j++)
        {
            if (j == size)
            {
                printf("\t\t\t+");
            }
            printf("---+");
        }
        printf("\n |   | ");
        for (j = 1; j < size; j++)
        {
            if (j > 26)
            {
                printf("%c%c| ", 'A' + (j - 1) / 26 - 1, 'A' + (j - 1) % 26);
            }
            else
            {
                printf("%c | ", 'A' + j - 1);
            }
        }
        printf("\t\t\t|   | ");
        for (j = 1; j < size; j++)
        {
            if (j > 26)
            {
                printf("%c%c| ", 'A' + (j - 1) / 26 - 1, 'A' + (j - 1) % 26);
            }
            else
            {
                printf("%c | ", 'A' + j - 1);
            }
        }
        printf("\n +");
    }
    for (i = 1; i < size; i++)
    {
        for (j = 0; j < 2 * size; j++)
        {
            if (j == size)
            {
                printf("\t\t\t+");
            }
            printf("---+");
        }
        printf("\n | ");
        if (i > 9)
        {
            printf("%d| ", i);
        }
        else
        {
            printf("%d | ", i);
        }
        for (j = 1; j < size; j++)
        {
            if (grilleJoueur[i][j].bateau > Close)
            {
                if (grilleJoueur[i][j].shot)
                {
                    printf("# | ");
                }
                else
                {
                    printf("O | ");
                }
            }
            else
            {
                if (grilleJoueur[i][j].shot)
                {
                    printf("X | ");
                }
                else
                {
                    printf("  | ");
                }
            }
        }
        printf("\t\t\t| ");
        if (i > 9)
        {
            printf("%d| ", i);
        }
        else
        {
            printf("%d | ", i);
        }
        for (j = 1; j < size; j++)
        {
            if (grilleIA[i][j].bateau > Close)
            {
                if (grilleIA[i][j].shot)
                {
                    printf("# | ");
                }
                else
                {
                    printf("O | ");
                }
            }
            else
            {
                if (grilleIA[i][j].shot)
                {
                    printf("X | ");
                }
                else
                {
                    printf("  | ");
                }
            }
        }
        printf("\n +");
    }
    for (j = 0; j < 2 * size; j++)
    {
        if (j == size)
        {
            printf("\t\t\t+");
        }
        printf("---+");
    }
    printf("\n\n\n");
}
// validate the input and update the reponse filed of player
int validate_reponse(char *s, PlayerPtr p, int size)
{
    regex_t regex;
    regex_t regex2;
    regex_t regex3;

    int value, value2, value3;
    char num[4];    // maximum 3 digits
    char letter[3]; // maximum 2 letters
    int tmp_x, tmp_y;
    tmp_x = tmp_y = 0;
    // num[] and letter[]  are tempoaray holders of input
    int l_counter = 0;
    int n_counter = 0;
    // create regex
    value = regcomp(&regex, "^[1-9][0-9]{0,2}[a-zA-Z]{1,2}$", REG_EXTENDED);
    value2 = regcomp(&regex2, "^[a-zA-Z]{1,2}[1-9][0-9]{0,2}$", REG_EXTENDED);

    // regex3 to match only letters or only numbers
    value3 = regcomp(&regex3, "^([a-zA-Z]{1,2}|[1-9][0-9]{0,2}){1}$", REG_EXTENDED);

    if (value == 0 && value2 == 0 && value3 == 0)
    {
        // printf("RegEx compiled successfully.\n");
    }
    else
    {
        printf("Erreur de compilation.");
        return 0;
    }

    value = regexec(&regex, s, 0, NULL, 0);
    value2 = regexec(&regex2, s, 0, NULL, 0);
    value3 = regexec(&regex3, s, 0, NULL, 0);

    //--------------process choice(or grid size) only -----------------
    // size=0 means it will check for rep->choice only;

    if (size == 0)
    {
        // match pattern
        if (value3 == 0)
        {

            while (*s)
            {
                if (isdigit(*s))
                {
                    num[n_counter++] = *s;
                }

                s++;
            }
            num[n_counter] = '\0';
            tmp_x = atoi(num);
            if (tmp_x > 0)
            {
                return tmp_x; // return user numeric input
            }
            else
            {
                return 0;
            }
        }
        else
        {
            return 0;
        }
    }
    //--------------process choice(or grid size) only -----------------

    // if it matches the pattern
    if (value == 0 || value2 == 0 || value3 == 0)
    {
        while (*s)
        {
            if (isalpha(*s))
            {
                letter[l_counter++] = *s;
            }
            else if (isdigit(*s))
            {
                num[n_counter++] = *s;
            }

            s++;
        }
        num[n_counter] = '\0';
        letter[l_counter] = '\0';
        if (strlen(num) > 0)
        {
            tmp_x = atoi(num);
        }
        if (strlen(letter) > 0)
        {
            tmp_y = convert_letters(letter); // convert the alphbetical input to numbers
        }

        if (tmp_x > 0 && tmp_x < size)
        {
            p->rep->x = tmp_x; // only take x when x is in the grid
        }
        if (tmp_y > 0 && tmp_y < size)
        {
            p->rep->y = tmp_y; // only take y when y is in the grid
        }

        return 1; // response valid and update the fileds of player
        // return 1 only means the input contains letters or numbers
    }
    else
    {
        return 0; // response invalid, do nothing
    }
}

// must pass in strlen>0
int convert_letters(char *s)
{
    char *ptr = s;
    int tmp = 0;

    // convert all letters to uppercase
    for (int i = 0; s[i]; i++)
    {
        if (s[i] >= 'a' && s[i] <= 'z')
        {
            s[i] = s[i] - 32;
        }
    }
    if (strlen(s) == 2)
    {
        tmp = (s[0] - 'A' + 1) * 26 + (s[1] - 'A' + 1);
    }
    else if (strlen(s) == 1)
    {
        tmp = s[0] - 'A' + 1;
    }

    return tmp;
}

void convert_int(int n, char s[])
{
    if(n<=26) {
        s[1]=(char)(64+n);
    }
    else {
        s[0]=(char)(64+n/26);
        s[1]=(char)(64+n%26);
    }
}

void reset_reponse(PlayerPtr p)
{
    p->rep->x = 0;
    p->rep->y = 0;
}

// return a matrix that the attack_range has covered
Box **range_ligne(int x, int y, int size)
{

    Box **range = malloc(size * sizeof(Box *));
    for (int i = 0; i < size; i++)
    {
        range[i] = malloc(size * sizeof(Box));
    }
    initGrille(size, range);

    if (x != 0)
    {
        for (int i = 0; i < size; i++)
        {
            range[x][i].shot = true;
        }
    }
    else if (y != 0)
    {
        for (int i = 0; i < size; i++)
        {
            range[i][y].shot = true;
        }
    }
    return range;
}

Box **range_croix(int x, int y, int size)
{
    Box **range = malloc(size * sizeof(Box *));
    for (int i = 0; i < size; i++)
    {
        range[i] = malloc(size * sizeof(Box));
    }
    initGrille(size, range);
    // mark the center
    range[x][y].shot = true;
    // top left
    if (x - 1 < size && y - 1 < size && x - 1 > 0 && y - 1 > 0)
    {
        range[x - 1][y - 1].shot = true;
    }
    // bottom left
    if (x + 1 < size && y - 1 < size && x + 1 > 0 && y - 1 > 0)
    {
        range[x + 1][y - 1].shot = true;
    }
    // top right
    if (x - 1 < size && y + 1 < size && x - 1 > 0 && y + 1 > 0)
    {
        range[x - 1][y + 1].shot = true;
    }
    // bottom right
    if (x + 1 < size && y + 1 < size && x + 1 > 0 && y + 1 > 0)
    {
        range[x + 1][y + 1].shot = true;
    }

    return range;
}

Box **range_plus(int x, int y, int size)
{
    Box **range = malloc(size * sizeof(Box *));
    for (int i = 0; i < size; i++)
    {
        range[i] = malloc(size * sizeof(Box));
    }
    initGrille(size, range);
    // mark the center
    range[x][y].shot = true;
    // center up
    if (x - 1 < size && x - 1 > 0)
    {
        range[x - 1][y].shot = true;
    }
    // center down
    if (x + 1 < size && x + 1 > 0)
    {
        range[x + 1][y].shot = true;
    }
    // center left
    if (y - 1 < size && y - 1 > 0)
    {
        range[x][y - 1].shot = true;
    }
    // center right
    if (y + 1 < size && y + 1 > 0)
    {
        range[x][y + 1].shot = true;
    }

    return range;
}

Box **range_carre(int x, int y, int size)
{
    Box **range = malloc(size * sizeof(Box *));
    for (int i = 0; i < size; i++)
    {
        range[i] = malloc(size * sizeof(Box));
    }
    initGrille(size, range);

    for (int i = x - 1; i <= x + 1; i++)
    {
        for (int j = y - 1; j <= y + 1; j++)
        {
            if (i < size && i > 0 && j < size && j > 0)
            {
                range[i][j].shot = true;
            }
        }
    }

    return range;
}

Box **range_standard(int x, int y, int size)
{
    Box **range = malloc(size * sizeof(Box *));
    for (int i = 0; i < size; i++)
    {
        range[i] = malloc(size * sizeof(Box));
    }
    initGrille(size, range);

    // seulement une case
    range[x][y].shot = true;

    return range;
}

PlayerPtr status_report(int x, int y, Box **grille, PlayerPtr tireur, PlayerPtr cible, int size);
PlayerPtr matrix_transform(Box **grille, Box **ult, int size, PlayerPtr tireur, PlayerPtr cible)
{
    PlayerPtr result;
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            // mark the shot cell of ultimate to the grille in status_report
            if (ult[i][j].shot && !grille[i][j].shot)
            {
                result = status_report(i, j, grille, tireur, cible, size);
            }
        }
    }
    return result;
}

// void sunk_report(PlayerPtr p)
// {
// }

void free_range_matrix(Box **b, int size)
{
    for (int i = 0; i < size; i++)
    {
        free(b[i]);
    }
    free(b);
}

PlayerPtr status_report(int x, int y, Box **grille, PlayerPtr tireur, PlayerPtr cible, int size)
{
    int *ptr;

    if (grille[x][y].bateau > Close && grille[x][y].shot == false)
    {
        grille[x][y].shot = true;
        if (tireur->bot)
        {
            if (tireur->etat->x && grille[tireur->etat->x][tireur->etat->y].bateau==grille[x][y].bateau)
            {
                if (tireur->etat->x != x)
                {
                    tireur->etat->sens = Vertical;
                }
                else
                {
                    tireur->etat->sens = Horizontal;
                }
            }
            else if (!tireur->etat->x)
            {
                if(grille[x-1][y].shot && grille[x-1][y].bateau > Close) {
                    tireur->etat->x = x-1;
                    tireur->etat->y = y;
                }
                else if(x + 1 < size && grille[x+1][y].shot && grille[x+1][y].bateau > Close) {
                    tireur->etat->x = x+1;
                    tireur->etat->y = y;
                }
                else if(grille[x][y-1].shot && grille[x][y-1].bateau > Close) {
                    tireur->etat->x = x;
                    tireur->etat->y = y-1;
                }
                else if(y + 1 < size && grille[x][y+1].shot && grille[x][y+1].bateau > Close) {
                    tireur->etat->x = x;
                    tireur->etat->y = y+1;
                }
                else {
                    tireur->etat->x = x;
                    tireur->etat->y = y;
                }
            }
        }
        switch (grille[x][y].bateau)
        {
        case PorteAvion:
            ptr = &(cible->porteAvion);
            break;
        case Croiseur:
            ptr = &(cible->croiseur);
            break;
        case Destroyer:
            ptr = &(cible->destroyer);
            break;
        case SousMarin:
            ptr = &(cible->sousMarin);
            break;
        case Torpilleur:
            ptr = &(cible->torpilleur);
            break;
        default:
            // Null and close: do nothing
            ;
        }
        (*ptr)--;
        if (*ptr)
        {
            printf("Touche !\n");
        }
        else
        {
            printf("Coule !\n");
            // if all boats are hit, becomes 0
            if (!((cible->porteAvion) + (cible->croiseur) + (cible->destroyer) + (cible->sousMarin) + (cible->torpilleur)))
            {
                return tireur; // return tireur as winner
            }
            else if (tireur->bot && grille[tireur->etat->x][tireur->etat->y].bateau==grille[x][y].bateau)
            {
                tireur->etat->x = 0;
                tireur->etat->y = 0;
                tireur->etat->sens = Inconnue;
            }
        }
    }
    else
    {
        if (tireur->bot && tireur->etat->x)
        {
            tireur->rep->x = tireur->etat->x;
            tireur->rep->y = tireur->etat->y;
        }
        printf("Dans l'eau...\n");
        grille[x][y].shot = true;
    }
    return NULL; // game does not end yet.
}

Box **special_shot(int size, PlayerPtr tireur, PlayerPtr cible, Box **grille, int choix)
{

    PlayerPtr result;

    // function pointers
    Box **(*att_ptr)(int, int, int);

    switch (choix)
    {
    case 5: // no need to deal with input starting from 0
        tireur->lastShotSuccess = false;
        return NULL;
        break;
    case 1:

        // if it contains numbers and letters at the same time, return errors
        if (tireur->rep->x != 0 && tireur->rep->y != 0)
        {
            printf("Entree invalide\n");
            return NULL;
        }
        else
        {
            att_ptr = &range_ligne;

            // mark matrice
            if (tireur->rep->x != 0)
            {

                // entire row
                // call function pointer to get the matrix of attack range
                Box **ult = att_ptr(tireur->rep->x, 0, size);
                // pass the attack matrix to compare with the original
                // result = matrix_transform(grille, ult, size, tireur, cible);

                // free_range_matrix(ult, size);
                att_ptr = NULL;

                // tirLigne used
                tireur->tirLigne = 0;
                tireur->lastShotSuccess = false;

                return ult;
            }
            else if (tireur->rep->y != 0)
            {

                Box **ult = att_ptr(0, tireur->rep->y, size);
                // entire column
                // result = matrix_transform(grille, ult, size, tireur, cible);

                // free_range_matrix(ult, size);
                att_ptr = NULL;
                tireur->tirLigne = 0;
                tireur->lastShotSuccess = false;
                return ult;
            }
        }
        break;
    case 2:

        // contains one number and at least one letter
        if (tireur->rep->x != 0 && tireur->rep->y != 0)
        {
            // function pointers
            att_ptr = &range_croix;
            // mark matrice

            // entire row
            // call function pointer to get the matrix of attack range
            Box **ult = att_ptr(tireur->rep->x, tireur->rep->y, size);
            // pass the attack matrix to compare with the original
            // result = matrix_transform(grille, ult, size, tireur, cible);
            // free_range_matrix(ult, size);
            // set function pointer back to null
            att_ptr = NULL;

            // tirCroix set to 'used'
            tireur->tirCroix = 0;
            tireur->lastShotSuccess = false;
            return ult;
        }
        else
        {
            printf("Entree invalide\n", tireur->rep->x, tireur->rep->y);
            return NULL;
        }

        break;
    case 3:

        // contains one number and at least one letter
        if (tireur->rep->x != 0 && tireur->rep->y != 0)
        {
            // function pointers

            att_ptr = &range_plus;
            // mark matrice

            // entire row
            // call function pointer to get the matrix of attack range
            Box **ult = att_ptr(tireur->rep->x, tireur->rep->y, size);
            // pass the attack matrix to compare with the original
            // result = matrix_transform(grille, ult, size, tireur, cible);
            // free_range_matrix(ult, size);
            // set function pointer back to null
            att_ptr = NULL;

            // tirCroix set to 'used'
            tireur->tirPlus = 0;
            tireur->lastShotSuccess = false;
            return ult;
        }
        else
        {
            printf("Pas de tir en plus", tireur->rep->x, tireur->rep->y);
            return NULL;
        }
        break;
    case 4:

        // contains one number and at least one letter
        if (tireur->rep->x != 0 && tireur->rep->y != 0)
        {
            // function pointers

            att_ptr = &range_carre;
            // mark matrice
            // entire row
            // call function pointer to get the matrix of attack range
            Box **ult = att_ptr(tireur->rep->x, tireur->rep->y, size);
            // pass the attack matrix to compare with the original

            // set function pointer back to null
            att_ptr = NULL;

            // tirCroix set to 'used'
            tireur->tirCarre = 0;
            tireur->lastShotSuccess = false;
            return ult;
        }
        else
        {
            printf("Pas de tir en carre");
            return NULL;
        }
        break;
    default:
        printf("Entree invalide\n");
        return NULL;
        break;
    }

    return NULL;
}

// standard shoot
PlayerPtr standard_shoot_result(int size, PlayerPtr tireur, PlayerPtr cible, int x, int y, Box **grille)
{
    int *ptr;

    if (grille[x][y].bateau > Close && grille[x][y].shot == false)
    {
        tireur->lastShotSuccess = true;
        grille[x][y].shot = true;
        if (tireur->bot)
        {
            if (tireur->etat->x)
            {
                if (tireur->etat->x != x)
                {
                    tireur->etat->sens = Vertical;
                }
                else
                {
                    tireur->etat->sens = Horizontal;
                }
            }
            else
            {
                if(grille[x-1][y].shot && grille[x-1][y].bateau > Close) {
                    tireur->etat->x = x-1;
                    tireur->etat->y = y;
                    tireur->etat->sens = Vertical;
                }
                else if(x + 1 < size && grille[x+1][y].shot && grille[x+1][y].bateau > Close) {
                    tireur->etat->x = x+1;
                    tireur->etat->y = y;
                    tireur->etat->sens = Vertical;
                }
                else if(grille[x][y-1].shot && grille[x][y-1].bateau > Close) {
                    tireur->etat->x = x;
                    tireur->etat->y = y-1;
                    tireur->etat->sens = Horizontal;
                }
                else if(y + 1 < size && grille[x][y+1].shot && grille[x][y+1].bateau > Close) {
                    tireur->etat->x = x;
                    tireur->etat->y = y+1;
                    tireur->etat->sens = Horizontal;
                }
                else {
                    tireur->etat->x = x;
                    tireur->etat->y = y;
                }
            }
        }
        switch (grille[x][y].bateau)
        {
        case PorteAvion:
            ptr = &(cible->porteAvion);
            break;
        case Croiseur:
            ptr = &(cible->croiseur);
            break;
        case Destroyer:
            ptr = &(cible->destroyer);
            break;
        case SousMarin:
            ptr = &(cible->sousMarin);
            break;
        case Torpilleur:
            ptr = &(cible->torpilleur);
            break;
        default:
            // Null and close: do nothing
            ;
        }
        (*ptr)--;
        if (*ptr)
        {
            printf("Touche !\n");

            return NULL;
        }
        else
        {
            printf("Coule !\n");

            // if all boats are hit, becomes 0
            if (!((cible->porteAvion) + (cible->croiseur) + (cible->destroyer) + (cible->sousMarin) + (cible->torpilleur)))
            {
                // return the winner
                return tireur;
            }
            else
            {
                if (tireur->bot)
                {
                    tireur->etat->x = 0;
                    tireur->etat->y = 0;
                    tireur->etat->sens = Inconnue;
                }
                return NULL;
            }
        }
    }
    else
    {
        if (tireur->bot && tireur->etat->x)
        {
            tireur->rep->x = tireur->etat->x;
            tireur->rep->y = tireur->etat->y;
        }
        tireur->lastShotSuccess = false;
        grille[x][y].shot = true;
        printf("Dans l'eau...\n");
    }

    return NULL;
}

PlayerPtr play(int size, PlayerPtr joueur, PlayerPtr adversaire)
{
    int i = 1, j = 1;
    int check = 0;
    int choix = 5;
    char conversation[6], letters[3]={' ',' ','\0'}, input[4]; // temp holder of choix
    PlayerPtr result = NULL;
    Box **att_range = NULL;
    Box **(*att_ptr)(int, int, int);
    //------------------------------------------------------
    if (joueur->bot)
    {
        if(joueur->lastShotSuccess && ((joueur->tirLigne && joueur->sousMarin != 0) || (joueur->tirCarre && joueur->porteAvion != 0) || (joueur->tirPlus && joueur->croiseur != 0) || (joueur->tirCroix && joueur->croiseur != 0))) {
            if(joueur->etat->x&&joueur->etat->y) {
                i=joueur->etat->x;
                j=joueur->etat->y;
            }
            else {
                i=size/2;
                j=size/2;
            }
            if (joueur->tirLigne && joueur->sousMarin != 0) {
                choix=1;
                if(joueur->rep->y>joueur->rep->x) {
                    joueur->rep->x=0;
                    convert_int(joueur->rep->y, letters);
                    printf("L'ordinateur tir en ligne sur %s.\n", letters);
                }
                else {
                    joueur->rep->y=0;
                    printf("L'ordinateur tir en ligne sur %d.\n", joueur->rep->x);
                }
            }
            else if (joueur->tirCarre && joueur->porteAvion != 0)
            {
                choix=4;
                joueur->rep->x = i;
                joueur->rep->y = i;
                convert_int(joueur->rep->y, letters);
                printf("L'ordinateur tir en carre sur %s%d.\n", letters, joueur->rep->x);
            }
            else if (joueur->tirPlus && joueur->croiseur != 0) {
                choix=3;
                joueur->rep->x = i;
                joueur->rep->y = j;
                convert_int(joueur->rep->y, letters);
                printf("L'ordinateur tir en plus sur %s%d.\n", letters, joueur->rep->x);
            }
            else if (joueur->tirCroix && joueur->croiseur != 0) {
                choix=2;
                if (j + 1 < size && !adversaire->grille[i][j + 1].shot)
                {
                    joueur->rep->x = i;
                    joueur->rep->y = j;
                }
                else if (i + 1 < size && !adversaire->grille[i + 1][j].shot)
                {
                    joueur->rep->x = i + 1;
                    joueur->rep->y = j;
                }
                else if (j - 1 > 0 && !adversaire->grille[i][j - 1].shot)
                {
                    joueur->rep->x = i;
                    joueur->rep->y = j - 1;
                }
                else if (i - 1 > 0 && !adversaire->grille[i - 1][j].shot)
                {
                    joueur->rep->x = i - 1;
                    joueur->rep->y = j;
                }
                else
                {
                    joueur->rep->x = i;
                    joueur->rep->y = j;
                }
                convert_int(joueur->rep->y, letters);
                printf("L'ordinateur tir en croix sur %s%d.\n", letters, joueur->rep->x);
            }
            else
            {
                printf("Erreur de l'IA (special shot)\n");
                exit(EXIT_FAILURE);
            }
            att_range = special_shot(size, joueur, adversaire, adversaire->grille, choix);
            result = matrix_transform(adversaire->grille, att_range, size, joueur, adversaire);
            free_range_matrix(att_range, size);
            joueur->lastShotSuccess=false;
        }
        else {
            if (joueur->etat->x)
            {
                if (joueur->etat->sens == Horizontal)
                {
                    if (joueur->rep->y + 1 < size && adversaire->grille[joueur->rep->x][joueur->rep->y].bateau > Close && !adversaire->grille[joueur->rep->x][joueur->rep->y + 1].shot)
                    {
                        i = joueur->rep->x;
                        j = joueur->rep->y + 1;
                    }
                    else if(joueur->rep->y + 2 && adversaire->grille[joueur->rep->x][joueur->rep->y + 1].shot < size && adversaire->grille[joueur->rep->x][joueur->rep->y + 1].bateau > Close && !adversaire->grille[joueur->rep->x][joueur->rep->y + 2].shot)
                    {
                        i = joueur->rep->x;
                        j = joueur->rep->y + 2;
                    }
                    else if(joueur->rep->y + 3 < size && adversaire->grille[joueur->rep->x][joueur->rep->y + 2].shot && adversaire->grille[joueur->rep->x][joueur->rep->y + 2].bateau > Close && !adversaire->grille[joueur->rep->x][joueur->rep->y + 3].shot)
                    {
                        i = joueur->rep->x;
                        j = joueur->rep->y + 3;
                    }
                    else if(joueur->rep->y + 4 < size && adversaire->grille[joueur->rep->x][joueur->rep->y + 3].shot && adversaire->grille[joueur->rep->x][joueur->rep->y + 3].bateau > Close && !adversaire->grille[joueur->rep->x][joueur->rep->y + 4].shot)
                    {
                        i = joueur->rep->x;
                        j = joueur->rep->y + 4;
                    }
                    else if(joueur->rep->y - 1 > 0 && !adversaire->grille[joueur->rep->x][joueur->rep->y - 1].shot)
                    {
                        i = joueur->rep->x;
                        j = joueur->rep->y - 1;
                    }
                    else if (joueur->rep->y - 2 > 0 && adversaire->grille[joueur->rep->x][joueur->rep->y - 1].shot && adversaire->grille[joueur->rep->x][joueur->rep->y - 1].bateau > Close && !adversaire->grille[joueur->rep->x][joueur->rep->y - 2].shot)
                    {
                        i = joueur->rep->x;
                        j = joueur->rep->y - 2;
                    }
                    else if (joueur->rep->y - 3 > 0 && adversaire->grille[joueur->rep->x][joueur->rep->y - 2].shot && adversaire->grille[joueur->rep->x][joueur->rep->y - 2].bateau > Close && !adversaire->grille[joueur->rep->x][joueur->rep->y - 3].shot)
                    {
                        i = joueur->rep->x;
                        j = joueur->rep->y - 3;
                    }
                    else if (joueur->rep->y - 4 > 0 && adversaire->grille[joueur->rep->x][joueur->rep->y - 3].shot && adversaire->grille[joueur->rep->x][joueur->rep->y - 3].bateau > Close && !adversaire->grille[joueur->rep->x][joueur->rep->y - 4].shot)
                    {
                        i = joueur->rep->x;
                        j = joueur->rep->y - 4;
                    }
                    else
                    {
                        printf("Erreur de l'IA (standard shot horizontal)\n");
                        exit(EXIT_FAILURE);
                    }
                }
                else if (joueur->etat->sens == Vertical)
                {
                    if (joueur->rep->x + 1 < size && adversaire->grille[joueur->rep->x][joueur->rep->y].bateau > Close && !adversaire->grille[joueur->rep->x + 1][joueur->rep->y].shot)
                    {
                        i = joueur->rep->x + 1;
                        j = joueur->rep->y;
                    }
                    else if(joueur->rep->x + 2 < size && adversaire->grille[joueur->rep->x + 1][joueur->rep->y].shot && adversaire->grille[joueur->rep->x + 1][joueur->rep->y].bateau > Close && !adversaire->grille[joueur->rep->x + 2][joueur->rep->y].shot)
                    {
                        i = joueur->rep->x + 2;
                        j = joueur->rep->y;
                    }
                    else if(joueur->rep->x + 3 < size && adversaire->grille[joueur->rep->x + 2][joueur->rep->y].shot && adversaire->grille[joueur->rep->x + 2][joueur->rep->y].bateau > Close && !adversaire->grille[joueur->rep->x + 3][joueur->rep->y].shot)
                    {
                        i = joueur->rep->x + 3;
                        j = joueur->rep->y;
                    }
                    else if(joueur->rep->x + 4 < size && adversaire->grille[joueur->rep->x + 3][joueur->rep->y].shot && adversaire->grille[joueur->rep->x + 3][joueur->rep->y].bateau > Close && !adversaire->grille[joueur->rep->x + 4][joueur->rep->y].shot)
                    {
                        i = joueur->rep->x + 4;
                        j = joueur->rep->y;
                    }
                    else if(joueur->rep->x - 1 > 0 && !adversaire->grille[joueur->rep->x - 1][joueur->rep->y].shot)
                    {
                        i = joueur->rep->x - 1;
                        j = joueur->rep->y;
                    }
                    else if (joueur->rep->x - 2 > 0 && adversaire->grille[joueur->rep->x - 1][joueur->rep->y].shot &&  adversaire->grille[joueur->rep->x - 1][joueur->rep->y].bateau > Close && !adversaire->grille[joueur->rep->x - 2][joueur->rep->y].shot)
                    {
                        i = joueur->rep->x - 2;
                        j = joueur->rep->y;
                    }
                    else if (joueur->rep->x - 3 > 0 && adversaire->grille[joueur->rep->x - 2][joueur->rep->y].shot &&  adversaire->grille[joueur->rep->x - 2][joueur->rep->y].bateau > Close && !adversaire->grille[joueur->rep->x - 3][joueur->rep->y].shot)
                    {
                        i = joueur->rep->x - 3;
                        j = joueur->rep->y;
                    }
                    else if (joueur->rep->x - 4 > 0 && adversaire->grille[joueur->rep->x - 3][joueur->rep->y].shot &&  adversaire->grille[joueur->rep->x - 3][joueur->rep->y].bateau > Close && !adversaire->grille[joueur->rep->x - 4][joueur->rep->y].shot)
                    {
                        i = joueur->rep->x - 4;
                        j = joueur->rep->y;
                    }
                    else
                    {
                        printf("Erreur de l'IA (standard shot vertical)\n");
                        exit(EXIT_FAILURE);
                    }
                }
                else
                {
                    if (joueur->etat->y + 1 < size && !adversaire->grille[joueur->etat->x][joueur->etat->y + 1].shot)
                    {
                        i = joueur->etat->x;
                        j = joueur->etat->y + 1;
                    }
                    else if (joueur->etat->x + 1 < size && !adversaire->grille[joueur->etat->x + 1][joueur->etat->y].shot)
                    {
                        i = joueur->etat->x + 1;
                        j = joueur->etat->y;
                    }
                    else if (joueur->etat->y - 1 > 0 && !adversaire->grille[joueur->etat->x][joueur->etat->y - 1].shot)
                    {
                        i = joueur->etat->x;
                        j = joueur->etat->y - 1;
                    }
                    else if (joueur->etat->x - 1 > 0 && !adversaire->grille[joueur->etat->x - 1][joueur->etat->y].shot)
                    {
                        i = joueur->etat->x - 1;
                        j = joueur->etat->y;
                    }
                    else
                    {
                        printf("Erreur de l'IA (standard shot unknown)\n");
                        exit(EXIT_FAILURE);
                    }
                }
            }
            else
            {
                while (adversaire->grille[i][j].shot || (adversaire->grille[i - 1][j].shot && adversaire->grille[i - 1][j].bateau <= Close) && (adversaire->grille[i][j - 1].shot && adversaire->grille[i][j - 1].shot <= Close ))
                {
                    if (i + 2 > size)
                    {
                        i = 1;
                        if (i + 2 > size) {
                            j=1;
                        }
                        else {
                            j++;
                        }
                    }
                    else
                    {
                        i++;
                    }
                }
            }
            joueur->rep->x = i;
            joueur->rep->y = j;
            convert_int(joueur->rep->y, letters);
            printf("L'ordinateur tir sur %s%d.\n", letters, joueur->rep->x);
            result = standard_shoot_result(size, joueur, adversaire, i, j, adversaire->grille);
        }
    }
    // the above for the bot
    // the below for the player
    else
    {

        if (joueur->lastShotSuccess && ((joueur->tirLigne && joueur->sousMarin != 0) || (joueur->tirCarre && joueur->porteAvion != 0) || (joueur->tirPlus && joueur->croiseur != 0) || (joueur->tirCroix && joueur->croiseur != 0)) && !att_range)
        {
            reset_reponse(joueur);
            // to add special shoots conversation:
            // display the special shots that the player has
            printf("\nChoisissez le type de tir :\n");
            if (joueur->tirLigne && joueur->sousMarin != 0)
            {
                printf("1 - Tir en ligne (%d cases)\n", size - 1);
                check++;
            }
            if (joueur->tirCroix && joueur->croiseur != 0)
            {
                printf("2 - Tir en croix (5 cases)\n");
                check++;
            }
            if (joueur->tirPlus && joueur->croiseur != 0)
            {
                printf("3 - Tir en plus (5 cases)\n");
                check++;
            }
            if (joueur->tirCarre && joueur->porteAvion != 0)
            {
                printf("4 - Tir en carre (9 cases)\n");
                check++;
            }
            printf("5 - Tir simple (1 case)\n");
            if (check == 0)
            {
                return NULL; // no shots left
            }
            //----------------------
            scanf("%s", input);

            choix = validate_reponse(input, joueur, 0);
            // if choix is still 0 after input
            if (!choix)
            {
                printf("Entree invalide\n");
                return play(size, joueur, adversaire);
            }
            reset_reponse(joueur); // delete the choix from reponse;
            switch (choix)
            {
            case 5: // no need to deal with input starting from 0
                joueur->lastShotSuccess = false;
                return play(size, joueur, adversaire);
                break;
            case 1:
                if (joueur->tirLigne && joueur->sousMarin != 0)
                {

                    printf("Tir en ligne : Entrer une lettre pour tirer sur une colonne ou un chiffre pour tirer sur une ligne\n");
                    // escape the newline from the previous scanf
                    scanf(" %s", conversation);
                    if (validate_reponse(conversation, joueur, size))
                    {

                        att_range = special_shot(size, joueur, adversaire, adversaire->grille, choix);
                    }
                    else
                    {
                        printf("Entree invalide");
                        return play(size, joueur, adversaire);
                    }
                }
                else
                {
                    printf("Pas de tir en ligne");
                    return play(size, joueur, adversaire);
                }
                break;
            case 2:
                if (joueur->tirCroix && joueur->croiseur != 0)
                {

                    printf("Tir en Croix : Entrer les coordonnees du centre de ce tir (ex: '3A'): ");
                    scanf(" %s", conversation);
                    if (validate_reponse(conversation, joueur, size))
                    {

                        att_range = special_shot(size, joueur, adversaire, adversaire->grille, choix);
                    }
                    else
                    {
                        printf("Entree invalide");
                        return play(size, joueur, adversaire);
                    }
                }
                else
                {
                    printf("Pas de tir en croix");
                    return play(size, joueur, adversaire);
                }
                break;
            case 3:
                if (joueur->tirPlus && joueur->croiseur != 0)
                {

                    printf("Tir en Plus : Entrer les coordonnees du centre de ce tir (ex: '3A'): ");
                    scanf(" %s", conversation);
                    if (validate_reponse(conversation, joueur, size))
                    {

                        att_range = special_shot(size, joueur, adversaire, adversaire->grille, choix);
                    }
                    else
                    {
                        printf("Entree invalide");
                        return play(size, joueur, adversaire);
                    }
                }
                else
                {
                    printf("Pas de tir en plus");
                    return play(size, joueur, adversaire);
                }
                break;
            case 4:
                if (joueur->tirCarre && joueur->porteAvion != 0)
                {
                    printf("Tir en Carre : Entrer les coordonnees du centre de ce tir (ex: '3A'): ");
                    // escape the newline from the previous scanf
                    scanf(" %s", conversation);
                    if (validate_reponse(conversation, joueur, size))
                    {

                        att_range = special_shot(size, joueur, adversaire, adversaire->grille, choix);
                    }
                    else
                    {
                        printf("Entree invalide");
                        return play(size, joueur, adversaire);
                    }
                }
                else
                {
                    printf("Pas de tir en carre");
                    return play(size, joueur, adversaire);
                }
                break;
            }
            result = matrix_transform(adversaire->grille, att_range, size, joueur, adversaire);
            free_range_matrix(att_range, size);
        }
        else
        {
            printf("\nEntrer les coordonnees de la cible (ex: '3A') : ");
            scanf("%s", conversation);
            if (!strcmp(conversation, "stop"))
            {
                return adversaire;
            }
            if (validate_reponse(conversation, joueur, size))
            {
                // contains one number and at least one letter
                if (joueur->rep->x && joueur->rep->y)
                {
                    // function pointers

                    att_ptr = &range_standard;
                    // mark matrice

                    Box **shoot;
                    // entire row

                    // first matrix is the cible, the second matrix is the range matrix
                    // result = matrix_transform(IA->grille, shoot, size, joueur, IA);

                    // call function pointer to get the matrix of attack range
                    shoot = att_ptr(joueur->rep->x, joueur->rep->y, size);

                    result = standard_shoot_result(size, joueur, adversaire, joueur->rep->x, joueur->rep->y, adversaire->grille);

                    free_range_matrix(shoot, size);
                    // set function pointer back to null
                    att_ptr = NULL;
                }
                else
                {
                    printf("Entree invalide\n");
                    return play(size, joueur, adversaire);
                }
                //reset_reponse(joueur); // remember to reset the reponse
            }
            else
            {
                printf("Entree invalide\n");
                return play(size, joueur, adversaire);
            }
        }
    }
    return result; // return the winner, if NULL game continue, no winner yet
}
int initialization(PlayerPtr joueur, PlayerPtr IA);
void start_game(PlayerPtr joueur, PlayerPtr IA)
{
    PlayerPtr winner;
    int init, size, replay = 0, attempts = 25, playerFirst;
    do
    {
        playerFirst = rand()%2;
        size = initialization(joueur, IA);
        init = 0;
        winner = NULL;
        for (int i = 0; i < attempts && !init; i++)
        {
            if (!initPlayer(size, joueur, false))
            {
                init++;
            }
        }
        if (!init)
        {
            printf("La grille est trop petite pour placer tous les navires\n\n");
            replay = 1;
        }
        while (init == 1)
        {
            if (!initPlayer(size, IA, true))
            {
                init++;
            }
        }
        if (init == 2)
        {
            printf("\n\t\t------------------------------------------------------------------\n\t\t|  ");
            if(playerFirst) {
                printf("Debut de la partie, c'est vous qui commencez. Bonne chance !");
            }
            else {
                printf(" Debut de la partie, c'est l'IA qui commence. Bonne chance !");

            }
            printf("  |\n\t\t------------------------------------------------------------------\n");
            printGrilles(size, joueur->grille, IA->grille);
            while (!winner)
            {
                if(playerFirst) {
                    winner = play(size, joueur, IA);
                    printGrilles(size, joueur->grille, IA->grille);
                    if (!winner)
                    {
                        winner = play(size, IA, joueur);
                        printGrilles(size, joueur->grille, IA->grille);
                    }
                }
                else {
                    winner = play(size, IA, joueur);
                    printGrilles(size, joueur->grille, IA->grille);
                    if (!winner)
                    {
                        winner = play(size, joueur, IA);
                        printGrilles(size, joueur->grille, IA->grille);
                    }
                }

            }
            printf("\n\t\t------------------------------------------------------------------------\n\t\t|  ");
            if (winner == joueur)
            {
                printf("Vous avez gagne la partie, tous les navires adverses sont coules !");
            }
            else
            {
                printf("Vous avez perdu la partie, l'adversaire a coule tous vos navires !");
            }
            printf("  |\n\t\t------------------------------------------------------------------------\n");
            printf("\nVoulez vous rejouer ?\t 0: Non\t\t1:Oui\n");
            scanf("%d", &replay); //  commented while we are programming
        }
    } while (replay);
}

int initialization(PlayerPtr joueur, PlayerPtr IA)
{
    int size = 0;
    char input[4]; // maximum 3 digits
    while (size < 5 || size > 702)
    {
        printf("Entrer la taille de la grille (entre 5 et 702) : ");
        scanf("%s", input);
        size = validate_reponse(input, joueur, 0); // the third parameter size=0 is a flag check for rep->choice
        // when third parameter set to 0, it will return the valid numeric input
    }
    size++;
    return size;
}

int main()
{
    srand(time(NULL));
    PlayerPtr joueur = malloc(sizeof(Player));
    PlayerPtr IA = malloc(sizeof(Player));
    start_game(joueur, IA);

    return 0;
}
