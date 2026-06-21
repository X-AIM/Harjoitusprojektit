#include <stdio.h>
#include <stdlib.h>

#define PUSKURI 1000 // Puskuri rivin lukemista varten (bufferikoko)

int main(int argumenttien_maara, char *argumentit[]) {
    if (argumenttien_maara == 1) { // Jos ei annettu yhtään tiedostoa, ohjelma päättyy normaalisti
        return 0;
    }

    // Käydään läpi kaikki komentorivillä annetut tiedostot
    for (int i = 1; i < argumenttien_maara; i++) {

        FILE *tiedosto = fopen(argumentit[i], "r"); // Avataan tiedosto

        if (tiedosto == NULL) {
            printf("my-cat: cannot open file\n");
            return 1;
        }

        char rivi[PUSKURI];

        // Luetaan tiedosto rivi kerrallaan ja tulostetaan rivi
        while (fgets(rivi, PUSKURI, tiedosto)) {
            printf("%s", rivi);
        }

        fclose(tiedosto);
    }

    return 0;
}