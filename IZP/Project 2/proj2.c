/*

Author: Martin Machacek	/ xmacha73
projekt: proj2 (Iterační výpočty)

*/


#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>


// Funkce pro výpočet tan pomocí Taylorova polynomu
// Paremetry 	- úhel a počet iterací

double taylor_tan(double x, unsigned int n){
	double result = 0;
	double prevX = x;
	
	double nom[13] =	{1,1,2,17,62,1382,21844,929569,  
            			6404582,443861162,18888466084,
            			113927491862,58870668456604};

	double denom[13] =	{1,3,15,315,2835,155925,6081075,
            			638512875,10854718875,1856156927625,194896477400625,
            			49308808782358125,3698160658676859375};
	
	for (int i = 0; i < (int)n; i++){
		result = result + (prevX * nom[i] / denom[i]);
		prevX = prevX * (x*x);
	}
	return result;
}

// Funkce pro výpočet tan pomocí zřetězených zlomků
// Parametry	- úhel a počet iterací ­

double cfrac_tan(double x, unsigned int n){
	double nom = x * x;
	double result;

	for (int i = n-1; i > 0; i--){
		int number = (i + 1) * 2 -1;
		double frac = nom / (number - frac);
		result = x / (1 - frac);
	}
	return result;
}

// Funkce pro výpis nápovědy --help

void printHelp(){
	printf("\nHELP\n");
	printf("Machacek Martin / xmacha73 / Iteracni vypocty");
	printf("\n "
	"--tan A N M\n"
	"   --tan vypocita a vypise tan daneho uhlu v radianech"
	"   A   uhel alpha \n"
	"   N   horni hranice iterace programu \n"
	"   M   spodni hranice iterace programu \n"
	"\n[-c X] -m A [B]\n"
	"   -m vypocita vzdalenost objektu od mericiho pristroje\n"
	"   -c (nepovinne) prida vysku mericiho zarizeni\n"
	"   A   uhel ktery svira vyska zarizeni se zemi (0 < A <= 1.4) \n"
	"   X   vyska zarizeni (0 < X <= 100) \n"
	"   B   uhel beta, ktery svira vzdalenost s nejvyssim bodem objektu (0 < B <= 1.4) \n"
	"   Je-li zadany uhel beta, vypocita se i vyska objektu\n\n");
	
}

// Funkce pro výpis tan pomocí Talorova polynomu, zřetězených zlomků a matematické knihovny při volbě --tan
// Parametry 	- úhel, první prvek iterace a poslední prvek iterace

void tanPrints(double a, int first, int last){
        for (int i = first; i <= last; i++){
       		double taylorMinusMath = (tan(a) - taylor_tan(a,i));
        	double cfracMinusMath = (tan(a) - cfrac_tan(a,i));

        	printf("%d ", i);
        	printf("%e ", tan(a));
        	printf("%e ", taylor_tan(a, i));
        	printf("%e ", taylorMinusMath);
        	printf("%e ", cfrac_tan(a, i));
        	printf("%e ", cfracMinusMath);
        	printf("\n");
     	}
}

// Funkce pro výpočet vzdálenosti měřícího zařízení od objektu
// Parametry 	- úhel alpha a výška měřícího zařízení

double calcDistance(double alpha, double heightOfDevice){
	double distance = heightOfDevice / cfrac_tan(alpha, 9); 
	return distance;	
}

// Funkce pro výpočet výšky objektu
// Parametry 	- výška měřícího zařízení, úhel beta a úhel alpha

double calcHeight(double heightOfDevice, double beta, double alpha){
	double objectHeight = cfrac_tan(beta, 9) * calcDistance(alpha, heightOfDevice) + heightOfDevice;
	return objectHeight;
}

int main(int argc, char *argv[]){

	if (argc == 1){
		fprintf(stderr, "Wrong input.\n Use --help for information on how this program works \n");
		return 1;
	}

	// výpis nápovědy v případě zadání --help na první argument		

	if (strcmp(argv[1], "--help") == 0){
		printHelp();
	}
	
	// Jestliže bude zadáno --tan, vytvoří a přířadí se hodnoty z argumentů k proměnným
	// Zavolá se funkce tanPrints

	else if (strcmp(argv[1], "--tan") == 0){
		double a = atof(argv[2]);
		double first = atof(argv[3]);
		double last = atof(argv[4]);		
	
		tanPrints(a, first, last);	
	}
	
	// Jestliže nebude na prvním argumentu --help ani --tan, začne program na výpočet vzdálenosti a výšky objektu

	else{		
		double heightOfDevice = 1.5;
		double alpha;
		double beta = 1;
		bool isBetaThere = false;
		
		// Jestliže bude na 1. pozici -c, bude kontrola, jestli existuje -m		
	
		if (strcmp(argv[1], "-c") == 0){
			if (strcmp(argv[3], "-m") == 0 && argv[4] != NULL){
				heightOfDevice = atof(argv[2]);
				alpha = atof(argv[4]);							
			}
			else{
				fprintf(stderr, "Wrong input.\n Use --help for information on how this program works \n");
				return 1;
			}
		}
		
		// Jestliže bude na 1. pozici -m, přiřadí se do alpha úhlu hodnota 2. argumentu

		else if (strcmp(argv[1], "-m")==0 && argv[2] != NULL){
			alpha = atof(argv[2]);
		}
		else{
			fprintf(stderr, "Wrong input.\n Use --help for information on how this program works \n");
			return 1;
		}
			
		// Jestliže bude délka argumentu 6, nebo 4, víme, že existuje úhel beta a bude to hodnota posledního argumentu	

		if (argc == 6 || argc == 4){
			isBetaThere = true;
			beta = atof(argv[argc-1]);
		}	

		// Kontrola úhlů a výšek podle pravidel v zadání

		if (alpha <= 0 || heightOfDevice >= 100 || heightOfDevice <= 0 || (isBetaThere && beta <= 0) || argc > 6 || alpha > 1.4 || beta > 1.4){
			fprintf(stderr, "Wrong input.\n Use --help for information on how this program works \n");
			return 1;
		}

		
		// výpis vzdálenosti pomocí funkce calcDistance

		printf("%.10e\n", calcDistance(alpha, heightOfDevice));

		// Jestliže byl zadán úhel beta, vypočítá se výška objektu pomocí funkce calcHeight a vypíše se

		if (isBetaThere)
			printf("%.10e\n", calcHeight(heightOfDevice, beta, alpha));		
			
	}
	return 0;
}