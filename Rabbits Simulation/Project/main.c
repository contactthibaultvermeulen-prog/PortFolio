#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "../MT.h"
#include "TP4.h"

// -----------------------------------------------------------------------------
// PARAMÈTRES GLOBAUX
// -----------------------------------------------------------------------------
#define N_REPLICATES 30   // Nombre d’expériences indépendantes
#define N_MOIS 80         // Durée d’une simulation (mois)
#define N_RAND 30000000   // Taille du tableau d’aléas
#define N_MALES_INIT 3
#define N_FEMELLES_INIT 3


int compare_double(const void *a, const void *b) {
    double diff = *(double *)a - *(double *)b;
    return (diff > 0) - (diff < 0);
}



int main(){
    // Initialisation du générateur aléatoire robuste
    unsigned long init[4]={0x123, 0x234, 0x345, 0x456}, length=4;
    init_by_array(init, length);


    // Écriture dans le fichier log.txt pour pouvoir rejouer
    FILE *log = fopen("log.txt", "w+");
    if (log == NULL) {
        perror("Erreur lors de l'ouverture de log.txt");
        return EXIT_FAILURE;
    }


    // Sauvegarde de la seed dans le fichier
    fprintf(log, "# Log de la simulation de lapins\n");
    fprintf(log, "# Seed utilisée pour MersenneTwister (tableau de %lu valeurs) :\n", length);
    for (unsigned long i = 0; i < length; i++) {
        fprintf(log, "%lu ", init[i]);
    }
    fprintf(log, "\n\n");


    // --- Modèle de fibonacci ---

    // Début du chronomètre pour le modèle de fibonacci
    clock_t startFibo = clock();

    long long int *tabFibo = simuLapinFibonacci(N_MOIS);

    // Fin du chronomètre pour fibonacci
    clock_t endFibo = clock();

    fprintf(log, "--- Simulation Fibonacci (%d mois) ---\n", N_MOIS);
    for (int i = 0; i <= N_MOIS; i++) {
        fprintf(log, "%lld ", tabFibo[i]);
    }
    fprintf(log, "--------------------------------------\n\n");
    free(tabFibo);


    // --- Modèle Complexe ---

    // Début du chronomètre pour le modèle complexe
    clock_t startComplexe = clock();

    int *simuLapin = simulationLapins(N_MOIS, N_MALES_INIT, N_FEMELLES_INIT, N_RAND, 1);

    // Fin du chronomètre pour le modèle complexe
    clock_t endComplexe = clock(); 


    printf("Temps d'exécution avec le modèle de Fibonacci : %f secondes\n", (double)(endFibo - startFibo) / CLOCKS_PER_SEC);
    printf("Temps d'exécution avec le modèle complexe: %.3f secondes\n", (double)(endComplexe - startComplexe) / CLOCKS_PER_SEC);


    fprintf(log, "--- Simulation complexe (%d mois) ---\n", N_MOIS);
    for (int i = 0; i <= N_MOIS; i++) {
        fprintf(log, "%d ", simuLapin[i]);
    }
    fprintf(log, "--------------------------------------\n\n");
    free(simuLapin);


    printf("Résultats enregistrés dans log.txt\n");




    // Tableau pour stocker le nombre de lapins à la fin de chaque simulation
    double results[N_REPLICATES];
    double sum = 0.0;


    clock_t start = clock();

    // Boucle principale sur les réplicats
    for (int r = 0; r < N_REPLICATES; r++)
    {
        int *simu = simulationLapins(N_MOIS, N_MALES_INIT, N_FEMELLES_INIT, N_RAND, 0);


        // On prend ici le nombre total de lapins à la fin du mois N_MOIS
        double final_population = simu[N_MOIS];
        results[r] = final_population;
        sum += final_population;

        printf("  Réplicat %2d : %.0f lapins finaux\n", r + 1, final_population);

        free(simu);
    }


    clock_t end = clock();
    double elapsed = (double)(end - start) / CLOCKS_PER_SEC;

    printf("Temps total : %f\n", elapsed);




    // Calcul de la moyenne
    double mean = sum / N_REPLICATES;

    // Calcul de la variance et de l’écart-type
    double variance = 0.0;
    for (int r = 0; r < N_REPLICATES; r++)
        variance += pow(results[r] - mean, 2);
    variance /= (N_REPLICATES - 1);

    double stddev = sqrt(variance);

    // Erreurs relatives (par rapport à la moyenne)
    double coeff_var = (100.0 * stddev / mean);

    // Calcul de l’intervalle de confiance à 95% (Student t pour n=30 => 2.042)
    double student_t = 2.042;
    double conf_radius = student_t * stddev / sqrt(N_REPLICATES);


    // Tri du tableau pour obtenir médiane, min et max
    qsort(results, N_REPLICATES, sizeof(double), compare_double);

    double mediane;
    if (N_REPLICATES % 2 == 0)
        mediane = (results[N_REPLICATES/2 - 1] + results[N_REPLICATES/2]) / 2.0;
    else
        mediane = results[N_REPLICATES/2];

    double min = results[0];
    double max = results[N_REPLICATES - 1];

    int k = 0;
    int nbExt = 0;
    while ((results[k] == 0 && k < N_REPLICATES)){
        nbExt++;
        k++;
    }

    double pourcentExt = ((double)nbExt / N_REPLICATES) * 100;


    // Résultats finaux
    printf("\n--------------------------------------------\n");
    printf("Médiane : %.3f lapins\n", mediane);
    printf("Min : %.0f lapins | Max : %.0f lapins\n", min, max);
    printf("Moyenne population finale : %.3f lapins\n", mean);
    printf("Variance : %.3e\n", variance);
    printf("Écart-type : %.3f\n", stddev);
    printf("Coefficient de variation : %.2f %%\n", coeff_var);
    printf("Intervalle de confiance 95%% : [%.3f, %.3f]\n",
           mean - conf_radius, mean + conf_radius);
    printf("Pourcentage d'extinction : %.2f%%\n", pourcentExt);
    printf("--------------------------------------------\n");



    fclose(log);

    return 0;
}