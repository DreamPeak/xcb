#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "ptrie.h"
#include "mem.h"
#include "dstr.h"

struct radix_tree *radix_new(void)
{
	struct radix_tree *rt = (struct radix_tree*)malloc(sizeof(struct radix_tree));

	rt->head = (struct node*)malloc(sizeof(struct node));
	strcpy(rt->head->key, "");
	rt->head->value_head = (struct value_struct*)malloc(sizeof(struct value_struct));
	rt->head->value_num = 0;
	rt->head->is_real = 0;
	rt->head->is_root = 1;
	rt->head->parent = NULL;
	rt->head->child_num = 0;
	rt->head->child = (struct node**)malloc(sizeof(struct node));

	return rt;
}

void radix_free(struct radix_tree *rt)
{
	node_free(rt->head);
	FREE(rt);
	rt = NULL;
}

void node_free(struct node *p)
{
	int i;
	for (i = 0; i < p->child_num; i++)
		node_free(p->child[i]);
	FREE(p);
	p = NULL;
}

int getNumberOfMatchingCharacters(char *key, struct node *p)
{
	int numberOfMatchingCharacters = 0;
	while (numberOfMatchingCharacters < strlen(key) && numberOfMatchingCharacters < strlen(p->key))
	{
            if (key[numberOfMatchingCharacters] != p->key[numberOfMatchingCharacters])
                break;
            numberOfMatchingCharacters++;
       }
	return numberOfMatchingCharacters;
}

void radix_insert(char *key, struct node *p)
{
	int numberOfMatchingCharacters = getNumberOfMatchingCharacters(key, p);
	if (p->is_root || numberOfMatchingCharacters == 0 || (numberOfMatchingCharacters < strlen(key) && numberOfMatchingCharacters >= strlen(p->key)))
	{
		int flag = 0;
		char *newText = (char*)malloc(sizeof(char)*(strlen(key) - numberOfMatchingCharacters + 1));
		strcpy(newText, key + numberOfMatchingCharacters);
		int i;
      		for (i = 0; i < p->child_num; i++)
		{
			if (p->child[i] == NULL && p->is_root)
			{
				struct node *tmp = (struct node*)malloc(sizeof(struct node));
				strcpy(tmp->key, newText);				
				tmp->child_num = 0;
				tmp->is_real = 1;
				tmp->parent = p;
				tmp->child = (struct node**)malloc(sizeof(struct node));

				p->child_num++;
				p->child = realloc(p->child, sizeof(struct node));
				p->child[p->child_num - 1] = tmp;

			}
			if (p->child[i]->key[0] == newText[0])
			{
				flag = 1;
				radix_insert(newText, p->child[i]);
			}
		}

		if (0 == flag)
		{
			struct node *rn = (struct node*)malloc(sizeof(struct node));

			strcpy(rn->key, newText);
			rn->child_num = 0;
			rn->parent = p;
			rn->is_real = 1;
			rn->child = (struct node**)malloc(sizeof(struct node));

			p->child_num++;
			p->child = realloc(p->child, sizeof(struct node));
	
			p->child[p->child_num - 1] = rn;
            	}
		FREE(newText);
		newText = NULL;
	}

       else if (numberOfMatchingCharacters == strlen(key) && numberOfMatchingCharacters == strlen(p->key))
	{
		if (1 == p->is_real)
			fprintf(stdout,"Duplicate key!\n");
		p->is_real = 1;
	}
       else if (numberOfMatchingCharacters > 0 && numberOfMatchingCharacters < strlen(p->key))
	{

		struct node *n1 = (struct node*)malloc(sizeof(struct node));
		char *newKey1 = (char*)malloc(sizeof(char)*(strlen(p->key) - numberOfMatchingCharacters + 1));
		strcpy(newKey1, p->key + numberOfMatchingCharacters);
		strcpy(n1->key, newKey1);
		FREE(newKey1);
		newKey1 = NULL;

		n1->value_head = p->value_head;
		n1->value_num = p->value_num;
		p->value_num = 0;
		p->value_head = NULL;

		n1->is_real = p->is_real;
		n1->child_num = p->child_num;
		int i;

		n1->child = (struct node**)malloc(sizeof(struct node) * n1->child_num);

		for (i = 0; i < n1->child_num; i++)
		{		
			n1->child[i] = p->child[i];
			p->child[i]->parent = n1;
		}
		n1->parent = p;
		if (1 == n1->child_num && 0 == n1->is_real)
			mergeNodes(n1, n1->child[0]);
		char *newKey2 = (char*)malloc(sizeof(char)* (numberOfMatchingCharacters + 1));
		strncpy(newKey2, key, numberOfMatchingCharacters);
		newKey2[numberOfMatchingCharacters] = '\0';
		
		strcpy(p->key, newKey2);
		FREE(newKey2);
		newKey2 = NULL;
		p->is_real = 0;
		p->child[p->child_num] = (struct node*)malloc(sizeof(struct node)); 
		p->child[0] = n1;
		p->child_num = 1;
	
		if(numberOfMatchingCharacters < strlen(key))
		{
			struct node *n2 = (struct node*)malloc(sizeof(struct node));
			char *newKey3 = (char*)malloc(sizeof(char)*(strlen(key) - numberOfMatchingCharacters + 1));
			strcpy(newKey3, key + numberOfMatchingCharacters);

			strcpy(n2->key,newKey3);
			n2->is_real = 1;
			n2->parent = p;
			n2->child_num = 0;
			n2->child = (struct node**)malloc(sizeof(struct node));
			p->child_num++;
			p->child = realloc(p->child, sizeof(struct node));
			p->child[1] = n2;
			
		}
		else
		{
			p->is_real = 1;
		}

	}
	else
	{
		struct node *n = (struct node*)malloc(sizeof(struct node));

		char *newKey4 = (char*)malloc(sizeof(char)*(strlen(p->key) - numberOfMatchingCharacters + 1));
		strcpy(newKey4, p->key + numberOfMatchingCharacters);

		strcpy(n->key, newKey4);

		n->value_head = p->value_head;
		n->value_num = p->value_num;
		p->value_num = 0;
		p->value_head = NULL;

		FREE(newKey4);
		newKey4 = NULL;
		n->parent = p;
		n->child_num = p->child_num;
		int i;
		for (i = 0; i < p->child_num; i++)
		{
			if (!n->child[i])
				n->child[i] = (struct node*)malloc(sizeof(struct node));
			n->child[i] = p->child[i];
		}
         	n->is_real = p->is_real;

		strcpy(p->key, key);
        	p->is_real = 1;
		p->child_num++;
		p->child = realloc(p->child, sizeof(struct node));

	       p->child[p->child_num] = n;
        }
}

void radix_delete(char *key, struct node *p)
{
	struct node *temp = radix_match(key, p, p);
	if (temp != NULL)
		visit(key, temp);
}

void visit(char *key, struct node *p)
{
	if (1 == p->is_real)
	{
		if (0 == p->child_num)
		{
			int i;
			for (i = 0; i < p->parent->child_num; i++)
			{
				if (!strcmp(p->parent->child[i]->key, p->key))
				{
					int j;
					for  (j = i; j < p->parent->child_num - 1; j++)
						strcpy(p->parent->child[j], p->parent->child[j + 1]);
					FREE(p->parent->child[p->parent->child_num - 1]);
					p->parent->child[p->parent->child_num - 1] = NULL;
					p->parent->child_num--;
				}
			}
			if (1 == p->parent->child_num && 0 == p->parent->is_real && 0 == p->is_root)
				mergeNodes(p->parent, p->parent->child[0]);
		}
		else if (1 == p->child_num)
			mergeNodes(p, p->child[0]);
		else
			p->is_real = 0;
	}
	return;
}

void mergeNodes(struct node *n_parent, struct node *n_child)
{
	n_parent->is_real = n_child->is_real;
	n_parent->value_head = n_child->value_head;
	n_child->value_head = NULL;
	if (n_parent->child_num < n_child->child_num)
		n_parent->child = realloc(n_parent->child, n_child->child_num - n_parent->child_num);
	n_parent->child_num = n_child->child_num;
	int i;
	for (i = 0; i < n_parent->child_num; i++)
	{
		n_parent->child[i] = n_child->child[i];
		n_parent->child[i]->parent = n_parent;
	}
	char *temp = (char*)malloc(strlen(n_parent->key) + strlen(n_child->key) + 1);
	strcpy(temp, n_parent->key);
	strcat(temp, n_child->key);

	strcpy(n_parent->key, temp);

	if (n_parent->child_num > n_child->child_num)
	{
		int i;
		for (i = n_child->child_num; i < n_parent->child_num; i++)
		{
			FREE(n_parent->child[i]);
			n_parent->child[i] = NULL;
		}
	}
	FREE(temp);
	temp = NULL;
	FREE(n_child);
	n_child = NULL;
}

void insert_value(char* key, void *value, struct node *p)
{
	struct node *node_temp = radix_match(key, p, p);

	if (node_temp != NULL)
	{
		struct value_struct *q = (struct value_struct*)malloc(sizeof(struct value_struct));
		q->value = value;

		if (0 == node_temp->value_num)
			node_temp->value_head = q;
		else
		{
			struct value_struct *value_temp = node_temp->value_head;

			int i = 0;
			for (i = 0; i < node_temp->value_num - 1; i++)
				value_temp = value_temp->next;
			value_temp->next = q;
		}
		node_temp->value_num++;
	}
}

void radix_block_insert(char *key, const char *sep, struct node *p)
{
	dstr *fields = NULL;
	int count = 0, i = 0;
	fields = dstr_split_len(key, strlen(key), sep, 1, &count);
	for (i = 0; i < count; i++)
	{
		int j = 0, flag = 0;
		for (j = 0; j < p->child_num; j++)
		{
			if (!strcmp(p->child[j]->key, fields[i]))
			{
				flag = 1;
				if (i == count - 1)
				{
					p->child[j]->is_real = 1;
				}
				p = p->child[j];
				break;
			}
		}
		if (0 == flag)
		{
			int k;
			struct node *q = (struct node*)malloc(sizeof(struct node));
			q = p;
			for (k = i; k < count; k++)
			{

				struct node *n = (struct node*)malloc(sizeof(struct node));
				strcpy(n->key,fields[k]);
				n->parent = q;
				if (k == count - 1)
				{
					n->is_real = 1;
				}
			
				q->child = realloc(q->child, sizeof(struct node));
				q->child[q->child_num] = n;
				q->child_num++;

				q = n;
			}
			break;
		}
	}
}

struct node *radix_match(char *prefix, struct node *p, struct node *head)
{
	int numberOfMatchingCharacters = getNumberOfMatchingCharacters(prefix, p);

	if (numberOfMatchingCharacters == strlen(prefix) && numberOfMatchingCharacters == strlen(p->key))
	{
		return p;
	}
	else if (p->is_root)
	{
		int flag = 0, i = 0;
		for (i = 0; i < p->child_num; i++)
		{
			if (getNumberOfMatchingCharacters(prefix, p->child[i]) != 0)
				flag = 1;
		}
		if (0 == flag)
		{
			return p;
		}
	}
	if (numberOfMatchingCharacters < strlen(prefix) && numberOfMatchingCharacters >= strlen(p->key))
	{
		char *newText = (char*)malloc(sizeof(char)*(strlen(prefix) - numberOfMatchingCharacters + 1));
		strcpy(newText, prefix + numberOfMatchingCharacters);

		int i;
      		for (i = 0; i < p->child_num; i++)
		{
			if (p->child[i]->key[0] == newText[0])
			{
				struct node *temp = (struct node*)malloc(sizeof(struct node));
				temp = radix_match(newText, p->child[i], head);
				FREE(newText);
				newText = NULL;
				return temp;
			}
		}
		FREE(newText);
		newText = NULL;
		return p;
	}
	if (p->parent != NULL)
		return p->parent;
	else
		return head;
}

void radix_show(struct node *p, int level)
{
	if (0 == level)
	{
		fprintf(stdout,"isroot = %d, level = root, key = %s, real = %d  child:%d  ",p->is_root, p->key, p->is_real,p->child_num);
		int i = 0;
		struct value_struct *temp = p->value_head;
		for (i = 0; i < p->value_num; i++)
		{
			fprintf(stdout,"%s,",temp->value);
			temp = temp->next;
		}
		fprintf(stdout,"\n");
	}
	else
	{
		fprintf(stdout,"isroot = %d, level = %d, key = %s, parent:%s, real = %d  child:%d ",p->is_root, level, p->key, p->parent->key, p->is_real,p->child_num);
		int i = 0;
		struct value_struct *temp = p->value_head;
		for (i = 0; i < p->value_num; i++)
		{
			fprintf(stdout,"%s,",temp->value);
			temp = temp->next;
		}
		fprintf(stdout,"\n");
	}
	int i;
	for (i = 0; i < p->child_num; i++)
		radix_show(p->child[i],level + 1);	
	return;
}