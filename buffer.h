/*
 * Autor: Ondřej Šlampa (xslamp01)
 * Datum: 14.02.2015
 * Soubor: buffer.h
 * Komentář: Práce se vstupně výstupním bufferem.
*/

#include<stdint.h>
#include"tree.h"

#ifndef __KKO_BUFFER_H__
#define __KKO_BUFFER_H__

#define BUFFER_SIZE 160

/*
    Vstupně výstupní buffer.
*/
typedef struct{
    //pole bitů
    char* array;
    //index políčka před prvním obsazeným políčkem 
    uint64_t start;
    //index políčka za posledním obsazeným políčkem
    uint64_t end;
} Buffer;

/*
    Alokuje nový buffer.
    return nový prázdný buffer nebo NULL, pokud došlo k chybě
*/
Buffer* new_buffer();
/*
    Uvolnění bufferu b.
    b buffer
*/
void free_buffer(Buffer* b);
/*
    Vloží jeden kód znaku do bufferu.
    buffer buffer
    tree huffmanův strom
    node uzel stromu tree značící vkládaný kód
*/
void add_code(Buffer* buffer, Tree* tree, TreeNode* node);
/*
    Přidá jeden byte do bufferu
    b buffer
    c byte
*/
void add_byte(Buffer* b, unsigned char c);
/*
    Vyjmutí jednoho bitu z bufferu.
    b buffer
    return jeden bit z bufferu nebo -1, pokud je buffer prázdný
*/
char get_bit(Buffer* b);
/*
    Vyjmutí jednoho bytu z bufferu.
    b buffer
    return jeden byte z bufferu nebo -1, pokud v bufferu není dostatek bitů
*/
int64_t get_byte(Buffer* b);
/*
    Dokud je v bufferu dostatek bitů zapisuje byty do výstuního souboru.
    b buffer
    f výstupní soubor
    return počet zapsaných bytů
*/
uint64_t write_bytes(Buffer* b, FILE* f);
/*
    Zapíše obsah bufferu do výstupního souboru. Poslední zapsaný byte doplní
    kódem NULL_POINT a nulovým bytem.
    b buffer
    t strom
    f výstupní soubor
*/
uint64_t flush(Buffer* b, Tree* t, FILE* f);

#endif
