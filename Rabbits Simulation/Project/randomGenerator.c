#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "../MT.h"

//b > a
int gaussianEntreAetB(double a, double b) {
    double u1, u2, z;

    // Génération via la méthode de Box-Muller
    do {
        u1 = genrand_real1(); // [0, 1]
        u2 = genrand_real1(); // [0, 1]
    } while (u1 <= 1e-12); // éviter log(0)

    // Transformation Box–Muller → N(0,1)
    z = sqrt(-2.0 * log(u1)) * cos(2.0 * M_PI * u2);

    // Centrée sur la moyenne de a et b, écart-type 1
    double value = (a+b)/2 + z * 1.0;

    // Tronquer entre a et b
    if (value < a) value = a;
    if (value > b) value = b;

    return (int) value;
}


int randomUniformeEntreAEtBEntier(int a, int b) {
    return a + (int)((b - a + 1) * genrand_real1());
}
