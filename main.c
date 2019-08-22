#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define CHAR_MIN_VAL 45

#define HT_PRIME_A 57
#define HT_PRIME_B 151
#define HT_MAX_LOAD_RATIO_100 40
#define HT_REL_SIZE 67
#define HT_ENT_SIZE 100103
#define HT_ENT_RELS_SIZE 9739
#define HT_REL_ENTS_SRCS_SIZE 4049
#define REPORT_ENTITIES_ARRAY_SIZE 100103

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
typedef struct rbt_rel_ents_el_key_s rbt_rel_ents_el_key_t;
typedef struct rbt_rel_ents_s rbt_rel_ents_t;
typedef struct  rbt_rel_el_s  rbt_rel_el_t;
typedef struct rbt_rel_s rbt_rel_t;

// Entities hash-table element
struct ht_ent_el_s {
    char *id;
    // incoming relations rbt nodes; indexed exclusively on rel id;
    ht_ent_rels_t* rels_in;
    // outgoing relations rbt nodes; indexed on id, chained on pointer.
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
    rbt_rel_ents_el_key_t* rbt_node_key;
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

struct rbt_rel_ents_el_key_s {
    // Pointer to parent data structure (actual rbt node)
    rbt_rel_ents_el_t* rbt_node;
    // Pointer to entities-ht element
    ht_ent_el_t* ent;
    int count;
    // Relations sources sub-table
    ht_rel_ents_srcs_t* sources;
};

// Entity trees element.
struct rbt_rel_ents_el_s {
    rbt_rel_ents_el_key_t *key;
    // Structure pointers
    struct rbt_rel_ents_el_s* parent;
    struct rbt_rel_ents_el_s* left;
    struct rbt_rel_ents_el_s* right;
    color_t color;
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
int ht_hash(const char *val, const int size, const int attempt);
int ht_hash_ptr(const void* val, const int size, const int attempt);

void ht_rel_init(ht_rel_t* ht, int size);
void ht_rel_insert(ht_rel_t* ht, ht_rel_el_t *el);
void ht_rel_delete(ht_rel_t* ht, char *id);
ht_rel_el_t *ht_rel_search(ht_rel_t* ht, char *id);

void ht_ent_init(ht_ent_t* ht, int size);
void ht_ent_insert(ht_ent_t* ht, ht_ent_el_t *el);
void ht_ent_delete(ht_ent_t* ht, char *id);
ht_ent_el_t *ht_ent_search(ht_ent_t* ht, char *id);
void ht_ent_free(ht_ent_t* ht);

void ht_ent_rels_insert(ht_ent_rels_t* ht, ht_ent_rels_el_t *el);
void ht_ent_rels_free(ht_ent_rels_t* ht);

void ht_rel_ents_srcs_init(ht_rel_ents_srcs_t* ht, int size);
void ht_rel_ents_srcs_insert(ht_rel_ents_srcs_t* ht, ht_ent_el_t *el);
void ht_rel_ents_srcs_delete(ht_rel_ents_srcs_t* ht, ht_ent_el_t *id);
ht_ent_el_t *ht_rel_ents_srcs_search(ht_rel_ents_srcs_t* ht, ht_ent_el_t *id);

void free_all();

// RED BLACK TREES

rbt_rel_el_t* rbt_rel_minimum(rbt_rel_t* rbt, rbt_rel_el_t* el);
rbt_rel_el_t* rbt_rel_maximum(rbt_rel_t* rbt, rbt_rel_el_t* el);
rbt_rel_el_t* rbt_rel_successor(rbt_rel_t* rbt, rbt_rel_el_t* el);

void rbt_rel_ents_init(rbt_rel_ents_t* rbt);
void rbt_rel_ents_el_free(rbt_rel_ents_el_t* el);
void rbt_rel_ents_insert(rbt_rel_ents_t* rbt, rbt_rel_ents_el_t* el);
rbt_rel_ents_el_t* rbt_rel_ents_delete(rbt_rel_ents_t* rbt, rbt_rel_ents_el_t* el);
rbt_rel_ents_el_t* rbt_rel_ents_search(rbt_rel_ents_t* rbt, ht_ent_el_t* ent);

void ht_rel_ents_srcs_free(ht_rel_ents_srcs_t *pS);

/*      GLOBALS     */
static ht_rel_el_t HT_REL_DELETED_ITEM = {NULL};
static ht_ent_el_t HT_ENT_DELETED_ITEM = {NULL};
static ht_ent_rels_el_t HT_ENT_RELS_DELETED_ITEM = {NULL};
static ht_ent_el_t HT_REL_ENTS_SRCS_DELETED_ITEM = {NULL};

static rbt_rel_ents_el_t RBT_REL_ENTS_NIL = {NULL, &RBT_REL_ENTS_NIL, &RBT_REL_ENTS_NIL, &RBT_REL_ENTS_NIL, (color_t) NULL};
static rbt_rel_el_t RBT_REL_NIL = {NULL, &RBT_REL_NIL, &RBT_REL_NIL, &RBT_REL_NIL, (color_t) NULL, NULL};

rbt_rel_t rbt_rel = {NULL, NULL};
ht_ent_t ht_ent = {0, 0, NULL};

/*      IMPLEMENTATION      */
/* MATH UTILS */

// 1 is prime; 0 is not prime; -1 is undefined (i.e. x < 2);
int is_prime(const int x) {
    if (x < 2) { return -1; }
    if (x < 4) { return 1; }
    if ((x % 2) == 0) { return 0; }
    for (int i = 3; i <= floor(sqrt((double) x)); i += 2) {
        if ((x % i) == 0) {
            return 0;
        }
    }
    return 1;
}

int next_prime(int x) {
    while (is_prime(x) != 1) {
        x++;
    }
    return x;
}

// Double hashing - hash function
int ht_hash(const char *val, const int size, const int attempt) {
    long hash_a = 0, hash_b = 0;
    int val_len = (int) strlen(val);

    for (int i = 0; i < val_len; i++) {
        hash_a = hash_a + (long) HT_PRIME_A * (val_len - (i + 1)) * (val[i] - CHAR_MIN_VAL);
//        hash_a = hash_a % size;
    }

    if (attempt > 0) {
        for (int i = 0; i < val_len; i++) {
            hash_b = hash_b + (long) HT_PRIME_B * (val_len - (i + 1)) * (val[i] - CHAR_MIN_VAL);
//        hash_b = hash_b % size;
        }
    }

    return ((int) hash_a + (attempt * ((int) hash_b + 1))) % size;
}

int ht_hash_ptr(const void* val, const int size, const int attempt) {
    long hash_a = 0, hash_b = 0;

    hash_a = hash_a + (long) HT_PRIME_A * (long) val;
    hash_a = hash_a % size;

    if (attempt > 0) {
        hash_b = hash_b + (long) HT_PRIME_B * (long) val;
        hash_b = hash_b % size;
    }
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
                break;
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
    ht->table = (ht_ent_el_t **) calloc((size_t) size, sizeof(ht_ent_el_t*));
}

// Hashtable resizing
void ht_ent_resize(ht_ent_t* ht, const int new_size) {
    ht_ent_t* tmp_ht = malloc(sizeof(ht_ent_t));
    ht_ent_init(tmp_ht, next_prime(new_size));

    // rehash elements
    for (int i = 0; i < tmp_ht->size; ++i) {
        ht_ent_el_t* item = ht->table[i];
        if (item != NULL && item != &HT_ENT_DELETED_ITEM) {
            ht_ent_insert(tmp_ht, item);
        }
    }
    tmp_ht->count = ht->count;
    // Switch between tmp_ht and ht
    const int tmp_size = ht->size;
    ht->size = tmp_ht->size;
    tmp_ht->size = tmp_size;

    ht_ent_el_t** tmp_table = ht->table;
    ht->table = tmp_ht->table;
    tmp_ht->table = tmp_table;

    ht_ent_free(tmp_ht);
}

// Insert new relation
void ht_ent_insert(ht_ent_t* ht, ht_ent_el_t *el) {
    const int load = ht->count * 100 / ht->size;
    if (load > HT_MAX_LOAD_RATIO_100) {
        ht_ent_resize(ht, ht->size * 2);
    }

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

void ht_ent_el_free(ht_ent_el_t *el) {
//    for (int i = 0; i < el->rels_in->size; ++i) {
//        if (el->rels_in->table[i] != NULL && el->rels_in->table[i] != &HT_ENT_RELS_DELETED_ITEM && el->rels_in->table[i]->id != NULL) {
//            rbt_rel_ents_el_free(el->rels_in->table[i]->rbt_node_key->rbt_node);
//            free(el->rels_in->table[i]);
//        }
//    }
//    for (int i = 0; i < el->rels_out->size; ++i) {
//        if (el->rels_out->table[i] != NULL && el->rels_out->table[i] != &HT_ENT_RELS_DELETED_ITEM && el->rels_out->table[i]->id != NULL) {
//            free(el->rels_out->table[i]);
//        }
//    }

    free(el->rels_in->table);
    free(el->rels_in);
    free(el->rels_out->table);
    free(el->rels_out);

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
                ht_ent_el_free(item);
                ht->table[index] = &HT_ENT_DELETED_ITEM;
                break;
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
        if ( item != &HT_ENT_DELETED_ITEM && strcmp(item->id, id) == 0) {
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
            ht_ent_el_free(ht->table[i]);
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

// Hashtable resizing
void ht_ent_rels_resize(ht_ent_rels_t* ht, const int new_size) {
    ht_ent_rels_t* tmp_ht = malloc(sizeof(ht_ent_rels_t));
    ht_ent_rels_init(tmp_ht, next_prime(new_size));

    // rehash elements
    for (int i = 0; i < tmp_ht->size; ++i) {
        ht_ent_rels_el_t* item = ht->table[i];
        if (item != NULL && item != &HT_ENT_RELS_DELETED_ITEM) {
            ht_ent_rels_insert(tmp_ht, item);
        }
    }
    tmp_ht->count = ht->count;
    // Switch between tmp_ht and ht
    const int tmp_size = ht->size;
    ht->size = tmp_ht->size;
    tmp_ht->size = tmp_size;

    ht_ent_rels_el_t** tmp_table = ht->table;
    ht->table = tmp_ht->table;
    tmp_ht->table = tmp_table;

    ht_ent_rels_free(tmp_ht);
}

// Insert new relation
void ht_ent_rels_insert(ht_ent_rels_t* ht, ht_ent_rels_el_t *el) {
    const int load = ht->count * 100 / ht->size;
    if (load > HT_MAX_LOAD_RATIO_100) {
        ht_ent_rels_resize(ht, ht->size * 2);
    }

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
    free(el);
}

void ht_ent_rels_delete(ht_ent_rels_t* ht, char *id, rbt_rel_ents_el_t* ent_node) {
    int index = ht_hash(id, ht->size, 0);
    ht_ent_rels_el_t* item = ht->table[index];
    int i = 1;
    if (ent_node != NULL) {
        while (item != NULL) {
            if (item != &HT_ENT_RELS_DELETED_ITEM) {
                if (item->rbt_node_key == ent_node->key) {
                    ht_ent_rels_el_delete(item);
                    ht->table[index] = &HT_ENT_RELS_DELETED_ITEM;
                    break;
                }
            }
            index = ht_hash(id, ht->size, i);
            item = ht->table[index];
            i++;
        }
    } else {
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
    }
    ht->count--;
}

ht_ent_rels_el_t* ht_ent_rels_search(ht_ent_rels_t* ht, char *id, rbt_rel_ents_el_key_t* ent_node_key) {
    int index = ht_hash(id, ht->size, 0);
    ht_ent_rels_el_t* item = ht->table[index];
    int i = 1;
    if (ent_node_key != NULL) {
        while (item != &HT_ENT_RELS_DELETED_ITEM && item != NULL) {
            if (item->rbt_node_key == ent_node_key) {
                return item;
            }
            index = ht_hash(id, ht->size, i);
            item = ht->table[index];
            i++;
        }
    }  else {
        while (item != NULL) {
            if (item != &HT_ENT_RELS_DELETED_ITEM && strcmp(item->id, id) == 0) {
                return item;
            }
            index = ht_hash(id, ht->size, i);
            item = ht->table[index];
            i++;
        }
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

// Hashtable resizing
void ht_rel_ents_srcs_resize(ht_rel_ents_srcs_t* ht, const int new_size) {
    ht_rel_ents_srcs_t* tmp_ht = malloc(sizeof(ht_rel_ents_srcs_t));
    ht_rel_ents_srcs_init(tmp_ht, next_prime(new_size));

    // rehash elements
    for (int i = 0; i < tmp_ht->size; ++i) {
        ht_ent_el_t* item = ht->table[i];
        if (item != NULL && item != &HT_ENT_DELETED_ITEM) {
            ht_rel_ents_srcs_insert(tmp_ht, item);
        }
    }
    tmp_ht->count = ht->count;
    // Switch between tmp_ht and ht
    const int tmp_size = ht->size;
    ht->size = tmp_ht->size;
    tmp_ht->size = tmp_size;

    ht_ent_el_t** tmp_table = ht->table;
    ht->table = tmp_ht->table;
    tmp_ht->table = tmp_table;

    ht_rel_ents_srcs_free(tmp_ht);
}

void ht_rel_ents_srcs_free(ht_rel_ents_srcs_t* ht) {
    free(ht->table);
    free(ht);
}

// Insert new relation
void ht_rel_ents_srcs_insert(ht_rel_ents_srcs_t* ht, ht_ent_el_t *el) {
    const int load = ht->count * 100 / ht->size;
    if (load > HT_MAX_LOAD_RATIO_100) {
        ht_rel_ents_srcs_resize(ht, ht->size * 2);
    }

    int index = ht_hash_ptr(el, ht->size, 0);
    ht_ent_el_t* cur_item = ht->table[index];
    int i = 1;
    while (cur_item != NULL) {
        index = ht_hash_ptr(el, ht->size, i);
        cur_item = ht->table[index];
        i++;
    }
    ht->table[index] = el;
    ht->count++;
}

void ht_rel_ents_srcs_delete(ht_rel_ents_srcs_t* ht, ht_ent_el_t* id) {
    int index = ht_hash_ptr(id, ht->size, 0);
    ht_ent_el_t* item = ht->table[index];
    int i = 1;
    while (item != NULL) {
        if (item != &HT_REL_ENTS_SRCS_DELETED_ITEM) {
            if (item == id) {
                ht->table[index] = &HT_REL_ENTS_SRCS_DELETED_ITEM;
                break;
            }
        }
        index = ht_hash_ptr(id, ht->size, i);
        item = ht->table[index];
        i++;
    }
    ht->count--;
}

ht_ent_el_t *ht_rel_ents_srcs_search(ht_rel_ents_srcs_t* ht, ht_ent_el_t *id) {
    int index = ht_hash_ptr(id, ht->size, 0);
    ht_ent_el_t* item = ht->table[index];
    int i = 1;
    while (item != NULL) {
        if (item == id) {
            return item;
        }
        index = ht_hash_ptr(id, ht->size, i);
        item = ht->table[index];
        i++;
    }
    return NULL;
}

void free_rbt_children(rbt_rel_t* rbt, rbt_rel_el_t* el) {
    if (el->left != rbt->nil) {
        free_rbt_children(rbt, el->left);
    }
    if (el->right != rbt->nil) {
        free_rbt_children(rbt, el->right);
    }
    free(el->id);
    free(el->ents_tree);
    free(el);
}

void free_all() {
    ht_ent_free(&ht_ent);

    // free rbt_rel
    free_rbt_children(&rbt_rel, rbt_rel.root);
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

int qsort_strcmp(const void* a, const void* b) {
    return strcmp(*(const char**) a, *(const char**) b);
}

void rbt_rel_ents_init(rbt_rel_ents_t* rbt) {
    rbt->nil = &RBT_REL_ENTS_NIL;
    rbt->root = rbt->nil;
}

// TODO check
void rbt_rel_ents_el_free(rbt_rel_ents_el_t* el) {
    free(el->key->sources->table);
    free(el->key->sources);
    free(el->key);
    free(el);
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
                } else {
                    if (el == node_a->right) {
                        el = node_a;
                        rbt_rel_ents_left_rotate(rbt, el);
                        node_a = el->parent;
                    }
                    node_a->color = BLACK;
                    node_a->parent->color = RED;
                    rbt_rel_ents_right_rotate(rbt, node_a->parent);
                }
            } else {
                rbt_rel_ents_el_t* node_b = node_a->parent->left;
                if (node_b->color == RED) {
                    node_a->color = BLACK;
                    node_b->color = BLACK;
                    node_a->parent->color = RED;
                    rbt_rel_ents_insert_fixup(rbt, node_a->parent);
                } else {
                    if (el == node_a->left) {
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
}

void rbt_rel_ents_insert(rbt_rel_ents_t* rbt, rbt_rel_ents_el_t* el) {
    rbt_rel_ents_el_t* parent = rbt->nil;
    rbt_rel_ents_el_t* node = rbt->root;

    while (node != rbt->nil) {
        parent = node;
        if (el->key->count < node->key->count) {
            node = node->left;
        } else {
            node = node->right;
        }
    }
    el->parent = parent;
    if (parent == rbt->nil) {
        rbt->root = el;
    } else if (el->key->count < parent->key->count) {
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
    rbt_rel_ents_el_t* node;
    if (el->left != rbt->nil) {
        return rbt_rel_ents_maximum(rbt, el->left);
    }
    node = el->parent;
    while (node != rbt->nil && el == node->left) {
        if (node == rbt->root) {
            // el is the min element, it has no predecessors
            return rbt->nil;
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
        } else {
            if (node->right->color == BLACK) {
                // case 3
                node->left->color = BLACK;
                node->color = RED;
                rbt_rel_ents_right_rotate(rbt, node);
                node = el->parent->right;
            }
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
        } else {
            if (node->left->color == BLACK) {
                // case 3
                node->right->color = BLACK;
                node->color = RED;
                rbt_rel_ents_left_rotate(rbt, node);
                node = el->parent->left;
            }
            node->color = el->parent->color;
            el->parent->color = BLACK;
            node->left->color = BLACK;
            rbt_rel_ents_right_rotate(rbt, el->parent);
        }
    }
}

rbt_rel_ents_el_t* rbt_rel_ents_delete(rbt_rel_ents_t* rbt, rbt_rel_ents_el_t* el) {
    rbt_rel_ents_el_t* node_a, *node_b;
    rbt_rel_ents_el_key_t* el_backup;
    el_backup = el->key;
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
        el->key = node_a->key;
        el->key->rbt_node = el;
        node_a->key = el_backup;
        node_a->key->rbt_node = node_a;
    }
    if (node_a->color == BLACK) {
        rbt_rel_ents_delete_fixup(rbt, node_b);
    }

    return node_a;
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
                } else {
                    if (el == node_a->right) {
                        el = node_a;
                        rbt_rel_left_rotate(rbt, el);
                        node_a = el->parent;
                    }
                    node_a->color = BLACK;
                    node_a->parent->color = RED;
                    rbt_rel_right_rotate(rbt, node_a->parent);
                }
            } else {
                rbt_rel_el_t* node_b = node_a->parent->left;
                if (node_b->color == RED) {
                    node_a->color = BLACK;
                    node_b->color = BLACK;
                    node_a->parent->color = RED;
                    rbt_rel_insert_fixup(rbt, node_a->parent);
                } else {
                    if (el == node_a->left) {
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
}

void rbt_rel_insert(rbt_rel_t* rbt, rbt_rel_el_t* el) {
    rbt_rel_el_t* parent = rbt->nil;
    rbt_rel_el_t* node = rbt->root;

    while (node != rbt->nil) {
        parent = node;
        if (strcmp(el->id, node->id) < 0) {
            node = node->left;
        } else {
            node = node->right;
        }
    }
    el->parent = parent;
    if (parent == rbt->nil) {
        rbt->root = el;
    } else if (strcmp(el->id, parent->id) < 0) {
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
    rbt_rel_el_t* node;
    if (el->right != rbt->nil) {
        return rbt_rel_minimum(rbt, el->right);
    }
    node = el->parent;
    while (node != rbt->nil && el == node->right) {
        if (node == rbt->root) {
            // el is the max element, it has no successors
            return rbt->nil;
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
        } else {
            if (node->right->color == BLACK) {
                // case 3
                node->left->color = BLACK;
                node->color = RED;
                rbt_rel_right_rotate(rbt, node);
                node = el->parent->right;
            }
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
        } else {
            if (node->left->color == BLACK) {
                // case 3
                node->right->color = BLACK;
                node->color = RED;
                rbt_rel_left_rotate(rbt, node);
                node = el->parent->left;
            }
            node->color = el->parent->color;
            el->parent->color = BLACK;
            node->left->color = BLACK;
            rbt_rel_right_rotate(rbt, el->parent);
        }
    }
}

// Unsafe, ents_tree MUST be freed before calling
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

    free(el->id);
    free(el->ents_tree);

    if (node_a != el) {
        el->id = node_a->id;
        el->ents_tree = node_a->ents_tree;
    }

    if (node_a->color == BLACK) {
        rbt_rel_delete_fixup(rbt, node_b);
    }

    free(node_a);
}

rbt_rel_el_t* rbt_rel_search(rbt_rel_t* rbt, rbt_rel_el_t* node, char* id) {
    if (node == rbt->nil || strcmp(node->id, id) == 0) return node;

    if (strcmp(id, node->id) < 0) return rbt_rel_search(rbt, node->left, id);
    else return rbt_rel_search(rbt, node->right, id);
}

int main(int argc, char const *argv[]) {
    // INIT
    rbt_rel_init(&rbt_rel);
    ht_ent_init(&ht_ent, HT_ENT_SIZE);

    //TESTING FILE
//    FILE *fp = freopen(
//            "/mnt/c/Users/Andrea Finazzi/OneDrive - Politecnico di Milano/Documents/Università/Corsi/Anno II/API/progetto-2019/suite1/batch1.1.in",
//            "r", stdin);

//    char *line = NULL, *command = NULL, *params = NULL, *id_orig = NULL, *id_dest = NULL, *id_rel = NULL;
    char** SRSC_entities = (char**) calloc((size_t) REPORT_ENTITIES_ARRAY_SIZE, sizeof(char*));
    int SRSC_size = REPORT_ENTITIES_ARRAY_SIZE;
    char line[200], command[7], params[150], id_orig[50], id_dest[50], id_rel[50];

    // scan first line and parse it
    scanf("%[^\n]\n", line);
    sscanf(line, "%s %[^\n]\n", command, params);

//    scanf("%m[^\n]\n", &line);
//    sscanf(line, "%ms %m[^\n]\n", &command, &params);
//    free(line);

    while (command[0] != COMMAND_END[0]) {
        switch (command[0]) {
            case 'a':
                if (command[3] == COMMAND_ADDENT[3]) {
                    // ADDENT
                    sscanf(params, "\"%[^\"]", id_orig);
//                    id_orig = params;

                    if (ht_ent_search(&ht_ent, id_orig) == NULL) {
                        ht_ent_el_t *new_ent = (ht_ent_el_t*) malloc(sizeof(ht_ent_el_t));
                        new_ent->id = strdup(id_orig);
                        new_ent->rels_in = (ht_ent_rels_t*) malloc(sizeof(ht_ent_rels_t));
                        new_ent->rels_out = (ht_ent_rels_t*) malloc(sizeof(ht_ent_rels_t));
                        ht_ent_rels_init(new_ent->rels_in, HT_ENT_RELS_SIZE);
                        ht_ent_rels_init(new_ent->rels_out, HT_ENT_RELS_SIZE);
                        ht_ent_insert(&ht_ent, new_ent);
                    }

                } else {
                    // ADDREL START
                    sscanf(params, "%*[\"]%[^\"]%*[ \"]%[^\"]%*[ \"]%[^\"]", id_orig, id_dest, id_rel);

                    ht_ent_el_t* ent_orig = ht_ent_search(&ht_ent, id_orig);
                    ht_ent_el_t* ent_dest = ht_ent_search(&ht_ent, id_dest);
                    // do both entities exist?
                    if (ent_orig != NULL && ent_dest != NULL) {
                        rbt_rel_el_t* rel = rbt_rel_search(&rbt_rel, rbt_rel.root, id_rel);
                        rbt_rel_ents_el_t* ent_node;
                        ht_ent_rels_el_t *ent_rels_in_el, *ent_rels_out_el;

                        // first rel type insert?
                        if (rel == NULL || rel == rbt_rel.nil) {
                            rel = (rbt_rel_el_t*) malloc(sizeof(rbt_rel_el_t));
                            rel->id = strdup(id_rel);
                            rel->color = RED;
                            rel->ents_tree = (rbt_rel_ents_t*) malloc(sizeof(rbt_rel_ents_t));
                            rbt_rel_ents_init(rel->ents_tree);

                            // Insert relation type node in rbt_rel
                            rbt_rel_insert(&rbt_rel, rel);
                        }

                        // can't search in binary tree because it's ordered on incoming rels count.
                        // So look for the pointback element in ent_dest->rels_in.
                        ent_rels_in_el = ht_ent_rels_search(ent_dest->rels_in, rel->id, NULL);
                        // check if it's the first incoming rel instance (of rel->id type) for ent_dest entity.
                        if (ent_rels_in_el == NULL) {
                            ent_rels_in_el = (ht_ent_rels_el_t*) malloc(sizeof(ht_ent_rels_el_t));
                            ent_rels_in_el->id = rel->id;
                            // Create entity node in relation sub tree
                            ent_node = (rbt_rel_ents_el_t*) malloc(sizeof(rbt_rel_ents_el_t));
                            ent_node->parent = rel->ents_tree->nil;
                            ent_node->right = rel->ents_tree->nil;
                            ent_node->left = rel->ents_tree->nil;
                            ent_node->color = RED;
                            ent_node->key = (rbt_rel_ents_el_key_t*) malloc(sizeof(rbt_rel_ents_el_key_t));
                            ent_node->key->ent = ent_dest;
                            ent_node->key->count = 1;
                            ent_node->key->rbt_node = ent_node;
                            // Create sources hash table
                            // TODO should have a specific type?
                            ent_node->key->sources = (ht_rel_ents_srcs_t*) malloc(sizeof(ht_rel_ents_srcs_t));
                            ht_rel_ents_srcs_init(ent_node->key->sources, HT_REL_ENTS_SRCS_SIZE);

                            ent_rels_in_el->rbt_node_key = ent_node->key;
                            ht_ent_rels_insert(ent_dest->rels_in, ent_rels_in_el);
                        } else {
                            ent_node = ent_rels_in_el->rbt_node_key->rbt_node;

                            // check if it's the first outgoing rel instance (of rel->id type) for ent_orig entity.
                            ent_rels_out_el = ht_ent_rels_search(ent_orig->rels_out, rel->id, ent_node->key);
                            // if ent_rels_out_el already exist, so does the relation.
                            if (ent_rels_out_el != NULL) break;

                            ent_node = rbt_rel_ents_delete(rel->ents_tree, ent_node);
                            ent_node->parent = rel->ents_tree->nil;
                            ent_node->right = rel->ents_tree->nil;
                            ent_node->left = rel->ents_tree->nil;
                            ent_node->color = RED;
                            ent_node->key->count++;
                            ent_node->key->rbt_node = ent_node;
                        }
                        // HERE (ent_rels_out_el == NULL && ent_node != NULL) is TRUE

                        ent_rels_out_el = (ht_ent_rels_el_t*) malloc(sizeof(ht_ent_rels_el_t));
                        ent_rels_out_el->id = rel->id;
                        ent_rels_out_el->rbt_node_key = ent_node->key;

                        ht_ent_rels_insert(ent_orig->rels_out, ent_rels_out_el);

                        // Add ent_orig pointer in sources ht
                        ht_rel_ents_srcs_insert(ent_node->key->sources, ent_orig);
                        rbt_rel_ents_insert(rel->ents_tree, ent_node);
                    }
                    // ADDREL END
                }
                break;
            case 'd':
                if (command[3] == COMMAND_DELENT[3]) {
                    // DELENT
                    sscanf(params, "\"%[^\"]", id_orig);
                    ht_ent_el_t* ent_orig = ht_ent_search(&ht_ent, id_orig);
                    rbt_rel_ents_el_t* rel_ents_el_node;
                    if (ent_orig != NULL) {
                        // for each rels_in
                        for (int i = 0; i < ent_orig->rels_in->size; ++i) {
                            if (ent_orig->rels_in->table[i] != NULL && ent_orig->rels_in->table[i] != &HT_ENT_RELS_DELETED_ITEM) {
                                rbt_rel_el_t* rel = rbt_rel_search(&rbt_rel, rbt_rel.root, ent_orig->rels_in->table[i]->id);
                                rel_ents_el_node = ent_orig->rels_in->table[i]->rbt_node_key->rbt_node;
                                // remove ent_orig from src->rels_out ht
                                for (int j = 0; j < rel_ents_el_node->key->sources->size; ++j) {
                                    if (rel_ents_el_node->key->sources->table[j] != NULL && rel_ents_el_node->key->sources->table[j] != &HT_REL_ENTS_SRCS_DELETED_ITEM) {
                                        ht_ent_rels_delete(rel_ents_el_node->key->sources->table[j]->rels_out, rel->id, rel_ents_el_node);
                                    }
                                }
                                // Remove rbt node from rbt tree
                                rel_ents_el_node = rbt_rel_ents_delete(rel->ents_tree, rel_ents_el_node);
                                rbt_rel_ents_el_free(rel_ents_el_node);
                                // Check if rel has or not other entities
                                if (rel->ents_tree->root == rel->ents_tree->nil) {
                                    rbt_rel_delete(&rbt_rel, rel);
                                }
                            }
                        }

                        // for each rels_out
                        for (int i = 0; i < ent_orig->rels_out->size; ++i) {
                            if (ent_orig->rels_out->table[i] != NULL && ent_orig->rels_out->table[i] != &HT_ENT_RELS_DELETED_ITEM) {
                                rbt_rel_el_t* rel = rbt_rel_search(&rbt_rel, rbt_rel.root, ent_orig->rels_out->table[i]->id);

                                rel_ents_el_node = ent_orig->rels_out->table[i]->rbt_node_key->rbt_node;
                                rel_ents_el_node = rbt_rel_ents_delete(rel->ents_tree, rel_ents_el_node);
                                rel_ents_el_node->key->count--;
                                // Remove pointer rel_ents_el_node (or equivalents)
                                ht_rel_ents_srcs_delete(rel_ents_el_node->key->sources, ent_orig);
                                ht_ent_rels_delete(ent_orig->rels_out, rel->id, rel_ents_el_node);
                                if (rel_ents_el_node->key->count < 1) {
                                    ht_ent_rels_delete(rel_ents_el_node->key->ent->rels_in, rel->id, NULL);
                                    rbt_rel_ents_el_free(rel_ents_el_node);
                                    // Check if rel has others entities
                                    if (rel->ents_tree->root == rel->ents_tree->nil) {
                                        rbt_rel_delete(&rbt_rel, rel);
                                    }
                                } else {
                                    rel_ents_el_node->parent = rel->ents_tree->nil;
                                    rel_ents_el_node->right = rel->ents_tree->nil;
                                    rel_ents_el_node->left = rel->ents_tree->nil;
                                    rel_ents_el_node->color = RED;
                                    rel_ents_el_node->key->rbt_node = rel_ents_el_node;

                                    rbt_rel_ents_insert(rel->ents_tree, rel_ents_el_node);
                                }
                            }
                        }

                        ht_ent_delete(&ht_ent, ent_orig->id);
                    }

//                    free(id_orig);
                } else {
                    // DELREL
                    sscanf(params, "%*[\"]%[^\"]%*[ \"]%[^\"]%*[ \"]%[^\"]", id_orig, id_dest, id_rel);

                    ht_ent_el_t* ent_orig = ht_ent_search(&ht_ent, id_orig);
                    ht_ent_el_t* ent_dest = ht_ent_search(&ht_ent, id_dest);

                    if (ent_orig != NULL && ent_dest) {
                        rbt_rel_el_t* rel = rbt_rel_search(&rbt_rel, rbt_rel.root, id_rel);
                        if (rel != rbt_rel.nil) {
                            rbt_rel_ents_el_t* ent_node;
                            ht_ent_rels_el_t *ent_rels_in_el, *ent_rels_out_el;

                            // can't search in binary tree because it's ordered on incoming rels count.
                            // So look for the pointback element in ent_dest->rels_in.
                            ent_rels_in_el = ht_ent_rels_search(ent_dest->rels_in, rel->id, NULL);
                            if (ent_rels_in_el != NULL) {
                                ent_node = ent_rels_in_el->rbt_node_key->rbt_node;
                                // check if it's the first outgoing rel instance (of rel->id type) for ent_orig entity.
                                ent_rels_out_el = ht_ent_rels_search(ent_orig->rels_out, rel->id, ent_node->key);
                                // if ent_rels_out_el already exist, so does the relation.
                                if (ent_rels_out_el != NULL) {
                                    // Remove rbt node from rbt tree
                                    ent_node = rbt_rel_ents_delete(rel->ents_tree, ent_node);
                                    ent_node->key->count--;
                                    // Remove pointer ent_node (or equivalents)
                                    ht_rel_ents_srcs_delete(ent_node->key->sources, ent_orig);
                                    // Remove outgoing rel from ent_orig->rels_out
                                    ht_ent_rels_delete(ent_orig->rels_out, rel->id, ent_node);
                                    if (ent_node->key->count < 1) {
                                        // Remove incoming rel from ent_dest->rels_in
                                        ht_ent_rels_delete(ent_dest->rels_in, rel->id, NULL);
                                        rbt_rel_ents_el_free(ent_node);
                                        // Check if rel has others entities
                                        if (rel->ents_tree->root == rel->ents_tree->nil) {
                                            rbt_rel_delete(&rbt_rel, rel);
                                        }
                                    } else {
                                        ent_node->parent = rel->ents_tree->nil;
                                        ent_node->right = rel->ents_tree->nil;
                                        ent_node->left = rel->ents_tree->nil;
                                        ent_node->color = RED;
                                        ent_node->key->rbt_node = ent_node;

                                        rbt_rel_ents_insert(rel->ents_tree, ent_node);
                                    }
                                }
                            }
                        }
                    }
                }

                break;
            case 'r':
                // REPORT
                if (rbt_rel.root != rbt_rel.nil) {
                    // Same-Rel-Same-Count entities array and counter
                    int entities_array_size = REPORT_ENTITIES_ARRAY_SIZE;
                    int i = 0;
                    // First rel in tree
                    rbt_rel_el_t* rel = rbt_rel_minimum(&rbt_rel, rbt_rel.root);
                    // Iterate over relations from min to max
                    while (true) {
                        fputs("\"", stdout);
                        fputs(rel->id, stdout);
                        fputs("\" ", stdout);
                        // Iterate over SRSC entities
                        rbt_rel_ents_el_t* ent = rbt_rel_ents_maximum(rel->ents_tree, rel->ents_tree->root);
                        int max_count = ent->key->count;
                        // TODO needs optimization
                        while (ent != rel->ents_tree->nil && ent->key->count == max_count) {
                            SRSC_entities[i++] = ent->key->ent->id;
                            // Check if SRSC_entities is full
                            if (i >= SRSC_size) {
                                SRSC_entities = (char**) realloc(SRSC_entities, SRSC_size * 2);
                                SRSC_size = SRSC_size * 2;
                            }

                            // NEXT ENT
                            ent = rbt_rel_ents_predecessor(rel->ents_tree, ent);
                        }
                        // Order the entities' array and print out
                        qsort(SRSC_entities, i, sizeof(char*), qsort_strcmp);
                        for (int j = 0; j < i; ++j) {
                            fputs("\"", stdout);
                            fputs(SRSC_entities[j], stdout);
                            fputs("\" ", stdout);
                            SRSC_entities[j] = NULL;
                        }
                        // NEXT REL
                        i = 0;
                        rel = rbt_rel_successor(&rbt_rel, rel);
                        if (rel == rbt_rel.nil) {
                            printf("%d", max_count);
                            fputs(";", stdout);
                            break;
                        } else {
                            printf("%d", max_count);
                            fputs("; ", stdout);
                        }
                    }
                    fputs("\n", stdout);
                } else {
                    fputs("none\n", stdout);
                }
                break;
        }

        // scan next line and parse it
        scanf("%[^\n]\n", line);
        sscanf(line, "%s %[^\n]\n", command, params);
//        free(line);
    }

    free(SRSC_entities);
    free_all();

    return 0;
}