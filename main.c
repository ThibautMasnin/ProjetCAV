#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>


typedef enum ship {Null, Close, PorteAvion, Croiseur, Destroyer, SousMarin, Torpilleur} Ship;

typedef struct box {
    bool shoted;
    Ship bateau;
} Box;

typedef struct player {
    Box **grille;
    char *name;
    bool lastShotSuccess;
    int porteAvion;
    int croiseur;
    int destroyer;
    int sousMarin;
    int torpilleur;
} Player;

typedef Player *PlayerPtr;

void initGrille(int size, Box grille[size][size]) {
    int i, j;
    for (i=0; i<size;i++) {
        for (j=0;j<size;j++) {
            grille[i][j].shoted=false;
            grille[i][j].bateau=Null;
        }
    }
}

int addShip(int size, Box grille[size][size], Ship bateau) {
    int x,y,lenght,tmp, attemps=10;
    size--;
    switch(bateau) {
        case PorteAvion:lenght=5;break;
        case Croiseur:lenght=4;break;
        case Destroyer:lenght=3;break;
        case SousMarin:lenght=3;break;
        case Torpilleur:lenght=2;break;
        default:lenght=size;break;
    }
    if(lenght>size) {
        return 1;
    }
    while(attemps--) {
        tmp=1;
        if(rand()%2) {
            x=rand()%(size-lenght+1)+1;
            y=rand()%(size)+1;
            if(!(grille[x][y].bateau)) {
                while(!(grille[x+tmp][y].bateau) && x+tmp<=size && tmp<lenght) {
                    tmp++;
                }
                if(tmp>=lenght) {
                    grille[x-1][y].bateau=Close;
                    grille[x-1][y-1].bateau=Close;
                    if(y+1<=size) {
                        grille[x-1][y+1].bateau=Close;
                    }
                    while(lenght--) {
                        grille[x][y].bateau=bateau;
                        grille[x][y-1].bateau=Close;
                        if(y+1<=size) {
                            grille[x][y+1].bateau=Close;
                        }
                        x++;
                    }
                    if(x<=size) {
                        grille[x][y].bateau=Close;
                        grille[x][y-1].bateau=Close;
                        if(y+1<=size) {
                            grille[x][y+1].bateau=Close;
                        }
                    }
                    return 0;
                }
            }
        }
        else {
            x=rand()%(size)+1;
            y=rand()%(size-lenght+1)+1;
            if(!(grille[x][y].bateau)) {
                while(!(grille[x][y+tmp].bateau) && y+tmp<=size && tmp<lenght) {
                    tmp++;
                }
                if(tmp>=lenght) {
                    grille[x][y-1].bateau=Close;
                    grille[x-1][y-1].bateau=Close;
                    if(x+1<=size) {
                        grille[x+1][y-1].bateau=Close;
                    }
                    while(lenght--) {
                        grille[x][y].bateau=bateau;
                        grille[x-1][y].bateau=Close;
                        if(x+1<=size) {
                            grille[x+1][y].bateau=Close;
                        }
                        y++;
                    }
                    if(y<=size) {
                        grille[x][y].bateau=Close;
                        grille[x-1][y].bateau=Close;
                        if(x+1<=size) {
                            grille[x+1][y].bateau=Close;
                        }
                    }
                    return 0;
                }
            }
        }
    }
    return 1;
}

int initPlayer(int size, PlayerPtr joueur, char* nom) {
    joueur->grille=malloc(size*size*sizeof(Box));
    initGrille(size, joueur->grille);
    joueur->name=nom;
    joueur->lastShotSuccess=false;
    joueur->porteAvion=5;
    joueur->croiseur=4;
    joueur->destroyer=3;
    joueur->sousMarin=3;
    joueur->torpilleur=2;
    if(addShip(size, joueur->grille, PorteAvion)) {
        return 1;
    }
    else if(addShip(size, joueur->grille, Croiseur)) {
        return 1;
    }
    else if(addShip(size, joueur->grille, Destroyer)) {
        return 1;
    }
    else if(addShip(size, joueur->grille, SousMarin)) {
        return 1;
    }
    else if(addShip(size, joueur->grille, Torpilleur)) {
        return 1;
    }
    return 0;
}

void printGrilles(int size, Box grilleJoueur[size][size], Box grilleIA[size][size]) {
    int i,j;
    printf("\nJoueur :  ");
    for(j=2;j<size;j++) {
        printf("    ");
    }
    printf("\t\t\tOrdinateur :\n +");
    for (j=1;j<size;j++) {
        for(j=0;j<2*size;j++) {
            if(j==size) {
                printf("\t\t\t+");
            }
            printf("---+");
        }
        printf("\n |   | ");
        for(j=1;j<size;j++) {
            if(j>26) {
                printf("%c%c| ", 'A'+(j-1)/26-1, 'A'+(j-1)%26);
            }
            else {
                printf("%c | ", 'A'+j-1);
            }
        }
        printf("\t\t\t|   | ");
        for(j=1;j<size;j++) {
            if(j>26) {
                printf("%c%c| ", 'A'+(j-1)/26-1, 'A'+(j-1)%26);
            }
            else {
                printf("%c | ", 'A'+j-1);
            }
        }
        printf("\n +");
    }
    for(i=1;i<size;i++) {
        for(j=0;j<2*size;j++) {
            if(j==size) {
                printf("\t\t\t+");
            }
            printf("---+");
        }
        printf("\n | ");
        if(i>9) {
            printf("%d| ", i);
        }
        else {
            printf("%d | ", i);
        }
        for(j=1;j<size;j++) {
            if(grilleJoueur[i][j].bateau>Close) {
                if(grilleJoueur[i][j].shoted) {
                    printf("# | ");
                }
                else {
                    printf("O | ");
                }
            }
            else {
                if(grilleJoueur[i][j].shoted) {
                    printf("X | ");
                }
                else {
                    printf("  | ");
                }
            }
        }
        printf("\t\t\t| ");
        if(i>9) {
            printf("%d| ", i);
        }
        else {
            printf("%d | ", i);
        }
        for(j=1;j<size;j++) {
            if(grilleIA[i][j].bateau>Close) {
                if(grilleIA[i][j].shoted) {
                    printf("# | ");
                }
                else {
                    printf("O | ");
                }
            }
            else {
                if(grilleIA[i][j].shoted) {
                    printf("X | ");
                }
                else {
                    printf("  | ");
                }
            }
        }
        printf("\n +");
    }
    for(j=0;j<2*size;j++) {
        if(j==size) {
            printf("\t\t\t+");
        }
        printf("---+");
    }
    printf("\n");
}

PlayerPtr shoot(int size, PlayerPtr tireur, PlayerPtr cible, int x, int y, Box grille[size][size]) {
    int *ptr;
    if(x>size-1 || y>size-1) {
        printf("Cible incorrecte !\n");
    }
    else {
        if(grille[x][y].bateau>Close && grille[x][y].shoted==false) {
            tireur->lastShotSuccess=true;
            switch(grille[x][y].bateau) {
               case PorteAvion:ptr=&(cible->porteAvion);break;
               case Croiseur:ptr=&(cible->croiseur);break;
               case Destroyer:ptr=&(cible->destroyer);break;
               case SousMarin:ptr=&(cible->sousMarin);break;
               case Torpilleur:ptr=&(cible->torpilleur);break;
           }
            printf("%d ", *ptr);
            (*ptr)--;
            printf("-> %d ", *ptr);
            if(*ptr) {
                printf("Touche !\n");
            }
            else {
                printf("Coule !\n");
                printf("%d ", !((cible->porteAvion)+(cible->croiseur)+(cible->destroyer)+(cible->sousMarin)+(cible->torpilleur)));
                if(!((cible->porteAvion)+(cible->croiseur)+(cible->destroyer)+(cible->sousMarin)+(cible->torpilleur))) {
                    grille[x][y].shoted=true;
                    return tireur;
                }
            }
        }
        else {
            tireur->lastShotSuccess=false;
            printf("Dans l'eau...\n");
        }
        grille[x][y].shoted=true;
    }
    return NULL;
}

PlayerPtr play(int size, PlayerPtr joueur, PlayerPtr IA) {
    int tmp=0;
    char cible[6], x[]="   ", y[]="  ", *ptrx=x, *ptry=y;
    printf("\nEntrer les coordonnees de la cible (ex: '3A') : ");
    scanf("%s", cible);
    if(!strcmp(cible, "stop")) {
        return IA;
    }
    else if(!strcmp(cible, "save")) {
        return IA;
    }
    else {
        for(int i=0; cible[i]!='\0'; i++) {
            if(cible[i]>='0' && cible[i]<='9') {
                if(*ptrx==' ') {
                    *ptrx=cible[i];
                    ptrx++;
                }
                else {
                    printf("Cible incorrecte !\n");
                    return play(size,joueur,IA);
                }
            }
            else if((cible[i]>='A' && cible[i]<='Z') || (cible[i]>='a' && cible[i]<='z')) {
                if(*ptry==' ') {
                    *ptry=cible[i];
                    ptry++;
                }
                else {
            printf("Cible incorrecte !\n");
                    return play(size,joueur,IA);
                }
            }
            else {
            printf("Cible incorrecte !\n");
                return play(size,joueur,IA);
            }
        }
        if(ptry==y || ptrx==x) {
            return play(size, joueur, IA);
        }
        while(ptry>y) {
            ptry--;
            if(*ptry>='A' && *ptry<='Z') {
                if(tmp) {
                    tmp+=(*ptry-64)*26;
                }
                else {
                    tmp=(*ptry-64);
                }
            }
            else {
                if(tmp) {
                    tmp+=(*ptry-96)*26;
                }
                else {
                    tmp=(*ptry-96);
                }
            }
        }
        printf("Cible : %d\n", tmp);
        return shoot(size, joueur, IA, atoi(x), tmp, IA->grille);
    }
}

int main() {
    srand (time (NULL));
    int size=0;
    PlayerPtr winner=NULL;
    while(size<5 || size>702) {
        printf("Entrer la taille de la grille (entre 5 et 702) : ");
        scanf("%d", &size);
    }
    size++;
    PlayerPtr joueur=malloc(sizeof(Player));
    PlayerPtr IA=malloc(sizeof(Player));
    if(initPlayer(size, joueur, "Joueur")) {
        printf("La grille est trop petite pour placer tous les navires de %s.", joueur->name);
    }
    else if(initPlayer(size, IA, "Ordinateur")) {
        printf("La grille est trop petite pour placer tous les navires de %s.", IA->name);
    }
    else {
        printGrilles(size, joueur->grille, IA->grille);
        while(!winner) {
            winner=play(size, joueur, IA);
            printGrilles(size, joueur->grille, IA->grille);
        }
        printf("\n%s a gagne la partie, tous les navires adverses sont coules !\n", winner->name);
    }
    return 0;
}
