DROP TABLE organizator CASCADE CONSTRAINTS;
DROP TABLE odbornik CASCADE CONSTRAINTS;
DROP TABLE patron CASCADE CONSTRAINTS;
DROP TABLE alkoholik CASCADE CONSTRAINTS;
DROP TABLE alkohol CASCADE CONSTRAINTS;
DROP TABLE odbornik_alkoholik CASCADE CONSTRAINTS;
DROP TABLE upominka CASCADE CONSTRAINTS;
DROP TABLE sezeni CASCADE CONSTRAINTS;
DROP TABLE sezeni_alkoholik CASCADE CONSTRAINTS;
DROP TABLE sezeni_patron CASCADE CONSTRAINTS;
DROP TABLE schuzka CASCADE CONSTRAINTS;
DROP TABLE sezeni_odbornik CASCADE CONSTRAINTS;


/*-----------------------------------------VYTVORENI TABULEK-----------------------------------------*/

CREATE TABLE organizator(
    id_organizator NUMBER GENERATED ALWAYS AS IDENTITY(START with 1 INCREMENT by 1) PRIMARY KEY,
    jmeno VARCHAR2(60) NOT NULL,
    prijmeni VARCHAR(60) NOT NULL,
    pohlavi VARCHAR2(10) NOT NULL CHECK(pohlavi IN('Muz','Zena')),
    datum_narozeni DATE NOT NULL
);

CREATE TABLE odbornik(
    rodne_cislo NUMBER NOT NULL CHECK(REGEXP_LIKE(rodne_cislo,'^[0-9]{2}([0257][1-9]|[1368][0-2])(0[1-9]|[12][0-9]|3[01])([0-9]{3,4})$')) PRIMARY KEY,
    jmeno VARCHAR2(60) NOT NULL,
    prijmeni VARCHAR(60) NOT NULL,
    pohlavi VARCHAR2(10) NOT NULL CHECK(pohlavi IN('Muz','Zena')),
    kontakt VARCHAR2(15) NOT NULL CHECK(REGEXP_LIKE(kontakt,'^\+(420|421)[0-9]{9}$')),
    datum_narozeni DATE NOT NULL,
    lekarska_praxe VARCHAR2(300) NOT NULL,
    expertiza VARCHAR2(200) NOT NULL
);

CREATE TABLE patron(
    id_patron NUMBER GENERATED ALWAYS AS IDENTITY(START with 1 INCREMENT by 1) PRIMARY KEY,
    jmeno VARCHAR2(60) NOT NULL,
    prijmeni VARCHAR(60) NOT NULL,
    pohlavi VARCHAR2(10) NOT NULL CHECK(pohlavi IN('Muz','Zena')),
    datum_narozeni DATE NOT NULL
);

CREATE TABLE alkoholik(
    id_alkoholik NUMBER NOT NULL PRIMARY KEY,
    datum_narozeni DATE NOT NULL,
    pohlavi VARCHAR2(10) NOT NULL CHECK (pohlavi IN('Muz','Zena')),
    kontakt VARCHAR2(15) NOT NULL CHECK(REGEXP_LIKE(kontakt,'^\+(420|421)[0-9]{9}$')),
    id_patron NUMBER NOT NULL,
    
    CONSTRAINT alkoholik_patron_FK FOREIGN KEY (id_patron)
    REFERENCES patron(id_patron)
);

CREATE TABLE alkohol(
    id_lahev NUMBER GENERATED ALWAYS AS IDENTITY(START with 1 INCREMENT by 1) PRIMARY KEY,
    mira NUMBER NOT NULL,
    puvod VARCHAR2(30),
    typ VARCHAR2(20) NOT NULL CHECK(typ IN('Vino','Pivo','Gin','Rum','Slivovice','Vodka','Jagermeister','Whisky','Burcak','Tequila')),
    id_alkoholik NUMBER NOT NULL,
    rodne_cislo NUMBER,
    
    CONSTRAINT alkohol_alkoholik_FK FOREIGN KEY (id_alkoholik)
    REFERENCES alkoholik(id_alkoholik),
    
    CONSTRAINT alkohol_odbornik_FK FOREIGN KEY (rodne_cislo)
    REFERENCES odbornik(rodne_cislo)
);

/*Preklenujici tabulka pro M:N alkoholik : odbornik*/
CREATE TABLE odbornik_alkoholik(
    id_odb_alk NUMBER GENERATED ALWAYS AS IDENTITY(START with 1 INCREMENT by 1) PRIMARY KEY,
    id_alkoholik NUMBER NOT NULL,
    rodne_cislo NUMBER NOT NULL,
    
    CONSTRAINT alkoholik_odbornik_FK FOREIGN KEY (id_alkoholik) 
    REFERENCES alkoholik(id_alkoholik),
    
    CONSTRAINT odbornik_alkoholik_FK FOREIGN KEY (rodne_cislo) 
    REFERENCES odbornik(rodne_cislo)
);

CREATE TABLE upominka(
    id_upominka NUMBER GENERATED ALWAYS AS IDENTITY(START with 1 INCREMENT by 1) PRIMARY KEY,
    posledni_navsteva DATE NOT NULL,
    
    id_alkoholik NUMBER NOT NULL,
    CONSTRAINT upominka_alkoholik_FK FOREIGN KEY (id_alkoholik) 
    REFERENCES alkoholik(id_alkoholik)    
);

CREATE TABLE sezeni(
    id_sezeni NUMBER GENERATED ALWAYS AS IDENTITY(START with 1 INCREMENT by 1) PRIMARY KEY,
    datum_cas TIMESTAMP NOT NULL,
    misto_konani VARCHAR2(50) NOT NULL,
    id_organizator NUMBER NOT NULL,
    
    CONSTRAINT sezeni_organizator_FK FOREIGN KEY (id_organizator) 
    REFERENCES organizator(id_organizator)
);

/*Preklenujici tabulka pro M:N sezeni:alkoholik*/
CREATE TABLE sezeni_alkoholik(
    id_sez_alk NUMBER GENERATED ALWAYS AS IDENTITY(START with 1 INCREMENT by 1) PRIMARY KEY,
    id_sezeni NUMBER NOT NULL,
    id_alkoholik NUMBER NOT NULL,
    
    CONSTRAINT sezeni_alkoholik_FK FOREIGN KEY (id_sezeni) 
    REFERENCES sezeni(id_sezeni),
    
    CONSTRAINT alkoholik_sezeni_FK FOREIGN KEY (id_alkoholik) 
    REFERENCES alkoholik(id_alkoholik)   
);

CREATE TABLE sezeni_patron(
    id_sez_pat NUMBER GENERATED ALWAYS AS IDENTITY(START with 1 INCREMENT by 1) PRIMARY KEY,
    id_sezeni NUMBER NOT NULL,
    id_patron NUMBER NOT NULL,
    
    CONSTRAINT sezeni_patron_FK FOREIGN KEY (id_sezeni) 
    REFERENCES sezeni(id_sezeni),
    
    CONSTRAINT patron_sezeni_FK FOREIGN KEY (id_patron) 
    REFERENCES patron(id_patron)   
);

CREATE TABLE sezeni_odbornik(
    id_sez_odb NUMBER GENERATED ALWAYS AS IDENTITY(START with 1 INCREMENT by 1) PRIMARY KEY,
    id_sezeni NUMBER NOT NULL,
    rodne_cislo NUMBER NOT NULL,
    
    CONSTRAINT sezeni_odbornik_FK FOREIGN KEY (id_sezeni) 
    REFERENCES sezeni(id_sezeni),
    
    CONSTRAINT odbornik_sezeni_FK FOREIGN KEY (rodne_cislo) 
    REFERENCES odbornik(rodne_cislo)
);


CREATE TABLE schuzka(
    id_schuzka NUMBER GENERATED ALWAYS AS IDENTITY(START with 1 INCREMENT by 1) PRIMARY KEY,
    datum DATE NOT NULL,
    misto_konani VARCHAR2(50) NOT NULL, 
    id_alkoholik NUMBER NOT NULL,
    id_patron NUMBER NOT NULL,
    
    CONSTRAINT schuzka_alkoholik_FK FOREIGN KEY (id_alkoholik) 
    REFERENCES alkoholik(id_alkoholik),
    
    CONSTRAINT schuzka_patron_FK FOREIGN KEY (id_patron) 
    REFERENCES patron(id_patron)
);

/*-----------------------------------------TRIGGERY-----------------------------------------*/

/*Trigger na kontrolu datumu planovane schuzky*/
CREATE OR REPLACE TRIGGER kontrola_datumu_schuzky
BEFORE INSERT ON schuzka
FOR EACH ROW
BEGIN
    IF  :new.datum < SYSDATE
    THEN
        RAISE_APPLICATION_ERROR(-20000, 'Termin schuzky nesmi byt v minulosti a nesmi byt prazdny');
    END IF;
END;
/


/* Trigger na inkrementaci ID alkoholika */
DROP SEQUENCE id_alkoholik_sq;
CREATE SEQUENCE id_alkoholik_sq START WITH 1 INCREMENT BY 1;
CREATE OR REPLACE TRIGGER id_alkoholika_trigger
 BEFORE INSERT ON alkoholik 
 FOR EACH ROW
BEGIN
  SELECT id_alkoholik_sq.nextval INTO :NEW.id_alkoholik FROM DUAL;
END;
/

/* Trigger na kontrolu poctu alkoholiku na sezeni - max 12 */
CREATE OR REPLACE TRIGGER kontrola_poctu_alkoholiku_na_sezeni
BEFORE INSERT ON sezeni_alkoholik
FOR EACH ROW
DECLARE pocet INTEGER;
BEGIN
    SELECT COUNT(sezeni_alkoholik.id_sezeni) INTO pocet
    FROM sezeni_alkoholik 
    WHERE sezeni_alkoholik.id_sezeni = :NEW.id_sezeni;

    IF (pocet >= 12) THEN
        RAISE_APPLICATION_ERROR(-20001, 'Maximalni pocet alkoholiku na jednom sezeni je 12');
    END IF;
END;
/


/*-----------------------------------------PROCEDURY-----------------------------------------*/

/* Procedura na procentualni zastoupeni druhu alkoholu mezi alkoholiky */
CREATE OR REPLACE PROCEDURE  procentualni_zastoupeni_podle_druhu_alkoholu AS 
    CURSOR veskery_alkohol IS 
    SELECT * FROM alkohol;
    
    alk alkohol%ROWTYPE;   
    pocet_alkoholiku NUMBER;
    vino NUMBER := 0;
    pivo NUMBER := 0;
    gin NUMBER := 0;
    rum NUMBER := 0;
    slivovice NUMBER := 0;
    vodka NUMBER := 0;
    jager NUMBER := 0;
    whisky NUMBER := 0;
    burcak NUMBER := 0;
    tequila NUMBER := 0;
    
BEGIN
    SELECT COUNT(alkoholik.id_alkoholik) INTO pocet_alkoholiku FROM alkoholik, alkohol
    WHERE alkoholik.id_alkoholik = alkohol.id_alkoholik ;
    IF (pocet_alkoholiku = 0) THEN
        RAISE NO_DATA_FOUND;
    END IF;
    OPEN veskery_alkohol;
    LOOP
        FETCH veskery_alkohol INTO alk;
        EXIT WHEN veskery_alkohol%NOTFOUND;
       
        
        IF alk.typ = 'Vino' THEN
            vino := vino+1;
        ELSIF alk.typ = 'Pivo' THEN
            pivo := pivo+1;
        ELSIF alk.typ = 'Gin' THEN
            gin := gin+1;
        ELSIF alk.typ = 'Rum' THEN
            rum := rum+1;
        ELSIF alk.typ = 'Slivovice' THEN
            slivovice := slivovice+1;
        ELSIF alk.typ = 'Vodka' THEN
            vodka := vodka+1;
        ELSIF alk.typ = 'Jagermeister' THEN
            jager := jager+1;
        ELSIF alk.typ = 'Whisky' THEN
            whisky := whisky+1;
        ELSIF alk.typ = 'Burcak' THEN
            burcak := burcak+1;
        ELSIF alk.typ = 'Tequila' THEN
            tequila := tequila+1;
        END IF;  
    END LOOP;
    
    
    dbms_output.put_line('Procentualni zastoupeni druhu alkoholu mezi alkoholiky:');
    dbms_output.put_line('Vino:         '|| vino/pocet_alkoholiku * 100 ||      '%');
    dbms_output.put_line('Pivo:         '|| pivo/pocet_alkoholiku * 100 ||      '%');
    dbms_output.put_line('Gin:          '|| gin/pocet_alkoholiku * 100 ||       '%');
    dbms_output.put_line('Rum:          '|| rum/pocet_alkoholiku * 100 ||       '%');
    dbms_output.put_line('Slivovice:    '|| slivovice/pocet_alkoholiku * 100 || '%');
    dbms_output.put_line('Vodka:        '|| vodka/pocet_alkoholiku * 100 ||     '%');
    dbms_output.put_line('Jagermeister: '|| jager/pocet_alkoholiku * 100 ||     '%');
    dbms_output.put_line('Whisky:       '|| whisky/pocet_alkoholiku * 100 ||    '%');
    dbms_output.put_line('Burcak:       '|| burcak/pocet_alkoholiku * 100 ||    '%');
    dbms_output.put_line('Tequila:      '|| tequila/pocet_alkoholiku * 100 ||   '%');
    
    EXCEPTION 
        WHEN NO_DATA_FOUND THEN
            RAISE_APPLICATION_ERROR(-20003, 'Spatny pocet alkoholiku v databazi - 0 ');
        WHEN OTHERS THEN
            RAISE_APPLICATION_ERROR(-20004, 'Err');
    CLOSE veskery_alkohol;
END;
/

/*Procedura na vypsani poctu alkoholiku v jednotlivych vekovych kategoriich*/
CREATE OR REPLACE PROCEDURE pocet_alkoholiku_podle_vekovych_kategorii AS
    CURSOR alkoholici IS
    SELECT * FROM alkoholik;
    alkhlk alkoholik%ROWTYPE;
    vek_0_20 NUMBER := 0;
    vek_21_30 NUMBER := 0;
    vek_31_40 NUMBER := 0;
    vek_41_50 NUMBER := 0;
    vek_51_60 NUMBER := 0;
    vek_61_inf NUMBER := 0;
    vek NUMBER;
    
BEGIN
    OPEN alkoholici;
    LOOP
        FETCH alkoholici INTO alkhlk;
        EXIT WHEN alkoholici%NOTFOUND;
        vek := (MONTHS_BETWEEN(SYSDATE, alkhlk.datum_narozeni) / 12);
                
        IF (vek <= 20) THEN
            vek_0_20 := vek_0_20 + 1;
        ELSIF (vek <= 30) THEN
            vek_21_30 := vek_21_30 + 1;
        ELSIF (vek <= 40) THEN
            vek_31_40 := vek_31_40 + 1;
        ELSIF (vek <= 50) THEN
            vek_41_50 := vek_41_50 + 1;
        ELSIF (vek <= 60) THEN
            vek_51_60 := vek_51_60 + 1;
        ELSE
            vek_61_inf := vek_61_inf + 1;
        END IF;
    END LOOP;
    dbms_output.put_line('Pocet alkoholiku v jednotlivych vekovych skupinach:');
    dbms_output.put_line('Pod 20 let:       '|| vek_0_20);
    dbms_output.put_line('21 az 30 let:     '|| vek_21_30);
    dbms_output.put_line('31 az 40 let:     '|| vek_31_40);
    dbms_output.put_line('41 az 50 let:     '|| vek_41_50);
    dbms_output.put_line('51 az 60 let:     '|| vek_51_60);
    dbms_output.put_line('Nad 60 let:       '|| vek_61_inf);
    
    CLOSE alkoholici;
END;
/


/*-----------------------------------------INSERTY-----------------------------------------*/

/*organizator*/
INSERT INTO organizator(jmeno, prijmeni, pohlavi, datum_narozeni) VALUES ('Otakar','Brezina','Muz',date '1975-03-12');
INSERT INTO organizator(jmeno, prijmeni, pohlavi, datum_narozeni) VALUES ('Angelika','Doe','Zena',date '1982-08-22');
INSERT INTO organizator(jmeno, prijmeni, pohlavi, datum_narozeni) VALUES ('Bartolomej','Novak','Muz',date '1969-01-30');
INSERT INTO organizator(jmeno, prijmeni, pohlavi, datum_narozeni) VALUES ('Lucie','Vocetkova','Zena',date '1986-12-15');

/*odbornici*/
INSERT INTO odbornik(rodne_cislo,jmeno, prijmeni, pohlavi, kontakt, datum_narozeni, lekarska_praxe, expertiza) VALUES (9653164554,'Ondrej', 'Somr','Muz','+420888555000',date '1967-03-25','Pracoval jsem 5 let v Psychiatricke nemocnici v Brne a nasledne 10 let v Kromerizi.','Zabyval jsem se lecbou dusevne chorych lidi, specialne lidi s roztrousenou sklerozou a schizofrenii.');
INSERT INTO odbornik(rodne_cislo,jmeno, prijmeni, pohlavi, kontakt, datum_narozeni, lekarska_praxe, expertiza) VALUES (9808267634,'Vladimir', 'Grumplon','Muz','+420580666182',date '1988-04-20','Me zkusenosti nejsou prilis rozsahle. Tri roky jsem byl na praxi v Psychiatricke lecebne v Jihlave. Nyni jsem opustil praci, kde jsem byl 2 roky a pracoval jako zednik.', 'Mirne zacatecnicka. Mam zkusenosti s lidmi a jejich alkoholickymi problemy.');
INSERT INTO odbornik(rodne_cislo,jmeno, prijmeni, pohlavi, kontakt, datum_narozeni, lekarska_praxe, expertiza) VALUES (9204078631,'zaneta', 'Vondalova','Zena','+420851877965',date '1982-09-18','Pracovala jsem 6 let jako primarka Psychiatricke nemocnice Havlickuv Brod. Nyni se chci zabyvat hlavne lidmi, kteri maji problem s alkoholem, nebo? si myslim, ze to v nasem state patri mezi jednu z nejvetsich potizi.', 'Zkusenosti mam bohate.');
INSERT INTO odbornik(rodne_cislo,jmeno, prijmeni, pohlavi, kontakt, datum_narozeni, lekarska_praxe, expertiza) VALUES (9661139719,'Pavel', 'Machacek','Muz','+420155788965',date '1961-03-25','Jsem velice zkuseny soukromy lekar s bohatou praxi s alkoholiky, tudiz si myslim, ze vam muzu spoustu nabidnout.', 'Vylecil jsem jiz spoustu lidi zavislych na alkoholu.');

/*patroni*/
INSERT INTO patron(jmeno,prijmeni,pohlavi,datum_narozeni) VALUES('Petr','Prochazka','Muz', date '1976-05-21');
INSERT INTO patron(jmeno,prijmeni,pohlavi,datum_narozeni) VALUES('Branislav','Kovar','Muz', date '1978-03-8');
INSERT INTO patron(jmeno,prijmeni,pohlavi,datum_narozeni) VALUES('Dalibor','Klobaska','Muz', date '1977-08-9');
INSERT INTO patron(jmeno,prijmeni,pohlavi,datum_narozeni) VALUES('Andrea','Dostalova','Zena', date '1972-03-22');
INSERT INTO patron(jmeno,prijmeni,pohlavi,datum_narozeni) VALUES('Alexandra','Souvlaki','Zena', date '1976-07-25');
INSERT INTO patron(jmeno,prijmeni,pohlavi,datum_narozeni) VALUES('Gregory','Masenevitch','Muz', date '1979-07-25');

/*alkoholici*/
INSERT INTO alkoholik(datum_narozeni, pohlavi, kontakt, id_patron) VALUES(date '1968-08-03','Muz','+420777859135',1);
INSERT INTO alkoholik(datum_narozeni, pohlavi, kontakt, id_patron) VALUES(date '1998-10-23','Muz','+421762859135',2);
INSERT INTO alkoholik(datum_narozeni, pohlavi, kontakt, id_patron) VALUES(date '1968-08-03','Zena','+420777859135',4);
INSERT INTO alkoholik(datum_narozeni, pohlavi, kontakt, id_patron) VALUES(date '1986-11-03','Muz','+421688129875',2);
INSERT INTO alkoholik(datum_narozeni, pohlavi, kontakt, id_patron) VALUES(date '1968-05-12','Muz','+420768468435',5);
INSERT INTO alkoholik(datum_narozeni, pohlavi, kontakt, id_patron) VALUES(date '1968-03-03','Zena','+420707899135',3);
INSERT INTO alkoholik(datum_narozeni, pohlavi, kontakt, id_patron) VALUES(date '1968-08-03','Muz','+420797121358', 3);


/*alkohol*/
INSERT INTO alkohol(mira, puvod, typ, id_alkoholik) VALUES(2.5,'Koupeno v obchode','Vino',3);
INSERT INTO alkohol(mira, puvod, typ, id_alkoholik,rodne_cislo) VALUES(4.8,'Na svatbe','Slivovice',2, 9808267634);
INSERT INTO alkohol(mira, puvod, typ, id_alkoholik) VALUES(1.2,'Od pratel','Pivo',6);
INSERT INTO alkohol(mira, puvod, typ, id_alkoholik) VALUES(0.95,'Z domacich zasob','Vino',1);
INSERT INTO alkohol(mira, puvod, typ, id_alkoholik,rodne_cislo) VALUES(6.5,'Na svatbe','Jagermeister',3, 9661139719);
INSERT INTO alkohol(mira, puvod, typ, id_alkoholik) VALUES(1.2,'Z domacich zasob','Slivovice',5);
INSERT INTO alkohol(mira, puvod, typ, id_alkoholik) VALUES(0.6,'Koupeno v obchode','Vino',3);
INSERT INTO alkohol(mira, puvod, typ, id_alkoholik,rodne_cislo) VALUES(1.35,'Hospoda','Gin',4, 9653164554);
INSERT INTO alkohol(mira, puvod, typ, id_alkoholik) VALUES(1.8,'Od pratel','Rum',6);
INSERT INTO alkohol(mira, puvod, typ, id_alkoholik) VALUES(0.7,'Ukradeno','Pivo',7);
INSERT INTO alkohol(mira, puvod, typ, id_alkoholik) VALUES(0.3,'Koupeno v obchode','Rum',2);
INSERT INTO alkohol(mira, puvod, typ, id_alkoholik) VALUES(2.26,'Na svatbe','Rum',7);
INSERT INTO alkohol(mira, puvod, typ, id_alkoholik) VALUES(5.8,'U kamarada','Rum',3);
INSERT INTO alkohol(mira, puvod, typ, id_alkoholik,rodne_cislo) VALUES(3.14,'U kamaradky','Rum',1, 9808267634);
INSERT INTO alkohol(mira, puvod, typ, id_alkoholik,rodne_cislo) VALUES(2.0,'U maminky','Rum',2, 9808267634);

INSERT INTO alkohol(mira, puvod, typ, id_alkoholik) VALUES(3.7,'Od pratel','Tequila',2);

/*provazani odborniku a alkoholiku*/
INSERT INTO odbornik_alkoholik(id_alkoholik,rodne_cislo) VALUES(1,9653164554);
INSERT INTO odbornik_alkoholik(id_alkoholik,rodne_cislo) VALUES(2,9808267634);
INSERT INTO odbornik_alkoholik(id_alkoholik,rodne_cislo) VALUES(3,9653164554);
INSERT INTO odbornik_alkoholik(id_alkoholik,rodne_cislo) VALUES(4,9653164554);
INSERT INTO odbornik_alkoholik(id_alkoholik,rodne_cislo) VALUES(5,9204078631);
INSERT INTO odbornik_alkoholik(id_alkoholik,rodne_cislo) VALUES(6,9661139719);
INSERT INTO odbornik_alkoholik(id_alkoholik,rodne_cislo) VALUES(7,9204078631);

/*upominky*/
INSERT INTO upominka(posledni_navsteva, id_alkoholik) VALUES (date '2018-01-04',3);
INSERT INTO upominka(posledni_navsteva, id_alkoholik) VALUES (date '2017-02-04',6);

/*zaznamy o sezeni*/
INSERT INTO sezeni(datum_cas, misto_konani,id_organizator) VALUES (timestamp '2019-12-23 16:15:00 +01:00', 'Becvarikova 14, Praha 4',1);
INSERT INTO sezeni(datum_cas, misto_konani,id_organizator) VALUES (timestamp '2019-6-26 18:30:00 +01:00', 'Bozetechova 12, Brno-Kralovo Pole',4);
INSERT INTO sezeni(datum_cas, misto_konani, id_organizator) VALUES (timestamp '2019-12-23 16:15:00 +01:00', 'Namesti Miru 259, Vracov',3);

/*propojeni alkoholiku a sezeni*/
INSERT INTO sezeni_alkoholik(id_sezeni, id_alkoholik) VALUES(1,1);
INSERT INTO sezeni_alkoholik(id_sezeni, id_alkoholik) VALUES(1,2);
INSERT INTO sezeni_alkoholik(id_sezeni, id_alkoholik) VALUES(1,3);
INSERT INTO sezeni_alkoholik(id_sezeni, id_alkoholik) VALUES(1,4);

INSERT INTO sezeni_alkoholik(id_sezeni, id_alkoholik) VALUES(2,1);
INSERT INTO sezeni_alkoholik(id_sezeni, id_alkoholik) VALUES(2,2);
INSERT INTO sezeni_alkoholik(id_sezeni, id_alkoholik) VALUES(2,3);
INSERT INTO sezeni_alkoholik(id_sezeni, id_alkoholik) VALUES(2,4);
INSERT INTO sezeni_alkoholik(id_sezeni, id_alkoholik) VALUES(2,5);
INSERT INTO sezeni_alkoholik(id_sezeni, id_alkoholik) VALUES(2,6);

INSERT INTO sezeni_alkoholik(id_sezeni, id_alkoholik) VALUES(3,1);
INSERT INTO sezeni_alkoholik(id_sezeni, id_alkoholik) VALUES(3,2);
INSERT INTO sezeni_alkoholik(id_sezeni, id_alkoholik) VALUES(3,3);
INSERT INTO sezeni_alkoholik(id_sezeni, id_alkoholik) VALUES(3,4);
INSERT INTO sezeni_alkoholik(id_sezeni, id_alkoholik) VALUES(3,5);
INSERT INTO sezeni_alkoholik(id_sezeni, id_alkoholik) VALUES(3,6);
INSERT INTO sezeni_alkoholik(id_sezeni, id_alkoholik) VALUES(3,7);
INSERT INTO sezeni_alkoholik(id_sezeni, id_alkoholik) VALUES(3,1);
INSERT INTO sezeni_alkoholik(id_sezeni, id_alkoholik) VALUES(3,2);
INSERT INTO sezeni_alkoholik(id_sezeni, id_alkoholik) VALUES(3,3);
INSERT INTO sezeni_alkoholik(id_sezeni, id_alkoholik) VALUES(3,4);
INSERT INTO sezeni_alkoholik(id_sezeni, id_alkoholik) VALUES(3,5);

/*propojeni patronu a sezeni*/
INSERT INTO sezeni_patron(id_sezeni,id_patron) VALUES(1,2);
INSERT INTO sezeni_patron(id_sezeni,id_patron) VALUES(1,4);
INSERT INTO sezeni_patron(id_sezeni,id_patron) VALUES(1,5);

INSERT INTO sezeni_patron(id_sezeni,id_patron) VALUES(2,2);

INSERT INTO sezeni_patron(id_sezeni,id_patron) VALUES(3,1);
INSERT INTO sezeni_patron(id_sezeni,id_patron) VALUES(3,2);
INSERT INTO sezeni_patron(id_sezeni,id_patron) VALUES(3,3);
INSERT INTO sezeni_patron(id_sezeni,id_patron) VALUES(3,5);

/*propojeni odborniku a sezeni*/
INSERT INTO sezeni_odbornik(id_sezeni,rodne_cislo) VALUES(1,9653164554);
INSERT INTO sezeni_odbornik(id_sezeni,rodne_cislo) VALUES(1,9808267634);
INSERT INTO sezeni_odbornik(id_sezeni,rodne_cislo) VALUES(1,9204078631);
INSERT INTO sezeni_odbornik(id_sezeni,rodne_cislo) VALUES(2,9661139719);
INSERT INTO sezeni_odbornik(id_sezeni,rodne_cislo) VALUES(3,9204078631);
INSERT INTO sezeni_odbornik(id_sezeni,rodne_cislo) VALUES(3,9661139719);

/*schuzky*/
INSERT INTO schuzka(datum,misto_konani,id_alkoholik,id_patron) VALUES(date '2020-11-08','Kabinet Hulicu, Brno',5,5);
INSERT INTO schuzka(datum,misto_konani,id_alkoholik,id_patron) VALUES(date '2021-09-11','Park, Vracov',1,1);
INSERT INTO schuzka(datum,misto_konani,id_alkoholik,id_patron) VALUES(date '2023-01-01','Amfiteatr, Straznice',3,4);
INSERT INTO schuzka(datum,misto_konani,id_alkoholik,id_patron) VALUES(date '2019-06-18','Petrinska rozhledna, Praha',4,2);
INSERT INTO schuzka(datum,misto_konani,id_alkoholik,id_patron) VALUES(date '2023-03-20','Londynske oko, Londyn',2,2);
INSERT INTO schuzka(datum,misto_konani,id_alkoholik,id_patron) VALUES(date '2020-03-20','Brno, Ceska republika',3,2);
INSERT INTO schuzka(datum,misto_konani,id_alkoholik,id_patron) VALUES(date '2021-08-02','Makaron, Madarsko',3,2);
INSERT INTO schuzka(datum,misto_konani,id_alkoholik,id_patron) VALUES(date '2020-03-22','Veseli nad Moravou, Ceska republika',4,2);
INSERT INTO schuzka(datum,misto_konani,id_alkoholik,id_patron) VALUES(date '2020-03-23','Berlin, Nemecko',4,1);


/*-----------------------------------------ZKOUSKY ZACHYCENI TRIGGERU-----------------------------------------*/
/* Test maximalniho poctu alkoholiku na jednom sezeni */
INSERT INTO sezeni_alkoholik(id_sezeni, id_alkoholik) VALUES(3,6);
INSERT INTO sezeni_alkoholik(id_sezeni, id_alkoholik) VALUES(3,5);
INSERT INTO sezeni_alkoholik(id_sezeni, id_alkoholik) VALUES(3,2);

/* Test spravnosti datumu schuzky - musi to byt plan do budoucna */
INSERT INTO schuzka(datum,misto_konani,id_alkoholik,id_patron) VALUES(date '2018-08-02','Makaron, Madarsko',3,2);
INSERT INTO schuzka(datum,misto_konani,id_alkoholik,id_patron) VALUES(date '2000-03-22','Kostel, Vracov',4,2);
INSERT INTO schuzka(datum,misto_konani,id_alkoholik,id_patron) VALUES(date '2019-03-23','Amfiteatr, Straznice',4,1);


/*-----------------------------------------SELECTY-----------------------------------------*/

/*Dotaz vypisujici patrony jednotlivych alkoholiku a datum narozeni patronu*/
SELECT alkoholik.id_alkoholik AS "Alkoholik", patron.jmeno || ' ' || patron.prijmeni AS "Patron", to_char(patron.datum_narozeni, 'DD.MM.YYYY') as "Datum narozeni"
FROM alkoholik,patron
WHERE alkoholik.id_patron = patron.id_patron;

/*Dotaz pro vypis organizatoru jednotlivych sezeni*/
SELECT sezeni.misto_konani AS "Misto", to_char(datum_cas, 'DD.MM.YYYY') as "Datum", to_char(sezeni.datum_cas,'HH24:MI') as "Cas", organizator.jmeno || ' ' || organizator.prijmeni AS "Organizator"
FROM sezeni, organizator
WHERE sezeni.id_organizator = organizator.id_organizator;

/*Dotaz pro vyber vsech alkoholiku, kteri se zucastnili daneho sezeni*/
SELECT id_alkoholik as "ID alkoholika", sezeni.misto_konani as "Misto" 
FROM 
    (SELECT sezeni_alkoholik.id_sezeni as sezeni, alkoholik.id_alkoholik as id_alkoholik
    FROM alkoholik,sezeni_alkoholik
    WHERE alkoholik.id_alkoholik = sezeni_alkoholik.id_alkoholik), sezeni
WHERE sezeni.id_sezeni = sezeni;

/*Dotaz pro vyber alkoholiku a jejich patronu zucastnenych na jimi sjednane schuzce*/
SELECT misto as "Misto", to_char(datum, 'DD.MM.YYYY') as "Datum", patron as "Patron", alkoholik.id_alkoholik as "ID alkoholika"
FROM
    (SELECT schuzka.misto_konani as misto, patron.jmeno || ' ' || patron.prijmeni AS patron, schuzka.datum as datum,schuzka.id_alkoholik as s_id_alkoholik
    FROM schuzka,patron
    WHERE schuzka.id_patron = patron.id_patron), alkoholik
WHERE alkoholik.id_alkoholik = s_id_alkoholik;

/*GROUP BY DOTAZY*/

/*Dotaz pro ziskani poctu alkoholiku jednotlivych odborniku*/
SELECT odbornik as "Odbornik", to_char(datum, 'DD.MM.YYYY') as "Datum narozeni", count(alkoholik.id_alkoholik) as "Pocet lecenych alkoholiku"
FROM
    (SELECT odbornik_alkoholik.id_alkoholik as MN_id_alkoholik, odbornik.jmeno || ' ' || odbornik.prijmeni as odbornik, odbornik.datum_narozeni as datum
    FROM odbornik_alkoholik,odbornik
    WHERE odbornik_alkoholik.rodne_cislo = odbornik.rodne_cislo), alkoholik
WHERE alkoholik.id_alkoholik = MN_id_alkoholik
GROUP BY odbornik, datum
ORDER BY count(alkoholik.id_alkoholik) ASC;

/*Dotaz pro vypis prumerne miry alkoholu v krvi alkoholika z evidence serazeno podle nejvetsiho alkoholika*/
SELECT alkoholik.id_alkoholik as "ID alkoholika", to_char(round(avg(alkohol.mira), 3), '99990D9') as "Prumer"
FROM alkohol, alkoholik
WHERE alkohol.id_alkoholik = alkoholik.id_alkoholik
GROUP BY alkoholik.id_alkoholik
ORDER BY avg(alkohol.mira) DESC;

/*DOTAZ S EXISTS*/
/*Vypise vsechny patrony, kteri patri k nejakemu alkoholikovi*/
SELECT patron.jmeno || ' ' || patron.prijmeni as "Patron", to_char(patron.datum_narozeni, 'DD.MM.YYYY') as "Datum narozeni"
FROM patron
WHERE EXISTS (SELECT alkoholik.id_alkoholik FROM alkoholik WHERE alkoholik.id_patron = patron.id_patron);

/*DOTAZ S IN*/
/*Vypise alkoholiky, kteri piji pivo*/
SELECT alkoholik.id_alkoholik as "ID alkoholika"
FROM alkoholik
WHERE alkoholik.id_alkoholik IN (
        SELECT alkohol.id_alkoholik FROM alkohol WHERE alkohol.id_alkoholik = alkoholik.id_alkoholik
        AND alkohol.typ = 'Pivo');
        
        
/*-----------------------------------------SPUSTENI PROCEDUR-----------------------------------------*/       
/*Zkouska procedur*/

SET serveroutput ON;

EXEC procentualni_zastoupeni_podle_druhu_alkoholu();    
EXEC pocet_alkoholiku_podle_vekovych_kategorii();


/*-----------------------------------------EXPLAIN PLANS S INDEXY-----------------------------------------*/
/* Prvni spusteni dotazu pred vytvorenim indexu */
EXPLAIN PLAN FOR
SELECT alkoholik.id_alkoholik as "ID alkoholika", to_char(round(avg(alkohol.mira), 3), '99990D9') as "Prumer"
FROM alkohol, alkoholik
WHERE alkohol.id_alkoholik = alkoholik.id_alkoholik
GROUP BY alkoholik.id_alkoholik
ORDER BY avg(alkohol.mira) DESC;

SELECT * FROM TABLE(dbms_xplan.display());

/* Vytvoreni indexu pro alkohol */
CREATE INDEX alk_index ON alkohol (id_alkoholik, mira);

/* Opakovane spusteni dotazu */
EXPLAIN PLAN FOR
SELECT alkoholik.id_alkoholik as "ID alkoholika", to_char(round(avg(alkohol.mira), 3), '99990D9') as "Prumer"
FROM alkohol, alkoholik
WHERE alkohol.id_alkoholik = alkoholik.id_alkoholik
GROUP BY alkoholik.id_alkoholik
ORDER BY avg(alkohol.mira) DESC;

SELECT * FROM TABLE(dbms_xplan.display());


/*-----------------------------------------PRISTUPOVA PRAVA GRANT-----------------------------------------*/

GRANT ALL ON organizator TO xgajdo22;
GRANT ALL ON odbornik TO xgajdo22;
GRANT ALL ON patron TO xgajdo22;
GRANT ALL ON alkoholik TO xgajdo22;
GRANT ALL ON alkohol TO xgajdo22;
GRANT ALL ON odbornik_alkoholik TO xgajdo22;
GRANT ALL ON upominka TO xgajdo22;
GRANT ALL ON sezeni TO xgajdo22;
GRANT ALL ON sezeni_alkoholik TO xgajdo22;
GRANT ALL ON sezeni_patron TO xgajdo22;
GRANT ALL ON sezeni_odbornik TO xgajdo22;
GRANT ALL ON schuzka TO xgajdo22;
GRANT EXECUTE ON pocet_alkoholiku_podle_vekovych_kategorii TO xgajdo22;
GRANT EXECUTE ON procentualni_zastoupeni_podle_druhu_alkoholu TO xgajdo22;

/*-----------------------------------------MATERIALIZOVANY POHLED-----------------------------------------*/

DROP MATERIALIZED VIEW alkoholik_MV_test;
/* Spousti XGAJDO22 - melo by fungovat bezchybne */

CREATE MATERIALIZED VIEW LOG ON xmacha73.alkoholik WITH PRIMARY KEY, ROWID(kontakt) INCLUDING NEW VALUES;
CREATE MATERIALIZED VIEW alkoholik_MV_test CACHE BUILD IMMEDIATE REFRESH ON COMMIT AS
    SELECT xmacha73.alkoholik.id_alkoholik AS "Alkoholik", xmacha73.patron.jmeno || ' ' || xmacha73.patron.prijmeni AS "Patron", to_char(xmacha73.patron.datum_narozeni, 'DD.MM.YYYY') as "Datum narozeni"
    FROM xmacha73.alkoholik, patron
    WHERE xmacha73.alkoholik.id_patron = xmacha73.patron.id_patron;
    
INSERT INTO xmacha73.alkoholik(datum_narozeni, pohlavi, kontakt, id_patron) VALUES (date '1968-08-03','Muz','+420777859135',1);
COMMIT;
SELECT * FROM alkoholik_MV_test;

GRANT ALL ON alkoholik_MV_test TO xgajdo22;
