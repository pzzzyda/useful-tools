#include "ut_tree_map.h"
#include "ut_errno.h"
#include "ut_memswap.h"
#include <stdlib.h>
#include <string.h>

#define UT_RED 0
#define UT_BLACK 1

struct __ut_tree_map {
	ut_tree_entry_t *root;
	size_t len;
	const struct ut_type *key;
	const struct ut_type *value;
};

struct __ut_tree_entry {
	int color;
	ut_tree_entry_t *left;
	ut_tree_entry_t *right;
	ut_tree_entry_t *parent;
	const struct ut_type *key;
	const struct ut_type *value;
};

struct __ut_tree_map_iter {
	struct ut_iter base;
	ut_tree_entry_t *curr;
	struct ut_pair kv;
};

static inline void *ut_tree_entry_key(ut_tree_entry_t *self)
{
	return (uint8_t *)self + sizeof(ut_tree_entry_t);
}

static inline void *ut_tree_entry_value(ut_tree_entry_t *self)
{
	return (uint8_t *)ut_tree_entry_key(self) + self->key->size;
}

static inline bool ut_tree_entry_is_red(ut_tree_entry_t *self)
{
	return !self ? false : self->color == UT_RED;
}

static inline bool ut_tree_entry_is_black(ut_tree_entry_t *self)
{
	return !self ? true : self->color == UT_BLACK;
}

static inline bool ut_tree_entry_is_root(ut_tree_entry_t *self)
{
	return !self ? false : self->parent == NULL;
}

static inline void *ut_tree_entry_data(ut_tree_entry_t *self)
{
	return (uint8_t *)self + sizeof(ut_tree_entry_t);
}

static ut_tree_entry_t *ut_tree_entry_new(const struct ut_type *key,
					  const void *key_data,
					  const struct ut_type *value,
					  const void *value_data)
{
	ut_tree_entry_t *self;
	size_t size;

	size = sizeof(ut_tree_entry_t) + key->size + value->size;
	self = malloc(size);

	if (!self)
		return NULL;

	self->color = UT_RED;
	self->left = NULL;
	self->right = NULL;
	self->parent = NULL;
	self->key = key;
	self->value = value;
	memcpy(ut_tree_entry_key(self), key_data, key->size);
	memcpy(ut_tree_entry_value(self), value_data, value->size);
	return self;
}

static void ut_tree_entry_delete(ut_tree_entry_t *self)
{
	if (self->key->drop)
		self->key->drop(ut_tree_entry_key(self));

	if (self->value->drop)
		self->value->drop(ut_tree_entry_value(self));

	free(self);
}

static void ut_tree_entry_update(ut_tree_entry_t *self, const void *value)
{
	if (self->value->drop)
		self->value->drop(ut_tree_entry_value(self));

	memcpy(ut_tree_entry_value(self), value, self->value->size);
}

static void ut_tree_entry_swap(ut_tree_entry_t *self, ut_tree_entry_t *other)
{
	size_t size = self->key->size + self->value->size;
	ut_memswap(ut_tree_entry_data(self), ut_tree_entry_data(other), size);
}

static void ut_tree_map_rotate_left(ut_tree_map_t *self, ut_tree_entry_t *x)
{
	ut_tree_entry_t *y;

	/*
	 *    x                y
	 *   / \      -->     / \
	 *  xl  y            x   yr
	 *     / \          / \
	 *    yl  yr       xl  yl
	 */

	y = x->right;
	x->right = y->left;

	if (y->left)
		y->left->parent = x;

	y->parent = x->parent;

	if (!x->parent)
		self->root = y;
	else if (x == x->parent->left)
		x->parent->left = y;
	else
		x->parent->right = y;

	y->left = x;
	x->parent = y;
}

static void ut_tree_map_rotate_right(ut_tree_map_t *self, ut_tree_entry_t *x)
{
	ut_tree_entry_t *y;

	/*
	 *      x            y
	 *     / \    -->   / \
	 *    y   xr       yl  x
	 *   / \              / \
	 *  yl  yr           yr  xr
	 */

	y = x->left;
	x->left = y->right;

	if (y->right)
		y->right->parent = x;

	y->parent = x->parent;

	if (!x->parent)
		self->root = y;
	else if (x == x->parent->left)
		x->parent->left = y;
	else
		x->parent->right = y;

	y->right = x;
	x->parent = y;
}

static void ut_tree_map_fix_insert(ut_tree_map_t *self, ut_tree_entry_t *node)
{
	ut_tree_entry_t *parent, *grandparent, *tmp;

	while (true) {
		if (ut_tree_entry_is_root(node)) {
			node->color = UT_BLACK;
			break;
		}

		if (ut_tree_entry_is_black(node->parent))
			break;

		parent = node->parent;
		grandparent = parent->parent;
		tmp = grandparent->left;

		if (parent != tmp) { /* parent == grandparent->right */
			if (ut_tree_entry_is_red(tmp)) {
				/*
				 * Case 1: Uncle is red.
				 *
				 *     g           G
				 *    / \         / \
				 *   U   P  -->  u   p
				 *        \           \
				 *         N           N
				 */
				parent->color = UT_BLACK;
				tmp->color = UT_BLACK;
				grandparent->color = UT_RED;
				node = grandparent;
				continue;
			}

			tmp = parent->right;

			if (node != tmp) {
				/*
				 * Case 2: Uncle is black and node is parent's
				 * left child.
				 *
				 *    g           g
				 *   / \         / \
				 *  u   P  -->  u   N
				 *     /             \
				 *    N               P
				 */
				ut_tree_map_rotate_right(self, parent);
			}

			/*
			 * Case 3: Uncle is black and node is parent's right
			 * child.
			 *
			 *    g               p
			 *   / \             / \
			 *  u   P    -->    G   N
			 *       \         /
			 *        N       u
			 */
			grandparent->right->color = UT_BLACK;
			grandparent->color = UT_RED;
			ut_tree_map_rotate_left(self, grandparent);
			break;
		} else { /* parent == grandparent->left */
			tmp = grandparent->right;

			if (ut_tree_entry_is_red(tmp)) {
				parent->color = UT_BLACK;
				tmp->color = UT_BLACK;
				grandparent->color = UT_RED;
				node = grandparent;
				continue;
			}

			tmp = parent->left;

			if (node != tmp)
				ut_tree_map_rotate_left(self, parent);

			grandparent->left->color = UT_BLACK;
			grandparent->color = UT_RED;
			ut_tree_map_rotate_right(self, grandparent);
			break;
		}
	}
}

static void ut_tree_map_fix_remove(ut_tree_map_t *self, ut_tree_entry_t *parent)
{
	ut_tree_entry_t *sibling;
	ut_tree_entry_t *node = NULL;

	while (true) {
		if (ut_tree_entry_is_root(node))
			break;

		if (ut_tree_entry_is_red(node)) {
			node->color = UT_BLACK;
			break;
		}

		if (node == parent->right) {
			sibling = parent->left;

			if (ut_tree_entry_is_red(sibling)) {
				/*
				 * Case 1: Sibling is red.
				 *
				 *      p            s
				 *     / \          / \
				 *    S   n  -->   sl  P
				 *   / \              / \
				 *  sl  sr           sr  n
				 */
				parent->color = UT_RED;
				sibling->color = UT_BLACK;
				ut_tree_map_rotate_right(self, parent);
				continue;
			}

			/*
			 * Case 2: Sibling is black and has no red children.
			 *
			 *      p?            p?
			 *     / \   -->     / \
			 *    s   n         S   n
			 *   / \           / \
			 *  sl  sr        sl  sr
			 */
			if (ut_tree_entry_is_black(sibling->left) &&
			    ut_tree_entry_is_black(sibling->right)) {
				sibling->color = UT_RED;
				node = parent;
				parent = node->parent;
				continue;
			}

			if (ut_tree_entry_is_red(sibling->left)) {
				/*
				 * Case 3: Sibling is black and its left child
				 * is red.
				 *
				 *      p?          s?
				 *     / \         / \
				 *    s   n  -->  sl  p
				 *   / \             / \
				 *  SL  sr?         sr? n
				 */
				sibling->left->color = sibling->color;
				sibling->color = parent->color;
				parent->color = UT_BLACK;
				ut_tree_map_rotate_right(self, parent);
				break;
			} else {
				/*
				 * Case 4: Sibling is black and its right child
				 * is red.
				 *
				 *      p?           sr?
				 *     / \           / \
				 *    s   n  -->    s   p
				 *   / \           /     \
				 *  sl  SR        sl      n
				 */
				sibling->right->color = parent->color;
				parent->color = UT_BLACK;
				ut_tree_map_rotate_left(self, sibling);
				ut_tree_map_rotate_right(self, parent);
				break;
			}
		} else { /* node == parent->left */
			sibling = parent->right;

			if (ut_tree_entry_is_red(sibling)) {
				parent->color = UT_RED;
				sibling->color = UT_BLACK;
				ut_tree_map_rotate_left(self, parent);
				continue;
			}

			if (ut_tree_entry_is_black(sibling->left) &&
			    ut_tree_entry_is_black(sibling->right)) {
				sibling->color = UT_RED;
				node = parent;
				parent = node->parent;
				continue;
			}

			if (ut_tree_entry_is_red(sibling->right)) {
				sibling->right->color = sibling->color;
				sibling->color = parent->color;
				parent->color = UT_BLACK;
				ut_tree_map_rotate_left(self, parent);
				break;
			} else {
				sibling->left->color = parent->color;
				parent->color = UT_BLACK;
				ut_tree_map_rotate_right(self, sibling);
				ut_tree_map_rotate_left(self, parent);
				break;
			}
		}
	}
}

static void ut_tree_map_remove_all(ut_tree_map_t *self, ut_tree_entry_t *entry)
{
	if (!entry)
		return;

	ut_tree_map_remove_all(self, entry->left);
	ut_tree_map_remove_all(self, entry->right);
	ut_tree_entry_delete(entry);
}

static ut_tree_entry_t **ut_tree_map_get_entry(ut_tree_map_t *self,
					       const void *key,
					       ut_tree_entry_t **parent)
{
	int cmp;
	ut_tree_entry_t *curr_parent = NULL;
	ut_tree_entry_t **curr = &(self->root);

	while (*curr) {
		cmp = self->key->compare(key, ut_tree_entry_key(*curr));

		if (cmp > 0) {
			curr_parent = *curr;
			curr = &((*curr)->right);
		} else if (cmp < 0) {
			curr_parent = *curr;
			curr = &((*curr)->left);
		} else {
			break;
		}
	}

	if (parent)
		*parent = curr_parent;

	return curr;
}

ut_tree_map_t *ut_tree_map_new(const struct ut_type *key,
			       const struct ut_type *value)
{
	ut_tree_map_t *self;

	if (!key || !key->size || !value)
		return NULL;

	self = malloc(sizeof(ut_tree_map_t));
	if (!self)
		return NULL;

	self->root = NULL;
	self->len = 0;
	self->key = key;
	self->value = value;
	return self;
}

void ut_tree_map_delete(ut_tree_map_t *self)
{
	ut_tree_map_clear(self);
	free(self);
}

void ut_tree_map_clear(ut_tree_map_t *self)
{
	if (!self->len)
		return;

	ut_tree_map_remove_all(self, self->root);
	self->root = NULL;
	self->len = 0;
}

int ut_tree_map_insert(ut_tree_map_t *self, const void *key, const void *value)
{
	ut_tree_entry_t **entry, *parent;

	if (!key || !value)
		return UT_EINVAL;

	entry = ut_tree_map_get_entry(self, key, &parent);

	if (*entry) {
		ut_tree_entry_update(*entry, value);
		return UT_OK;
	} else {
		*entry = ut_tree_entry_new(self->key, key, self->value, value);

		if (!*entry)
			return UT_ENOMEM;

		(*entry)->parent = parent;
		ut_tree_map_fix_insert(self, *entry);
		self->len++;
		return UT_OK;
	}
}

void ut_tree_map_remove(ut_tree_map_t *self, const void *key)
{
	ut_tree_entry_t **entry, *tmp;

	if (!key)
		return;

	entry = ut_tree_map_get_entry(self, key, 0);

	if (!*entry)
		return;

	if ((*entry)->left && (*entry)->right) {
		tmp = *entry;
		entry = &((*entry)->right);

		while ((*entry)->left)
			entry = &((*entry)->left);

		ut_tree_entry_swap(*entry, tmp);
	}

	tmp = *entry;

	if ((*entry)->left || (*entry)->right) {
		*entry = (*entry)->left ? (*entry)->left : (*entry)->right;
		(*entry)->color = UT_BLACK;
		(*entry)->parent = tmp->parent;
	} else {
		*entry = NULL;
		if (tmp->color == UT_BLACK)
			ut_tree_map_fix_remove(self, tmp->parent);
	}

	self->len--;
	ut_tree_entry_delete(tmp);
}

void *ut_tree_map_get(ut_tree_map_t *self, const void *key)
{
	return ut_tree_map_get_key_value(self, key).value;
}

struct ut_pair ut_tree_map_get_key_value(ut_tree_map_t *self, const void *key)
{
	ut_tree_entry_t **entry, *parent;
	struct ut_pair kv = { NULL, NULL };

	if (!key)
		return kv;

	entry = ut_tree_map_get_entry(self, key, &parent);

	if (*entry) {
		kv.key = ut_tree_entry_key(*entry);
		kv.value = ut_tree_entry_value(*entry);
	}

	return kv;
}

size_t ut_tree_map_length(const ut_tree_map_t *self)
{
	return self->len;
}

bool ut_tree_map_is_empty(const ut_tree_map_t *self)
{
	return self->len == 0;
}

static void *ut_tree_map_iter_next(struct __ut_tree_map_iter *self)
{
	ut_tree_entry_t *parent;

	if (!self->curr)
		return NULL;

	self->kv.key = ut_tree_entry_key(self->curr);
	self->kv.value = ut_tree_entry_value(self->curr);

	if (self->curr->right) {
		self->curr = self->curr->right;
		while (self->curr->left)
			self->curr = self->curr->left;
	} else {
		parent = self->curr->parent;
		while (parent && self->curr == parent->right) {
			self->curr = parent;
			parent = parent->parent;
		}
		self->curr = parent;
	}

	return &self->kv;
}

struct ut_iter *ut_tree_map_iter_new(ut_tree_map_t *map)
{
	struct __ut_tree_map_iter *self;

	if (!map)
		return NULL;

	self = malloc(sizeof(struct __ut_tree_map_iter));
	if (!self)
		return NULL;

	self->base.next = (void *)&ut_tree_map_iter_next;
	self->curr = map->root;

	if (self->curr) {
		while (self->curr->left)
			self->curr = self->curr->left;
	}

	return (struct ut_iter *)self;
}

void ut_tree_map_iter_delete(struct ut_iter *self)
{
	free(self);
}
