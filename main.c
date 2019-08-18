#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define CHAR_MIN_VAL 45

#define HT_PRIME_A 37
#define HT_PRIME_B 151
#define HT_REL_SIZE 67
#define HT_ENT_SIZE 257
#define HT_ENT_RELS_SIZE 257
#define HT_REL_ENTS_SRCS_SIZE 37

#define COMMAND_ADDENT "addent"
#define COMMAND_DELENT "delent"
#define COMMAND_ADDREL "addrel"
#define COMMAND_DELREL "delrel"
#define COMMAND_REPORT "report"
#define COMMAND_END "end"

/*      DATA STRUCTURES      */
typedef enum { false, true } bool;

typedef struct ht_ent_el_s ht_ent_el_t;
typedef struct ht_rel_el_s ht_rel_el_t;
typedef struct ht_ent_rels_el_s ht_ent_rels_el_t;
typedef struct ht_ent_s ht_ent_t;
typedef struct ht_rel_s ht_rel_t;
typedef struct ht_ent_rels_s ht_ent_rels_t;
typedef struct ht_rel_ents_srcs_s ht_rel_ents_srcs_t;

typedef struct  rbt_rel_ents_el_s  rbt_rel_ents_el_t;
typedef struct rbt_rel_ents_s rbt_rel_ents_t;
typedef struct  rbt_rel_el_s  rbt_rel_el_t;
typedef struct rbt_rel_s rbt_rel_t;

// Entities hash-table element
struct ht_ent_el_s {
    char *id;
    ht_ent_rels_t* rels_in;
    ht_ent_rels_t* rels_out;
};

// Relations hash-table element
struct ht_rel_el_s {
    char *id;
    rbt_rel_ents_t* ents;
};

// Relations hash-table element
struct ht_ent_rels_el_s {
    char *id;
    rbt_rel_ents_el_t* rbt_node;
};

// entities hash-table
struct ht_ent_s {
    int size;
    int count;
    ht_ent_el_t **table;
};

// entities hash-table
struct ht_rel_s {
    int size;
    int count;
    ht_rel_el_t **table;
};

// entities hash-table
struct ht_ent_rels_s {
    int size;
    int count;
    ht_ent_rels_el_t** table;
};

struct ht_rel_ents_srcs_s {
    int size;
    int count;
    ht_ent_el_t** table;
};

// RB TREE
typedef enum {RED, BLACK, NIL} color_t;

// Entity trees element.
struct rbt_rel_ents_el_s {
    // Pointer to entities-ht element
    ht_ent_el_t* ent;
    // Structure pointers
    struct rbt_rel_ents_el_s* parent;
    struct rbt_rel_ents_el_s* left;
    struct rbt_rel_ents_el_s* right;
    color_t color;
    int count;
    // Relations sources sub-table
    ht_rel_ents_srcs_t* sources;
};

// Relations rb tree. Contains in each node a pointer to the entity target of a relation instance.
struct rbt_rel_ents_s {
    rbt_rel_ents_el_t* root;
    rbt_rel_ents_el_t* nil;
};

// Relations tree element.
struct rbt_rel_el_s {
    // Relation identifier
    char* id;
    // Structure pointers
    struct rbt_rel_el_s* parent;
    struct rbt_rel_el_s* left;
    struct rbt_rel_el_s* right;
    color_t color;
    // Relations sources sub-table
    rbt_rel_ents_t* ents_tree;
};

// Relations rb tree. Contains in each node an ents rb tree.
struct rbt_rel_s {
    rbt_rel_el_t* root;
    rbt_rel_el_t* nil;
};


/*      PROTOTYPES      */
// Commands
void addent(char *id_ent);

void delent(char *id_ent);

void addrel(char *id_orig, char *id_dest, char *id_rel);

void delrel(char *id_orig, char *id_dest, char *id_rel);

void report();

// Data structure procedures
// HASH TREES
int ht_hash();
int ht_hash_ptr();

void ht_rel_init(ht_rel_t* ht, int size);
void ht_rel_insert(ht_rel_t* ht, ht_rel_el_t *el);
void ht_rel_delete(ht_rel_t* ht, char *id);
ht_rel_el_t *ht_rel_search(ht_rel_t* ht, char *id);

void ht_ent_init(ht_ent_t* ht, int size);
void ht_ent_insert(ht_ent_t* ht, ht_ent_el_t *el);
void ht_ent_delete(ht_ent_t* ht, char *id);
ht_ent_el_t *ht_ent_search(ht_ent_t* ht, char *id);

void ht_rel_ents_srcs_init(ht_rel_ents_srcs_t* ht, int size);
void ht_rel_ents_srcs_insert(ht_rel_ents_srcs_t* ht, ht_ent_el_t *el);
void ht_rel_ents_srcs_delete(ht_rel_ents_srcs_t* ht, ht_ent_el_t *id);
ht_ent_el_t *ht_rel_ents_srcs_search(ht_rel_ents_srcs_t* ht, ht_ent_el_t *id);

void ht_free();

// RED BLACK TREES

void rbt_rel_ents_init(rbt_rel_ents_t* rbt);
void rbt_rel_ents_insert(rbt_rel_ents_t* rbt, rbt_rel_ents_el_t* el);
void rbt_rel_ents_delete(rbt_rel_ents_t* rbt, rbt_rel_ents_el_t* el);
rbt_rel_ents_el_t* rbt_rel_ents_search(rbt_rel_ents_t* rbt, ht_ent_el_t* ent);

/*      GLOBALS     */
static ht_rel_el_t HT_REL_DELETED_ITEM = {NULL};
static ht_ent_el_t HT_ENT_DELETED_ITEM = {NULL};
static ht_ent_rels_el_t HT_ENT_RELS_DELETED_ITEM = {NULL};
static ht_ent_el_t HT_REL_ENTS_SRCS_DELETED_ITEM = {NULL};

static rbt_rel_ents_el_t RBT_REL_ENTS_NIL = {NULL, NULL, NULL, NULL, (color_t) NULL, 0, NULL};
static rbt_rel_el_t RBT_REL_NIL = {NULL, NULL, NULL, NULL, (color_t) NULL, NULL};

rbt_rel_t rbt_rel;
ht_ent_t ht_ent;

/*      IMPLEMENTATION      */
// Double hashing - hash function
int ht_hash(const char *val, const int size, const int attempt) {
    long hash_a = 0, hash_b = 0;
    int val_len = (int) strlen(val);

    for (int i = 0; i < val_len; i++) {
        hash_a = hash_a + (long) HT_PRIME_A * (val_len - (i + 1)) * (val[i] - CHAR_MIN_VAL);
        hash_a = hash_a % size;
    }

    for (int i = 0; i < val_len; i++) {
        hash_b = hash_b + (long) HT_PRIME_B * (val_len - (i + 1)) * (val[i] - CHAR_MIN_VAL);
        hash_b = hash_b % size;
    }

    return ((int) hash_a + (attempt * ((int) hash_b + 1))) % size;
}

int ht_hash_ptr(int val, const int size, const int attempt) {
    long hash_a = 0, hash_b = 0;

    hash_a = hash_a + (long) HT_PRIME_A * val;
    hash_a = hash_a % size;

    hash_b = hash_b + (long) HT_PRIME_B * val;
    hash_b = hash_b % size;

    return ((int) hash_a + (attempt * ((int) hash_b + 1))) % size;
}

// Initialize relations hashmap
void ht_rel_init(ht_rel_t* ht, int size) {
    ht->size = size;
    ht->count = 0;
    ht->table = (ht_rel_el_t**) calloc((size_t) size, sizeof(ht_rel_el_t*));
}

// Insert new relation
void ht_rel_insert(ht_rel_t* ht, ht_rel_el_t* el) {
    int index = ht_hash(el->id, ht->size, 0);
    ht_rel_el_t* cur_item = ht->table[index];
    int i = 1;
    while (cur_item != NULL) {
        index = ht_hash(el->id, ht->size, i);
        cur_item = ht->table[index];
        i++;
    }
    ht->table[index] = el;
    ht->count++;
}

// Delete single element
void ht_rel_el_delete(ht_rel_el_t *el) {
    free(el->id);
    free(el);
}

// Set element to del. Doesn't break chaining.
void ht_rel_delete(ht_rel_t* ht, char *id) {
    int index = ht_hash(id, ht->size, 0);
    ht_rel_el_t* item = ht->table[index];
    int i = 1;
    while (item != NULL) {
        if (item != &HT_REL_DELETED_ITEM) {
            if (strcmp(item->id, id) == 0) {
                ht_rel_el_delete(item);
                ht->table[index] = &HT_REL_DELETED_ITEM;
            }
        }
        index = ht_hash(id, ht->size, i);
        item = ht->table[index];
        i++;
    }
    ht->count--;
}

ht_rel_el_t *ht_rel_search(ht_rel_t* ht, char *id) {
    int index = ht_hash(id, ht->size, 0);
    ht_rel_el_t* item = ht->table[index];
    int i = 1;
    while (item != NULL) {
        if (strcmp(item->id, id) == 0) {
            return item;
        }
        index = ht_hash(id, ht->size, i);
        item = ht->table[index];
        i++;
    }
    return NULL;
}

void ht_rel_free(ht_rel_t* ht) {
    for (int i = 0; i < ht->size; ++i) {
        if (ht->table[i] != NULL && ht->table[i]->id != NULL) {
            ht_rel_el_delete(ht->table[i]);
        }
    }

    free(ht->table);
}

// Initialize relations hashmap
void ht_ent_init(ht_ent_t* ht, int size) {
    ht->size = size;
    ht->count = 0;
    ht->table = (ht_ent_el_t **) calloc((size_t) size, sizeof(ht_ent_el_t *));
}

// Insert new relation
void ht_ent_insert(ht_ent_t* ht, ht_ent_el_t *el) {
    int index = ht_hash(el->id, ht->size, 0);
    ht_ent_el_t* cur_item = ht->table[index];
    int i = 1;
    while (cur_item != NULL) {
        index = ht_hash(el->id, ht->size, i);
        cur_item = ht->table[index];
        i++;
    }
    ht->table[index] = el;
    ht->count++;
}

void ht_ent_el_delete(ht_ent_el_t *el) {
    free(el->id);
    free(el);
}

void ht_ent_delete(ht_ent_t* ht, char *id) {
    int index = ht_hash(id, ht->size, 0);
    ht_ent_el_t* item = ht->table[index];
    int i = 1;
    while (item != NULL) {
        if (item != &HT_ENT_DELETED_ITEM) {
            if (strcmp(item->id, id) == 0) {
                ht_ent_el_delete(item);
                ht->table[index] = &HT_ENT_DELETED_ITEM;
            }
        }
        index = ht_hash(id, ht->size, i);
        item = ht->table[index];
        i++;
    }
    ht->count--;
}

ht_ent_el_t *ht_ent_search(ht_ent_t* ht, char *id) {
    int index = ht_hash(id, ht->size, 0);
    ht_ent_el_t* item = ht->table[index];
    int i = 1;
    while (item != NULL) {
        if (strcmp(item->id, id) == 0) {
            return item;
        }
        index = ht_hash(id, ht->size, i);
        item = ht->table[index];
        i++;
    }
    return NULL;
}

void ht_ent_free(ht_ent_t* ht) {
    for (int i = 0; i < ht->size; ++i) {
        if (ht->table[i] != NULL && ht->table[i]->id != NULL) {
            ht_ent_el_delete(ht->table[i]);
        }
    }

    free(ht->table);
}

// SUB ENTITY HT POINTING TO RELATIONS RBTs
// Initialize relations hashmap
void ht_ent_rels_init(ht_ent_rels_t* ht, int size) {
    ht->size = size;
    ht->count = 0;
    ht->table = (ht_ent_rels_el_t**) calloc((size_t) size, sizeof(ht_ent_rels_el_t*));
}

// Insert new relation
void ht_ent_rels_insert(ht_ent_rels_t* ht, ht_ent_rels_el_t *el) {
    int index = ht_hash(el->id, ht->size, 0);
    ht_ent_rels_el_t* cur_item = ht->table[index];
    int i = 1;
    while (cur_item != NULL) {
        index = ht_hash(el->id, ht->size, i);
        cur_item = ht->table[index];
        i++;
    }
    ht->table[index] = el;
    ht->count++;
}

void ht_ent_rels_el_delete(ht_ent_rels_el_t* el) {
    free(el->id);
    free(el);
}

void ht_ent_rels_delete(ht_ent_rels_t* ht, char *id) {
    int index = ht_hash(id, ht->size, 0);
    ht_ent_rels_el_t* item = ht->table[index];
    int i = 1;
    while (item != NULL) {
        if (item != &HT_ENT_RELS_DELETED_ITEM) {
            if (strcmp(item->id, id) == 0) {
                ht_ent_rels_el_delete(item);
                ht->table[index] = &HT_ENT_RELS_DELETED_ITEM;
            }
        }
        index = ht_hash(id, ht->size, i);
        item = ht->table[index];
        i++;
    }
    ht->count--;
}

ht_ent_rels_el_t* ht_ent_rels_search(ht_ent_rels_t* ht, char *id) {
    int index = ht_hash(id, ht->size, 0);
    ht_ent_rels_el_t* item = ht->table[index];
    int i = 1;
    while (item != NULL) {
        if (strcmp(item->id, id) == 0) {
            return item;
        }
        index = ht_hash(id, ht->size, i);
        item = ht->table[index];
        i++;
    }
    return NULL;
}

void ht_ent_rels_free(ht_ent_rels_t* ht) {
    for (int i = 0; i < ht->size; ++i) {
        if (ht->table[i] != NULL && ht->table[i]->id != NULL) {
            ht_ent_rels_el_delete(ht->table[i]);
        }
    }

    free(ht->table);
}

// SUB ENTITY HT POINTING BACK TO ENTITIES HT
// Initialize relations hashmap
void ht_rel_ents_srcs_init(ht_rel_ents_srcs_t* ht, int size) {
    ht->size = size;
    ht->count = 0;
    ht->table = (ht_ent_el_t**) calloc((size_t) size, sizeof(ht_ent_el_t*));
}

// Insert new relation
void ht_rel_ents_srcs_insert(ht_rel_ents_srcs_t* ht, ht_ent_el_t *el) {
    int index = ht_hash_ptr((int) el, ht->size, 0);
    ht_ent_el_t* cur_item = ht->table[index];
    int i = 1;
    while (cur_item != NULL) {
        index = ht_hash_ptr((int) el, ht->size, i);
        cur_item = ht->table[index];
        i++;
    }
    ht->table[index] = el;
    ht->count++;
}

void ht_rel_ents_srcs_delete(ht_rel_ents_srcs_t* ht, ht_ent_el_t* id) {
    int index = ht_hash_ptr((int) id, ht->size, 0);
    ht_ent_el_t* item = ht->table[index];
    int i = 1;
    while (item != NULL) {
        if (item != &HT_REL_ENTS_SRCS_DELETED_ITEM) {
            if (item == id) {
                ht->table[index] = &HT_REL_ENTS_SRCS_DELETED_ITEM;
            }
        }
        index = ht_hash_ptr((int) id, ht->size, i);
        item = ht->table[index];
        i++;
    }
    ht->count--;
}

ht_ent_el_t *ht_rel_ents_srcs_search(ht_rel_ents_srcs_t* ht, ht_ent_el_t *id) {
    int index = ht_hash_ptr((int) id, ht->size, 0);
    ht_ent_el_t* item = ht->table[index];
    int i = 1;
    while (item != NULL) {
        if (item == id) {
            return item;
        }
        index = ht_hash_ptr((int) id, ht->size, i);
        item = ht->table[index];
        i++;
    }
    return NULL;
}

void ht_free() {
    ht_ent_free(&ht_ent);
}

bool str_a_came_before_than_b(char* str_a, char* str_b) {
    int str_a_len = (int) strlen(str_a);
    int str_b_len = (int) strlen(str_b);

    if (str_a_len < str_b_len) {
        for (int i = 0; i < str_a_len; ++i) {
            if (str_a[i] > str_b[i]) return false;
        }

        // Mario came before Marione
        return true;
    } else {
        for (int i = 0; i < str_b_len; ++i) {
            if (str_a[i] > str_b[i]) return false;
        }
        // Marione came after Mario
        return false;
    }
}

void rbt_rel_ents_init(rbt_rel_ents_t* rbt) {
    rbt->nil = &RBT_REL_ENTS_NIL;
    rbt->root = rbt->nil;
}

void rbt_rel_ents_right_rotate(rbt_rel_ents_t* rbt, rbt_rel_ents_el_t* el) {
    rbt_rel_ents_el_t* node = el->left;
    el->left = node->right;

    if (node->right != rbt->nil) {
        node->right->parent = el;
    }
    node->parent = el->parent;
    if (el->parent == rbt->nil) {
        rbt->root = node;
    } else if (el == el->parent->right) {
        el->parent->right = node;
    } else {
        el->parent->left = node;
    }
    node->right = el;
    el->parent = node;
}

void rbt_rel_ents_left_rotate(rbt_rel_ents_t* rbt, rbt_rel_ents_el_t* el) {
    rbt_rel_ents_el_t* node = el->right;
    el->right = node->left;

    if (node->left != rbt->nil) {
        node->left->parent = el;
    }
    node->parent = el->parent;
    if (el->parent == rbt->nil) {
        rbt->root = node;
    } else if (el == el->parent->left) {
        el->parent->left = node;
    } else {
        el->parent->right = node;
    }
    node->left = el;
    el->parent = node;
}

void rbt_rel_ents_insert_fixup(rbt_rel_ents_t* rbt, rbt_rel_ents_el_t* el) {
    if (el == rbt->root) {
        rbt->root->color = BLACK;
    } else {
        rbt_rel_ents_el_t* node_a = el->parent;
        if (node_a->color == RED) {
            if (node_a == node_a->parent->left) {
                rbt_rel_ents_el_t* node_b = node_a->parent->right;
                if (node_b->color == RED) {
                    node_a->color = BLACK;
                    node_b->color = BLACK;
                    node_a->parent->color = RED;
                    rbt_rel_ents_insert_fixup(rbt, node_a->parent);
                } else if (el == node_a->right) {
                    el = node_a;
                    rbt_rel_ents_left_rotate(rbt, el);
                    node_a = el->parent;
                }
                node_a->color = BLACK;
                node_a->parent->color = RED;
                rbt_rel_ents_right_rotate(rbt, node_a->parent);
            } else {
                rbt_rel_ents_el_t* node_b = node_a->parent->left;
                if (node_b->color == RED) {
                    node_a->color = BLACK;
                    node_b->color = BLACK;
                    node_a->parent->color = RED;
                    rbt_rel_ents_insert_fixup(rbt, node_a->parent);
                } else if (el == node_a->left) {
                    el = node_a;
                    rbt_rel_ents_right_rotate(rbt, el);
                    node_a = el->parent;
                }
                node_a->color = BLACK;
                node_a->parent->color = RED;
                rbt_rel_ents_left_rotate(rbt, node_a->parent);
            }
        }
    }
}

void rbt_rel_ents_insert(rbt_rel_ents_t* rbt, rbt_rel_ents_el_t* el) {
    rbt_rel_ents_el_t* parent = rbt->nil;
    rbt_rel_ents_el_t* node = rbt->root;

    while (node != rbt->nil) {
        parent = node;
        if (el->count < node->count) {
            node = node->left;
        } else {
            node = node->right;
        }
    }
    el->parent = parent;
    if (parent == rbt->nil) {
        rbt->root = el;
    } else if (el->count < parent->count) {
        parent->left = el;
    } else {
        parent->right = el;
    }
    el->left = rbt->nil;
    el->right = rbt->nil;
    el->color = RED;

    rbt_rel_ents_insert_fixup(rbt, el);
}

rbt_rel_ents_el_t* rbt_rel_ents_minimum(rbt_rel_ents_t* rbt, rbt_rel_ents_el_t* el) {
    rbt_rel_ents_el_t* node = el;
    while (node->left != rbt->nil) {
        node = node->left;
    }

    return node;
}

rbt_rel_ents_el_t* rbt_rel_ents_maximum(rbt_rel_ents_t* rbt, rbt_rel_ents_el_t* el) {
    rbt_rel_ents_el_t* node = el;
    while (node->right != rbt->nil) {
        node = node->right;
    }

    return node;
}

rbt_rel_ents_el_t* rbt_rel_ents_successor(rbt_rel_ents_t* rbt, rbt_rel_ents_el_t* el) {
    rbt_rel_ents_el_t* el_backup = el;
    rbt_rel_ents_el_t* node;
    if (el->right != rbt->nil) {
        return rbt_rel_ents_minimum(rbt, el->right);
    }
    node = el->parent;
    while (node != rbt->nil && el == node->right) {
        if (node == rbt->root) {
            // el is the max element, it has no successors
            return el_backup;
        }
        el = node;
        node = node->parent;
    }

    return node;
}

rbt_rel_ents_el_t* rbt_rel_ents_predecessor(rbt_rel_ents_t* rbt, rbt_rel_ents_el_t* el) {
    rbt_rel_ents_el_t* el_backup = el;
    rbt_rel_ents_el_t* node;
    if (el->left != rbt->nil) {
        return rbt_rel_ents_maximum(rbt, el->left);
    }
    node = el->parent;
    while (node != rbt->nil && el == node->left) {
        if (node == rbt->root) {
            // el is the min element, it has no predecessors
            return el_backup;
        }
        el = node;
        node = node->parent;
    }
    return node;
}

void rbt_rel_ents_delete_fixup(rbt_rel_ents_t* rbt, rbt_rel_ents_el_t* el) {
    rbt_rel_ents_el_t* node;
    // case 1
    if (el->color == RED || el->parent == rbt->nil) {
        el->color = BLACK;
    } else if (el == el->parent->left) {
        node = el->parent->right;
        if (node->color == RED) {
            node->color = BLACK;
            el->parent->color = RED;
            rbt_rel_ents_left_rotate(rbt, el->parent);
            node = el->parent->right;
        }
        // case 2
        if (node->left->color == BLACK && node->right->color == BLACK) {
            node->color = RED;
            rbt_rel_ents_delete_fixup(rbt, el->parent);
        } else if (node->right->color == BLACK) {
            // case 3
            node->left->color = BLACK;
            node->color = RED;
            rbt_rel_ents_right_rotate(rbt, node);
            node = el->parent->right;
        } else {
            node->color = el->parent->color;
            el->parent->color = BLACK;
            node->right->color = BLACK;
            rbt_rel_ents_left_rotate(rbt, el->parent);
        }
    } else {
        node = el->parent->left;
        if (node->color == RED) {
            node->color = BLACK;
            el->parent->color = RED;
            rbt_rel_ents_right_rotate(rbt, el->parent);
            node = el->parent->left;
        }
        // case 2
        if (node->left->color == BLACK && node->right->color == BLACK) {
            node->color = RED;
            rbt_rel_ents_delete_fixup(rbt, el->parent);
        } else if (node->left->color == BLACK) {
            // case 3
            node->right->color = BLACK;
            node->color = RED;
            rbt_rel_ents_left_rotate(rbt, node);
            node = el->parent->left;
        } else {
            node->color = el->parent->color;
            el->parent->color = BLACK;
            node->left->color = BLACK;
            rbt_rel_ents_right_rotate(rbt, el->parent);
        }
    }
}

void rbt_rel_ents_delete(rbt_rel_ents_t* rbt, rbt_rel_ents_el_t* el) {
    rbt_rel_ents_el_t* node_a, *node_b;
    if (el->left == rbt->nil || el->right == rbt->nil) {
        node_a = el;
    } else {
        node_a = rbt_rel_ents_successor(rbt, el);
    }

    if (node_a->left != rbt->nil) {
        node_b = node_a->left;
    } else {
        node_b = node_a->right;
    }

    node_b->parent = node_a->parent;

    if (node_a->parent == rbt->nil) {
        rbt->root = node_b;
    } else if (node_a == node_a->parent->left) {
        node_a->parent->left = node_b;
    } else {
        node_a->parent->right = node_b;
    }

    if (node_a != el) {
        el->count = node_a->count;
        el->ent = node_a->ent;
        el->sources = node_a->sources;
    }
    if (node_a->color == BLACK) {
        rbt_rel_ents_delete_fixup(rbt, node_b);
    }
}

rbt_rel_ents_el_t* rbt_rel_ents_search(rbt_rel_ents_t* rbt, ht_ent_el_t* ent) {

}

void rbt_rel_init(rbt_rel_t* rbt) {
    rbt->nil = &RBT_REL_NIL;
    rbt->root = rbt->nil;
}

void rbt_rel_right_rotate(rbt_rel_t* rbt, rbt_rel_el_t* el) {
    rbt_rel_el_t* node = el->left;
    el->left = node->right;

    if (node->right != rbt->nil) {
        node->right->parent = el;
    }
    node->parent = el->parent;
    if (el->parent == rbt->nil) {
        rbt->root = node;
    } else if (el == el->parent->right) {
        el->parent->right = node;
    } else {
        el->parent->left = node;
    }
    node->right = el;
    el->parent = node;
}

void rbt_rel_left_rotate(rbt_rel_t* rbt, rbt_rel_el_t* el) {
    rbt_rel_el_t* node = el->right;
    el->right = node->left;

    if (node->left != rbt->nil) {
        node->left->parent = el;
    }
    node->parent = el->parent;
    if (el->parent == rbt->nil) {
        rbt->root = node;
    } else if (el == el->parent->left) {
        el->parent->left = node;
    } else {
        el->parent->right = node;
    }
    node->left = el;
    el->parent = node;
}

void rbt_rel_insert_fixup(rbt_rel_t* rbt, rbt_rel_el_t* el) {
    if (el == rbt->root) {
        rbt->root->color = BLACK;
    } else {
        rbt_rel_el_t* node_a = el->parent;
        if (node_a->color == RED) {
            if (node_a == node_a->parent->left) {
                rbt_rel_el_t* node_b = node_a->parent->right;
                if (node_b->color == RED) {
                    node_a->color = BLACK;
                    node_b->color = BLACK;
                    node_a->parent->color = RED;
                    rbt_rel_insert_fixup(rbt, node_a->parent);
                } else if (el == node_a->right) {
                    el = node_a;
                    rbt_rel_left_rotate(rbt, el);
                    node_a = el->parent;
                }
                node_a->color = BLACK;
                node_a->parent->color = RED;
                rbt_rel_right_rotate(rbt, node_a->parent);
            } else {
                rbt_rel_el_t* node_b = node_a->parent->left;
                if (node_b->color == RED) {
                    node_a->color = BLACK;
                    node_b->color = BLACK;
                    node_a->parent->color = RED;
                    rbt_rel_insert_fixup(rbt, node_a->parent);
                } else if (el == node_a->left) {
                    el = node_a;
                    rbt_rel_right_rotate(rbt, el);
                    node_a = el->parent;
                }
                node_a->color = BLACK;
                node_a->parent->color = RED;
                rbt_rel_left_rotate(rbt, node_a->parent);
            }
        }
    }
}

void rbt_rel_insert(rbt_rel_t* rbt, rbt_rel_el_t* el) {
    rbt_rel_el_t* parent = rbt->nil;
    rbt_rel_el_t* node = rbt->root;

    while (node != rbt->nil) {
        parent = node;
        if (str_a_came_before_than_b(el->id, node->id)) {
            node = node->left;
        } else {
            node = node->right;
        }
    }
    el->parent = parent;
    if (parent == rbt->nil) {
        rbt->root = el;
    } else if (str_a_came_before_than_b(el->id, parent->id)) {
        parent->left = el;
    } else {
        parent->right = el;
    }
    el->left = rbt->nil;
    el->right = rbt->nil;
    el->color = RED;

    rbt_rel_insert_fixup(rbt, el);
}

rbt_rel_el_t* rbt_rel_minimum(rbt_rel_t* rbt, rbt_rel_el_t* el) {
    rbt_rel_el_t* node = el;
    while (node->left != rbt->nil) {
        node = node->left;
    }

    return node;
}

rbt_rel_el_t* rbt_rel_maximum(rbt_rel_t* rbt, rbt_rel_el_t* el) {
    rbt_rel_el_t* node = el;
    while (node->right != rbt->nil) {
        node = node->right;
    }

    return node;
}

rbt_rel_el_t* rbt_rel_successor(rbt_rel_t* rbt, rbt_rel_el_t* el) {
    rbt_rel_el_t* el_backup = el;
    rbt_rel_el_t* node;
    if (el->right != rbt->nil) {
        return rbt_rel_minimum(rbt, el->right);
    }
    node = el->parent;
    while (node != rbt->nil && el == node->right) {
        if (node == rbt->root) {
            // el is the max element, it has no successors
            return el_backup;
        }
        el = node;
        node = node->parent;
    }

    return node;
}

rbt_rel_el_t* rbt_rel_predecessor(rbt_rel_t* rbt, rbt_rel_el_t* el) {
    rbt_rel_el_t* el_backup = el;
    rbt_rel_el_t* node;
    if (el->left != rbt->nil) {
        return rbt_rel_maximum(rbt, el->left);
    }
    node = el->parent;
    while (node != rbt->nil && el == node->left) {
        if (node == rbt->root) {
            // el is the min element, it has no predecessors
            return el_backup;
        }
        el = node;
        node = node->parent;
    }
    return node;
}

void rbt_rel_delete_fixup(rbt_rel_t* rbt, rbt_rel_el_t* el) {
    rbt_rel_el_t* node;
    // case 1
    if (el->color == RED || el->parent == rbt->nil) {
        el->color = BLACK;
    } else if (el == el->parent->left) {
        node = el->parent->right;
        if (node->color == RED) {
            node->color = BLACK;
            el->parent->color = RED;
            rbt_rel_left_rotate(rbt, el->parent);
            node = el->parent->right;
        }
        // case 2
        if (node->left->color == BLACK && node->right->color == BLACK) {
            node->color = RED;
            rbt_rel_delete_fixup(rbt, el->parent);
        } else if (node->right->color == BLACK) {
            // case 3
            node->left->color = BLACK;
            node->color = RED;
            rbt_rel_right_rotate(rbt, node);
            node = el->parent->right;
        } else {
            node->color = el->parent->color;
            el->parent->color = BLACK;
            node->right->color = BLACK;
            rbt_rel_left_rotate(rbt, el->parent);
        }
    } else {
        node = el->parent->left;
        if (node->color == RED) {
            node->color = BLACK;
            el->parent->color = RED;
            rbt_rel_right_rotate(rbt, el->parent);
            node = el->parent->left;
        }
        // case 2
        if (node->left->color == BLACK && node->right->color == BLACK) {
            node->color = RED;
            rbt_rel_delete_fixup(rbt, el->parent);
        } else if (node->left->color == BLACK) {
            // case 3
            node->right->color = BLACK;
            node->color = RED;
            rbt_rel_left_rotate(rbt, node);
            node = el->parent->left;
        } else {
            node->color = el->parent->color;
            el->parent->color = BLACK;
            node->left->color = BLACK;
            rbt_rel_right_rotate(rbt, el->parent);
        }
    }
}

void rbt_rel_delete(rbt_rel_t* rbt, rbt_rel_el_t* el) {
    rbt_rel_el_t* node_a, *node_b;
    if (el->left == rbt->nil || el->right == rbt->nil) {
        node_a = el;
    } else {
        node_a = rbt_rel_successor(rbt, el);
    }

    if (node_a->left != rbt->nil) {
        node_b = node_a->left;
    } else {
        node_b = node_a->right;
    }

    node_b->parent = node_a->parent;

    if (node_a->parent == rbt->nil) {
        rbt->root = node_b;
    } else if (node_a == node_a->parent->left) {
        node_a->parent->left = node_b;
    } else {
        node_a->parent->right = node_b;
    }

    if (node_a != el) {
        el->id = node_a->id;
        el->ents_tree = node_a->ents_tree;
    }
    if (node_a->color == BLACK) {
        rbt_rel_delete_fixup(rbt, node_b);
    }
}

rbt_rel_el_t* rbt_rel_search(rbt_rel_t* rbt, rbt_rel_el_t* node, char* id) {
    if (node == rbt->nil || node->id == id) return node;

    if (str_a_came_before_than_b(id, node->id)) return rbt_rel_search(rbt, node->left, id);
    else return rbt_rel_search(rbt, node->right, id);
}

int main(int argc, char const *argv[]) {
    // INIT
    rbt_rel_init(&rbt_rel);
    ht_ent_init(&ht_ent, HT_ENT_SIZE);

    //TESTING FILE
    FILE *fp = freopen(
            "/mnt/c/Users/Andrea Finazzi/OneDrive - Politecnico di Milano/Documents/Università/Corsi/Anno II/API/progetto-2019/suite1/batch1.2.in",
            "r", stdin);

    char *line = NULL, *command = NULL, *params = NULL, *id_orig = NULL, *id_dest = NULL, *id_rel = NULL;

    // scan first line and parse it
    scanf("%m[^\n]\n", &line);
    sscanf(line, "%ms %m[^\n]\n", &command, &params);
    free(line);

    while (command[0] != COMMAND_END[0]) {
        switch (command[0]) {
            case 'a':
                if (command[3] == COMMAND_ADDENT[3]) {
                    // ADDENT
                    sscanf(params, "\"%ms", &id_orig);

                    if (ht_ent_search(&ht_ent, id_orig) == NULL) {
                        ht_ent_el_t *new_ent = (ht_ent_el_t*) malloc(sizeof(ht_ent_el_t));
                        new_ent->id = strdup(id_orig);
                        new_ent->rels_in = (ht_ent_rels_t*) malloc(sizeof(ht_ent_rels_t));
                        new_ent->rels_out = (ht_ent_rels_t*) malloc(sizeof(ht_ent_rels_t));
                        ht_ent_rels_init(new_ent->rels_in, HT_ENT_RELS_SIZE);
                        ht_ent_rels_init(new_ent->rels_out, HT_ENT_RELS_SIZE);
                        ht_ent_insert(&ht_ent, new_ent);
                    }

                    free(id_orig);
                } else {
                    // ADDREL
                    sscanf(params, "\"%ms \"%ms \"%ms", &id_orig, &id_dest, &id_rel);

                    ht_ent_el_t* ent_orig = ht_ent_search(&ht_ent, id_orig);
                    ht_ent_el_t* ent_dest = ht_ent_search(&ht_ent, id_dest);
                    // do both entities exist?
                    if (ent_orig != NULL && ent_dest != NULL) {
                        rbt_rel_el_t* rel = rbt_rel_search(&rbt_rel, rbt_rel.root, id_rel);
                        rbt_rel_ents_el_t* ent_node;
                        // first rel type insert?
                        if (rel == NULL || rel == rbt_rel.nil) {
                            rel = (rbt_rel_el_t*) malloc(sizeof(rbt_rel_el_t));
                            rel->id = strdup(id_rel);
                            rel->color = RED;
                            rel->ents_tree = (rbt_rel_ents_t*) malloc(sizeof(rbt_rel_ents_t));
                            rbt_rel_ents_init(rel->ents_tree);

                            // Create relation type node
                            rbt_rel_insert(&rbt_rel, rel);

                            // Create entity node in relation sub tree
                            ent_node = (rbt_rel_ents_el_t*) malloc(sizeof(rbt_rel_ents_el_t));
                            ent_node->parent = rel->ents_tree->nil;
                            ent_node->right = rel->ents_tree->nil;
                            ent_node->left = rel->ents_tree->nil;
                            ent_node->color = RED;
                            // Create sources hash table
                            // TODO should have a specific type?
                            ent_node->sources = (ht_rel_ents_srcs_t*) malloc(sizeof(ht_rel_ents_srcs_t));
                            ht_rel_ents_srcs_init(ent_node->sources, HT_REL_ENTS_SRCS_SIZE);

                        } else {
                            // can't search in binary tree because it's ordered on incoming rels count.
                            // check if it's the first rel instance for this entity
                            ht_ent_rels_el_t* ent_rels_in_el = ht_ent_rels_search(ent_dest->rels_in, rel->id);
                            ht_ent_rels_el_t* ent_rels_out_el = ht_ent_rels_search(ent_orig->rels_out, rel->id);
                            if (ent_rels_in_el == NULL) {
                                ent_rels_in_el = (ht_ent_rels_el_t*) malloc(sizeof(ht_ent_rels_el_t));
                                ent_rels_in_el->id = rel->id;

                                // Create entity node in relation sub tree
                                ent_node = (rbt_rel_ents_el_t*) malloc(sizeof(rbt_rel_ents_el_t));
                                ent_node->parent = rel->ents_tree->nil;
                                ent_node->right = rel->ents_tree->nil;
                                ent_node->left = rel->ents_tree->nil;
                                ent_node->color = RED;
                                // Create sources hash table
                                // TODO should have a specific type?
                                ent_node->sources = (ht_rel_ents_srcs_t*) malloc(sizeof(ht_rel_ents_srcs_t));
                                ht_rel_ents_srcs_init(ent_node->sources, HT_REL_ENTS_SRCS_SIZE);
                            } else {
                                ent_node = ent_rels_in_el->rbt_node;
                            }

                            ent_rels_in_el->rbt_node = ent_node;
                            // TODO save relation instance in ent_orig->rels_out ht
                            ht_ent_rels_insert(ent_orig->rels_in, ent_rels_in_el);
                        }

                        rbt_rel_ents_insert(rel->ents_tree, ent_node);
                    }

                    free(id_orig);
                    free(id_dest);
                    free(id_rel);
                }

                break;
            case 'd':
                if (command[3] == COMMAND_DELENT[3]) {
                    // DELENT
                    sscanf(params, "\"%ms\"", &id_orig);

                    ht_ent_delete(&ht_ent, id_orig);

                    free(id_orig);
                } else {
                    // DELREL
                    sscanf(params, "\"%ms\" \"%ms\" \"%ms\"", &id_orig, &id_dest, &id_rel);

                    free(id_orig);
                    free(id_dest);
                    free(id_rel);
                }

                break;
            case 'r':
                // REPORT
                break;
        }
        setbuf(stdout, 0);
        printf("%s\n", command);

        // free last line allocated memory
        free(command);
        free(params);

        // scan next line and parse it
        scanf("%m[^\n]\n", &line);
        sscanf(line, "%ms %m[^\n]\n", &command, &params);
        free(line);
    }

    // free last line allocated memory
    free(command);
    free(params);

    ht_free();

    return 0;
}