#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
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

typedef enum ultimate
{
    standard,
    ligne, // sous-marrin
    croix, // croiseur
    plus,  // croisseur
    carre  // porte-avion

} Ultimate;

typedef struct box
{
    bool shot;
    Ship bateau;
} Box;

typedef struct reponse
{
    int num;
    char *alphbet;
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
    Ultimate ult;
    Reponse *rep;
} Player;

typedef Player *PlayerPtr;

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
                    grille[x - 1][y - 1].bateau = Close;
                    if (y + 1 <= size)
                    {
                        grille[x - 1][y + 1].bateau = Close;
                    }
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
                        grille[x][y - 1].bateau = Close;
                        if (y + 1 <= size)
                        {
                            grille[x][y + 1].bateau = Close;
                        }
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
                    grille[x - 1][y - 1].bateau = Close;
                    if (x + 1 <= size)
                    {
                        grille[x + 1][y - 1].bateau = Close;
                    }
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
                        grille[x - 1][y].bateau = Close;
                        if (x + 1 <= size)
                        {
                            grille[x + 1][y].bateau = Close;
                        }
                    }
                    return 0;
                }
            }
        }
    }
    return 1;
}

int initPlayer(int size, PlayerPtr joueur)
{
    joueur->grille = malloc(size * sizeof(Box *));
    for (int i = 0; i < size; i++)
    {
        joueur->grille[i] = malloc(size * sizeof(Box));
    }

    initGrille(size, joueur->grille);
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
    joueur->rep->num = 0;
    joueur->rep->alphbet = calloc(3, sizeof(char)); // maximum 2 letters acceptable

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
int validate_reponse(char *s, PlayerPtr p)
{
    regex_t regex;
    regex_t regex2;
    regex_t regex3;
    int value, value2, value3;
    char num[4];
    int s_counter = 0;
    int n_counter = 0;
    // create regex
    value = regcomp(&regex, "[1-9][0-9]{0,2}[a-zA-Z]{1,2}", REG_EXTENDED);
    value2 = regcomp(&regex2, "[a-zA-Z]{1,2}[1-9][0-9]{0,2}", REG_EXTENDED);

    // regex3 to match only letters or only numbers
    value3 = regcomp(&regex3, "([a-zA-Z]{1,2}|[1-9][0-9]{0,2}){1}", REG_EXTENDED);

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
    // if it matches the pattern
    if (value == 0 || value2 == 0 || value3 == 0)
    {
        while (*s)
        {
            if (isalpha(*s))
            {
                p->rep->alphbet[s_counter++] = *s;
            }
            else if (isdigit(*s))
            {
                num[n_counter++] = *s;
            }

            s++;
        }
        num[n_counter] = '\0';
        p->rep->alphbet[s_counter] = '\0';
        if (strlen(num) > 0)
        {
            p->rep->num = atoi(num);
        }
        return 1; // response valid and update the fileds of player
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
    p->rep->num = 0;
    for (int i = 0; i < 2; i++)
    {
        p->rep->alphbet[i] = '\0';
    }
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
    int choix;
    char conversation[6];
    int x, y;
    int result;
    // function pointers
    Box **(*att_ptr)(int, int, int);
    // display the special shots that the player has
    printf("\nEntrer 0 pour ne pas utilier ultimate | ");
    if (tireur->tirLigne && tireur->sousMarin != 0)
    {
        printf("Entrer 1 pour tireLigne |");
    }
    if (tireur->tirCroix && tireur->croiseur != 0)
    {
        printf("Entrer 2 pour tireCroix | ");
    }
    if (tireur->tirPlus && tireur->croiseur != 0)
    {
        printf("Entrer 3 pour tirePlus | ");
    }
    if (tireur->tirCarre && tireur->porteAvion != 0)
    {
        printf("Entrer 4 pour tireCarre ");
    }
    //----------------------
    printf("\n");
    scanf("%d", &choix);
    switch (choix)
    {
    case 0:
        break;
    case 1:
        if (tireur->tirLigne && tireur->sousMarin != 0)
        {

            printf("Tir en ligne : Entrer une lettre pour une colonne, un chiffre pour un rang ");
            // escape the newline from the previous scanf
            scanf(" %s", conversation);
            if (validate_reponse(conversation, tireur))
            {

                // it means input contains numbers and letters at the same time
                // return errors
                if (tireur->rep->num != 0 && strlen(tireur->rep->alphbet) > 0)
                {
                    printf("entrée invalide");
                    return special_shot(size, tireur, cible, grille);
                }
                else
                {
                    att_ptr = &range_ligne;

                    // mark matrice
                    if (tireur->rep->num != 0)
                    {
                        x = tireur->rep->num;
                        // entire row
                        // call function pointer to get the matrix of attack range
                        Box **ult = att_ptr(x, 0, size);
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
                    else if (strlen(tireur->rep->alphbet) > 0)
                    {
                        y = convert_letters(tireur->rep->alphbet);
                        Box **ult = att_ptr(0, y, size);
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
                printf("entrée invalide");
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

            printf("Tir en Croix : les coordonnees du centre de ce tir special(ex: '3A'): ");
            scanf(" %s", conversation);
            if (validate_reponse(conversation, tireur))
            {
                // contains one number and at least one letter
                if (tireur->rep->num != 0 && strlen(tireur->rep->alphbet) > 0)
                {
                    // function pointers

                    att_ptr = &range_croix;
                    // mark matrice
                    x = tireur->rep->num;
                    y = convert_letters(tireur->rep->alphbet);
                    // entire row
                    // call function pointer to get the matrix of attack range
                    Box **ult = att_ptr(x, y, size);
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
                    printf("entrée invalide");
                    return special_shot(size, tireur, cible, grille);
                }
            }
            else
            {
                printf("entrée invalide");
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

            printf("Tir en Plus : les coordonnees de du centre de ce tir special(ex: '3A'): ");
            scanf(" %s", conversation);
            if (validate_reponse(conversation, tireur))
            {
                // contains one number and at least one letter
                if (tireur->rep->num != 0 && strlen(tireur->rep->alphbet) > 0)
                {
                    // function pointers

                    att_ptr = &range_plus;
                    // mark matrice
                    x = tireur->rep->num;
                    y = convert_letters(tireur->rep->alphbet);
                    // entire row
                    // call function pointer to get the matrix of attack range
                    Box **ult = att_ptr(x, y, size);
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
                    printf("entrée invalide");
                    return special_shot(size, tireur, cible, grille);
                }
            }
            else
            {
                printf("entrée invalide");
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

            printf("Tir en Carre : les coordonnees de du centre de ce tir special(ex: '3A'): ");
            scanf(" %s", conversation);
            if (validate_reponse(conversation, tireur))
            {
                // contains one number and at least one letter
                if (tireur->rep->num != 0 && strlen(tireur->rep->alphbet) > 0)
                {
                    // function pointers

                    att_ptr = &range_carre;
                    // mark matrice
                    x = tireur->rep->num;
                    y = convert_letters(tireur->rep->alphbet);
                    // entire row
                    // call function pointer to get the matrix of attack range
                    Box **ult = att_ptr(x, y, size);
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
                    printf("entrée invalide");
                    return special_shot(size, tireur, cible, grille);
                }
            }
            else
            {
                printf("entrée invalide");
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
        printf("entrée invalide");
        return special_shot(size, tireur, cible, grille);
        break;
    }
    return NULL;
}

// standard shoot
PlayerPtr shoot(int size, PlayerPtr tireur, PlayerPtr cible, int x, int y, Box **grille)
{
    int *ptr;
    // int result = status_report(x, y, grille, tireur, cible);
    if (x > size - 1 || y > size - 1)
    {
        printf("Cible incorrecte !\n");
    }
    else
    {
        if (grille[x][y].bateau > Close && grille[x][y].shot == false)
        {
            tireur->lastShotSuccess = true;
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

                printGrilles(size, tireur->grille, cible->grille);
                // ask to use special shots
                return special_shot(size, tireur, cible, grille);
            }
            else
            {
                printf("Coule !\n");
                printGrilles(size, tireur->grille, cible->grille);

                // if all boats are hit, becomes 0
                if (!((cible->porteAvion) + (cible->croiseur) + (cible->destroyer) + (cible->sousMarin) + (cible->torpilleur)))
                {
                    // return the winner
                    return tireur;
                }
                else
                {
                    // ask to use special shots
                    return special_shot(size, tireur, cible, grille);
                }
            }
        }
        else
        {
            tireur->lastShotSuccess = false;
            grille[x][y].shot = true;
            printf("Dans l'eau...\n");
        }
    }
    return NULL;
}

PlayerPtr play(int size, PlayerPtr joueur, PlayerPtr IA)
{
    int tmp = 0;
    char cible[6], x[] = "   ", y[] = "  ", *ptrx = x, *ptry = y;
    //-----------------regex----------------------------------------
    regex_t regex;
    regex_t regex2;
    int value, value2;
    // create regex
    value = regcomp(&regex, "[1-9][0-9]{0,2}[a-zA-Z]{1,2}", REG_EXTENDED);
    value2 = regcomp(&regex2, "[a-zA-Z]{1,2}[1-9][0-9]{0,2}", REG_EXTENDED);

    if (value == 0 && value2 == 0)
    {
        // printf("RegEx compiled successfully.\n");
    }
    else
    {
        printf("Compilation error.");
    }
    //------------------------------------------------------

    printf("\nEntrer les coordonnees de la cible (ex: '3A') : ");
    scanf("%s", cible);
    // strcpy(cible, "3cc");
    if (!strcmp(cible, "stop"))
    {
        return IA;
    }
    else if (!strcmp(cible, "save"))
    {
        return IA;
    }
    else
    {
        value = regexec(&regex, cible, 0, NULL, 0);
        value2 = regexec(&regex2, cible, 0, NULL, 0);
        // if it matches the pattern
        if (value == 0 || value2 == 0)
        {

            for (int i = 0; cible[i] != '\0'; i++)
            {
                if (cible[i] >= '0' && cible[i] <= '9')
                {
                    if (*ptrx == ' ')
                    {
                        *ptrx = cible[i];
                        ptrx++;
                    }
                    else
                    {
                        printf("Cible incorrecte !\n");
                        return play(size, joueur, IA);
                    }
                }
                else if ((cible[i] >= 'A' && cible[i] <= 'Z') || (cible[i] >= 'a' && cible[i] <= 'z'))
                {
                    if (*ptry == ' ')
                    {
                        *ptry = cible[i];
                        ptry++;
                    }
                    else
                    {
                        printf("Cible incorrecte !\n");
                        return play(size, joueur, IA);
                    }
                }
                else
                {
                    printf("Cible incorrecte !\n");
                    return play(size, joueur, IA);
                }
            }
        }
        if (ptry == y || ptrx == x)
        {
            return play(size, joueur, IA);
        }
        while (ptry > y)
        {
            ptry--;
            if (*ptry >= 'A' && *ptry <= 'Z')
            {
                if (tmp)
                {
                    tmp += (*ptry - 64) * 26;
                }
                else
                {
                    tmp = (*ptry - 64);
                }
            }
            else
            {
                if (tmp)
                {
                    tmp += (*ptry - 96) * 26;
                }
                else
                {
                    tmp = (*ptry - 96);
                }
            }
        }
        return shoot(size, joueur, IA, atoi(x), tmp, IA->grille);
    }
}

void start_game(int size, int attempts, PlayerPtr joueur, PlayerPtr IA)
{
    PlayerPtr winner;
    int init, replay = 0;
    do
    {
        init = 0;
        winner = NULL;
        for (int i = 0; i < attempts && !init; i++)
        {
            if (initPlayer(size, joueur))
            {
                printf("La grille est trop petite pour placer tous vos navires.\n");
            }
            else
            {
                init++;
            }
        }
        while (init == 1)
        {
            if (initPlayer(size, IA))
            {
                printf("La grille est trop petite pour placer tous les navires adverses.\n");
            }
            else
            {
                init++;
            }
        }
        if (init == 2)
        {
            printGrilles(size, joueur->grille, IA->grille);
            while (!winner)
            {
                winner = play(size, joueur, IA);
                printGrilles(size, joueur->grille, IA->grille);
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
            scanf("%d", &replay);
        }
    } while (replay);
}

int initialization(PlayerPtr joueur, PlayerPtr IA)
{
    int size = 0;
    while (size < 5 || size > 702)
    {

        printf("Entrer la taille de la grille (entre 5 et 702) : ");
        scanf("%d", &size);
    }
    size++;
    return size;
}

int main()
{
    srand(time(NULL));
    int attempts = 10, size;
    PlayerPtr joueur = malloc(sizeof(Player));
    PlayerPtr IA = malloc(sizeof(Player));
    size = initialization(joueur, IA);
    start_game(size, attempts, joueur, IA);

    return 0;
}
