#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

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

typedef struct box
{
    bool shot;
    Ship bateau;
} Box;

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
    int x, y, length, tmp, attemps = 50;
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
    while (attemps--)
    {
        tmp = 1;
        if (rand() % 2)
        {
            x = rand() % (size - length + 1) + 1;
            y = rand() % (size) + 1;
            if (!(grille[x][y].bateau))
            {
                while (!(grille[x + tmp][y].bateau) && x + tmp <= size && tmp < length)
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

PlayerPtr shoot(int size, PlayerPtr tireur, PlayerPtr cible, int x, int y, Box **grille)
{
    int *ptr;
    if (x > size - 1 || y > size - 1)
    {
        printf("Cible incorrecte !\n");
    }
    else
    {
        if (grille[x][y].bateau > Close && grille[x][y].shot == false)
        {
            tireur->lastShotSuccess = true;
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
            // need to handle null and close, temporary fix
            default:
                // do sth here
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
                printf("Coule !\n");
                if (!((cible->porteAvion) + (cible->croiseur) + (cible->destroyer) + (cible->sousMarin) + (cible->torpilleur)))
                {
                    grille[x][y].shot = true;
                    return tireur;
                }
            }
        }
        else
        {
            tireur->lastShotSuccess = false;
            printf("Dans l'eau...\n");
        }
        grille[x][y].shot = true;
    }
    return NULL;
}

PlayerPtr play(int size, PlayerPtr joueur, PlayerPtr IA)
{
    int tmp = 0;
    char cible[6], x[] = "   ", y[] = "  ", *ptrx = x, *ptry = y;
    printf("\nEntrer les coordonnees de la cible (ex: '3A') : ");
    scanf("%s", cible);
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

int main()
{
    srand(time(NULL));
    int i, init, size = 0, replay = 0, attemps = 10;
    PlayerPtr winner;
    while (size < 5 || size > 702)
    {
        printf("Entrer la taille de la grille (entre 5 et 702) : ");
        scanf("%d", &size);
    }
    size++;
    PlayerPtr joueur = malloc(sizeof(Player));
    PlayerPtr IA = malloc(sizeof(Player));
    do
    {
        init = 0;
        winner = NULL;
        for (i = 0; i < attemps && !init; i++)
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
    return 0;
}
