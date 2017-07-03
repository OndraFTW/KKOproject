/*
 * Autor: Ondřej Šlampa (xslamp01)
 * Datum: 14.02.2015
 * Soubor: main.h
 * Komentář: Zpracování parametrů příkazové řádky a práce se soubory.
*/


#ifndef __KKO_MAIN_H__
#define __KKO_MAIN_H__

/*
    Akce, které může program provédst.
*/
typedef enum{
    //kodování
    ENCODE,
    //dekodování
    DECODE,
    //vypsání nápovědy
    HELP,
    //chyba
    ERROR
} Action;

/*
    Struktura pro uložení argumentů příkazové řádky.
*/
typedef struct{
    //vstupní soubor
    char* input;
    //výstupní soubor
    char* output;
    //logovací soubor
    char* log;
    //akce
    Action action;
} Args;

//text nápovědy
char* HELP_MSG="Huffman encoder/decoder.\nAuthor:Ondrej Slampa,xslamp01@stud.fit.vutbr.cz\n./ahed [options]\nOptions:\n\t-i FILE input file location\n\t-o FILE output file location\n\t-l FILE log file location\n\t-c encode\n\t-x decode\n\t-h print help\n";

#endif

