#include <stdio.h>
#include <stdlib.h>

// Kirjoittaa pakatun tiedoston stdoutiin
void kirjoita_pakkaus(int maara, char merkki) {
    fwrite( &maara, sizeof(int), 1, stdout ); // kirjoittaa lukumäärän
    fwrite( &merkki, sizeof(char), 1, stdout); // kirjoittaa merkin
}

int main(int argumenttien_maara, char *argumentit[]) {
    if (argumenttien_maara < 2) { // virheen tarkistus sille että argumentteja on oikea määrä
        printf( "my-zip: file1 [file2 ...]\n" );
        return 1;
    }

    int edellinen = EOF;
    int laskuri = 0;

    // Käydään läpi kaikki syötetyt tiedostot
    for ( int i = 1; i < argumenttien_maara; i++) {
        FILE *tiedosto = fopen( argumentit[i], "r" );

        if (!tiedosto) {
            return 1;
        }

        int merkki;

        // lukee tiedoston merkki kerrallaan
        while ((merkki = fgetc(tiedosto)) != EOF) {
            if ( laskuri == 0 ) {
                edellinen = merkki;
                laskuri = 1;
            } else if ( merkki == edellinen ) {
                laskuri++;
            } else {
                kirjoita_pakkaus( laskuri, edellinen );
                edellinen = merkki;
                laskuri = 1;
            }
        }
        fclose(tiedosto);
    }

    if (laskuri > 0) {
        kirjoita_pakkaus( laskuri, edellinen);
    }

    return 0;
}