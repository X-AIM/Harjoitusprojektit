#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// lukee tiedoston riveittäin ja tulostaa rivit jotka sisältää hakusanan
void etsi_rivit(FILE *tiedosto, char *hakusana) {
    char *rivi = NULL;
    size_t koko = 0

    while (getline(&rivi, &koko, tiedosto) != -1) {
        if (strstr(rivi, hakusana)) {
            printf("%s", rivi);
        }
    }

    free(rivi); // Vapautetaan varattu muisti
}

int main(int argumenttien_maara, char *argumentit[]) {
    if (argumenttien_maara < 2) { // virheen tarkistus sille että hakusana on annettu
        printf("my-grep: searchterm [file ...]\n");
        return 1;
    }

    char *hakusana = argumentit[1]; // ensimmäinen argumentti on hakusana

    if (argumenttien_maara == 2) { // jos tiedostoja ei annettu, luetaan stdinistä
        etsi_rivit(stdin, hakusana);
        return 0;
    }

    // Käydään läpi annetut tiesotot
    for (int i = 2; i < argumenttien_maara; i++) {
        FILE *tiedosto = fopen(argumentit[i], "r");

        if (!tiedosto) {
            printf("my-grep: cannot open file\n");
            return 1;
        }

        etsi_rivit(tiedosto, hakusana);

        fclose(tiedosto);
    }

    return 0;
}