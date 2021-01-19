// Martin Macháček - xmacha73
// První IZP projekt

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

// kontrola, jestli je zadaný char již v poli 
// parametry - kontrolovaný znak a pole, ve kterém se kontroluje

bool isInArray(char c, char *array){
        for (int i = 0; i <= (int)strlen(array); i++){
                if (c == array[i]){
                        return 1;
                }
        }
        return 0;
}

// převedení stringu na uppercase
// parametry - pole, které chceme převést na uppercase
// je potřeba toupper funkce

void toUpperString(char *array){
	for (int i = 0; i <= (int)strlen(array); i++){
		array[i] = toupper(array[i]);
	}
}

// třídění pole
// parametry - pole, které chceme setřídit
 
void sort(char *array){
	char temp;
	for (int i = 0; i < (int)strlen(array) -1; i++){
		for (int j = i + 1; j < (int)strlen(array); j++){
			if (array[i] > array[j]){
				temp = array[i];
				array[i] = array[j];
				array[j] = temp;
			}
		}
	}
	
}

// výpisy na základě počtu nalezených měst
// parametry - K počitadlo měst, pole nalezených měst, pole znaků
// musí tam být obě dvě pole, protože nevíme přesně, která ze tří možností se vypíše

void prints(int k, char *foundCities, char *enableChars){
    if ( k == 0 )
        printf("Not found\n");
    if ( k == 1 )
        printf("Found: %s", foundCities);
    if ( k > 1 )
        printf("Enable: %s\n", enableChars);
}

// MAIN

#define MAX 101

int main(int argc, char *argv[]){

	// deklarace polí a proměnných

        char city[MAX];					
        char enableChars[MAX];					
        char foundCities[MAX];				
        int i = 0;					
        int k = 0;				
        int cisloMesta = 0;						

	// kontrola počtu argument
        if (argc == 1){			

		// načítání města, porovnání zadaného argv s městem			
							
                while(fgets(city, MAX, stdin)){							
                        if (!isInArray(toupper(city[0]), enableChars)){	

				// přiřazení dalšího charu do pole enable charů
					
                                enableChars[i] = toupper(city[0]);				
                                i++;
                        }
                }
	
		// reřazení, výpis a ukončení
	
		enableChars[i] = '\0';
		sort(enableChars);
		printf("Enable: %s\n", enableChars);
		return 0;
        }

        else if (argc == 2){						

		// načítání města, dokud jich není 42, následné porovnání argv s městem, převedení na uppercase
                while(fgets(city, MAX, stdin) && cisloMesta <= 42){				
			toUpperString(argv[1]);							
			toUpperString(city);							

                        if (strncmp(argv[1], city, strlen(argv[1]))==0){			
                                if (!isInArray(city[strlen(argv[1])], enableChars)){		
                                        enableChars[i] = city[strlen(argv[1])];		
                                        i++;							
                                }
                        	strcpy(foundCities, city);        				
                                k++;								
                        }
                        cisloMesta++;								 
                }
        }

	else {
		fprintf(stderr, "Wrong input\n");
		
	}

	// poslední znak v poli nastavit jako nulový znak
		
	enableChars[i] = '\0';									
	
	// seřazení pole povolených znaků podle abecedy
	
	sort(enableChars);
										
	// výpisy na základě kontroly, kolik měst se přiřadilo do pole foundCities
	
 	prints(k, foundCities, enableChars);
        
	return 0;
}

