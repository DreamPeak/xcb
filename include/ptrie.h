#ifndef PTRIE_INCLUDED
#define PTRIE_INCLUDED

struct value_struct
{
	void *value;
	struct value_struct *next;
};

struct node
{
	char key[300];
	struct value_struct *value_head;
	long value_num;
	int is_real;
	int is_root;
	long child_num;
	struct node *parent;
	struct node **child;
};

struct radix_tree
{
	long size;
	struct node *head;
	pthread_mutex_t lock;
	pthread_rwlock_t rwlock;
};

extern struct radix_tree *radix_new(void);
extern void radix_free(struct radix_tree *rt);
extern void node_free(struct node *p);
extern int getNumberOfMatchingCharacters(char *key, struct node *p);
extern struct node *radix_match(char *prefix, struct node *p, struct node *head);
extern void radix_insert(char *key, struct node *p);
extern struct node *radix_find(char *prefix, struct node *p);
extern void radix_delete(char *key, struct node *p);
extern void visit(char *key, struct node *p);
extern void mergeNodes(struct node *n_parent, struct node *n_child);
extern int radix_contain(char *prefix, struct node *p);
extern void radix_show(struct node *p, int level);
extern void insert_value(char* key, void *value, struct node *p);
extern void radix_block_insert(char *key, const char *sep, struct node *p);

#endif /* PTRIE_INCLUDED */