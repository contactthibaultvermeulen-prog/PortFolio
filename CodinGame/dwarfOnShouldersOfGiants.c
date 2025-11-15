
#include <stdio.h>
#include <stdlib.h>

struct maillon{
    int val;
    struct maillon * child;
    struct maillon * sibling;
};

struct liste2chainee{
    struct maillon * debut;
};

typedef struct maillon maillon;
typedef struct liste2chainee liste2chainee;
typedef struct Element Element;

struct Element {
    maillon *maillon;
    Element *suivant;
};

// Fonction pour ajouter un élément à la liste
void ajouter_element(Element **debut, maillon *maillon) {
    Element *element = (Element*)malloc(sizeof(Element));
    element->maillon = maillon;
    element->suivant = *debut;
    *debut = element;
}

// Fonction récursive pour parcourir l'arbre et ajouter les maillons dont la valeur est x à la liste
void parcourir_arbre(maillon *maillon, int x, Element **debut) {
    if (maillon == NULL) {
        return;
    }
    if (maillon->val == x) {
        ajouter_element(debut, maillon);
    }
    parcourir_arbre(maillon->sibling, x, debut);
    parcourir_arbre(maillon->child, x, debut);
}

// Fonction principale qui appelle la fonction de parcours et renvoie la liste
Element* trouver_maillons(maillon *pSurX, int x) {
    Element *debut = NULL;
    parcourir_arbre(pSurX, x, &debut);
    return debut;
}

maillon * chercher(int x, maillon * courant) {
    if (courant == NULL) {
        return NULL; // élément non trouvé
    }
    if (courant->val == x) {
        return courant; // élément trouvé
    }
    maillon * resultat = chercher(x, courant->child);
    if (resultat != NULL) {
        return resultat;
    }
    return chercher(x, courant->sibling); // passer au frère suivant
}



maillon * copy(maillon * pSurM){
   if (pSurM == NULL){
    return NULL;
   } else {
    maillon * new = (maillon *) malloc(sizeof(maillon));
    new->val = pSurM->val;
    new->sibling = copy(pSurM->sibling);
    new->child = copy(pSurM->child);
    return new;
   }
}

void destruction (maillon * pSurM){
    if (pSurM != NULL){
        destruction(pSurM->child);
        destruction(pSurM->sibling);
        free(pSurM);
    }
}

void ajoute(int x, int y, liste2chainee * pSurListe){
    maillon * pSurX;
    maillon * pSurY;
    if (pSurListe->debut == NULL){
        pSurY = (maillon *) malloc(sizeof(maillon));
        pSurY->val = y;
        pSurY->sibling = NULL;
        pSurY->child = NULL;
        pSurX = (maillon *) malloc(sizeof(maillon));
        pSurX->val = x;
        pSurX->sibling = NULL;
        pSurX->child = pSurY;
        pSurListe->debut = pSurX;
    } else {
        maillon * pSurM;
        pSurM = pSurListe->debut;
        if (chercher(y, pSurM) != NULL){
            pSurY = copy(chercher(y, pSurM));
        }else{
            pSurY = (maillon *) malloc(sizeof(maillon));
            pSurY->val = y;
            pSurY->sibling = NULL;
            pSurY->child = NULL;
        }
        if (chercher(x, pSurM) == NULL){
            pSurX = (maillon *) malloc(sizeof(maillon));
            pSurX->val = x;
            pSurX->sibling = NULL;
            pSurX->child = pSurY;
            while (pSurM->sibling != NULL){
                pSurM = pSurM->sibling;
            }
            pSurM->sibling = pSurX;
        } else {
            Element * liste;
            Element * pred;
            maillon* pTemp;
            maillon* pChild;
            maillon * new;
            liste = trouver_maillons(pSurM, x);
            do{
                new = copy(pSurY);
                pTemp = liste->maillon;
                pChild = pTemp->child;
                if (pChild == NULL){
                    pTemp->child = new;
                }else{
                    while(pChild->sibling != NULL){
                        pChild = pChild->sibling;
                    }
                    pChild->sibling = new;
                }
                pred = liste;
                liste = liste->suivant;
                free(pred);
            } while(liste != NULL);
            destruction(pSurY);
        }
    }
}

liste2chainee * creerListeVide(){
    liste2chainee * pSurListe;
    pSurListe = (liste2chainee *) malloc(sizeof(liste2chainee));
    pSurListe->debut = NULL;
    return pSurListe;
}

void afficher(maillon * pSurM){
    maillon * pTemp;
    while (pSurM != NULL){
        pTemp = pSurM;
        while (pTemp != NULL){
            fprintf(stderr, "%d -> ", pTemp->val);
            if (pTemp->child != NULL){
            }
            pTemp = pTemp->sibling;
        }
        fprintf(stderr, "NULL\n");
        fprintf(stderr, "v\n");
        pSurM = pSurM->child;
    }
    fprintf(stderr, "NULL\n");
}


float max(float a, float b){
  return a>b?a:b;
}

int compter_etages(maillon * pSurM) {
    if (pSurM == NULL) {
        return 0;
    } else {
        int max_etages_fils = 0;
        struct maillon* fils_courant = pSurM->child;
        while (fils_courant != NULL) {
            int etages_fils_courant = compter_etages(fils_courant);
            max_etages_fils = max(max_etages_fils, etages_fils_courant);
            fils_courant = fils_courant->sibling;
        }
        return 1 + max_etages_fils;
    }
}

int max_arbre(maillon* pSurM){
    int max_etages = 0;
    while (pSurM != NULL){
        int max_etages_courant = (compter_etages(pSurM));
        max_etages = max(max_etages, max_etages_courant);
        pSurM = pSurM->sibling;
    }
    return max_etages;
}


int main(int argc, char * argv[]){
    (void) argc;
    (void) argv;
    liste2chainee * pSurListe;
    maillon * pSurMaillon;
    pSurListe = creerListeVide();
    int n, i;
    scanf("%d", &n);
    int x, y;
    for (i=0; i<n; i++){
        scanf("%d%d", &x, &y);
        ajoute(x, y, pSurListe);
    }
    pSurMaillon = pSurListe->debut;
    printf("%d", max_arbre(pSurMaillon));
    pSurMaillon = chercher(11, pSurListe->debut);
    while (pSurMaillon != NULL){
        afficher(pSurMaillon);
        pSurMaillon = pSurMaillon->sibling;
    }
    free(pSurListe);
    return 0;

}
