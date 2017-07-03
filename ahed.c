/*
 * Autor: Ondøej ©lampa (xslamp01)
 * Datum: 14.02.2015
 * Soubor: ahed.c
 * Komentáø: Huffmanovo kódování/dekódování.
*/

#include<stdlib.h>
#include<stdint.h>
#include"ahed.h"
#include"tree.h"
#include"buffer.h"

/* Nazev:
 *   AHEDEncoding
 * Cinnost:
 *   Funkce koduje vstupni soubor do vystupniho souboru a porizuje zaznam o kodovani.
 * Parametry:
 *   ahed - zaznam o kodovani
 *   inputFile - vstupni soubor (nekodovany)
 *   outputFile - vystupní soubor (kodovany)
 * Navratova hodnota: 
 *    0 - kodovani probehlo v poradku
 *    -1 - pøi kodovani nastala chyba
 */
int AHEDEncoding(tAHED *ahed, FILE *inputFile, FILE *outputFile){
	int64_t c=-1;
	uint64_t input_cnt=0;
	uint64_t output_cnt=0;
	Tree* tree=new_tree();
	Buffer* b=new_buffer();
	TreeNode* node=NULL;
    
    //kódování prvního bytu
    c=getc(inputFile);
    //pokud je vstupní soubor prázdný, je výstupní soubor aké prázdný
    if(c==-1){
        ahed->uncodedSize=0;
	    ahed->codedSize=0;
        free_tree(tree);
        free_buffer(b);
        return AHEDOK;
    }
    //zakódování prvního bytu
    else{
	    add_byte(b, c);
	    input_cnt++;
	    output_cnt+=write_bytes(b, outputFile);
	    node=create_node(tree, c);
	    increase_number(tree, node);
    }
    
    //kódování ostatních bytù
	while((c=getc(inputFile))!=EOF){
	    input_cnt++;
	    node=get_node(tree, c);
	    //tento byte se objevil poprvé - musím pøidat uzel do grafu
	    if(node==NULL){
	        add_code(b, tree, get_node(tree, NULL_POINT));
	        output_cnt+=write_bytes(b, outputFile);
	        add_byte(b, c);
	        output_cnt+=write_bytes(b, outputFile);
	        node=create_node(tree, c);
	        increase_number(tree, node);
	    }
	    //tento byte je ji¾ ve stromì - pouze zvý¹ím hodnotu uzlu
	    else{
	        add_code(b, tree, node);
	        output_cnt+=write_bytes(b, outputFile);
	        increase_number(tree, node);
	    }
	}
	
	output_cnt+=flush(b, tree, outputFile);
	ahed->uncodedSize=input_cnt;
	ahed->codedSize=output_cnt;
    free_tree(tree);
    free_buffer(b);
	return AHEDOK;
}

/*
    Dekóduje jeden byte ze vstupního souboru.
    t huffmanùv strom
    b buffer
    f vstupní soubor
    input_cnt poèítadlo vstupních bytù
    return uzel stromu reprezentující dekódovaný byte nebo NULL, pokud ve
    vstupním souboru není dal¹í zakódovaný byte
*/
TreeNode* decode_point(Tree* t, Buffer* b, FILE* f, uint64_t* input_cnt){
    //zaèínám prohledávat strom od koøene
    TreeNode* node=t->root;
    char bit=-1;
    int64_t byte=-1;
    
    //dokud nenarazím na list
    while(node->point<0){
        //naètu jeden bit
        bit=get_bit(b);
        //pokud v bufferu není jeden bit
        if(bit==-1){
            //naètu byte ze souboru do bufferu
            byte=getc(f);
            //pokud je soubor prázdný konèím
            if(byte==-1){
                return NULL;
            }
            *input_cnt+=1;
            add_byte(b, byte);
            bit=get_bit(b);
        }
        
        //pokud se naèetla 0 pøejdeme k levému uzlu jinak k pravému
        if(bit==0){
            node=node->left;
        }
        else{
            node=node->right;
        }
    }
    
    return node;
}

/* Nazev:
 *   AHEDDecoding
 * Cinnost:
 *   Funkce dekoduje vstupni soubor do vystupniho souboru a porizuje zaznam o dekodovani.
 * Parametry:
 *   ahed - zaznam o dekodovani
 *   inputFile - vstupni soubor (kodovany)
 *   outputFile - vystupní soubor (nekodovany)
 * Navratova hodnota: 
 *    0 - dekodovani probehlo v poradku
 *    -1 - pøi dekodovani nastala chyba
 */
int AHEDDecoding(tAHED *ahed, FILE *inputFile, FILE *outputFile){
	TreeNode* node=NULL;
	uint64_t input_cnt=0;
	uint64_t output_cnt=0;
	int64_t character=-1;
	int64_t byte=-1;
	Tree* tree=new_tree();
	Buffer* b=new_buffer();
	
	byte=getc(inputFile);
	//pokud je soubor prázdný
    if(byte==-1){
        ahed->uncodedSize=0;
	    ahed->codedSize=0;
        free_tree(tree);
        free_buffer(b);
        return AHEDOK;
    }
    //dekódování prvního bytu
    else{
	    add_byte(b, byte);
	    input_cnt++;
	    output_cnt+=write_bytes(b, outputFile);
	    node=create_node(tree, byte);
	    increase_number(tree, node);
    }
	
	//dekódování ostatních bytù
	//pokud je mo¾né dekódovat dal¹í byte opakuji
	while((node=decode_point(tree, b, inputFile, &input_cnt))!=NULL){
	    //byl dekódován NULL_POINT
	    if(node->point==NULL_POINT){
	        //naètu dal¹í byte
	        character=get_byte(b);
            //pokud není v bufferu naètu ho ze souboru
            if(character==-1){
                byte=getc(inputFile);
                //pokud v souboru není konèím
                if(byte==-1){
                    break;
                }
                input_cnt+=1;
                add_byte(b, byte);
                character=get_byte(b);
            }
            //vytvoøení nového uzlu stromu
            node=create_node(tree, character);
            increase_number(tree, node);
            output_cnt++;
            putc(character, outputFile);
	    }
	    //byl dekódován byte
	    else{
	        output_cnt++;
	        putc(node->point, outputFile);
	        increase_number(tree, node);
	    }
	}
	
	ahed->uncodedSize=output_cnt;
	ahed->codedSize=input_cnt;
    free_tree(tree);
    free_buffer(b);
	return AHEDOK;
}

