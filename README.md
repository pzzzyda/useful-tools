# useful-tools

## Introduction

useful-tools is a simple generic container library implemented in C.
| Name | Description |
| - | - |
| `ut_array_t` | A dynamic array that scales automatically. |
| `ut_deque_t` | A double-ended queue based on a growable ring buffer implementation. |
| `ut_list_t` | A doubly linked list. |
| `ut_hash_map_t` | A hash map. |
| `ut_hash_set_t` | A hash set. |
| `ut_tree_map_t` | An ordered map based on red-black tree. |
| `ut_tree_set_t` | An ordered set based on red-black tree. |
| `ut_heap_t` | A binary heap. |
| `struct ut_string` | A simple string. |

## Build

```bash
mkdir build
cmake -B build
cmake --build build
```

## Install

```bash
sudo cmake --install build
```

## Example

```c
int main()
{
	ut_array_t *arr = ut_array_new(ut_type_int());
	ut_array_push(arr, &(int){ 10 });
	ut_array_push(arr, &(int){ 11 });
	ut_array_push(arr, &(int){ 12 });
	ut_array_push(arr, &(int){ 13 });

	struct ut_iter *iter = ut_array_iter_new(arr);
	int *elem;
	while ((elem = iter->next(iter))) {
		printf("%d\n", *elem);
	}

	ut_array_delete(arr);
	return 0;
}

```
