/*
 * Copyright (c) 1994,1995,1996,1997 by David R. Hanson.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this
 * software and associated documentation files (the "Software"), to deal in the Software
 * without restriction, including without limitation the rights to use, copy, modify,
 * merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies
 * or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * <http://www.opensource.org/licenses/mit-license.php>
 */

#include <stdarg.h>
#include "macros.h"
#include "mem.h"
#include "ring.h"

struct ring_t {
	size_t	length;
	struct node {
		struct node	*prev, *next;
		void		*value;
	}	*head;
};

ring_t ring_new(void) {
	ring_t ring;

	if (unlikely(NEW(ring) == NULL))
		return NULL;
	ring->length = 0;
	ring->head   = NULL;
	return ring;
}

ring_t ring_ring(void *x, ...) {
	ring_t ring;
	va_list ap;

	if ((ring = ring_new()) == NULL)
		return NULL;
	va_start(ap, x);
	for (; x; x = va_arg(ap, void *))
		ring_addhi(ring, x);
	va_end(ap);
	return ring;
}

void ring_free(ring_t *rp) {
	struct node *p, *q;

	/* assert(rp && *rp); */
	if (unlikely(rp == NULL || *rp == NULL))
		return;
	if ((p = (*rp)->head) != NULL) {
		int n = (*rp)->length;

		for (; n-- > 0; p = q) {
			q = p->next;
			FREE(p);
		}
	}
	FREE(*rp);
}

size_t ring_length(ring_t ring) {
	/* assert(ring); */
	if (unlikely(ring == NULL))
		return 0;
	return ring->length;
}

void *ring_put(ring_t ring, int i, void *x) {
	struct node *q;
	void *prev;

	/* assert(ring);                       */
	/* assert(i >= 0 && i < ring->length); */
	if (unlikely(ring == NULL))
		return NULL;
	if (i < 0 || i >= ring->length)
		return NULL;
	/* This code takes the shortest route to the ith node: if i does not exceed
	 * one-half the ring's length, the first for loop goes clockwise via the next pointers
	 * to the desired node. Otherwise, the second for loop goes counterclockwise
	 * via the prev pointers. */
	{
		int n;

		q = ring->head;
		if (i <= ring->length / 2)
			for (n = i; n-- > 0; )
				q = q->next;
		else
			for (n = ring->length - i; n-- > 0; )
				q = q->prev;
	}
	prev = q->value;
	q->value = x;
	return prev;
}

void *ring_get(ring_t ring, int i) {
	struct node *q;

	/* assert(ring);                       */
	/* assert(i >= 0 && i < ring->length); */
	if (unlikely(ring == NULL))
		return NULL;
	if (i < 0 || i >= ring->length)
		return NULL;
	{
		int n;

		q = ring->head;
		if (i <= ring->length / 2)
			for (n = i; n-- > 0; )
				q = q->next;
		else
			for (n = ring->length - i; n-- > 0; )
				q = q->prev;
	}
	return q->value;
}

void *ring_add(ring_t ring, int pos, void *x) {
	/* assert(ring);                                            */
	/* assert(pos >= -ring->length && pos <= ring->length + 1); */
	if (unlikely(ring == NULL))
		return NULL;
	if (pos < -ring->length || pos > ring->length + 1)
		return NULL;
	if (pos == 1 || pos == -ring->length)
		return ring_addlo(ring, x);
	else if (pos == 0 || pos == ring->length + 1)
		return ring_addhi(ring, x);
	else {
		struct node *p, *q;
		int i = pos < 0 ? pos + ring->length : pos - 1;

		{
			int n;

			q = ring->head;
			if (i <= ring->length / 2)
				for (n = i; n-- > 0; )
					q = q->next;
			else
				for (n = ring->length - i; n-- > 0; )
					q = q->prev;
		}
		if (NEW(p) == NULL)
			return NULL;
		{
			p->prev = q->prev;
			q->prev->next = p;
			p->next = q;
			q->prev = p;
		}
		++ring->length;
		return p->value = x;
	}
}

void *ring_addlo(ring_t ring, void *x) {
	/* assert(ring); */
	if (unlikely(ring == NULL))
		return NULL;
	ring_addhi(ring, x);
	ring->head = ring->head->prev;
	return x;
}

void *ring_addhi(ring_t ring, void *x) {
	struct node *p, *q;

	/* assert(ring); */
	if (unlikely(ring == NULL))
		return NULL;
	if (NEW(p) == NULL)
		return NULL;
	if ((q = ring->head) != NULL) {
		p->prev = q->prev;
		q->prev->next = p;
		p->next = q;
		q->prev = p;
	} else
		ring->head = p->prev = p->next = p;
	++ring->length;
	return p->value = x;
}

void *ring_remove(ring_t ring, int i) {
	struct node *q;
	void *x;

	/* assert(ring);                       */
	/* assert(ring->length > 0);           */
	/* assert(i >= 0 && i < ring->length); */
	if (unlikely(ring == NULL || ring->length <= 0))
		return NULL;
	if (i < 0 || i >= ring->length)
		return NULL;
	{
		int n;

		q = ring->head;
		if (i <= ring->length / 2)
			for (n = i; n-- > 0; )
				q = q->next;
		else
			for (n = ring->length - i; n-- > 0; )
				q = q->prev;
	}
	if (i == 0)
		ring->head = ring->head->next;
	x = q->value;
	q->prev->next = q->next;
	q->next->prev = q->prev;
	FREE(q);
	if (--ring->length == 0)
		ring->head = NULL;
	return x;
}

void *ring_remlo(ring_t ring) {
	/* assert(ring);             */
	/* assert(ring->length > 0); */
	if (unlikely(ring == NULL || ring->length <= 0))
		return NULL;
	ring->head = ring->head->next;
	return ring_remhi(ring);
}

void *ring_remhi(ring_t ring) {
	struct node *q;
	void *x;

	/* assert(ring);             */
	/* assert(ring->length > 0); */
	if (unlikely(ring == NULL || ring->length <= 0))
		return NULL;
	q = ring->head->prev;
	x = q->value;
	q->prev->next = q->next;
	q->next->prev = q->prev;
	FREE(q);
	if (--ring->length == 0)
		ring->head = NULL;
	return x;
}

void ring_rotate(ring_t ring, int n) {
	int i;
	struct node *q;

	/* assert(ring);                                    */
	/* assert(n >= -ring->length && n <= ring->length); */
	if (unlikely(ring == NULL))
		return;
	if (n < -ring->length || n > ring->length)
		return;
	if (n >= 0)
		i = n % ring->length;
	else
		i = n + ring->length;
	{
		int n;

		q = ring->head;
		if (i <= ring->length / 2)
			for (n = i; n-- > 0; )
				q = q->next;
		else
			for (n = ring->length - i; n-- > 0; )
				q = q->prev;
	}
	ring->head = q;
}

