/*
 * Autor: Ond�ej �lampa (xslamp01)
 * Datum: 14.02.2015
 * Soubor: ahed.c
 * Koment��: Huffmanovo k�dov�n�/dek�dov�n�.
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
 *   outputFile - vystupn� soubor (kodovany)
 * Navratova hodnota: 
 *    0 - kodovani probehlo v poradku
 *    -1 - p�i kodovani nastala chyba
 */
int AHEDEncoding(tAHED *ahed, FILE *inputFile, FILE *outputFile){
	int64_t c=-1;
	uint64_t input_cnt=0;
	uint64_t output_cnt=0;
	Tree* tree=new_tree();
	Buffer* b=new_buffer();
	TreeNode* node=NULL;
    
    //k�dov�n� prvn�ho bytu
    c=getc(inputFile);
    //pokud je vstupn� soubor pr�zdn�, je v�stupn� soubor ak� pr�zdn�
    if(c==-1){
        ahed->uncodedSize=0;
	    ahed->codedSize=0;
        free_tree(tree);
        free_buffer(b);
        return AHEDOK;
    }
    //zak�dov�n� prvn�ho bytu
    else{
	    add_byte(b, c);
	    input_cnt++;
	    output_cnt+=write_bytes(b, outputFile);
	    node=create_node(tree, c);
	    increase_number(tree, node);
    }
    
    //k�dov�n� ostatn�ch byt�
	while((c=getc(inputFile))!=EOF){
	    input_cnt++;
	    node=get_node(tree, c);
	    //tento byte se objevil poprv� - mus�m p�idat uzel do grafu
	    if(node==NULL){
	        add_code(b, tree, get_node(tree, NULL_POINT));
	        output_cnt+=write_bytes(b, outputFile);
	        add_byte(b, c);
	        output_cnt+=write_bytes(b, outputFile);
	        node=create_node(tree, c);
	        increase_number(tree, node);
	    }
	    //tento byte je ji� ve strom� - pouze zv���m hodnotu uzlu
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
    Dek�duje jeden byte ze vstupn�ho souboru.
    t huffman�v strom
    b buffer
    f vstupn� soubor
    input_cnt po��tadlo vstupn�ch byt�
    return uzel stromu reprezentuj�c� dek�dovan� byte nebo NULL, pokud ve
    vstupn�m souboru nen� dal�� zak�dovan� byte
*/
TreeNode* decode_point(Tree* t, Buffer* b, FILE* f, uint64_t* input_cnt){
    //za��n�m prohled�vat strom od ko�ene
    TreeNode* node=t->root;
    char bit=-1;
    int64_t byte=-1;
    
    //dokud nenaraz�m na list
    while(node->point<0){
        //na�tu jeden bit
        bit=get_bit(b);
        //pokud v bufferu nen� jeden bit
        if(bit==-1){
            //na�tu byte ze souboru do bufferu
            byte=getc(f);
            //pokud je soubor pr�zdn� kon��m
            if(byte==-1){
                return NULL;
            }
            *input_cnt+=1;
            add_byte(b, byte);
            bit=get_bit(b);
        }
        
        //pokud se na�etla 0 p�ejdeme k lev�mu uzlu jinak k prav�mu
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
 *   outputFile - vystupn� soubor (nekodovany)
 * Navratova hodnota: 
 *    0 - dekodovani probehlo v poradku
 *    -1 - p�i dekodovani nastala chyba
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
	//pokud je soubor pr�zdn�
    if(byte==-1){
        ahed->uncodedSize=0;
	    ahed->codedSize=0;
        free_tree(tree);
        free_buffer(b);
        return AHEDOK;
    }
    //dek�dov�n� prvn�ho bytu
    else{
	    add_byte(b, byte);
	    input_cnt++;
	    output_cnt+=write_bytes(b, outputFile);
	    node=create_node(tree, byte);
	    increase_number(tree, node);
    }
	
	//dek�dov�n� ostatn�ch byt�
	//pokud je mo�n� dek�dovat dal�� byte opakuji
	while((node=decode_point(tree, b, inputFile, &input_cnt))!=NULL){
	    //byl dek�dov�n NULL_POINT
	    if(node->point==NULL_POINT){
	        //na�tu dal�� byte
	        character=get_byte(b);
            //pokud nen� v bufferu na�tu ho ze souboru
            if(character==-1){
                byte=getc(inputFile);
                //pokud v souboru nen� kon��m
                if(byte==-1){
                    break;
                }
                input_cnt+=1;
                add_byte(b, byte);
                character=get_byte(b);
            }
            //vytvo�en� nov�ho uzlu stromu
            node=create_node(tree, character);
            increase_number(tree, node);
            output_cnt++;
            putc(character, outputFile);
	    }
	    //byl dek�dov�n byte
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

