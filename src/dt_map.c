/*
 * dt_map.c: Associative arrays for Unit 5, Section E.
 *
 * An array does not store its indices. This map stores its keys.
 * An array calculates a position with one subtraction.
 * The map calculates a hash and then compares keys in one bucket.
 *
 * Hashing turns the key into a bucket number. Compare all keys in that bucket
 * because two keys can select it. Use a linked list for each bucket. Start the
 * unsigned accumulator at 14695981039346656037ULL. For each unsigned byte,
 * exclusive-or the byte into it and multiply by 1099511628211ULL.
 *
 * A separate list stores insertion order for stable output. dt_map_key_at
 * reads this list. Updating a key preserves its position. Removing and
 * reinserting a key moves it to the end.
 */

#include "dt.h"

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define DT_MAP_BUCKETS 16

typedef struct dt_map_entry {
    char *key;
    dt_value value;
    struct dt_map_entry *next;
} dt_map_entry;

struct dt_map {
    dt_map_entry *buckets[DT_MAP_BUCKETS];
    char **order;
    size_t len;
    size_t capacity;
    //int placeholder; /* TODO: Add the buckets and insertion-order data. */
};

static size_t dt_map_hash(const char *key)
{
    uint64_t hash = 14695981039346656037ULL;

    while (*key != '\0') {
        unsigned char byte = (unsigned char)*key;
        hash ^= byte;
        hash *= 1099511628211ULL;
        key++;
    }

    return (size_t)(hash % DT_MAP_BUCKETS);
}

/*
 * dt_map_new builds an empty map. It returns NULL after an allocation failure.
 */
dt_map *dt_map_new(void)
{
    dt_map *m = malloc(sizeof(dt_map));

    if (m == NULL)
        return NULL;

    for (size_t i=0; i < DT_MAP_BUCKETS; i++)
        m->buckets[i] = NULL;

    m->order = NULL;
    m->len = 0;
    m->capacity = 0;

    return m;
    /* TODO: Return an allocated empty map. Return NULL after an allocation failure.
       dt_map_new()  -> a map whose dt_map_len is 0
       cases/normal/map_basics.case */
}

/*
 * dt_map_free releases each entry, copied key, order array, and map.
 * It accepts NULL. The environment owns the values.
 */
void dt_map_free(dt_map *m)
{
    if (m == NULL)
        return;

       for (size_t i = 0; i < DT_MAP_BUCKETS; i++) {
        dt_map_entry *cur = m->buckets[i];

        while (cur != NULL) {
            dt_map_entry *next = cur->next;
            free(cur->key);
            free(cur);
            cur = next;
        }
       }
    
    free(m->order);
    free(m);
    /* TODO: Release each entry, copied key, order array, and map.
       Preserve the values. The environment owns them.
       a map holding a string value  -> the nodes and keys go, the string stays
       dt_map_free(NULL)             -> returns, having done nothing
       cases/cleanup/map_churn.case */
}

/*
 * dt_map_len returns the number of keys in constant time.
 */
size_t dt_map_len(const dt_map *m)
{
    /* TODO: Return the current key count.
       Replacing a value does not change this count.
       after put alpha, beta, gamma:  dt_map_len(m) -> 3
       after put beta again:          dt_map_len(m) -> 3, still
       after del alpha:               dt_map_len(m) -> 2
       cases/normal/map_basics.case */
    return m->len;
}

/*
 * dt_map_put binds v to key.
 * An existing key keeps its insertion position. A new key becomes the last key.
 * Copy each new key because the caller owns the source buffer.
 * Return DT_ERR_CAPACITY after an allocation failure.
 */
dt_status dt_map_put(dt_map *m, const char *key, dt_value v)
{
    size_t bucket = dt_map_hash(key);
    dt_map_entry *cur = m->buckets[bucket];

    while (cur != NULL) {
        if (strcmp(cur->key, key) == 0) {
            cur->value = v;
            return DT_OK;
        }

        cur = cur->next;
    }

    dt_map_entry *entry = malloc(sizeof(dt_map_entry));

    if (entry == NULL)
        return DT_ERR_CAPACITY;

    entry->key = malloc(strlen(key) + 1);

    if (entry->key == NULL) {
        free(entry);
        return DT_ERR_CAPACITY;
    }

    strcpy(entry->key, key);
    entry->value = v;
    entry->next = NULL;

        if (m->len == m->capacity) {
            size_t new_capacity;

            if (m->capacity == 0) {
                new_capacity = 4;
            } else {
                new_capacity = m->capacity * 2;
            }

            char **new_order = realloc(
                m->order,
                new_capacity * sizeof(char *)
            );

        if (new_order == NULL) {
            free(entry->key);
            free(entry);
            return DT_ERR_CAPACITY;
        }

        m->order = new_order;
        m->capacity = new_capacity;
    }

    entry->next = m->buckets[bucket];
    m->buckets[bucket] = entry;

    m->order[m->len] = entry->key;
    m->len++;

    /* TODO: Replace the value for an existing key.
       Add a new entry for a new key. Copy each new key.
       Hash the key. Select its bucket. Search the bucket chain.
       Add a new entry to the chain and insertion list.
       put "beta" -> 2 on an empty map    -> DT_OK, "beta" is last in order
       put "beta" -> 22 on that map       -> DT_OK, same position, new value
       an allocation failure              -> DT_ERR_CAPACITY, map unchanged
       cases/normal/map_basics.case */

    return DT_OK;
}

/*
 * dt_map_get writes the value for key to *out.
 * It returns DT_ERR_KEY and does not change *out when the key is absent.
 * An absent key differs from a nil value.
 */
dt_status dt_map_get(const dt_map *m, const char *key, dt_value *out)
{
    size_t bucket = dt_map_hash(key);
    dt_map_entry *cur = m->buckets[bucket];

    while (cur != NULL) {
        if (strcmp(cur->key, key) == 0) {
            *out = cur->value;
            return DT_OK;
        }

        cur = cur->next;
    }
    /* TODO: Return DT_ERR_KEY when the key is absent.
       Preserve *out after this error. A nil value can be present.
       after put "beta" -> 22:
         dt_map_get(m, "beta", &out)   -> DT_OK, *out is the integer 22
         dt_map_get(m, "ghost", &out)  -> DT_ERR_KEY, *out untouched
       cases/normal/map_basics.case, cases/boundary/map_missing_key.case */
    return DT_ERR_KEY;
}

/*
 * dt_map_remove removes key from its bucket and insertion position.
 * It releases the copied key. It returns DT_ERR_KEY when the key is absent.
 */
dt_status dt_map_remove(dt_map *m, const char *key)
{
    size_t bucket = dt_map_hash(key);
    dt_map_entry *cur = m->buckets[bucket];
    dt_map_entry *prev = NULL;

    while (cur != NULL) {
        if (strcmp(cur->key, key) == 0)
            break;

        prev = cur;
        cur = cur->next;  
    }

    if (cur == NULL)
        return DT_ERR_KEY;

    if (prev == NULL)
        m->buckets[bucket] = cur->next;
    else
        prev->next = cur->next;

    for (size_t i = 0; i < m->len; i++) {
        if (strcmp(m->order[i], key) == 0) {
            for (size_t j = i; j + 1 < m->len; j++)
                m->order[j] = m->order[j + 1];

            break;
        }
    }

    m->len--;
    
    free(cur->key);
    free(cur);

    return DT_OK;
    /* TODO: Remove the entry from its bucket and insertion position.
       Release the copied key. Return DT_ERR_KEY when the key is absent.
       a map holding alpha, beta, gamma:
         dt_map_remove(m, "alpha")  -> DT_OK, order is now beta, gamma
         dt_map_remove(m, "ghost")  -> DT_ERR_KEY, nothing changes
       reinserting "alpha" appends it after "gamma"
       cases/normal/map_basics.case, cases/boundary/map_remove_missing_key.case */
}

/*
 * dt_map_key_at writes the key at insertion position index to *out.
 * It returns DT_ERR_RANGE and does not change *out for an invalid index.
 */
dt_status dt_map_key_at(const dt_map *m, size_t index, const char **out)
{
    if (index >= m->len)
        return DT_ERR_RANGE;

    *out = m->order[index];
    
    return DT_OK;
    /* TODO: Write the key at the specified insertion position to *out.
       Return DT_ERR_RANGE for an invalid position. Preserve *out after this error.
       The printer uses this order.
       a map holding alpha, beta, gamma:
         dt_map_key_at(m, 0, &out)  -> DT_OK, *out = "alpha"
         dt_map_key_at(m, 3, &out)  -> DT_ERR_RANGE, *out untouched
       cases/normal/map_basics.case */
}
