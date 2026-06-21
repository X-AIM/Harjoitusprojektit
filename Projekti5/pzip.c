#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Lohko-rakenne
typedef struct {
    char *data;
    long alku;
    long loppu;
    int *maarat;
    char *merkit;
    int sarjoja;
} Lohko;

// Pakkaa funktio, joka tekee run-lenght encodin -pakausta annetulle datan osalle
void *pakkaa(void *parametri) {
    Lohko *lohko = (Lohko *)parametri;

    long maksimi = lohko->loppu - lohko->alku + 1; // Arvioitu maksimi RLE-sarjojen määrä
    // varaa muistia tuloksille
    lohko->maarat = malloc( sizeof(int) * maksimi );
    lohko->merkit = malloc( sizeof(char) * maksimi );

    // Virheen tarkistus sille että muistivaraus onnistui
    if (!lohko->maarat || !lohko->merkit) {
        exit(1);
    }

    // Jos lohko on tyhjä, ei tehdä mitään
    if (lohko->alku >= lohko->loppu) {
        lohko->sarjoja = 0;
        return NULL;
    }

    int maara = 1;
    int sarja = 0;

    // Käydään lohko läpi ja tehdään RLE-pakkaus
    for ( long i = lohko->alku + 1; i < lohko->loppu; i++ ) {
        if ( lohko->data[i] == lohko->data[i - 1] ) {
            maara++;
        } else {
            // sarja päättyy
            lohko->maarat[sarja] = maara;
            lohko->merkit[sarja] = lohko->data[i - 1];
            sarja++;
            maara = 1;
        }
    }

    // viimeinen sarja talteen
    lohko->maarat[sarja] = maara;
    lohko->merkit[sarja] = lohko->data[lohko->loppu - 1];
    lohko->sarjoja = sarja + 1;

    return NULL;
}

// Kirjoittaa yhden RLE-paketin stdoutiin
void kirjoita(int maara, char merkki) {
    fwrite(&maara, sizeof(int), 1, stdout);
    fwrite(&merkki, sizeof(char), 1, stdout);
}

int main(int argumenttien_maara, char *argumentit[]) {
    if (argumenttien_maara < 2) { // Tarkistaa argumenttien määrän
        printf( "pzip: file1 [file2 ...]\n");
        return 1;
    }

    char *puskuri = NULL;
    long koko = 0;

    // Luetaan kaikki tiedostot yhteen puskuriin
    for (int i = 1; i < argumenttien_maara; i++) {
        FILE *tiedosto = fopen(argumentit[i], "rb");

        if (!tiedosto) {
            printf("cannot open file\n");
            return 1;
        }

        // Siirrytään tiedoston loppuun ja saadaan koko
        fseek(tiedosto, 0, SEEK_END);
        long tiedoston_koko = ftell(tiedosto);
        rewind(tiedosto);

        puskuri = realloc(puskuri, koko + tiedoston_koko); // kasvatetaan puskuria

        fread(puskuri + koko, 1, tiedoston_koko, tiedosto); // luetaan tiedosto muistiin

        koko += tiedoston_koko;

        fclose(tiedosto);
    }

    // Lopettaa, jos ei ole dataa
    if (koko == 0) {
        free(puskuri);
        return 0;
    }

    int saikeita = 4;
    pthread_t saikeet[4];
    Lohko lohkot[4];

    long lohkon_koko = koko / saikeita;

    // Jaetaan data lohkoihin ja käynnistetään säikeet
    for (int i = 0; i < saikeita; i++) {

        lohkot[i].data = puskuri;

        lohkot[i].alku = i * lohkon_koko;

        if (i == saikeita - 1) {
            lohkot[i].loppu = koko;
        } else {
            lohkot[i].loppu = (i + 1) * lohkon_koko;
        }

        pthread_create(&saikeet[i], NULL, pakkaa, &lohkot[i]);
    }

    // Kaikki säikeet valmistuu
    for (int i = 0; i < saikeita; i++) {
        pthread_join(saikeet[i], NULL);
    }

    int nykyinen = 0;
    char nykyinen_merkki = 0;

    // yhdistetään säikeiden tulokset
    for (int i = 0; i < saikeita; i++) {
        for (int j = 0; j < lohkot[i].sarjoja; j++) {
            if (nykyinen == 0) {
                nykyinen = lohkot[i].maarat[j];
                nykyinen_merkki = lohkot[i].merkit[j];
            } else if (nykyinen_merkki == lohkot[i].merkit[j]) { // sama merkki niin ne yhdistetään
                nykyinen += lohkot[i].maarat[j];
            } else { // eri merkki niin kirjoitetaan edellin ulos
                kirjoita(nykyinen, nykyinen_merkki);

                nykyinen = lohkot[i].maarat[j];
                nykyinen_merkki = lohkot[i].merkit[j];
            }
        }
    }

    // kirjoittaa viimeisen pakkauksen
    if (nykyinen > 0) {
        kirjoita(nykyinen, nykyinen_merkki);
    }

    // vapauttaa varatun muistin
    for (int i = 0; i < saikeita; i++) {
        free(lohkot[i].maarat);
        free(lohkot[i].merkit);
    }

    free(puskuri);

    return 0;
}