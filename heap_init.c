#include "codexion.h"

t_heap	*heap_create(int capacity)
{
	t_heap	*heap;

	heap = malloc(sizeof(t_heap));
	if (!heap)
		return (NULL);
	heap->req = malloc(sizeof(t_req) * capacity);
	if (!heap->req)
	{
		free(heap);
		return (NULL);
	}
	heap->capacity = capacity;
	heap->size = 0;
	return (heap);
}

void	heap_freed(t_heap *heap)
{
	if (heap)
	{
		if (heap->req)
			free(heap->req);
		free(heap);
	}
}

void	heap_swap(t_heap *heap, int req_one, int req_two)
{
	t_req tmp;

	tmp = heap->req[req_one];
	heap->req[req_one] = heap->req[req_two];
	heap->req[req_two] = tmp;
}