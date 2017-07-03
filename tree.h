/*
 * Autor: Ondřej Šlampa (xslamp01)
 * Datum: 14.02.2015
 * Soubor: tree.h
 * Komentář: Práce s huffmanovým stromem.
*/

#include<stdint.h>
#include<limits.h>

#ifndef __KKO_TREE_H__
#define __KKO_TREE_H__

#define NUMBER_OF_NODES 514
#define MAXIMUM_NUMBER UINT64_MAX
#define NULL_POINT 256

/*
    Typ uzel stromu.
*/
typedef struct treenode TreeNode;

/*
    Uzel stromu.
*/
struct treenode{
    //znak představovaný tímto listem
    //nebo identifikace vnitřního uzlu (používá se pouze při ladění)
    int64_t point;
    //počet výskytů znaku point
    uint64_t number;
    //rodič tohoto uzlu
    TreeNode* parent;
    //levý potomek
    TreeNode* left;
    //pravý potomek
    TreeNode* right;
};

/*
    Strom
*/
typedef struct{
    //seznam všech uzlů
    TreeNode* nodes;
    //kořen
    TreeNode* root;
    //počet uzlů
    uint64_t number_of_nodes;
    //nejnižší ident
    int64_t next_id;
} Tree;

/*
    Alokuje nový huffmanův strom, který obsahuje dva uzly: kořen a NULL_POINT.
    return nový huffmanův strom
*/
Tree* new_tree();
/*
    Uvolní strom z paměti.
    t strom na uvolnění
*/
void free_tree(Tree* t);
/*
    Zvýší hodnotu v uzlu node a upraví strom tree odpovídajícím způsobem.
    tree strom obsahující uzel node
    node uzel
*/
void increase_number(Tree* tree, TreeNode* node);
/*
    Vrátí uzel označený jako point.
    tree strom
    point označení uzlu
    return uzel označený jako point nebo NULL, pokud není uzel nalezen
*/

TreeNode* get_node(Tree* tree, int64_t point);
/*
    Vytvoří nový uzel ve stromu t s hodnotou 0.
    t strom
    point hodnota bytu v novém uzlu
    return nový uzel
*/
TreeNode* create_node(Tree* tree, int64_t point);

#endif

