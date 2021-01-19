///	Projekt:	B - ARM-FITkit3: Jednoduchy elektronicky klavir
///	Predmet:	IMP 2019/2020
///	Autor:		Martin Machacek
///	Login:		xmacha73

#include "MK60D10.h"

#define SPK 0x10

//	jednotlive tony pro jednodussi prehravani
//	rozmezi 3-6. Je nutne definovat vsechny zvlast protoze
//	vynasobenim frekvence dvema pro vyssi oktavu obcas neznelo presne
#define ASTHREE	233
#define	BTHREE	247
#define CFOUR 	262
#define CFIVE 	523
#define	CSIX	1047
#define	CSFOUR	277
#define CSFIVE	554
#define	CSSIX	1109
#define DFOUR	294
#define	DFIVE	587
#define	DSIX	1175
#define	EFOUR	330
#define	EFIVE	659
#define	ESIX	1319
#define	FFOUR	349
#define	FFIVE	698
#define	FSIX	1397
#define	FSFOUR	370
#define	FSFIVE	740
#define	FSSIX	1480
#define	GFOUR	392
#define GFIVE	784
#define	GSIX	1568
#define	GSFOUR	415
#define	GSFIVE	831
#define	GSSIX	1661
#define	AFOUR	440
#define	AFIVE	880
#define	ASIX	1760
#define	ASFOUR	466
#define	ASFIVE	932
#define	ASSIX	1865
#define	BFOUR	494
#define	BFIVE	988
#define	BSIX	1976

//	Funkce na delay pro cekani
void delay(long long bound) {
  long long i;
  for(i = 0; i < bound; i++);
}

//	Inicializace MCU
void MCUInit(void)  {
    MCG_C4 |= ( MCG_C4_DMX32_MASK | MCG_C4_DRST_DRS(0x01) );
    SIM_CLKDIV1 |= SIM_CLKDIV1_OUTDIV1(0x00);
    WDOG_STCTRLH &= ~WDOG_STCTRLH_WDOGEN_MASK;
}

//	Inicializace Pinu/Portu, potrebujeme UART5, PORTE, PORTA, PORTB a FTM0
//	Speaker je podle schematu Fitkitu na 0x03
void PortsInit(void){
	SIM->SCGC1 |= SIM_SCGC1_UART5_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK | SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTB_MASK;
	SIM->SCGC6 |= SIM_SCGC6_FTM0_MASK;

	PORTE->PCR[8] = (0 | PORT_PCR_MUX(0x03));
	PORTE->PCR[9] = (0 | PORT_PCR_MUX(0x03));
	PORTA->PCR[4] = PORT_PCR_MUX(0x03);  			// Buzzer
}

//	Inicializace UART5 pro input z putty konzole
void UART5Init(void) {
	UART5->C2 &= ~(UART_C2_TE_MASK | UART_C2_RE_MASK);
	UART5->BDH = 0;
	UART5->BDL = 26;
	UART5->C4 = 15;
	UART5->C1 = 0;
	UART5->C3 = 0;
	UART5->MA1 = 0;
	UART5->MA2 = 0;
	UART5->C2 |= (UART_C2_TE_MASK | UART_C2_RE_MASK);
}

//	Inicializace Timeru ktery se vyuziva pro prehravani zvuku.
//	Nastavena delicka na 8
void TimerInit(void){
	FTM0_CNT = 0;				// Counter do modula
	FTM0_C1SC =	0b00101000;		// Kanal
	FTM0_SC = 0b00001011;		// Delicka 8
}

//	Funkce zajistujici vypis zadaneho charu do konzole
void SendCh(char ch)  {
	while(!(UART5->S1 & UART_S1_TDRE_MASK) && !(UART5->S1 & UART_S1_TC_MASK));
	UART5->D = ch;
}

//	Funkce zajistujici vypis zadaneho stringu do konzole pomoci SendCh()
void SendStr(char *s)  {
	int i = 0;
	while (s[i] != 0){
		SendCh(s[i++]);
	}
}

//	Funkce ktera bere input chary z konzole (putty nejlepe)
char ReceiveCh(void) {
	while(!(UART5->S1 & UART_S1_RDRF_MASK));
	return UART5->D;
}

// 	Funkce na prehrani tonu za pomoci informaci jako frekvence a delka
void playTone(int hz, int length){
	// Core = 50Mhz
	// Divider 8
	// 50/8 = 6Mhz (zaokrouhleno)
	// 6 000 000 / Hz = Modulo
	int modulo = 6000000 / hz;
	FTM0_MOD = modulo;
	FTM0_C1V = modulo / 2;
	delay(length * 1000);
	// vypnuti po dokonceni prehravani tonu (vyprseny delay)
	FTM0_C1V = 0;
}

//	Intro tony pro Megalovania, ktere se opakuji na zacatku
void playIntro(){
	playTone(1, 10);
	playTone(DFIVE, 400);
	playTone(1, 350);
	playTone(AFOUR, 600);
	playTone(1, 350);
	playTone(GSFOUR, 350);
	playTone(1, 350);
	playTone(GFOUR, 350);
	playTone(1, 350);
	playTone(FFOUR, 600);
	playTone(1, 120);
	playTone(DFOUR, 350);
	playTone(FFOUR, 350);
	playTone(GFOUR, 350);
}

//	Megalovania song
void playMegalovania(){

	//INTRO 4 TIMES
	playTone(DFOUR, 350);
	playTone(1, 10);
	playTone(DFOUR, 350);
	playIntro();
	playTone(CFOUR, 350);
	playTone(1, 10);
	playTone(CFOUR, 350);
	playIntro();
	playTone(BTHREE, 350);
	playTone(1, 10);
	playTone(BTHREE, 350);
	playIntro();
	playTone(ASTHREE, 350);
	playTone(1, 10);
	playTone(ASTHREE, 350);
	playIntro();
	//MAIN
	playTone(FFIVE, 350);
	playTone(1, 350);
	playTone(FFIVE, 350);
	playTone(1, 350);
	playTone(FFIVE, 350);
	playTone(1, 100);
	playTone(FFIVE, 380);
	playTone(1, 200);
	playTone(FFIVE, 700);
	playTone(1, 150);
	playTone(DFIVE, 350);
	playTone(1, 350);
	playTone(DFIVE, 850);
	playTone(1, 150);
	playTone(FFIVE, 350);
	playTone(1, 350);
	playTone(FFIVE, 350);
	playTone(1, 350);
	playTone(FFIVE, 350);
	playTone(1, 10);
	playTone(GFIVE, 350);
	playTone(1, 350);
	playTone(GSFIVE, 600);
	playTone(1, 20);
	playTone(GFIVE, 350);
	playTone(FFIVE, 350);
	playTone(DFIVE, 350);
	playTone(FFIVE, 350);
	playTone(1, 20);
	playTone(GFIVE, 420);
	playTone(1, 550);
	playTone(FFIVE, 350);
	playTone(1, 350);
	playTone(FFIVE, 350);
	playTone(1, 100);
	playTone(FFIVE, 350);
	playTone(1, 350);
	playTone(GFIVE, 350);
	playTone(1, 350);
	playTone(GSFIVE, 280);
	playTone(1, 350);
	playTone(AFIVE, 350);
	playTone(1, 350);
	playTone(CSIX, 350);
	playTone(1, 350);
	playTone(AFIVE, 620);
	playTone(1, 350);
	playTone(DSIX, 350);
	playTone(1, 350);
	playTone(DSIX, 350);
	playTone(1, 350);
	playTone(DSIX, 350);
	playTone(AFIVE, 350);
	playTone(DSIX, 370);
	playTone(CSIX, 1400);
	playTone(1, 350);
	playTone(AFIVE, 350);
	playTone(1, 350);
	playTone(AFIVE, 350);
	playTone(1, 400);
	playTone(AFIVE, 350);
	playTone(1, 100);
	playTone(AFIVE, 350);
	playTone(1, 350);
	playTone(AFIVE, 600);
	playTone(GFIVE, 350);
	playTone(1, 350);
	playTone(GFIVE, 1200);
	playTone(1, 350);
	playTone(AFIVE, 350);
	playTone(1, 350);
	playTone(AFIVE, 350);
	playTone(1, 400);
	playTone(AFIVE, 350);
	playTone(1, 400);
	playTone(GFIVE, 350);
	playTone(1, 350);
	playTone(AFIVE, 350);
	playTone(1, 1);
	playTone(DSIX, 350);
	playTone(1, 350);
	playTone(AFIVE, 350);
	playTone(1, 80);
	playTone(GFIVE, 380);
	playTone(1, 250);
	playTone(DSIX, 700);
	playTone(AFIVE, 680);
	playTone(GFIVE, 680);
	playTone(FFIVE, 680);
	playTone(CSIX, 700);
	playTone(GFIVE, 680);
	playTone(FFIVE, 680);
	playTone(EFIVE, 580);
	playTone(1, 100);
	playTone(ASFOUR, 500);
	playTone(1, 350);
	playTone(DFOUR, 350);
	playTone(1, 350);
	playTone(FFIVE, 350);
	playTone(1, 350);
	playTone(CSIX, 2800);
	playTone(1, 400);
	playTone(DFIVE, 1700);
	playTone(1, 700);
	playTone(FFIVE, 350);
	playTone(DFIVE, 310);
	playTone(FFIVE, 320);
	playTone(GFIVE, 350);
	playTone(GSFIVE, 350);
	playTone(GFIVE, 320);
	playTone(FFIVE, 350);
	playTone(DFIVE, 310);
	playTone(FFIVE, 1200);
	playTone(GFIVE, 1700);
	playTone(1, 400);
	playTone(GSFIVE, 350);
	playTone(1, 20);
	playTone(AFIVE, 350);
	playTone(1, 20);
	playTone(CSIX, 350);
	playTone(1, 350);
	playTone(AFIVE, 400);
	playTone(GSFIVE, 350);
	playTone(GFIVE, 350);
	playTone(FFIVE, 350);
	playTone(DFIVE, 350);
	playTone(EFIVE, 350);
	playTone(1, 11);
	playTone(FFIVE, 650);
	playTone(1, 15);
	playTone(GFIVE, 650);
	playTone(1, 15);
	playTone(AFIVE, 650);
	playTone(1, 5);
	playTone(CSIX, 650);
	playTone(1, 15);
	playTone(CSSIX, 800);
	playTone(GSFIVE, 350);
	playTone(1, 350);
	playTone(GSFIVE, 350);
	playTone(GFIVE, 350);
	playTone(FFIVE, 350);
	playTone(GFIVE, 1500);

}

//	Funkce na rozpoznavani inputu v putty, jednotlive chary se premeni na odpovidajici ton.
//	Funkcni klavesy:
//	q,w,e,r,t,z,u,i,o,p
//	a,s,d,f,g,h,j,k,l
//	Po zmacknuti mezerniku (space) se prehraje pisnicka Megalovania
//	Vsechny ostatni znaky jsou brane jako default, tudiz jenom vypis do konzole: 'Play:\r\n'
void mapChar(char c){
	switch(c){
		//262 - 494
		case 'q':
			SendStr("C4 ");
			playTone(CFOUR, 300);
			break;

		case 'w':
			SendStr("D4 ");
			playTone(DFOUR, 300);
			break;

		case 'e':
			SendStr("E4 ");
			playTone(EFOUR, 300);
			break;

		case 'r':
			SendStr("F4 ");
			playTone(FFOUR, 300);
			break;

		case 't':
			SendStr("G4 ");
			playTone(GFOUR, 300);
			break;

		case 'z':
			SendStr("A4 ");
			playTone(AFOUR, 300);
			break;

		case 'u':
			SendStr("B4 ");
			playTone(BFOUR, 300);
			break;

		// 523 - 1047
		case 'i':
			SendStr("C5 ");
			playTone(CFIVE, 300);
			break;

		case 'o':
			SendStr("D5 ");
			playTone(DFIVE, 300);
			break;

		case 'p':
			SendStr("E5 ");
			playTone(EFIVE, 300);
			break;

		case 'a':
			SendStr("F5 ");
			playTone(FFIVE, 300);
			break;

		case 's':
			SendStr("G5 ");
			playTone(GFIVE, 300);
			break;

		case 'd':
			SendStr("A5 ");
			playTone(AFIVE, 300);
			break;

		case 'f':
			SendStr("B5 ");
			playTone(BFIVE, 300);
			break;

		case 'g':
			SendStr("C6 ");
			playTone(CSIX, 300);
			break;

		case 32:
			SendStr("\r\nMegalovania\r\n");
			SendStr("...\r\n");
			playMegalovania();
			SendStr("Play:\r\n");
			break;

		default:
			SendStr("\r\n");
			break;
	}
}

//	MAIN na init a nekonecnou smycku cekajici na input
int main(void){
	MCUInit();
	PortsInit();
	UART5Init();
	TimerInit();

	SendStr("--- Jednoduchy klavir ---\r\n");
	char c;
    SendStr("Play (q-p, a-g):\r\n");
    while (1) {
    	c = ReceiveCh();
    	mapChar(c);
    }
}
