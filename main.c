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
            grille[i][j].shot = false;
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
    joueur->bot=bot;
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
    if(bot) {
        joueur->etat=malloc(sizeof(State));
        joueur->etat->x=0;
        joueur->etat->y=0;
        joueur->etat->sens=Inconnue;
    }
    else {
        joueur->etat=NULL;
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
    printf("\n");
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
        printf("Compilation error.");
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

int status_report(int x, int y, Box **grille, PlayerPtr tireur, PlayerPtr cible);
int matrix_transform(Box **grille, Box **ult, int size, PlayerPtr tireur, PlayerPtr cible)
{
    int result = 0;
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            // mark the shot cell of ultimate to the grille in status_report
            if (ult[i][j].shot && !grille[i][j].shot)
            {
                result = status_report(i, j, grille, tireur, cible);
                if (result)
                {
                    // result=1, game ends, we got a winner
                    return result;
                }
            }
        }
    }
    return result; //0: game does not end
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

int status_report(int x, int y, Box **grille, PlayerPtr tireur, PlayerPtr cible)
{
    int *ptr;

    if (grille[x][y].bateau > Close && grille[x][y].shot == false)
    {
        grille[x][y].shot = true;

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
        printf("%d ", *ptr);
        (*ptr)--;
        printf("-> %d ", *ptr);
        if (*ptr)
        {
            printf("Touche !\n");
        }
        else
        {
            printf("Coule: !\n");
            // if all boats are hit, becomes 0
            if (!((cible->porteAvion) + (cible->croiseur) + (cible->destroyer) + (cible->sousMarin) + (cible->torpilleur)))
            {

                return 1; // return tireur as winner
            }
        }
    }
    else
    {
        printf("Dans l'eau...\n");
        grille[x][y].shot = true;
    }
    return 0; // game does not end yet.
}

PlayerPtr special_shot(int size, PlayerPtr tireur, PlayerPtr cible, Box **grille)
{
    int choix = 0;
    char conversation[6];
    char input[4]; // temp holder of choix
    int result;
    int check = 0; // check if there are special shots left
    // function pointers
    Box **(*att_ptr)(int, int, int);

    tireur->lastShotSuccess=false;

    // display the special shots that the player has
    printf("\nChoisissez le type de tir :\n");
    if (tireur->tirLigne && tireur->sousMarin != 0)
    {
        printf("1 - Tir en ligne (%d cases)\n", size-1);
        check++;
    }
    if (tireur->tirCroix && tireur->croiseur != 0)
    {
        printf("2 - Tir en croix (5 cases)\n");
        check++;
    }
    if (tireur->tirPlus && tireur->croiseur != 0)
    {
        printf("3 - Tir en plus (5 cases)\n");
        check++;
    }
    if (tireur->tirCarre && tireur->porteAvion != 0)
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

    choix = validate_reponse(input, tireur, 0);
    // if choix is still 0 after input
    if (!choix)
    {
        printf("entree invalide\n");
        return special_shot(size, tireur, cible, grille);
    }

    // reset before taking new coordinates
    reset_reponse(tireur);

    switch (choix)
    {
    case 5: // no need to deal with input starting from 0
        return play(size, tireur, cible);
        break;
    case 1:
        if (tireur->tirLigne && tireur->sousMarin != 0)
        {

            printf("Tir en ligne : Entrer une lettre pour une colonne, un chiffre pour un rang\n");
            // escape the newline from the previous scanf
            scanf(" %s", conversation);
            if (validate_reponse(conversation, tireur, size))
            {

                // if it contains numbers and letters at the same time, return errors
                if (tireur->rep->x != 0 && tireur->rep->y != 0)
                {
                    printf("entree invalide\n");
                    return special_shot(size, tireur, cible, grille);
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
                        result = matrix_transform(grille, ult, size, tireur, cible);

                        free_range_matrix(ult, size);
                        att_ptr = NULL;

                        // tirLigne used
                        tireur->tirLigne = 0;
                        if (result)
                        {
                            return tireur; // return the winner
                        }
                    }
                    else if (tireur->rep->y != 0)
                    {

                        Box **ult = att_ptr(0, tireur->rep->y, size);
                        // entire column
                        result = matrix_transform(grille, ult, size, tireur, cible);

                        free_range_matrix(ult, size);
                        att_ptr = NULL;
                        tireur->tirLigne = 0;
                        if (result)
                        {
                            return tireur; // return the winner
                        }
                    }
                }
            }
            else
            {
                printf("entree invalide\n");
                return special_shot(size, tireur, cible, grille);
            }
        }
        else
        {
            printf("Pas de tir en ligne");
            return special_shot(size, tireur, cible, grille);
        }
        break;
    case 2:
        if (tireur->tirCroix && tireur->croiseur != 0)
        {

            printf("Tir en Croix : les coordonnees du centre de ce tir special(ex: '3A'):\n");
            scanf("%s", conversation);
            if (validate_reponse(conversation, tireur, size))
            {
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
                    result = matrix_transform(grille, ult, size, tireur, cible);
                    free_range_matrix(ult, size);
                    // set function pointer back to null
                    att_ptr = NULL;

                    // tirCroix set to 'used'
                    tireur->tirCroix = 0;
                    if (result)
                    {
                        return tireur; // return the winner
                    }
                }
                else
                {
                    printf("entree invalide\n");
                    return special_shot(size, tireur, cible, grille);
                }
            }
            else
            {
                printf("entree invalide\n");
                return special_shot(size, tireur, cible, grille);
            }
        }
        else
        {
            printf("Pas de tir en croix");
            return special_shot(size, tireur, cible, grille);
        }
        break;
    case 3:
        if (tireur->tirPlus && tireur->croiseur != 0)
        {

            printf("Tir en Plus : les coordonnees de du centre de ce tir special(ex: '3A'):\n");
            scanf(" %s", conversation);
            if (validate_reponse(conversation, tireur, size))
            {
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
                    result = matrix_transform(grille, ult, size, tireur, cible);
                    free_range_matrix(ult, size);
                    // set function pointer back to null
                    att_ptr = NULL;

                    // tirCroix set to 'used'
                    tireur->tirPlus = 0;
                    if (result)
                    {
                        return tireur; // return the winner
                    }
                }
                else
                {
                    printf("entree invalide\n");
                    return special_shot(size, tireur, cible, grille);
                }
            }
            else
            {
                printf("entree invalide\n");
                return special_shot(size, tireur, cible, grille);
            }
        }
        else
        {
            printf("Pas de tir en plus");
            return special_shot(size, tireur, cible, grille);
        }
        break;
    case 4:
        if (tireur->tirCarre && tireur->porteAvion != 0)
        {
            printf("Tir en Carre : les coordonnees de du centre de ce tir special(ex: '3A'):\n");
            scanf(" %s", conversation);
            if (validate_reponse(conversation, tireur, size))
            {
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
                    result = matrix_transform(grille, ult, size, tireur, cible);
                    free_range_matrix(ult, size);
                    // set function pointer back to null
                    att_ptr = NULL;

                    // tirCroix set to 'used'
                    tireur->tirCarre = 0;
                    if (result)
                    {
                        return tireur; // return the winner
                    }
                }
                else
                {
                    printf("entree invalide\n");
                    return special_shot(size, tireur, cible, grille);
                }
            }
            else
            {
                printf("entree invalide\n");
                return special_shot(size, tireur, cible, grille);
            }
        }
        else
        {
            printf("Pas de tir en carre");
            return special_shot(size, tireur, cible, grille);
        }
        break;
    default:
        printf("entree invalide\n");
        return special_shot(size, tireur, cible, grille);
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
        if(tireur->bot) {
            if(tireur->etat->x) {
                if(tireur->etat->x!=x) {
                    tireur->etat->sens=Vertical;
                }
                else {
                    tireur->etat->sens=Horizontal;
                }
            }
            else {
                tireur->etat->x=x;
                tireur->etat->y=y;
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
        printf("%d ", *ptr);
        (*ptr)--;
        printf("-> %d ", *ptr);
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
                if(tireur->bot) {
                    tireur->etat->x=0;
                    tireur->etat->y=0;
                    tireur->etat->sens=Inconnue;
                }
                return NULL;
            }
        }
    }
    else
    {
        if(tireur->bot) {
            if(tireur->etat->x) {
                tireur->rep->x=tireur->etat->x;
                tireur->rep->y=tireur->etat->y;
            }
        }
        tireur->lastShotSuccess = false;
        grille[x][y].shot = true;
        printf("Dans l'eau...\n");
    }

    return NULL;
}

PlayerPtr play(int size, PlayerPtr joueur, PlayerPtr adversaire)
{
    int i=1, j=1;
    char coordonnees[6];
    PlayerPtr result=NULL;
    Box **(*att_ptr)(int, int, int);
    //------------------------------------------------------
    if(joueur->bot) {
        /*if(joueur->lastShotSuccess) {
            printf("\n[%d;%d]\n", joueur->etat->x, joueur->etat->y);
            //result = special_shot(size, joueur, adversaire, adversaire->grille);
            joueur->lastShotSuccess=false;
        }
        else { */
            if(joueur->etat->x) {
                if(joueur->etat->sens==Horizontal) {
                    if(adversaire->grille[joueur->rep->x][joueur->rep->y].bateau > Close && joueur->rep->y+1<size && !adversaire->grille[joueur->rep->x][joueur->rep->y+1].shot) {
                        i=joueur->rep->x;
                        j=joueur->rep->y+1;
                    }
                    else {
                        i=joueur->rep->x;
                        j=joueur->rep->y-1;
                    }
                }
                else if(joueur->etat->sens==Vertical) {
                    if(adversaire->grille[joueur->rep->x][joueur->rep->y].bateau > Close && joueur->rep->x+1<size && !adversaire->grille[joueur->rep->x+1][joueur->rep->y].shot) {
                        i=joueur->rep->x+1;
                        j=joueur->rep->y;
                    }
                    else {
                        i=joueur->rep->x-1;
                        j=joueur->rep->y;
                    }
                }
                else {
                    printf("\n[%d;%d]\n", joueur->etat->x, joueur->etat->y);
                    if(joueur->etat->y+1<size && !adversaire->grille[joueur->etat->x][joueur->etat->y+1].shot) {
                        i=joueur->etat->x;
                        j=joueur->etat->y+1;
                    }
                    else if(joueur->etat->x+1<size && !adversaire->grille[joueur->etat->x+1][joueur->etat->y].shot) {
                        i=joueur->etat->x+1;
                        j=joueur->etat->y;
                    }
                    else if(joueur->etat->y-1>0 && !adversaire->grille[joueur->etat->x][joueur->etat->y-1].shot) {
                        i=joueur->etat->x;
                        j=joueur->etat->y-1;
                    }
                    else if(joueur->etat->x-1>0 && !adversaire->grille[joueur->etat->x-1][joueur->etat->y].shot) {
                        i=joueur->etat->x-1;
                        j=joueur->etat->y;
                    }
                    else {
                        printf("Erreur de l'IA\n");
                        exit(EXIT_FAILURE);
                    }
                }
            }
            else {
                while(adversaire->grille[i-1][j].shot || adversaire->grille[i][j].shot || (i+1<size && adversaire->grille[i+1][j].shot) || (j+1<size && adversaire->grille[i][j+1].shot) || adversaire->grille[i][j-1].shot) {
                    printf("%d,",i);
                    if(i+2>size) {
                        i=1;
                        j++;
                    }
                    else {
                        i++;
                    }
                }
            }
            joueur->rep->x=i;
            joueur->rep->y=j;
            result = standard_shoot_result(size, joueur, adversaire, i, j, adversaire->grille);
            printf("\n\n!!!!!!!!!!!!!!!!!!!!!!!!!! TOUR DU BOT : (%d,%d) !!!!!!!!!!!!!!!!!!!!!\n\n", i, j);
        //}
    }
    else {
        reset_reponse(joueur);
        if(joueur->lastShotSuccess) {
            result = special_shot(size, joueur, adversaire, adversaire->grille);
        }
        else {
            printf("\nEntrer les coordonnees de la cible (ex: '3A') : ");
            scanf("%s", coordonnees);
            if (!strcmp(coordonnees, "stop"))
            {
                return adversaire;
            }
            else if (!strcmp(coordonnees, "save"))
            {
                return adversaire;
            }

            if (validate_reponse(coordonnees, joueur, size))
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
                    printf("entree invalide\n");
                    return play(size, joueur, adversaire);
                }
                //reset_reponse(joueur); // remember to reset the reponse
            }
            else
            {
                printf("entree invalide\n");
                return play(size, joueur, adversaire);
            }
        }
    }
    return result; // return the winner, if NULL game continue, no winner yet
}

void start_game(PlayerPtr joueur, PlayerPtr IA)
{
    PlayerPtr winner;
    int init, size, replay = 0, attempts = 25;
    do
    {
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
        if(!init) {
            printf("La grille est trop petite pour placer tous les navires\n\n");
            replay=1;
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
            printGrilles(size, joueur->grille, IA->grille);
            while (!winner)
            {
                //winner = play(size, joueur, IA);
                //printGrilles(size, joueur->grille, IA->grille);
                if(!winner) {
                    winner = play(size, IA, joueur);
                    printGrilles(size, joueur->grille, IA->grille);
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
            scanf("%d", &replay);     //  commented while we are programming
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
