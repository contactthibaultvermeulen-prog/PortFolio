#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#include "randomGenerator.h"
#include "../MT.h"
#include "TP4.h"


/*----------PARTIE 1----------*/

/*----------------------------------------------------------------------*
* En entrée :                                                           *
*    - un entier nbMois                                                 *
* En sortie :                                                           *
*    - un tableau d'entiers contenant le nombre de couples de lapins    *
*      chaque mois                                                      *
*----------------------------------------------------------------------*/
long long int* simuLapinFibonacci(int nbMois) {
    // On veut un tableau contenant les valeurs de 0 à nbMois inclus
    // Utilisation de long long int car on a des valeurs très grandes
    long long int* tab = malloc((nbMois + 1) * sizeof(long long int));
    if (tab == NULL) {
        perror("Erreur d'allocation mémoire");
        exit(EXIT_FAILURE);
    }

    // Cas de base
    tab[0] = 1;
    if (nbMois > 0)
        tab[1] = 1;

    // Remplissage du tableau
    for (int i = 2; i <= nbMois; i++) {
        tab[i] = tab[i - 1] + tab[i - 2];
    }

    // Multiplication par 2 car ici on comptait les couples de lapins
    for (int i = 0; i <= nbMois; i++){
        tab[i] *= 2;
    }

    return tab;
}

/*----------------------------------------------------------------------*
* En entrée :                                                           *
*    - un entier nbMois                                                 *
* En sortie :                                                           *
*    - affichage du nombre de lapins pour chaque mois                   *
*----------------------------------------------------------------------*/
void afficheSimuLapinFibonacci(int nbMois) {
    long long int* tab = simuLapinFibonacci(nbMois);

    printf("--- Simulation de la croissance des lapins sur %d mois ---\n", nbMois);
    for (int i = 0; i <= nbMois; i++) {
        printf("Mois %2d : %lld couples de lapins\n", i, tab[i]);
    }
    printf("-----------------------------------------------------------\n\n");

    free(tab); // Libération mémoire
}



/*----------PARTIE 2-----------*/

/*----------------------------------------------------------------------*
* En entrée :                                                           *
*    - une fonction de génération de nombre aléatoire dans [0, 1]       *
*    - un float représentant la probabilité d'avoir une femelle         *
* En sortie :                                                           *
*    - Un Lapindont le sexe est déterminé à partir du générateur entrée *
*      en paramètre et en fonction de la probabilité d'avoir une        *
*      femelle                                                          *                                                     
*----------------------------------------------------------------------*/
Lapin * genLapin(double (*randomGenerator)(void), float probaFemelle){
    Lapin * lap = malloc(sizeof(Lapin));
    if (lap == NULL) {
        fprintf(stderr, "Erreur : allocation mémoire échouée pour un lapin\n");
        exit(EXIT_FAILURE);
    }
    lap->age = 0; // Le lapin vient de naître
    lap->isAdult = 0; // Encore enfant

    // Initialisation des valeurs pour les lapin femelles
    lap->isEnceinte = 0;
    lap->compteurMois = -1;
    lap->nbPortee = 0;

    // Génération aléatoire en suivant un modèle gaussien pour déterminer à partir de quel mois
    // le lapin sera considéré comme adulte
    lap->moisMaturite = gaussianEntreAetB(5, 8); 

    // Sexe : 'M' ou 'F' avec une probabilité probaFemelle
    lap->sexe = (randomGenerator() < probaFemelle) ? 'F' : 'M';

    // Uniquement pour les lapins femelles, pas besoin de réserver de l'espace pour les males
    if (lap->sexe == 'F'){
        lap->repartitionPortee = malloc(sizeof(int) * 12);
        for (int i = 0; i < 12; i++){
            lap->repartitionPortee[i] = i + 1; // Représente les mois
        }
    }

    return lap;
}


/*----------------------------------------------------------------------*
* En entrée :                                                           *
*    - un entier nbFemelleAdulte représentant le nombre de femelle à    *
*      l'age adulte du mois actuelle                                    *
*    - un booléen isThereMale indiquant s'il y a actuellement au moins  *
*      un adulte lapin male en vie                                      *
*    - un tableau tabMersenneTwister contenant les nombres aléatoires   *
*      générés au début de l'expérience                                 *
*    - un pointeur sur un entier index, qui indique là où on se trouve  *
*      dans le tableau tabMerenneTwister                                *
*    - un entier tailleTabMersenneTwister qui indique la taille max du  *
*      tableau                                                          *
*                                                                       *
* En sortie :                                                           *
*    - un entier représentant le nombre de nouveaux lapins du prochain  *
*      mois                                                             * 
*    - met également à jour l'état de la femelle et l'index par effet   *
*      de bord                                                          *
*----------------------------------------------------------------------*/
int nbEnfantLapin(Lapin * lap, int isThereMale, unsigned long int * tabMersenneTwister, int * index, int tailleTabMersenneTwister){
    int somme = 0;

    if (lap->isEnceinte){ // Est enceinte et donc fait des enfants
        somme += randomUniformeEntreAEtBEntier(3, 6);
        lap->isEnceinte = 0;
    } 

    // Fin de l'année associée à ses portées OU c'est la première fois que la femelle passe par 
    // cette fonction, et donc doit être initialisée
    if ((lap->compteurMois > 12) || (lap->compteurMois == -1)){ 
        lap->compteurMois = 0;
        lap->nbPortee = gaussianEntreAetB(3, 9); // Répartition gaussienne pour le nombre de portée

        // Mélange Fisher-Yates partiel pour répartir les portées sur l'année qui vient
        for (int i = 0; i < lap->nbPortee; i++) {
            // Ici on accède directement aux valeurs qu'on a généré au début de l'expérience ce qui 
            // permet de faire uniquement des accès tableaux et donc d'être plus rapide
            int j = (int)(i + tabMersenneTwister[*index] % (12 - i));
            *index = (*index + 1) % tailleTabMersenneTwister; 

            int tmp = lap->repartitionPortee[i];
            lap->repartitionPortee[i] = lap->repartitionPortee[j];
            lap->repartitionPortee[j] = tmp;
        }
    }

    if (isThereMale){ // Devient enceinte uniquement si il y a un male
        for (int i = 0; i < lap->nbPortee; i++){ 
            // Ici on vérifie selon le tirage au sort qu'on a fait, si le mois prochain, la femelle
            // peut avoir des portées
            if ((lap->compteurMois + 1) == lap->repartitionPortee[i]){
                lap->isEnceinte = 1; // Au prochain mois la femelle va avoir des enfants
                break; // Pas besoin de continuer, la femelle ne peut avoir qu'une portée à la fois
            }
        }
    }

    lap->compteurMois++; // Passage au mois suivant

    return somme;
}


/*----------------------------------------------------------------------*
* En entrée :                                                           *
*    - un triple pointeur sur Lapin, tableau, contenant des pointeurs   *
*      sur des lapins                                                   *
*    - un pointeur sur un entier, taille, contenant la taille du        *
*      tableau                                                          *
*    - un pointeur sur un Lapin, lapin, contenant le lapin à ajouter    *
*                                                                       *
* En sortie :                                                           *
*    - ajoute par effet de bord le pointeur sur le lapin dans le        *
*      tableau                                                          *
*    - met à jour par effet de bord la taille du tableau                *          
*----------------------------------------------------------------------*/
void ajouterLapin(Lapin ***tableau, int *taille, Lapin *lapin) {
    // Allocation d'un nouvel espace pour le lapin
    *tableau = realloc(*tableau, (*taille + 1) * sizeof(Lapin*)); 
    if (!*tableau) {
        fprintf(stderr, "Erreur realloc\n");
        exit(EXIT_FAILURE);
    }
    (*tableau)[*taille] = lapin;
    (*taille)++; // Mise à jour de la taille
}

/*----------------------------------------------------------------------*
* En entrée :                                                           *
*    - un triple pointeur sur Lapin, jeunesMales, étant une tableau     *
*      contenant des pointeurs sur les jeunes lapins males              *
*    - un pointeur sur un entier, nbJeunesMales, contenant la taille du * 
*      tableau jeunesMales                                              *
*                                                                       *
*    - un triple pointeur sur Lapin, jeunesFemelles, étant une tableau  *
*      contenant des pointeurs sur les jeunes lapins femelles           *
*    - un pointeur sur un entier, nbJeunesFemelles, contenant la taille *
*      du tableau jeunesFemelles                                        *
*                                                                       *
*    - un triple pointeur sur Lapin, adulteMales, étant une tableau     *
*      contenant des pointeurs sur les lapins males adultes             *
*    - un pointeur sur un entier, nbAdultesMales, contenant la taille   *
*      du tableau adultesMales                                          *
*                                                                       *
*    - un triple pointeur sur Lapin, adultesFemelles, étant une tableau *
*      contenant des pointeurs sur les lapins femelles adultes          *
*    - un pointeur sur un entier, nbAdultesFemelles, contenant la       *
*      taille du tableau adultesFemelles                                *
*                                                                       *
* En sortie :                                                           *
*    - la gestion par effet de bord dans chaque tableau de l'évolution  *
*      de chaque lapin le mois suivant (vieillissement et mort)         *
*    - met à jour par effet de bord les tailles des tableaux            *          
*----------------------------------------------------------------------*/
void miseAJourEtatLapins(
    Lapin ***jeunesMales, int *nbJeunesMales,
    Lapin ***jeunesFemelles, int *nbJeunesFemelles,
    Lapin ***adultesMales, int *nbAdultesMales,
    Lapin ***adultesFemelles, int *nbAdultesFemelles)
{
    // ---- Traitement des JEUNES ----
    for (int i = 0; i < *nbJeunesMales;) { // Jeunes Males
        Lapin *lap = (*jeunesMales)[i];
        lap->age++; // Vieillissement de 1 mois

        // Mort ?
        double pMort = 0.65 / 12; // On divise par 12 car ici c'est par mois et non année
        if (genrand_real1() < pMort) {
            free(lap); // Libération de l'espace
            // On change décrémente la taille du tableau et on remplace le lapin actuel
            // par le dernier stocké dans le tableau
            (*jeunesMales)[i] = (*jeunesMales)[--(*nbJeunesMales)];

            // On fait un continue sans incrémenter i car on a un nouveau lapin à traiter
            // comme on vient de le remplacer
            continue; 
        }

        // Passage adulte ?
        if (lap->age >= lap->moisMaturite) {
            lap->isAdult = 1;
            ajouterLapin(adultesMales, nbAdultesMales, lap);// On change le lapin de tableau 

            // Ici on ne free pas le lapin car il va être utilisé dans l'autre tableau 
            (*jeunesMales)[i] = (*jeunesMales)[--(*nbJeunesMales)];
            // On fait un continue sans incrémenter i car on a un nouveau lapin à traiter
            // comme on vient de le remplacer par le dernier
            continue;
        }

        i++; // Incrémentation
    }

    for (int i = 0; i < *nbJeunesFemelles;) { // Jeunes Femelles
        Lapin *lap = (*jeunesFemelles)[i];
        lap->age++;

        double pMort = 0.65 / 12;
        if (genrand_real1() < pMort) {
            free(lap->repartitionPortee);
            free(lap);
            (*jeunesFemelles)[i] = (*jeunesFemelles)[--(*nbJeunesFemelles)];
            continue;
        }

        if (lap->age >= lap->moisMaturite) {
            lap->isAdult = 1;
            ajouterLapin(adultesFemelles, nbAdultesFemelles, lap);
            (*jeunesFemelles)[i] = (*jeunesFemelles)[--(*nbJeunesFemelles)];
            continue;
        }

        i++;
    }

    // ---- Traitement des ADULTES ----
    for (int i = 0; i < *nbAdultesMales;) { // Males Adultes
        Lapin *lap = (*adultesMales)[i];
        lap->age++;

        // Gestion de la probabilité de mourrir en fonction de l'age
        // Si le lapin a moins de 10 ans, alors il a 40% de chance de mourrir à l'année
        // Sinon si il a moins de 15 ans, tous les ans il a 10% de chance en plus de mourrir par année
        // Sinon, mort assurée
        // Tout cela divisé par 12 car on fonctionne en mois
        double pMort = (lap->age < 120) ? 0.40 / 12 :
                       (lap->age < 180) ? (0.40 + 0.1 * ((lap->age - 120) / 12)) / 12 : 1.0;

        if (genrand_real1() < pMort) {
            free(lap);
            (*adultesMales)[i] = (*adultesMales)[--(*nbAdultesMales)];
            continue;
        }

        i++;
    }

    for (int i = 0; i < *nbAdultesFemelles;) { // Femelles Adultes
        Lapin *lap = (*adultesFemelles)[i];
        lap->age++;

        double pMort = (lap->age < 120) ? 0.40 / 12 :
                       (lap->age < 180) ? (0.40 + 0.1 * ((lap->age - 120) / 12)) / 12 : 1.0;

        if (genrand_real1() < pMort) {
            free(lap->repartitionPortee);
            free(lap);
            (*adultesFemelles)[i] = (*adultesFemelles)[--(*nbAdultesFemelles)];
            continue;
        }

        i++;
    }
}



/*----------------------------------------------------------------------*
* En entrée :                                                           *
*    - un entier, nbMois, représentant la durée de la simulation        *
*    - un entier, nbMale, représentant le nombre de male au départ      *
*    - un entier, nbFemelle, représentant le nombre de femelle au       *
*      départ                                                           *
*    - un entier tailleTabMersenneTwister qui va définir la taille du   *
*      tableau qui va contenir nos nombres générés de manière aléatoire *
*      au début de l'expérience                                         *
*    - un entier debug, qui permet de gerer l'affichage en fonction de  *
*      sa valeur                                                        *
*                                                                       *
* En sortie :                                                           *
*    - un tableau d'entier, nbLapinMois, contenant pour chaque mois, le *
*      nombre total de lapin vivant                                     *
*----------------------------------------------------------------------*/
int * simulationLapins(int nbMois, int nbMale, int nbFemelle, int tailleTabMersenneTwister, int debug) {
    // Allocation de l'espace pour le tableau qui nous sera renvoyé
    int * nbLapinMois = malloc(sizeof(int) * (nbMois + 1));  
    nbLapinMois[0] = nbMale + nbFemelle;

    // Initialisation des tableaux/entiers qu'on va utiliser pour stocker les lapins
    Lapin **jeunesMales = NULL, **jeunesFemelles = NULL;
    Lapin **adultesMales = NULL, **adultesFemelles = NULL;
    int nbJeunesMales = 0, nbJeunesFemelles = 0;
    int nbAdultesMales = 0, nbAdultesFemelles = 0;

    // Initialisation des lapins de départ
    for (int i = 0; i < nbMale; i++) {
        ajouterLapin(&jeunesMales, &nbJeunesMales, genLapin(genrand_real1, 0.0f));
    }
    for (int i = 0; i < nbFemelle; i++){
        ajouterLapin(&jeunesFemelles, &nbJeunesFemelles, genLapin(genrand_real1, 1.0f));
    }

    
    // Initialisation du tableau contenant les nombres aléatoires générés à l'avance
    clock_t start = clock();

    unsigned long int * tabMersenneTwister = malloc(sizeof(unsigned long int) * tailleTabMersenneTwister);
    int index = 0;

    for (int i = 0; i < tailleTabMersenneTwister; i++){
        tabMersenneTwister[i] = genrand_int32();
    }

    clock_t end = clock();


    if (debug){
        printf("Temps utilisé pour la génération des nombres aléatoires : %.3f secondes\n", (double)(end - start) / CLOCKS_PER_SEC);

        printf("Début de la simulation : %d mois\n", nbMois);
        printf("Population initiale : %d jeunes males et %d jeunes femelles\n\n", nbMale, nbFemelle);
    }

    // Début de la simulation
    for (int mois = 1; mois <= nbMois; mois++) {
        // --- Étape 1 : mise à jour de l'état de tous les lapins ---
        // Vieillissement, morts, transitions
        miseAJourEtatLapins(
            &jeunesMales, &nbJeunesMales,
            &jeunesFemelles, &nbJeunesFemelles,
            &adultesMales, &nbAdultesMales,
            &adultesFemelles, &nbAdultesFemelles);


        // --- Étape 2 : naissance des nouveaux lapins ---
        // Naissances
        int nbNouveaux = 0; // Nombre de bébé lapin de ce mois

        // Booléen valant 1 si on a au moins un male adulte
        int isThereMale = 0; 

        // On regarde pour chaque lapin femelle adulte s'il y a de nouveaux bébés ou non
        // Et mise à jour de leur état en fonction de s'il y a un male ou non
        if (nbAdultesMales > 0) {
            isThereMale = 1;
            for (int i = 0; i < nbAdultesFemelles; i++) 
                nbNouveaux += nbEnfantLapin(adultesFemelles[i], isThereMale, tabMersenneTwister, &index, tailleTabMersenneTwister);
        } else {
            isThereMale = 0;
            for (int i = 0; i < nbAdultesFemelles; i++)
                nbNouveaux += nbEnfantLapin(adultesFemelles[i], isThereMale, tabMersenneTwister, &index, tailleTabMersenneTwister);
        }

        // Génération des nouveaux lapins et association à leur tableau respectif
        for (int i = 0; i < nbNouveaux; i++) {
            Lapin *bebe = genLapin(genrand_real1, 0.5f);
            if (bebe->sexe == 'M')
                ajouterLapin(&jeunesMales, &nbJeunesMales, bebe);
            else
                ajouterLapin(&jeunesFemelles, &nbJeunesFemelles, bebe);
        }

        // --- Étape 3 : affichage mensuel ---
        int total = nbJeunesMales + nbJeunesFemelles + nbAdultesMales + nbAdultesFemelles; // Nombre total de lapin vivant de ce mois
        nbLapinMois[mois] = total; // Stockage du nombre total de lapin 


        if (debug){
            printf("\nMois %3d -----------------------------------\n", mois);
            printf("Nombre total de lapins : %9d\n", total);
            printf("Nombre de males %d : (jeunes %d, adultes %d)\n",
                nbJeunesMales + nbAdultesMales, nbJeunesMales, nbAdultesMales);
            printf("Nombre de femelles %d : (jeunes %d, adultes %d)\n",
                nbJeunesFemelles + nbAdultesFemelles, nbJeunesFemelles, nbAdultesFemelles);
            printf("--------------------------------------------\n");
        }

    }

    // Libération
    // Uniquement les femelles ont allouées de la mémoire pour la répartition de leurs portées
    for (int i = 0; i < nbJeunesFemelles; i++) {
        free(jeunesFemelles[i]->repartitionPortee);
        free(jeunesFemelles[i]);
    }
    for (int i = 0; i < nbAdultesFemelles; i++){
        free(adultesFemelles[i]->repartitionPortee);
        free(adultesFemelles[i]);
    } 

    for (int i = 0; i < nbAdultesMales; i++) free(adultesMales[i]);
    for (int i = 0; i < nbJeunesMales; i++) free(jeunesMales[i]);

    free(jeunesMales);
    free(jeunesFemelles);
    free(adultesMales);
    free(adultesFemelles);

    free(tabMersenneTwister);

    if (debug) printf("\nSimulation terminée !\n");
    return nbLapinMois;
}