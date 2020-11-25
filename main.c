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

void addShip(int size, Box grille[size][size], Ship bateau) {
    int x,y,lenght,tmp, attemps=100;
    size--;
    if(bateau==PorteAvion) {
        lenght=5;
    }
    else if(bateau==Croiseur) {
        lenght=4;
    }
    else if(bateau==Destroyer) {
        lenght=3;
    }
    else if(bateau==SousMarin) {
        lenght=3;
    }
    else if(bateau==Torpilleur) {
        lenght=2;
    }
    if(lenght>size) {
        printf("\n/!\\ LA GRILLE EST TROP PETITE /!\\\n");
        return;
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
                    grille[x-1][y+1].bateau=Close;
                    while(lenght--) {
                        grille[x][y].bateau=PorteAvion;
                        grille[x][y-1].bateau=Close;
                        grille[x][y+1].bateau=Close;
                        x++;
                    }
                    grille[x][y].bateau=Close;
                    grille[x][y-1].bateau=Close;
                    grille[x][y+1].bateau=Close;
                    return;
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
                    grille[x+1][y-1].bateau=Close;
                    while(lenght--) {
                        grille[x][y].bateau=PorteAvion;
                        grille[x+1][y].bateau=Close;
                        grille[x-1][y].bateau=Close;
                        y++;
                    }
                    grille[x][y].bateau=Close;
                    grille[x+1][y].bateau=Close;
                    grille[x-1][y].bateau=Close;
                    return;
                }
            }
        }
    }
    printf("\n/!\\ AUCUNE PLACE N'A ETE TROUVEE POUR LE NAVIRE DE TAILLE %d /!\\\n", lenght);
}

void initPlayer(int size, PlayerPtr joueur) {
    joueur->grille=malloc(size*size*sizeof(Box));
    initGrille(size, joueur->grille);
    joueur->lastShotSuccess=false;
    joueur->porteAvion=5;
    joueur->croiseur=4;
    joueur->destroyer=3;
    joueur->sousMarin=3;
    joueur->torpilleur=2;
    addShip(size, joueur->grille, PorteAvion);
    addShip(size, joueur->grille, Croiseur);
    addShip(size, joueur->grille, Destroyer);
    addShip(size, joueur->grille, SousMarin);
    addShip(size, joueur->grille, Torpilleur);
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

bool play(int size, PlayerPtr joueur, PlayerPtr IA) {
    int tmp1=0, tmp2=0;
    char cible[6], x[]="   ", y[]="  ", *ptrx=x, *ptry=y;
    printf("\nEntrer les coordonnees de la cible (ex: '3A') : ");
    scanf("%s", cible);
    if(!strcmp(cible, "stop")) {
        return true;
    }
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
    while(ptry>y) {
        ptry--;
        if(*ptry>='A' && *ptry<='Z') {
            if(tmp1) {
                tmp2+=(*ptry-64)*tmp1*26;
            }
            else {
                tmp2=(*ptry-64);
            }
        }
        else {
            if(tmp1) {
                tmp2+=(*ptry-96)*tmp1*26;
            }
            else {
                tmp2=(*ptry-96);
            }
        }
        tmp1++;
    }
    printf("Cible : (%d,%d)\n", atoi(x), tmp2);
    printGrilles(size, joueur->grille, IA->grille);
    printf("FIN DU TOUR\n");
    return false;
}

int main() {
    srand (time (NULL));
    int size=0;
    bool over=false;
    while(size<5 || size>702) {
        printf("Entrer la taille de la grille (entre 5 et 702) : ");
        scanf("%d", &size);
    }
    size++;
    PlayerPtr joueur=malloc(sizeof(Player));
    PlayerPtr IA=malloc(sizeof(Player));
    initPlayer(size, joueur);
    initPlayer(size, IA);
    printGrilles(size, joueur->grille, IA->grille);
    while(!over) {
        over=play(size, joueur, IA);
    }
    return 0;
}
