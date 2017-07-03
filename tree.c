/*
 * Autor: Ondřej Šlampa (xslamp01)
 * Datum: 14.02.2015
 * Soubor: tree.c
 * Komentář: Práce s huffmanovým stromem.
*/


#include<stdlib.h>
#include<stdio.h>
#include"tree.h"

/*
    Alokuje nový huffmanův strom, který obsahuje dva uzly: kořen a NULL_POINT.
    return nový huffmanův strom nebo NULL, pokud došlo k chybě
*/
Tree* new_tree(){
    //alokace stromu
    Tree* tree=malloc(sizeof(Tree));
    if(tree==NULL){
        return NULL;
    }
    
    //alokace uzlů
    tree->nodes=malloc(sizeof(TreeNode)*NUMBER_OF_NODES);
    if(tree->nodes==NULL){
        free(tree);
        return NULL;
    }
    
    tree->root=&(tree->nodes[0]);
    
    //inicializace kořene
    tree->root->parent=NULL;
    tree->root->left=&(tree->nodes[1]);
    tree->root->right=NULL;
    tree->root->point=-1;
    tree->root->number=0;
    
    //inicializace uzlu NULL_POINT
    tree->nodes[1].parent=tree->root;
    tree->nodes[1].left=NULL;
    tree->nodes[1].right=NULL;
    tree->nodes[1].point=NULL_POINT;
    tree->nodes[1].number=0;
    
    tree->number_of_nodes=2;
    tree->next_id=-2;
    
    return tree;
}

/*
    Uvolní strom z paměti.
    t strom na uvolnění
*/
void free_tree(Tree* t){
    if(t!=NULL){
        free(t->nodes);
        free(t);
    }
}

/*
    Vytvoří nový uzel ve stromu t s hodnotou 0.
    t strom
    point hodnota bytu v novém uzlu
    return nový uzel
*/
TreeNode* create_node(Tree* tree, int64_t point){
    //pokud jsou ve stromě pouze dva uzly - nový uzel bude pravý potomek kořene
    if(tree->number_of_nodes==2){
        tree->nodes[2].parent=tree->root;
        tree->nodes[2].left=NULL;
        tree->nodes[2].right=NULL;
        tree->nodes[2].point=point;
        tree->nodes[2].number=0;
        tree->root->number++;
        tree->root->right=&(tree->nodes[2]);
        tree->number_of_nodes++;
    }
    //jinak nový uzel bude vytvořen vpravo od uzlu NULL_POINT
    //vytvoří se nový vnitřní uzel, který nahradí uzel NULL_POINT
    //potomky tohoto uzlu jsou uzel NULL_POINT a nový list s hodnotou point
    else{
        TreeNode* null_node=get_node(tree, NULL_POINT);
        
        //inicializace nového vnitřního uzlu
        tree->nodes[tree->number_of_nodes].parent=null_node->parent;
        tree->nodes[tree->number_of_nodes].left=null_node;
        tree->nodes[tree->number_of_nodes].right=&(tree->nodes[tree->number_of_nodes+1]);
        tree->nodes[tree->number_of_nodes].point=tree->next_id;
        tree->nodes[tree->number_of_nodes].number=0;
        
        //inicializace nového listu s hodnotou point
        tree->nodes[tree->number_of_nodes+1].parent=&(tree->nodes[tree->number_of_nodes]);
        tree->nodes[tree->number_of_nodes+1].left=NULL;
        tree->nodes[tree->number_of_nodes+1].right=NULL;
        tree->nodes[tree->number_of_nodes+1].point=point;
        tree->nodes[tree->number_of_nodes+1].number=0;
        
        //vložení uzlů do stromu
        if(null_node->parent->left==null_node){
            null_node->parent->left=&(tree->nodes[tree->number_of_nodes]);
        }
        else{
            null_node->parent->right=&(tree->nodes[tree->number_of_nodes]);
        }
        
        null_node->parent=&(tree->nodes[tree->number_of_nodes]);
        
        tree->next_id--;
        tree->number_of_nodes+=2;
    }
    return &(tree->nodes[tree->number_of_nodes-1]);
}

/*
    Vypočítá novou hodnotu uzlu.
    node uzel
    return nová hodnota uzlu node
*/
uint64_t node_renumber(TreeNode* node){
    if(node->left!=NULL && node->right!=NULL){
        node->number=node_renumber(node->left)+node_renumber(node->left);
    }
    return node->number;
}

/*
    Vypočítá nové hodnoty všech uzlů ve stromu tree.
    tree strom
*/
void tree_renumber(Tree* tree){
    node_renumber(tree->root);
}

/*
    Vrátí uzel označený jako point.
    tree strom
    point označení uzlu
    return uzel označený jako point nebo NULL, pokud není uzel nalezen
*/
TreeNode* get_node(Tree* tree, int64_t point){
    for(uint64_t i=0;i<tree->number_of_nodes;i++){
        if(tree->nodes[i].point==point){
            return &(tree->nodes[i]);
        }
    }
    return NULL;
}

/*
    Vrátí nejlevějšího potomka uzlu node na urovni level pod tímto uzlem.
    Pokud takový uzel není nalezen vrátí NULL.
    Pokud je level roven 0 vrátí node.
    node uzel
    level úroveň na které se má nacházet uzel
    return nejlevější potomek uzlu node na urovni level nebo NULL pokud takový
    uzel není nalezen
*/
TreeNode* get_leftest(TreeNode* node, uint64_t level){
    //printf("get_leftest(%d)\n",node->point);
    if(level==0){
        return node;
    }
    //list nemá potomky
    if(node->left==NULL && node->right==NULL){
        return NULL;
    }
    //prohledám potomky vnitřního uzlu
    else{
        TreeNode* value=get_leftest(node->left, level-1);
        if(value==NULL){
            value=get_leftest(node->right, level-1);
        }
        return value;
    }
}

/*
    Najde následující uzel ve stromě tree.
    tree strom
    node uzel
    return další uzel ve stromě tree na stejné úrovni jako node, nebo nejlevější
    uzel na nižší úrovni
*/
TreeNode* next_node(Tree* tree, TreeNode* node){
    TreeNode* child=node;
    TreeNode* parent=child->parent;
    TreeNode* next=NULL;
    uint64_t level=0;
    
    //hledám následující uzel na stejné úrovni jako node
    while(parent!=NULL && next==NULL){
        //pokud je současný uzel levým potomkem rodiče prohledávám pravý podstrom rodiče
        if(child==parent->left){
            next=get_leftest(parent->right, level);
        }
        //pokud následující uzel nebyl nalezen prohledávám posunu se o úroveň
        //blíže ke kořenu
        if(next==NULL){
            child=parent;
            parent=parent->parent;
            level++;
        }
    }
    
    //pokud neexistuje následující uzel na stejné úrovni vrátí nejlevější
    //na nižší úrovni
    if(next==NULL){
        next=get_leftest(tree->root, level-1);
    }
    
    return next;
}

/*
    Provede výměnu dvou uzlů ve stromu.
    a první uzel
    b druhý uzel
*/
void swap_nodes(TreeNode* a, TreeNode* b){
    TreeNode* a_parent=a->parent;
    TreeNode* b_parent=b->parent;
    
    //výměna ukazatelů na potomky
    if(a==a_parent->left){
        if(b==b_parent->left){
            a_parent->left=b;
            b_parent->left=a;
        }
        else{
            a_parent->left=b;
            b_parent->right=a;
        }
    }
    else{
        if(b==b_parent->left){
            a_parent->right=b;
            b_parent->left=a;
        }
        else{
            a_parent->right=b;
            b_parent->right=a;
        }
    }
    
    //výměna ukazatelů na rodiče
    a->parent=b_parent;
    b->parent=a_parent;
}

/*
    Zvýší hodnotu v uzlu node a upraví strom tree odpovídajícím způsobem.
    tree strom obsahující uzel node
    node uzel
*/
void increase_number(Tree* tree, TreeNode* node){
    TreeNode* next=NULL;
    //postupně zvyšuji hodnoty uzlů od node po kořen
    do{
        next=next_node(tree, node);
        //pokud má následující uzel hodnotu menší nebo rovnu hodnotě node
        //prohodím je
        while(next->number<=node->number && node->parent!=next){
            swap_nodes(node, next);
            next=next_node(tree, node);
        }
        
        node->number++;
        node=node->parent;
    }
    while(node!=tree->root);
    
    tree->root->number++;
    
    //pokud bylo dosaženo maximální hodnoty v kořenu vydělím hodnoty v listech 2
    //a dopočítám hodnoty vnitřních uzlů
    if(tree->root->number==MAXIMUM_NUMBER){
        for(uint64_t i=0; i<(tree->number_of_nodes); i++){
            tree->nodes[i].number/=2;
        }
        tree_renumber(tree);
    }
    
}

