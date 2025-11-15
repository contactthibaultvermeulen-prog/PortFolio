#ifndef TP4_H
#define TP4_H

struct Lapin{
    int age; // En mois
    char sexe; // 'F' pour femelle 'M' pour male
    int isAdult; // Vaut 1 si le lapin est adulte, 0 sinon
    int moisMaturite; // Le mois à partir du quel le lapin sera considéré comme adulte
    int isEnceinte; // Si le lapin est enceint
    int compteurMois; // Pour se repérer en fonction des mois pour gérer la répartition des portées
    int nbPortee; // Le nombre de portées de cette année
    
    // Le tableau contenant dans les nbPortee premières cases les mois durant lesquelles la femelle
    // lapin pourra avoir une portée
    int * repartitionPortee; 
};
typedef struct Lapin Lapin;

long long int * simuLapinFibonacci (int n);
void afficheSimuLapinFibonacci(int n);


Lapin * genLapin(double (*randomGenerator)(void), float probaFemelle);
int nbEnfantLapin(Lapin * lap, int isThereMale, unsigned long int * tabMersenneTwister, int * index, int tailleTabMersenneTwister);
void ajouterLapin(Lapin ***tableau, int *taille, Lapin *lapin);
void miseAJourEtatLapins(
    Lapin ***jeunesMales, int *nbJeunesMales,
    Lapin ***jeunesFemelles, int *nbJeunesFemelles,
    Lapin ***adultesMales, int *nbAdultesMales,
    Lapin ***adultesFemelles, int *nbAdultesFemelles);

int * simulationLapins(int nbMois, int nbMale, int nbFemelle, int tailleTabMersenneTwister, int debug);

#endif