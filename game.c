#include "game.h"

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