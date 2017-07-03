/*
 * Autor: Ondřej Šlampa (xslamp01)
 * Datum: 14.02.2015
 * Soubor: main.c
 * Komentář: Zpracování argumentů příkazové řádky a práce se soubory.
*/

#include<stdlib.h>
#include<stdio.h>
#include<stdint.h>
#include<unistd.h>
#include<getopt.h>
#include"ahed.h"
#include"main.h"

/*
    Zpracuje argumenty příkazové řádky.
    argc počet argumentů
    argv argumenty
    return zpracované argumenty
*/
Args parse_args(int argc, char **argv){
    Args a;
    a.input=NULL;
    a.output=NULL;
    a.log=NULL;
    a.action=ERROR;
    int64_t c=-1;
    
    //zpracování argumnetů pomocí fce getopt()
    while((c=getopt(argc, argv, "i:o:l:cxh"))!=-1){
        switch(c){
            case'i':a.input=optarg;break;
            case'o':a.output=optarg;break;
            case'l':a.log=optarg;break;
            case'c':a.action=ENCODE;break;
            case'x':a.action=DECODE;break;
            case'h':a.action=HELP;break;
        }
    }
    
    return a;
}

/*
    Hlavní funkce.
    argc počet argumentů
    argv argumenty
    return EXIT_SUCCESS nebo EXIT_FAILURE
*/
int main(int argc, char **argv){
    //argumenty příkazové řádky
    Args a=parse_args(argc, argv);
    
    //pokud nastala chyba ukončím program
    if(a.action==ERROR){
        return EXIT_FAILURE;
    }
    
    //vypsání nápovědy
    if(a.action==HELP){
        printf("%s", HELP_MSG);
        return EXIT_SUCCESS;
    }

    //vstupní soubor a jeho otevření
    FILE* input_file=NULL;
    if(a.input==NULL){
        input_file=stdin;
    }
    else{
        input_file=fopen(a.input, "r");
        if(input_file==NULL){
            return EXIT_FAILURE;
        }
    }
    
    //výstupní soubor a jeho otevření
    FILE* output_file=NULL;
    if(a.output==NULL){
        output_file=stdout;
    }
    else{
        output_file=fopen(a.output, "w");
        if(output_file==NULL){
            fclose(input_file);
            return EXIT_FAILURE;
        }
    }

    int64_t return_value=0;
    tAHED record;
    record.uncodedSize=0;
    record.codedSize=0;
    
    //provedení zadané operace
    if(a.action==ENCODE){
        return_value=AHEDEncoding(&record, input_file, output_file);
    }
    else if(a.action==DECODE){
        return_value=AHEDDecoding(&record, input_file, output_file);
    }
    
    //uzavření vstupného a výstupního souboru
    if(a.input!=NULL){
        fclose(input_file);
    }
    if(a.output!=NULL){
        fclose(output_file);
    }
    
    //pokud nastala chyba ukončím s chybou
    if(return_value==-1){
        return EXIT_FAILURE;
    }
    
    //pokus se má zapsat log zapíše se
    if(a.log!=NULL){
        FILE* log_file=fopen(a.log, "w");
        if(log_file==NULL){
            return EXIT_FAILURE;
        }
        fprintf(log_file, "login = xslamp01\nuncodedSize = %ld\ncodedSize = %ld", record.uncodedSize, record.codedSize);
        fclose(log_file);
    }
    
    return EXIT_SUCCESS;
}
	
