#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Solmu {
    char *rivi;
    struct Solmu *seuraava;
} Solmu;

// Tulosta virheilmoitus ja lopeta ohjelma
void virhe(const char *viesti) {
    fprintf(stderr, "%s\n", viesti);
    exit(1);
}

// Vapauta listan muisti
void vapauta_lista(Solmu *alku) {
    while (alku != NULL) {
        Solmu *valiaikainen = alku;

        alku = alku->seuraava;

        free(valiaikainen->rivi);
        free(valiaikainen);
    }
}

int main(int argumenttien_maara,
         char *argumentit[]) {

    FILE *syote = stdin;
    FILE *tuloste = stdout;

    // Virheilmoitus, jos on liikaa argumentteja
    if (argumenttien_maara > 3) {
        fprintf(stderr, "usage: reverse <input> <output>\n");
        exit(1);
    }

    // Virheilmoitus, jos sama tiedosto ole on syöte ja tuloste
    if (argumenttien_maara == 3 &&
        strcmp(argumentit[1],
               argumentit[2]) == 0) {

        fprintf(stderr, "Input and output file must differ\n");
        exit(1);
    }

    // Avaa syötetiedosto
    if (argumenttien_maara >= 2) {
        syote = fopen(argumentit[1], "r");

        if (syote == NULL) {
            fprintf(stderr,
                    "error: cannot open file '%s'\n",
                    argumentit[1]);
            exit(1);
        }
    }

    // Avaa tulostiedosto
    if (argumenttien_maara == 3) {
        tuloste = fopen(argumentit[2], "w");

        if (tuloste == NULL) {
            if (syote != stdin) {
                fclose(syote);
            }

            fprintf(stderr, "error: cannot open file '%s'\n", argumentit[2]);
            exit(1);
        }
    }

    Solmu *lista = NULL;
    char *puskuri = NULL;
    size_t kapasiteetti = 0;
    ssize_t tavuja;

    // Lue rivit ja lisää alkuun
    while ((tavuja = getline(&puskuri, &kapasiteetti, syote)) != -1) {
        Solmu *uusi_solmu = malloc(sizeof(Solmu));

        if (uusi_solmu == NULL) {
            free(puskuri);

            vapauta_lista(lista);

            if (syote != stdin)
                fclose(syote);
            if (tuloste != stdout)
                fclose(tuloste);

            virhe("malloc failed");
        }

        uusi_solmu->rivi =
            malloc(tavuja + 1);

        if (uusi_solmu->rivi == NULL) {
            free(uusi_solmu);

            free(puskuri);

            vapauta_lista(lista);

            if (syote != stdin)
                fclose(syote);
            if (tuloste != stdout)
                fclose(tuloste);

            virhe("malloc failed");
        }
        strcpy(uusi_solmu->rivi, puskuri);

        uusi_solmu->seuraava = lista;

        lista = uusi_solmu;
    }
    free(puskuri);

    // Tulosta käänteisessä järjestyksessä
    Solmu *nykyinen = lista;

    while (nykyinen != NULL) {
        fprintf(tuloste,
                "%s",
                nykyinen->rivi);

        nykyinen =
            nykyinen->seuraava;
    }

    vapauta_lista(lista);

    if (syote != stdin) {
        fclose(syote);
    }
    if (tuloste != stdout) {
        fclose(tuloste);
    }

    return 0;
}
