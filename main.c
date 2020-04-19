#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

#define CHAR_MIN_VAL 45

#define HT_PRIME_A 54367
#define HT_PRIME_B 90947

#define HT_MAX_LOAD_RATIO_100 40

#define HT_ENT_SIZE 64679
#define HT_ENT_RELS_SIZE 11
#define HT_REL_ENTS_SRCS_SIZE 19

#define HT_ENT_RELS_BUFFER_SIZE (HT_ENT_RELS_SIZE * 1024)

/*      DATA STRUCTURES      */
typedef enum {
    false,
    true
} bool;

typedef struct ht_ent_el_s ht_ent_el_t;
typedef struct ht_rel_el_s ht_rel_el_t;
typedef struct ht_ent_rels_el_s ht_ent_rels_el_t;
typedef struct ht_ent_s ht_ent_t;
typedef struct ht_rel_s ht_rel_t;
typedef struct ht_ent_rels_s ht_ent_rels_t;
typedef struct ht_rel_ents_srcs_s ht_rel_ents_srcs_t;

typedef struct rbt_rel_ents_el_s rbt_rel_ents_el_t;
typedef struct rbt_rel_ents_el_key_s rbt_rel_ents_el_key_t;
typedef struct rbt_rel_ents_s rbt_rel_ents_t;
typedef struct rel_mc_ents_el_s rel_mc_ents_el_t;
typedef struct rbt_rel_el_s rbt_rel_el_t;
typedef struct rbt_rel_s rbt_rel_t;


// Relations hash-table element
struct ht_rel_el_s {
    char *id;
    rbt_rel_ents_t *ents;
};

// Relations hash-table element
struct ht_ent_rels_el_s {
    char *id;
    rbt_rel_ents_el_key_t *rbt_node_key;
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
    ht_ent_rels_el_t **table;
};

// Entities hash-table element
struct ht_ent_el_s {
    char *id;
    // incoming relations rbt nodes; indexed exclusively on rel id;
    ht_ent_rels_t rels_in;
    // outgoing relations rbt nodes; indexed on id, chained on pointer.
    ht_ent_rels_t rels_out;
};

struct ht_rel_ents_srcs_s {
    int size;
    int count;
    ht_ent_el_t **table;
};

// RB TREE
typedef enum {
    RED,
    BLACK,
    NIL
} color_t;

struct rbt_rel_ents_el_key_s {
    // Pointer to parent data structure (actual rbt node)
    rbt_rel_ents_el_t *rbt_node;
    // Pointer to entities-ht element
    ht_ent_el_t *ent;
    int count;
    // Relations sources sub-table
    ht_rel_ents_srcs_t sources;
};

// Entity trees element.
struct rbt_rel_ents_el_s {
    rbt_rel_ents_el_key_t *key;
    // Structure pointers
    struct rbt_rel_ents_el_s *parent;
    struct rbt_rel_ents_el_s *left;
    struct rbt_rel_ents_el_s *right;
    color_t color;
};

// Relations rb tree. Contains in each node a pointer to the entity target of a relation instance.
struct rbt_rel_ents_s {
    rbt_rel_ents_el_t *root;
    rbt_rel_ents_el_t *nil;
};

struct rel_mc_ents_el_s {
    ht_ent_el_t *ent;
    struct rel_mc_ents_el_s *prev;
    struct rel_mc_ents_el_s *next;
};

// Relations tree element.
struct rbt_rel_el_s {
    // Relation identifier
    char *id;
    // Structure pointers
    struct rbt_rel_el_s *parent;
    struct rbt_rel_el_s *left;
    struct rbt_rel_el_s *right;
    color_t color;
    // Relations dests sub-tree
    rbt_rel_ents_t ents_tree;
    // MC_entities datas
    struct rel_mc_ents_el_s *mc_ents;
    int max_count;
};

// Relations rb tree. Contains in each node an ents rb tree.
struct rbt_rel_s {
    rbt_rel_el_t *root;
    rbt_rel_el_t *nil;
};

/*      PROTOTYPES      */

// Data structure procedures
// HASH TREES
inline static const int ht_hash(const char *val, const int size, const int attempt);

inline static const unsigned int ht_hash_ptr(const void *val, const unsigned int size, const unsigned int attempt);

inline static void ht_ent_init(ht_ent_t *ht, int size);

inline static void ht_ent_insert(ht_ent_t *ht, ht_ent_el_t *el);

inline static void ht_ent_delete(ht_ent_t *ht, char *id);

inline static ht_ent_el_t *ht_ent_search(ht_ent_t *ht, char *id);

inline static void ht_ent_free(ht_ent_t *ht);

inline static void ht_ent_rels_insert(ht_ent_rels_t *ht, ht_ent_rels_el_t *el);

inline static void ht_ent_rels_free(ht_ent_rels_t *ht);

inline static void ht_rel_ents_srcs_init(ht_rel_ents_srcs_t *ht, int size);

inline static void ht_rel_ents_srcs_insert(ht_rel_ents_srcs_t *ht, ht_ent_el_t *el);

inline static void ht_rel_ents_srcs_delete(ht_rel_ents_srcs_t *ht, ht_ent_el_t *id);

ht_ent_el_t *ht_rel_ents_srcs_search(ht_rel_ents_srcs_t *ht, ht_ent_el_t *id);

inline static void free_all();

// RED BLACK TREES

inline static void rel_MC_ents_insert(rbt_rel_el_t *rel, ht_ent_el_t *ent);

void rel_MC_ents_delete(rbt_rel_el_t *rel, ht_ent_el_t *ent);

inline static void rel_MC_ents_clean(rbt_rel_el_t *rel);

inline static rbt_rel_el_t *rbt_rel_minimum(rbt_rel_t *rbt, rbt_rel_el_t *el);

inline static rbt_rel_el_t *rbt_rel_maximum(rbt_rel_t *rbt, rbt_rel_el_t *el);

inline static rbt_rel_el_t *rbt_rel_successor(rbt_rel_t *rbt, rbt_rel_el_t *el);

inline static void rbt_rel_ents_init(rbt_rel_ents_t *rbt);

inline static void rbt_rel_ents_el_free(rbt_rel_ents_el_t *el);

inline static void rbt_rel_ents_insert(rbt_rel_el_t *rel, rbt_rel_ents_el_t *el);

inline static rbt_rel_ents_el_t *rbt_rel_ents_delete(rbt_rel_el_t *rel, rbt_rel_ents_el_t *el);

rbt_rel_ents_el_t *rbt_rel_ents_search(rbt_rel_ents_t *rbt, ht_ent_el_t *ent);

inline static void ht_rel_ents_srcs_free(ht_rel_ents_srcs_t *pS);

/*      GLOBALS     */
static ht_rel_el_t HT_REL_DELETED_ITEM = {NULL};
static ht_ent_el_t HT_ENT_DELETED_ITEM = {NULL};
static ht_ent_rels_el_t HT_ENT_RELS_DELETED_ITEM = {NULL};
static ht_ent_el_t HT_REL_ENTS_SRCS_DELETED_ITEM = {NULL};

static rbt_rel_ents_el_t RBT_REL_ENTS_NIL = {NULL, &RBT_REL_ENTS_NIL, &RBT_REL_ENTS_NIL, &RBT_REL_ENTS_NIL,
                                             (color_t) NULL};
static rbt_rel_el_t RBT_REL_NIL = {NULL, &RBT_REL_NIL, &RBT_REL_NIL, &RBT_REL_NIL, (color_t) NULL, NULL, NULL, 0};

rbt_rel_t rbt_rel = {NULL, NULL};
ht_ent_t ht_ent = {0, 0, NULL};

/*      IMPLEMENTATION      */
/* MATH UTILS */

void print_int(int val) {
    // Remove the last digit and recur
    if (val / 10)
        print_int(val / 10);

    // Print the last digit
    putchar_unlocked(val % 10 + '0');
}

// 1 is prime; 0 is not prime; -1 is undefined (i.e. x < 2);
int is_prime(const int x) {
    if (x < 2) {
        return -1;
    }
    if (x < 4) {
        return 1;
    }
    if ((x % 2) == 0) {
        return 0;
    }
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

inline static const int ht_hash(const char *val, const int size, const int attempt) {
#define FNV_PRIME_32 16777619
#define FNV_OFFSET_32 2166136261U
    uint32_t hash = FNV_OFFSET_32, i = 1;
    while (*++val != '\0') {
        hash = hash ^ *val;
        hash = hash * FNV_PRIME_32;
    }
    return (hash + attempt) % size;
}

inline static const unsigned int ht_hash_ptr(const void *val, const unsigned int size, const unsigned int attempt) {
    unsigned long long hash_a = 0;

    hash_a = hash_a + ((uint64_t) val / 32);

    return ((unsigned int) hash_a + (attempt)) % size;
    // const size_t shift = (size_t) log2(1 + sizeof(val));
    // return (((size_t) (val) >> shift) + attempt) % size;
}

// Initialize relations hashmap
void ht_ent_init(ht_ent_t *ht, int size) {
    ht->size = size;
    ht->count = 0;
    ht->table = (ht_ent_el_t **) calloc((size_t) size, sizeof(ht_ent_el_t *));

    // init CALLOC BUFFER
//    calloc_buffer = (ht_ent_rels_el_t **) calloc((size_t) HT_ENT_RELS_BUFFER_SIZE, sizeof(ht_ent_rels_el_t *));
//    calloc_buffer_count = 0;
}

// Hashtable resizing
void ht_ent_resize(ht_ent_t *ht, const int new_size) {
    ht_ent_t tmp_ht;
    ht_ent_init(&tmp_ht, next_prime(new_size));

    // rehash elements
    for (int i = 0; i < ht->size; ++i) {
        ht_ent_el_t *item = ht->table[i];
        if (item != NULL && item != &HT_ENT_DELETED_ITEM) {
            ht_ent_insert(&tmp_ht, item);
        }
    }
    tmp_ht.count = ht->count;
    // Switch between tmp_ht and ht
    const int tmp_size = ht->size;
    ht->size = tmp_ht.size;
    tmp_ht.size = tmp_size;

    ht_ent_el_t **tmp_table = ht->table;
    ht->table = tmp_ht.table;
    tmp_ht.table = tmp_table;

    free(tmp_ht.table);
}

// Insert new relation
static void ht_ent_insert(ht_ent_t *ht, ht_ent_el_t *el) {
    const int load = ht->count * 100 / ht->size;
    if (load > HT_MAX_LOAD_RATIO_100) {
        ht_ent_resize(ht, ht->size * 2);
    }

    int index = ht_hash(el->id, ht->size, 0);
    ht_ent_el_t *cur_item = ht->table[index];
    int i = 1;
    while (cur_item != NULL && cur_item != &HT_ENT_DELETED_ITEM) {
        index = ht_hash(el->id, ht->size, i);
        cur_item = ht->table[index];
        i++;
    }
    ht->table[index] = el;
    ht->count++;
}

void ht_ent_el_free(ht_ent_el_t *el) {
    free(el->rels_in.table);
    free(el->rels_out.table);

    free(el->id);
    free(el);
}

void ht_ent_delete(ht_ent_t *ht, char *id) {
    int index = ht_hash(id, ht->size, 0);
    ht_ent_el_t *item = ht->table[index];
    int i = 1;
    while (item != NULL) {
        if (item != &HT_ENT_DELETED_ITEM) {
            if (item->id == id) {
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

inline static int strcmp_lite(char *str1, char *str2) {
    // skip first char (always double quotes)
    int i = 1;
    while (str1[i] != '\0') {
        if (str1[i] != str2[i])
            return -1;
        i++;
    }
    return 0;
}

inline static ht_ent_el_t *ht_ent_search(ht_ent_t *ht, char *id) {
    int index = ht_hash(id, ht->size, 0);
    ht_ent_el_t *item = ht->table[index];
    int i = 1;
    while (item != NULL) {
        if (item != &HT_ENT_DELETED_ITEM && strcmp_lite(item->id, id) == 0) {
            return item;
        }
        index = ht_hash(id, ht->size, i);
        item = ht->table[index];
        i++;
    }
    return NULL;
}

inline static void ht_ent_free(ht_ent_t *ht) {
    for (int i = 0; i < ht->size; ++i) {
        if (ht->table[i] != NULL && ht->table[i]->id != NULL) {
            ht_ent_el_free(ht->table[i]);
        }
    }

    free(ht->table);
}

// SUB ENTITY HT POINTING TO RELATIONS RBTs
// Initialize relations hashmap
inline static void ht_ent_rels_init(ht_ent_rels_t *ht, int size) {
    ht->size = size;
    ht->count = 0;

    // check if new calloc needed
//    int next_buffer_count = calloc_buffer_count + size;
//    if (next_buffer_count <= HT_ENT_RELS_BUFFER_SIZE)
//    {
//        ht->table = calloc_buffer + calloc_buffer_count;
//        calloc_buffer_count = next_buffer_count;
//    }
//    else
//    {
//        calloc_buffer = (ht_ent_rels_el_t**) calloc((size_t) HT_ENT_RELS_BUFFER_SIZE, sizeof(ht_ent_rels_el_t *));
//        calloc_buffer_count = size;
//        ht->table = calloc_buffer;
//    }
    ht->table = (ht_ent_rels_el_t **) calloc((size_t) size, sizeof(ht_ent_rels_el_t *));
}

// Hashtable resizing
inline static void ht_ent_rels_resize(ht_ent_rels_t *ht, const int new_size) {
    ht_ent_rels_t *tmp_ht = malloc(sizeof(ht_ent_rels_t));
    ht_ent_rels_init(tmp_ht, next_prime(new_size));

    // rehash elements
    for (int i = 0; i < ht->size; ++i) {
        ht_ent_rels_el_t *item = ht->table[i];
        if (item != NULL && item != &HT_ENT_RELS_DELETED_ITEM) {
            ht_ent_rels_insert(tmp_ht, item);
        }
    }
    tmp_ht->count = ht->count;
    // Switch between tmp_ht and ht
    const int tmp_size = ht->size;
    ht->size = tmp_ht->size;
    tmp_ht->size = tmp_size;

    ht_ent_rels_el_t **tmp_table = ht->table;
    ht->table = tmp_ht->table;
    tmp_ht->table = tmp_table;

    free(tmp_ht->table);
    free(tmp_ht);
}

// Insert new relation
void ht_ent_rels_insert(ht_ent_rels_t *ht, ht_ent_rels_el_t *el) {
    const int load = ht->count * 100 / ht->size;
    if (load > HT_MAX_LOAD_RATIO_100) {
        ht_ent_rels_resize(ht, ht->size * 2);
    }

    int index = ht_hash_ptr(el->id, ht->size, 0);
    ht_ent_rels_el_t *cur_item = ht->table[index];
    int i = 1;
    while (cur_item != NULL && cur_item != &HT_ENT_RELS_DELETED_ITEM) {
        index = ht_hash_ptr(el->id, ht->size, i);
        cur_item = ht->table[index];
        i++;
    }
    ht->table[index] = el;
    ht->count++;
}

inline static void ht_ent_rels_el_delete(ht_ent_rels_el_t *el) {
    free(el);
}

inline static void ht_ent_rels_delete(ht_ent_rels_t *ht, char *id, rbt_rel_ents_el_t *ent_node) {
    unsigned int index = ht_hash_ptr(id, ht->size, 0);
    ht_ent_rels_el_t *item = ht->table[index];
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
            index = ht_hash_ptr(id, ht->size, i);
            item = ht->table[index];
            i++;
        }
    } else {
        while (item != NULL) {
            if (item != &HT_ENT_RELS_DELETED_ITEM) {
                if (item->id == id) {
                    ht_ent_rels_el_delete(item);
                    ht->table[index] = &HT_ENT_RELS_DELETED_ITEM;
                }
            }
            index = ht_hash_ptr(id, ht->size, i);
            item = ht->table[index];
            i++;
        }
    }
    ht->count--;
}

inline static ht_ent_rels_el_t *ht_ent_rels_search(ht_ent_rels_t *ht, char *id, rbt_rel_ents_el_key_t *ent_node_key) {
    int index = ht_hash_ptr(id, ht->size, 0);
    ht_ent_rels_el_t *item = ht->table[index];
    int i = 1;
    if (ent_node_key != NULL) {
        while (item != NULL) {
            if (item != &HT_ENT_RELS_DELETED_ITEM && item->rbt_node_key == ent_node_key) {
                return item;
            }
            index = ht_hash_ptr(id, ht->size, i);
            item = ht->table[index];
            i++;
        }
    } else {
        while (item != NULL) {
            if (item != &HT_ENT_RELS_DELETED_ITEM && item->id == id) {
                return item;
            }
            index = ht_hash_ptr(id, ht->size, i);
            item = ht->table[index];
            i++;
        }
    }
    return NULL;
}

void ht_ent_rels_free(ht_ent_rels_t *ht) {
    for (int i = 0; i < ht->size; ++i) {
        if (ht->table[i] != NULL && ht->table[i]->id != NULL) {
            ht_ent_rels_el_delete(ht->table[i]);
        }
    }

    free(ht->table);
}

// SUB ENTITY HT POINTING BACK TO ENTITIES HT
// Initialize relations hashmap
void ht_rel_ents_srcs_init(ht_rel_ents_srcs_t *ht, int size) {
    ht->size = size;
    ht->count = 0;
    ht->table = (ht_ent_el_t **) calloc((size_t) size, sizeof(ht_ent_el_t *));
}

// Hashtable resizing
void ht_rel_ents_srcs_resize(ht_rel_ents_srcs_t *ht, const int new_size) {
    ht_rel_ents_srcs_t *tmp_ht = malloc(sizeof(ht_rel_ents_srcs_t));
    ht_rel_ents_srcs_init(tmp_ht, next_prime(new_size));

    // rehash elements
    for (int i = 0; i < ht->size; ++i) {
        ht_ent_el_t *item = ht->table[i];
        if (item != NULL && item != &HT_ENT_DELETED_ITEM) {
            ht_rel_ents_srcs_insert(tmp_ht, item);
        }
    }
    tmp_ht->count = ht->count;
    // Switch between tmp_ht and ht
    const int tmp_size = ht->size;
    ht->size = tmp_ht->size;
    tmp_ht->size = tmp_size;

    ht_ent_el_t **tmp_table = ht->table;
    ht->table = tmp_ht->table;
    tmp_ht->table = tmp_table;

    ht_rel_ents_srcs_free(tmp_ht);
}

void ht_rel_ents_srcs_free(ht_rel_ents_srcs_t *ht) {
    free(ht->table);
    free(ht);
}

// Insert new relation
void ht_rel_ents_srcs_insert(ht_rel_ents_srcs_t *ht, ht_ent_el_t *el) {
    const int load = ht->count * 100 / ht->size;
    if (load > HT_MAX_LOAD_RATIO_100) {
        ht_rel_ents_srcs_resize(ht, ht->size * 2);
    }

    int index = ht_hash_ptr(el, ht->size, 0);
    ht_ent_el_t *cur_item = ht->table[index];
    int i = 1;
    while (cur_item != NULL && cur_item != &HT_REL_ENTS_SRCS_DELETED_ITEM) {
        index = ht_hash_ptr(el, ht->size, i);
        cur_item = ht->table[index];
        i++;
    }
    ht->table[index] = el;
    ht->count++;
}

void ht_rel_ents_srcs_delete(ht_rel_ents_srcs_t *ht, ht_ent_el_t *id) {
    int index = ht_hash_ptr(id, ht->size, 0);
    ht_ent_el_t *item = ht->table[index];
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

ht_ent_el_t *ht_rel_ents_srcs_search(ht_rel_ents_srcs_t *ht, ht_ent_el_t *id) {
    int index = ht_hash_ptr(id, ht->size, 0);
    ht_ent_el_t *item = ht->table[index];
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

inline static void free_rbt_children(rbt_rel_t *rbt, rbt_rel_el_t *el) {
    if (el->left != rbt->nil) {
        free_rbt_children(rbt, el->left);
    }
    if (el->right != rbt->nil) {
        free_rbt_children(rbt, el->right);
    }
    free(el->id);
    if (el != rbt->nil)
        free(el);
}

inline static void free_all() {
    ht_ent_free(&ht_ent);

    // free rbt_rel
    free_rbt_children(&rbt_rel, rbt_rel.root);
}

inline static void rbt_rel_ents_init(rbt_rel_ents_t *rbt) {
    rbt->nil = &RBT_REL_ENTS_NIL;
    rbt->root = rbt->nil;
}

// TODO check
void rbt_rel_ents_el_free(rbt_rel_ents_el_t* el) {
    free(el->key->sources.table);
//    free(el->key->sources);
    free(el->key);
    free(el);
}

inline static void rbt_rel_ents_right_rotate(rbt_rel_ents_t *rbt, rbt_rel_ents_el_t *el) {
    rbt_rel_ents_el_t *node = el->left;
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

inline static void rbt_rel_ents_left_rotate(rbt_rel_ents_t *rbt, rbt_rel_ents_el_t *el) {
    rbt_rel_ents_el_t *node = el->right;
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

inline static void rbt_rel_ents_insert_fixup(rbt_rel_ents_t *rbt, rbt_rel_ents_el_t *el) {
    if (el == rbt->root) {
        rbt->root->color = BLACK;
    } else {
        rbt_rel_ents_el_t *node_a = el->parent;
        if (node_a->color == RED) {
            if (node_a == node_a->parent->left) {
                rbt_rel_ents_el_t *node_b = node_a->parent->right;
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
                rbt_rel_ents_el_t *node_b = node_a->parent->left;
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

inline static void rbt_rel_ents_insert(rbt_rel_el_t *rel, rbt_rel_ents_el_t *el) {
    rbt_rel_ents_t *rbt = &rel->ents_tree;
    rbt_rel_ents_el_t *parent = rbt->nil;
    rbt_rel_ents_el_t *node = rbt->root;

    // check if MC_ents changes -> el.count >= rel.max_count
    if (el->key->count == rel->max_count) {
        rel_MC_ents_insert(rel, el->key->ent);
        rel->max_count = el->key->count;
    } else if (el->key->count > rel->max_count) {
        rel_MC_ents_clean(rel);
        rel_MC_ents_insert(rel, el->key->ent);
        rel->max_count = el->key->count;
    }

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

inline static rbt_rel_ents_el_t *rbt_rel_ents_minimum(rbt_rel_ents_t *rbt, rbt_rel_ents_el_t *el) {
    rbt_rel_ents_el_t *node = el;
    while (node->left != rbt->nil) {
        node = node->left;
    }

    return node;
}

inline static rbt_rel_ents_el_t *rbt_rel_ents_maximum(rbt_rel_ents_t *rbt, rbt_rel_ents_el_t *el) {
    rbt_rel_ents_el_t *node = el;
    while (node->right != rbt->nil) {
        node = node->right;
    }

    return node;
}

inline static rbt_rel_ents_el_t *rbt_rel_ents_successor(rbt_rel_ents_t *rbt, rbt_rel_ents_el_t *el) {
    rbt_rel_ents_el_t *el_backup = el;
    rbt_rel_ents_el_t *node;
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

inline static rbt_rel_ents_el_t *rbt_rel_ents_predecessor(rbt_rel_ents_t *rbt, rbt_rel_ents_el_t *el) {
    rbt_rel_ents_el_t *node;
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

inline static void rbt_rel_ents_delete_fixup(rbt_rel_ents_t *rbt, rbt_rel_ents_el_t *el) {
    rbt_rel_ents_el_t *node;
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

inline static rbt_rel_ents_el_t *rbt_rel_ents_delete(rbt_rel_el_t *rel, rbt_rel_ents_el_t *el) {
    rbt_rel_ents_t *rbt = &rel->ents_tree;
    rbt_rel_ents_el_t *node_a, *node_b;
    rbt_rel_ents_el_key_t *el_backup;
    el_backup = el->key;
    bool cleaned = false;

    // check if MC_ents changes -> el.count == rel.max_count
    if (el->key->count == rel->max_count) {
        rel_MC_ents_clean(rel);
        cleaned = true;
    }

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

    if (cleaned) {
        // fill with other entities at same level
        rbt_rel_ents_el_t *max_el = rbt_rel_ents_maximum(&rel->ents_tree, rel->ents_tree.root);
        if (max_el != rel->ents_tree.nil) {
            int max_count = max_el->key->count;
            while (max_el != rel->ents_tree.nil && max_el->key->count == max_count) {
                rel_MC_ents_insert(rel, max_el->key->ent);
                max_el = rbt_rel_ents_predecessor(&rel->ents_tree, max_el);
            }
            rel->max_count = max_count;
        }
    }

    return node_a;
}

inline static void rbt_rel_init(rbt_rel_t *rbt) {
    rbt->nil = &RBT_REL_NIL;
    rbt->root = rbt->nil;
}

inline static void rbt_rel_right_rotate(rbt_rel_t *rbt, rbt_rel_el_t *el) {
    rbt_rel_el_t *node = el->left;
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

inline static void rbt_rel_left_rotate(rbt_rel_t *rbt, rbt_rel_el_t *el) {
    rbt_rel_el_t *node = el->right;
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

inline static void rbt_rel_insert_fixup(rbt_rel_t *rbt, rbt_rel_el_t *el) {
    if (el == rbt->root) {
        rbt->root->color = BLACK;
    } else {
        rbt_rel_el_t *node_a = el->parent;
        if (node_a->color == RED) {
            if (node_a == node_a->parent->left) {
                rbt_rel_el_t *node_b = node_a->parent->right;
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
                rbt_rel_el_t *node_b = node_a->parent->left;
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

inline static void rbt_rel_insert(rbt_rel_t *rbt, rbt_rel_el_t *el) {
    rbt_rel_el_t *parent = rbt->nil;
    rbt_rel_el_t *node = rbt->root;

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

// insert ent in alfabetical order into rel->MC_entities
inline static void rel_MC_ents_insert(rbt_rel_el_t *rel, ht_ent_el_t *ent) {
    // Create new list element
    rel_mc_ents_el_t *new_el = (rel_mc_ents_el_t *) malloc(sizeof(rel_mc_ents_el_t));
    new_el->ent = ent;
    new_el->prev = NULL;
    new_el->next = NULL;

    rel_mc_ents_el_t *next_el = rel->mc_ents, *last_el;

    // is new_el the first el?
    if (next_el == NULL) {
        rel->mc_ents = new_el;
        return;
    }
    while (next_el != NULL) {
        if (strcmp(new_el->ent->id, next_el->ent->id) < 0) {
            // is next_el the first el?
            if (next_el->prev == NULL) {
                rel->mc_ents = new_el;
            } else {
                next_el->prev->next = new_el;
            }
            new_el->prev = next_el->prev;
            new_el->next = next_el;
            next_el->prev = new_el;
            return;
        }

        last_el = next_el;
        next_el = next_el->next;
    }
    // new_el should be placed at the end of the list
    last_el->next = new_el;
    new_el->prev = last_el;
}

// TODO complete this
void rel_MC_ents_delete(rbt_rel_el_t *rel, ht_ent_el_t *ent) {
    rel_mc_ents_el_t *el = rel->mc_ents;
    while (el != NULL) {
        if (el->ent == ent) {
            if (el->next != NULL) {
                el->next->prev = el->prev;
            }
            if (el->prev != NULL)
                el->prev->next = el->next;
            else {
                rel->mc_ents = el->next;
            }
            if (rel->mc_ents == NULL) {
                rel->max_count = 0;
            }

            free(el);
            return;
        }

        el = el->next;
    }
}

inline static void rel_MC_ents_clean(rbt_rel_el_t *rel) {
    rel_mc_ents_el_t *next, *el = rel->mc_ents;

    while (el != NULL) {
        next = el->next;
        free(el);
        el = next;
    }

    rel->mc_ents = NULL;
    rel->max_count = 0;
}

void rel_MC_ents_calc() {
}

inline static rbt_rel_el_t *rbt_rel_minimum(rbt_rel_t *rbt, rbt_rel_el_t *el) {
    rbt_rel_el_t *node = el;
    while (node->left != rbt->nil) {
        node = node->left;
    }

    return node;
}

inline static rbt_rel_el_t *rbt_rel_maximum(rbt_rel_t *rbt, rbt_rel_el_t *el) {
    rbt_rel_el_t *node = el;
    while (node->right != rbt->nil) {
        node = node->right;
    }

    return node;
}

inline static rbt_rel_el_t *rbt_rel_successor(rbt_rel_t *rbt, rbt_rel_el_t *el) {
    rbt_rel_el_t *node;
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

rbt_rel_el_t *rbt_rel_predecessor(rbt_rel_t *rbt, rbt_rel_el_t *el) {
    rbt_rel_el_t *el_backup = el;
    rbt_rel_el_t *node;
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

inline static void rbt_rel_delete_fixup(rbt_rel_t *rbt, rbt_rel_el_t *el) {
    rbt_rel_el_t *node;
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
inline static void rbt_rel_delete(rbt_rel_t *rbt, rbt_rel_el_t *el) {
    rbt_rel_el_t *node_a, *node_b;
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

    if (node_a != el) {
        el->id = node_a->id;
        el->ents_tree = node_a->ents_tree;
    }

    if (node_a->color == BLACK) {
        rbt_rel_delete_fixup(rbt, node_b);
    }

    free(node_a);
}

inline static rbt_rel_el_t *rbt_rel_search(rbt_rel_t *rbt, rbt_rel_el_t *node, char *id) {
    if (node == rbt->nil || strcmp_lite(node->id, id) == 0)
        return node;

    if (strcmp(id, node->id) < 0)
        return rbt_rel_search(rbt, node->left, id);
    else
        return rbt_rel_search(rbt, node->right, id);
}

inline static rbt_rel_el_t *rbt_rel_search_ptr(rbt_rel_t *rbt, rbt_rel_el_t *node, char *id) {
    if (node == rbt->nil || node->id == id)
        return node;

    if (strcmp(id, node->id) < 0)
        return rbt_rel_search_ptr(rbt, node->left, id);
    else
        return rbt_rel_search_ptr(rbt, node->right, id);
}

int main(int argc, char const *argv[]) {
    // INIT
    rbt_rel_init(&rbt_rel);
    ht_ent_init(&ht_ent, HT_ENT_SIZE);
    int r = 0, j = 0;
    //TESTING FILE

    // line model:
    // 'command "id_orig" "id_dest" "id_rel"'

    char line[200];
    char *id_orig = NULL, *id_dest = NULL, *id_rel = NULL;
    // scan first line and parse it
    // scan next line and parse it

    while ((line[r] = getchar_unlocked()) != '\n')
        r++;
//    getchar_unlocked();

    while (line[0] != 'e') {
        switch (line[0]) {
            case 'a':
                if (line[3] == 'e') {
                    // ADDENT

                    //                    id_orig points to first " char (always in )
                    id_orig = line + 7;
                    //                    replace space char with string termination (so id_orig = line[7] -> line[j])
                    line[r] = '\0';

                    if (ht_ent_search(&ht_ent, id_orig) == NULL) {
                        ht_ent_el_t *new_ent = (ht_ent_el_t *) malloc(sizeof(ht_ent_el_t));
                        new_ent->id = strdup(id_orig);
                        ht_ent_rels_init(&new_ent->rels_in, HT_ENT_RELS_SIZE);
                        ht_ent_rels_init(&new_ent->rels_out, HT_REL_ENTS_SRCS_SIZE);
                        ht_ent_insert(&ht_ent, new_ent);
                    }
                } else {
                    // ADDREL START
                    //                    id_orig points to first " char
                    id_orig = line + 7;
                    j = 7;
                    while (line[j] != ' ') {
                        j++;
                    }
                    //                    replace space char with string termination (so id_orig = line[7] -> line[j])
                    line[j] = '\0';
                    id_dest = line + j + 1;
                    while (line[j] != ' ') {
                        j++;
                    }
                    //                    replace space char with string termination
                    line[j] = '\0';

                    id_rel = line + j + 1;
                    //                    replace space char with string termination
                    line[r] = ' ';
                    line[r + 1] = '\0';

                    ht_ent_el_t *ent_orig = ht_ent_search(&ht_ent, id_orig);
                    ht_ent_el_t *ent_dest = ht_ent_search(&ht_ent, id_dest);
                    // do both entities exist?
                    if (ent_orig != NULL && ent_dest != NULL) {
                        rbt_rel_el_t *rel = rbt_rel_search(&rbt_rel, rbt_rel.root, id_rel);
                        rbt_rel_ents_el_t *ent_node;
                        ht_ent_rels_el_t *ent_rels_in_el, *ent_rels_out_el;

                        // first rel type insert?
                        if (rel == NULL || rel == rbt_rel.nil) {
                            rel = (rbt_rel_el_t *) malloc(sizeof(rbt_rel_el_t));
                            rel->id = strdup(id_rel);
                            rel->color = RED;
                            rbt_rel_ents_init(&rel->ents_tree);
                            rel->max_count = 0;
                            rel->mc_ents = NULL;

                            // Insert relation type node in rbt_rel
                            rbt_rel_insert(&rbt_rel, rel);
                        }

                        // can't search in binary tree because it's ordered on incoming rels count.
                        // So look for the pointback element in ent_dest->rels_in.
                        ent_rels_in_el = ht_ent_rels_search(&ent_dest->rels_in, rel->id, NULL);
                        // check if it's the first incoming rel instance (of rel->id type) for ent_dest entity.
                        if (ent_rels_in_el == NULL) {
                            ent_rels_in_el = (ht_ent_rels_el_t *) malloc(sizeof(ht_ent_rels_el_t));
                            ent_rels_in_el->id = rel->id;
                            // Create entity node in relation sub tree
                            ent_node = (rbt_rel_ents_el_t *) malloc(sizeof(rbt_rel_ents_el_t));
                            ent_node->parent = rel->ents_tree.nil;
                            ent_node->right = rel->ents_tree.nil;
                            ent_node->left = rel->ents_tree.nil;
                            ent_node->color = RED;
                            ent_node->key = (rbt_rel_ents_el_key_t *) malloc(sizeof(rbt_rel_ents_el_key_t));
                            ent_node->key->ent = ent_dest;
                            ent_node->key->count = 1;
                            ent_node->key->rbt_node = ent_node;
                            // Create sources hash table
                            // TODO should have a specific type?
                            ht_rel_ents_srcs_init(&ent_node->key->sources, HT_REL_ENTS_SRCS_SIZE);

                            ent_rels_in_el->rbt_node_key = ent_node->key;
                            ht_ent_rels_insert(&ent_dest->rels_in, ent_rels_in_el);
                        } else {
                            ent_node = ent_rels_in_el->rbt_node_key->rbt_node;

                            // check if it's the first outgoing rel instance (of rel->id type) for ent_orig entity.
                            ent_rels_out_el = ht_ent_rels_search(&ent_orig->rels_out, rel->id, ent_node->key);
                            // if ent_rels_out_el already exist, so does the relation.
                            if (ent_rels_out_el != NULL)
                                break;

                            ent_node = rbt_rel_ents_delete(rel, ent_node);
                            ent_node->parent = rel->ents_tree.nil;
                            ent_node->right = rel->ents_tree.nil;
                            ent_node->left = rel->ents_tree.nil;
                            ent_node->color = RED;
                            ent_node->key->count++;
                            ent_node->key->rbt_node = ent_node;
                        }
                        // HERE (ent_rels_out_el == NULL && ent_node != NULL) is TRUE

                        ent_rels_out_el = (ht_ent_rels_el_t *) malloc(sizeof(ht_ent_rels_el_t));
                        ent_rels_out_el->id = rel->id;
                        ent_rels_out_el->rbt_node_key = ent_node->key;

                        ht_ent_rels_insert(&ent_orig->rels_out, ent_rels_out_el);

                        // Add ent_orig pointer in sources ht
                        ht_rel_ents_srcs_insert(&ent_node->key->sources, ent_orig);
                        rbt_rel_ents_insert(rel, ent_node);
                    }
                    // ADDREL END
                }
                break;
            case 'd':
                if (line[3] == 'e') {
                    // DELENT
                    //                    id_orig points to first " char (always in )
                    id_orig = line + 7;
                    //                    replace endline char with string termination (so id_orig = line[7] -> line[r])
                    line[r] = '\0';

                    ht_ent_el_t *ent_orig = ht_ent_search(&ht_ent, id_orig);
                    rbt_rel_ents_el_t *rel_ents_el_node;
                    if (ent_orig != NULL) {
                        // for each rels_in
                        for (int i = 0; i < ent_orig->rels_in.size; ++i) {
                            if (ent_orig->rels_in.table[i] != NULL &&
                                ent_orig->rels_in.table[i] != &HT_ENT_RELS_DELETED_ITEM) {
                                rbt_rel_el_t *rel = rbt_rel_search_ptr(&rbt_rel, rbt_rel.root,
                                                                       ent_orig->rels_in.table[i]->id);
                                rel_ents_el_node = ent_orig->rels_in.table[i]->rbt_node_key->rbt_node;
                                // remove ent_orig from src->rels_out ht
                                for (int j = 0; j < rel_ents_el_node->key->sources.size; ++j) {
                                    if (rel_ents_el_node->key->sources.table[j] != NULL &&
                                        rel_ents_el_node->key->sources.table[j] != &HT_REL_ENTS_SRCS_DELETED_ITEM) {
                                        ht_ent_rels_delete(&rel_ents_el_node->key->sources.table[j]->rels_out, rel->id,
                                                           rel_ents_el_node);
                                    }
                                }
                                // Remove rbt node from rbt tree
                                rel_ents_el_node = rbt_rel_ents_delete(rel, rel_ents_el_node);
                                rbt_rel_ents_el_free(rel_ents_el_node);
                                // Check if rel has or not other entities
                                if (rel->ents_tree.root == rel->ents_tree.nil) {
                                    rbt_rel_delete(&rbt_rel, rel);
                                }
                            }
                        }

                        // for each rels_out
                        for (int i = 0; i < ent_orig->rels_out.size; ++i) {
                            if (ent_orig->rels_out.table[i] != NULL &&
                                ent_orig->rels_out.table[i] != &HT_ENT_RELS_DELETED_ITEM) {
                                rbt_rel_el_t *rel = rbt_rel_search_ptr(&rbt_rel, rbt_rel.root,
                                                                       ent_orig->rels_out.table[i]->id);

                                rel_ents_el_node = ent_orig->rels_out.table[i]->rbt_node_key->rbt_node;
                                rel_ents_el_node = rbt_rel_ents_delete(rel, rel_ents_el_node);
                                rel_ents_el_node->key->count--;
                                // Remove pointer rel_ents_el_node (or equivalents)
                                ht_rel_ents_srcs_delete(&rel_ents_el_node->key->sources, ent_orig);
                                ht_ent_rels_delete(&ent_orig->rels_out, rel->id, rel_ents_el_node);
                                if (rel_ents_el_node->key->count < 1) {
                                    ht_ent_rels_delete(&rel_ents_el_node->key->ent->rels_in, rel->id, NULL);
                                    rbt_rel_ents_el_free(rel_ents_el_node);
                                    // Check if rel has others entities
                                    if (rel->ents_tree.root == rel->ents_tree.nil) {
                                        rbt_rel_delete(&rbt_rel, rel);
                                    }
                                } else {
                                    rel_ents_el_node->parent = rel->ents_tree.nil;
                                    rel_ents_el_node->right = rel->ents_tree.nil;
                                    rel_ents_el_node->left = rel->ents_tree.nil;
                                    rel_ents_el_node->color = RED;
                                    rel_ents_el_node->key->rbt_node = rel_ents_el_node;

                                    rbt_rel_ents_insert(rel, rel_ents_el_node);
                                }
                            }
                        }

                        ht_ent_delete(&ht_ent, ent_orig->id);
                    }
                } else {
                    // DELREL
                    //                    id_orig points to first " char
                    id_orig = line + 7;
                    j = 7;
                    while (line[j] != ' ') {
                        j++;
                    }
                    //                    replace space char with string termination (so id_orig = line[7] -> line[j])
                    line[j] = '\0';
                    id_dest = line + j + 1;
                    while (line[j] != ' ') {
                        j++;
                    }
                    //                    replace space char with string termination
                    line[j] = '\0';

                    id_rel = line + j + 1;
                    //                    replace space char with string termination
                    line[r] = ' ';
                    line[r + 1] = '\0';

                    ht_ent_el_t *ent_orig = ht_ent_search(&ht_ent, id_orig);
                    ht_ent_el_t *ent_dest = ht_ent_search(&ht_ent, id_dest);

                    if (ent_orig != NULL && ent_dest) {
                        rbt_rel_el_t *rel = rbt_rel_search(&rbt_rel, rbt_rel.root, id_rel);
                        if (rel != rbt_rel.nil) {
                            rbt_rel_ents_el_t *ent_node;
                            ht_ent_rels_el_t *ent_rels_in_el, *ent_rels_out_el;

                            // can't search in binary tree because it's ordered on incoming rels count.
                            // So look for the pointback element in ent_dest->rels_in.
                            ent_rels_in_el = ht_ent_rels_search(&ent_dest->rels_in, rel->id, NULL);
                            if (ent_rels_in_el != NULL) {
                                ent_node = ent_rels_in_el->rbt_node_key->rbt_node;
                                // check if it's the first outgoing rel instance (of rel->id type) for ent_orig entity.
                                ent_rels_out_el = ht_ent_rels_search(&ent_orig->rels_out, rel->id, ent_node->key);
                                // if ent_rels_out_el already exist, so does the relation.
                                if (ent_rels_out_el != NULL) {
                                    // Remove rbt node from rbt tree
                                    ent_node = rbt_rel_ents_delete(rel, ent_node);
                                    ent_node->key->count--;
                                    // Remove pointer ent_node from ent_node sources hashtable
                                    ht_rel_ents_srcs_delete(&ent_node->key->sources, ent_orig);
                                    // Remove outgoing rel from ent_orig->rels_out
                                    ht_ent_rels_delete(&ent_orig->rels_out, rel->id, ent_node);
                                    if (ent_node->key->count < 1) {
                                        // Remove incoming rel from ent_dest->rels_in
                                        ht_ent_rels_delete(&ent_dest->rels_in, rel->id, NULL);
                                        rbt_rel_ents_el_free(ent_node);
                                        // Check if rel has others entities
                                        if (rel->ents_tree.root == rel->ents_tree.nil) {
                                            rbt_rel_delete(&rbt_rel, rel);
                                        }
                                    } else {
                                        ent_node->parent = rel->ents_tree.nil;
                                        ent_node->right = rel->ents_tree.nil;
                                        ent_node->left = rel->ents_tree.nil;
                                        ent_node->color = RED;
                                        ent_node->key->rbt_node = ent_node;

                                        rbt_rel_ents_insert(rel, ent_node);
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
                    // First rel in tree
                    rbt_rel_el_t *rel = rbt_rel_minimum(&rbt_rel, rbt_rel.root);
                    int max_count = 0;
                    // Iterate over relations from min to max
                    while (true) {
                        max_count = rel->max_count;

                        // rel.id includes space char
                        fputs_unlocked(rel->id, stdout);

                        // Iterate over SRSC entities START
                        rel_mc_ents_el_t *ent = rel->mc_ents;
                        while (ent != NULL) {
                            fputs_unlocked(ent->ent->id, stdout);
                            putchar_unlocked(' ');
                            ent = ent->next;
                        }
                        // Iterate over SRSC entities END

                        // NEXT REL
                        rel = rbt_rel_successor(&rbt_rel, rel);
                        if (rel == rbt_rel.nil) {
                            print_int(max_count);
                            fputs_unlocked(";\n", stdout);
                            break;
                        } else {
                            print_int(max_count);
                            fputs_unlocked("; ", stdout);
                        }
                    }
                } else {
                    fputs_unlocked("none\n", stdout);
                }
                break;
        }

        // reset line length
        r = 0;
        // scan next line and parse it
        while ((line[r] = getchar_unlocked()) != '\n')
            r++;
//        getchar_unlocked();
    }

    //free_all();

    return 0;
}