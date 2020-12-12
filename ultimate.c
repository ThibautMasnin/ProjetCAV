#include "ultimate.h"
#include "game.h"

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
