#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include < string.h > 
#include "../BinTree/btree.h"
#include <malloc.h>
typedef struct btreenode {
	struct  btreenode* parent;
	struct  btreenode* left;
	struct  btreenode* right;
	BTreeItem Btreeitem;
}btreenode;

typedef struct bintree {
	size_t keySize;
	size_t valueSize;
	size_t count;
	int(*compare)(const void*, const void *);
	btreenode *btree;
}binarytree;

void * btree_create(size_t keySize, size_t valueSize, int(*compare)(const void *, const void *)) {
	if (keySize <= 0|| valueSize <= 0|| compare == NULL) return NULL;;
	binarytree* new_tree = malloc(sizeof(binarytree));
	new_tree->compare = compare;
	new_tree->keySize = keySize;
	new_tree->valueSize = valueSize;
	new_tree->btree= NULL;
	new_tree->count = 0;
	return new_tree;
}

void btree_destroy(void * btree, void(*destroy)(void *)) {
	if (btree == NULL||destroy==NULL) return;
	btree_clear(btree,destroy);
	free(btree);
	btree = NULL;
}
void * btree_init(void * bitree, size_t keySize, size_t valueSize, int(*compare)(const void *, const void *)) {
	if (bitree == NULL|| keySize <= 0|| valueSize <= 0|| compare == NULL) return NULL;
	binarytree* btree = bitree;
	btree->compare = compare;
	btree->keySize = keySize;
	btree->valueSize = valueSize;
	btree->count = 0;
	return btree;
}
size_t btree_count(void * btree) {
	if (btree == NULL) return (size_t)-1;
	return ((binarytree*)btree)->count;
}

btreenode* search_node(btreenode* tree_node, const void* key, int(*compare)(const void *, const void *)) {
	if (tree_node== NULL ) return NULL; 
	int c = compare((&tree_node->Btreeitem)->key, key);
	if (c == 0) return tree_node;
	if (c > 0) return search_node(tree_node->left, key, compare);
	return search_node(tree_node->right, key, compare);
}

void* btree_item(void * btree, const void * key) {
	if (btree == NULL || key == NULL) return NULL;
	btreenode* node = ((binarytree*)btree)->btree;
	btreenode* f_node = search_node(node, key, ((binarytree*)btree)->compare);
	if (f_node == NULL) return NULL;
	return (&f_node->Btreeitem)->value;
}

btreenode* init_node(const void* key,size_t keySize,size_t valueSize) {
	btreenode* node = malloc(sizeof(btreenode));
    memset(node, 0, sizeof(btreenode));
    memset(&node->Btreeitem, 0, sizeof(BTreeItem));
    (&node->Btreeitem)->key = malloc(keySize);
	memcpy((void*)(&node->Btreeitem)->key, key, keySize);
	(&node->Btreeitem)->value = malloc(valueSize);
    memset((&node->Btreeitem)->value, 0, valueSize);
	node->left=NULL;
	node->right = NULL;
	return node;
}
btreenode* search_parent(btreenode* root,const void* key, int(*compare)(const void *, const void *)) {
	
	int c = compare((&root->Btreeitem)->key, key);
	if (c > 0) {
		if (root->left != NULL) root=search_parent(root->left, key, compare);
		return root;
	}
	if (c < 0) {
		if (root->right != NULL) root=search_parent(root->right, key, compare);
		return root;
	}
    return NULL;
}
void* node_create(binarytree* btree, const void* key) {
	btreenode* root=btree->btree;
	if (root == NULL) {
		((binarytree*)btree)->count = 1;
		root = init_node(key, btree->keySize, btree->valueSize);
		root->parent = NULL;
		btree->btree = root;
		return (&root->Btreeitem)->value;
	}
	btreenode* parent= search_parent(root, key, btree->compare);
	int c = btree->compare((&parent->Btreeitem)->key, key);
	if (c < 0) {
		parent->right = init_node(key, btree->keySize, btree->valueSize);
		parent->right->parent = parent;
		((binarytree*)btree)->count++;
		return (&parent->right->Btreeitem)->value;
	}
	if (c > 0) {
		parent->left = init_node(key, btree->keySize, btree->valueSize);
		parent->left ->parent = parent;
		((binarytree*)btree)->count++;
		return (&parent->left->Btreeitem)->value;
	}
	return NULL;
}
void * btree_insert(void * btree, const void * key, bool * createFlag) {
	if (btree == NULL || createFlag == NULL || key ==NULL) return NULL;
	void* item_data = btree_item(btree, key);
	if (item_data!= NULL) {
		*createFlag = false;
		return item_data;
	}
	*createFlag = true;
	return node_create(((binarytree*)btree),key);
	
}
void remove_node(btreenode* node, void(*destroy)(void *)) {
	destroy(&node->Btreeitem);
	free((void*)(&node->Btreeitem)->key);
	free((void*)(&node->Btreeitem)->value);
	free(node);
}
size_t btree_next(const void * btree, size_t item_id);
btreenode* leftest_node(btreenode* node);


void btree_remove(void * btree, const void * key, void(*destroy)(void *)) {
	if (btree == NULL || key==NULL || destroy==NULL || ((binarytree*)btree)->btree == NULL) return;
	binarytree* tree = (binarytree*)btree;
	btreenode* victim = search_node(tree->btree, key, tree->compare);
	if (victim == NULL) return;
	tree->count--;
	if (victim->right == NULL && victim->left == NULL) {
		if (victim->parent == NULL) tree->btree = NULL;
		else {
			if (tree->compare((&victim->parent->Btreeitem)->key,(&victim->Btreeitem)->key)>0) victim->parent->left = NULL;
			else victim->parent->right = NULL;
		}
		remove_node(victim, destroy);
		return;
	}
	if (victim->right == NULL && victim->left != NULL) {
		if (victim->parent) {

			if (tree->compare((&victim->parent->Btreeitem)->key, (&victim->Btreeitem)->key) > 0) victim->parent->left = victim->left;
			else victim->parent->right = victim->left;
			victim->left->parent = victim->parent;
			remove_node(victim, destroy);
		}
		else {
			tree->btree = victim->left;
			tree->btree->parent = NULL;
			remove_node(victim, destroy);
		}
		return;
	}
	if (victim->right != NULL && victim->left == NULL) {
		if (victim->parent) {
	
			if (tree->compare((&victim->parent->Btreeitem)->key, (&victim->Btreeitem)->key) > 0) victim->parent->left = victim->right;
			else victim->parent->right = victim->right;
			victim->right->parent = victim->parent;
			remove_node(victim, destroy);
		}
		else {
			tree->btree = victim->right;
			tree->btree->parent = NULL;
			remove_node(victim, destroy);
		}
		return;
	}
	if (victim->right != NULL && victim->left != NULL) {
		btreenode* target = leftest_node(victim->right);
		if (target->right == NULL) {
			if (victim->parent) {
				if (tree->compare((&victim->parent->Btreeitem)->key, (&victim->Btreeitem)->key) > 0) victim->parent->left = target;
				else victim->parent->right = target;
			}
			else tree->btree = target;
			if (target->parent != victim) {
				if (tree->compare((&target->parent->Btreeitem)->key, (&target->Btreeitem)->key) > 0) target->parent->left = NULL;
				else target->parent->right = NULL;
				target->right = victim->right;
				victim->right->parent = target;
			}
		}
		else {
			if (victim->parent) {
				if (tree->compare((&victim->parent->Btreeitem)->key, (&victim->Btreeitem)->key) > 0) victim->parent->left = target;
				else victim->parent->right = target;
			}
			else tree->btree = target;
			if (target->parent != victim) {
				if (tree->compare((&target->parent->Btreeitem)->key, (&target->Btreeitem)->key) > 0) target->parent->left = target->right;
				else target->parent->right = target->right;
				target->right->parent = target->parent;
				target->right = victim->right;
				victim->right->parent = target;
			}
		}
			target->parent = victim->parent;
			target->left = victim->left;
			victim->left->parent = target;
	
			remove_node(victim, destroy);
			return;
		
	}
}

size_t btree_first(const void * btree) {
	if (btree == NULL || ((binarytree*)btree)->btree == NULL) return btree_stop(btree);
	btreenode* current_node = ((binarytree*)btree)->btree;
    if (current_node == NULL) 
        return btree_stop(btree);
	while (current_node->left != NULL) {
		current_node = current_node->left;
	}
	return (size_t)current_node;
}
size_t btree_last(const void * btree) {
	if (btree == NULL || ((binarytree*)btree)->btree==NULL) return btree_stop(btree);
	btreenode* current_node = ((binarytree*)btree)->btree;
    if (current_node == NULL)
        return btree_stop(btree);
    while (current_node->right!=NULL) {
		current_node = current_node->right;
	}
	return (size_t)current_node;
}

btreenode* leftest_node(btreenode* node) {
	btreenode* current = node;
	while (current->left != NULL) {
		current = current->left;
	}
	return current;
}
btreenode* rightest_node(btreenode* node) {
	btreenode* current = node;
	while (current->right != NULL) {
		current = current->right;
	}
	return current;
}
btreenode* grand_parent(btreenode* node,int direction) { 
	btreenode* current = node;
	if (direction==0){
		while (current->parent != NULL && current->parent->parent != NULL && current->parent->parent->left != current->parent) {
			current = current->parent;
		}
		return current->parent->parent;
	}
	if (direction == 1) {
		while (current->parent != NULL && current->parent->parent != NULL && current->parent->parent->right != current->parent) {
			current = current->parent;
		}
		return current->parent->parent;
	}
	else return NULL;
}

size_t btree_next(const void * btree, size_t item_id) {
	if (btree == NULL || item_id == (size_t)-1 || item_id <= 0|| btree_count(btree) == 0) return btree_stop(btree);
	if (btree_last(btree) == item_id) return btree_stop(btree);
	btreenode* node = (btreenode*)item_id;
	if (node->parent != NULL) {
		if (node->parent->left == node && node->right == NULL) return (size_t)node->parent;
		if (node->parent->left == node && node->right != NULL) {
			btreenode* leftest = leftest_node(node->right);
			return (size_t)leftest;
		}
		if (node->parent->right == node) {
			if (node->right != NULL) return (size_t)leftest_node(node->right);
			return (size_t)grand_parent(node,0);
		}
	}
	return (size_t)leftest_node(node->right);
}

size_t btree_prev(const void * btree, size_t item_id) {
	if (btree == NULL || item_id==(size_t)-1 || item_id<=0|| btree_count(btree) == 0) return btree_stop(btree);
	btreenode* node = (btreenode*)item_id;
	if (btree_first(btree) == item_id) return btree_stop(btree);
	if (node->parent != NULL) {
		if (node->parent->right == node) {
			if (node->left == NULL) return (size_t)node->parent;
			return (size_t)rightest_node(node->left);
		}
		if (node->parent->left == node) {
			if (node->left != NULL) return (size_t)node->left;
			return (size_t)grand_parent(node, 1);
		}
	}
    return (size_t)rightest_node(node->left);
}

size_t btree_stop(const void * btree) {
    if (btree == NULL) return (size_t)-1;
    return (size_t)NULL;
}

void * btree_current(const void * btree, size_t item_id) {
	if (btree == NULL || item_id == (size_t)-1 || item_id <= 0|| item_id==(size_t)NULL|| btree_count(btree) == 0) return NULL;
	return &((btreenode*)item_id)->Btreeitem;
}

void btree_erase(void * btree, size_t item_id, void(*destroy)(void *)) {
	if (btree == NULL || item_id == (size_t)-1 ||destroy==NULL || item_id <= 0 || item_id == (size_t)NULL || btree_count(btree)==0) return;
	btree_remove(btree, (&((btreenode*)item_id)->Btreeitem)->key , destroy);
}
void deleting_items(void* btree, void(*destroy)(void*)) {
	size_t current = btree_first(btree);
	size_t next = 0;
	while (current != btree_stop(btree)) {
		next = btree_next(btree, current);
		btree_erase(btree, current, destroy);
		current = next;
	}
}
void btree_clear(void* btree, void(*destroy)(void *)) {
	if (btree == NULL || destroy == NULL) return;
	deleting_items(btree, destroy);
	((binarytree*)btree)->count = 0;
}