/*
 * Autor: Ondřej Šlampa (xslamp01)
 * Datum: 14.02.2015
 * Soubor: buffer.c
 * Komentář: Práce se vstupně výstupním bufferem.
*/

#include<stdlib.h>
#include<stdio.h>
#include<stdint.h>
#include"tree.h"
#include"buffer.h"

/*
    Alokuje nový buffer.
    return nový prázdný buffer nebo NULL, pokud došlo k chybě
*/
Buffer* new_buffer(){
    //alokace bufferu
    Buffer* b=malloc(sizeof(Buffer));
    if(b==NULL){
        return NULL;
    }
    
    //alokace paměti bufferu
    b->array=malloc(sizeof(char)*BUFFER_SIZE);
    if(b->array==NULL){
        free(b);
        return NULL;
    }
    
    //inicializace paměti bufferu
    for(uint64_t i=0;i<BUFFER_SIZE;i++){
        b->array[i]=-1;
    }
    
    b->start=0;
    b->end=1;
    
    return b;
}

/*
    Uvolnění bufferu b.
    b buffer
*/
void free_buffer(Buffer* b){
    if(b!=NULL){
        free(b->array);
        free(b);
    }
}

/*
    Přidá jeden bit do bufferu.
    c bit
    b buffer
*/
void add_bit(Buffer* b, char c){
    b->array[b->end]=c;
    b->end=(b->end+1)%BUFFER_SIZE;
}

/*
    Přidá jeden byte do bufferu
    b buffer
    c byte
*/
void add_byte(Buffer* b, unsigned char c){
    for(uint64_t i=0;i<8;i++){
        if(c<128){
            add_bit(b, 0);
        }
        else{
            add_bit(b, 1);
        }
        c<<=1;
    }
}

/*
    Vloží jeden kód znaku do bufferu.
    buffer buffer
    tree huffmanův strom
    node uzel stromu tree značící vkládaný kód
*/
void add_code(Buffer* buffer, Tree* tree, TreeNode* node){
    //pomocné pole
    char* s=malloc(32*sizeof(char));
    uint64_t i=0;
    
    //vkložím bity do pomocného pole
    while(node!=tree->root){
        //pokud je současný uzel levý potomek vložím 0 do pomocného pole
        if(node->parent->left==node){
            s[i]=0;
        }
        //jinak 1
        else{
            s[i]=1;
        }
        i++;
        node=node->parent;
    }
    
    //vložím bity do bufferu v opačném pořadí
    do{
        i--;
        add_bit(buffer, s[i]);
    }
    while(i>0);
    free(s);
}

/*
    Vyjmutí jednoho bitu z bufferu.
    b buffer
    return jeden bit z bufferu nebo -1, pokud je buffer prázdný
*/
char get_bit(Buffer* b){
    uint64_t next=(b->start+1)%BUFFER_SIZE;
    if(next==b->end){
        return -1;
    }
    else{
        b->start=next;
        return b->array[next];
    }
}

/*
    Spočítá počet bitů v bufferu.
    b buffer
    return počet bitů v bufferu b
*/
uint64_t get_number(Buffer* b){
    if(b->end>b->start){
        return b->end-b->start-1;
    }
    else{
        return b->end+BUFFER_SIZE-b->start-1;
    }
}

/*
    Vyjmutí jednoho bytu z bufferu.
    b buffer
    return jeden byte z bufferu nebo -1, pokud v bufferu není dostatek bitů
*/
int64_t get_byte(Buffer* b){
    //pokud je v bufferu málo bitů
    if(get_number(b)<8){
        return -1;
    }
    
    unsigned char c=0;
    char bit=-1;
    
    //vyjmutí bytu
    for(uint64_t i=0;i<8;i++){
        c<<=1;
        bit=get_bit(b);
        if(bit==1){
            c++;
        }
    }
    
    return c;
}

/*
    Dokud je v bufferu dostatek bitů zapisuje byty do výstuního souboru.
    b buffer
    f výstupní soubor
    return počet zapsaných bytů
*/
uint64_t write_bytes(Buffer* b, FILE* f){
    int64_t c=-1;
    uint64_t i=0;
    while((c=get_byte(b))!=-1){
        fputc(c, f);
        i++;
    }
    return i;
}

/*
    Zapíše obsah bufferu do výstupního souboru. Poslední zapsaný byte doplní
    kódem NULL_POINT a nulovým bytem.
    b buffer
    t strom
    f výstupní soubor
*/
uint64_t flush(Buffer* b, Tree* t, FILE* f){
    //počet zapsaných bytů
    uint64_t i=write_bytes(b, f);
    //pokud byly zapsány všechny bity
    if(get_number(b)==0){
        return i;
    }
    //vycpání bufferu kódem NULL_POINT
    add_code(b, t, get_node(t, NULL_POINT));
    //pokud v bufferu není dostatek bitů na zapsání bytu, vycpu ho nulovým bytem
    if(get_number(b)<8){
        add_byte(b, 0);
    }
    //zapsání jednoho bytu
    fputc(get_byte(b), f);
    i++;
    return i;
}

