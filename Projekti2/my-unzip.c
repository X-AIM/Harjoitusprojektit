#include <stdio.h>
#include <stdlib.h>

int main( int argumenttien_maara, char *argumentit[] ) {
    if ( argumenttien_maara < 2 ) { // virheen tarkistus sille että argumentteja on oikea määrä
        printf( "my-unzip: file1 [file2 ...]\n");
        return 1;
    }

    // Käydään läpi syötetyt tiedostot
    for ( int i = 1; i < argumenttien_maara; i++ ) {
        FILE *tiedosto = fopen( argumentit[i], "rb" );

        if (!tiedosto) {
            return 1;
        }

        int maara;
        char merkki;

        // Luetaan tiedosto pareittain, ensin 4-byte kokonaisluku ja sitten yksi merkki
        while ( fread(&maara, sizeof(int), 1, tiedosto) == 1 && fread(&merkki, sizeof(char), 1, tiedosto) == 1 ) {
            for ( int j = 0; j < maara; j++ ) {
                putchar( merkki );
            }
        }

        fclose(tiedosto);
    }

    return 0;
}