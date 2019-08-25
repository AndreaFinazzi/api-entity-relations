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
        if (el->parent->)
        el->parent-> =
    }
    if (node_a->color == BLACK) {
        rbt_rel_ents_delete_fixup(rbt, node_b);
    }
}