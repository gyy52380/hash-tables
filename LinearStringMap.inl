#pragma once

#include <vector>
#include "typedef.h"
#include "str.h"
#include <stdio.h>

//u64 string_hash(const char *str)
//{
//	u64 hash = 5381;
//	int c;
//
//	while (c = *str++)
//		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
//
//	return hash;
//}

#define INITIAL_CAPACITY 32
#define BITS_IN_GRAVE_FLAG_INT 32
#define LOAD_FACTOR 0.6f
#define GROWTH_FACTOR 2.0f

template<typename valT>
struct _TableEntry
{
	str key;
	valT value;
};

template<typename valT>
struct LinearStringMap
{
	typedef _TableEntry<valT> TableEntry;


	TableEntry* table;
	u32 capacity;
	u32 item_count;
	u32 item_and_deleted_count;
	u32 item_threshold;

	valT default_return_value = {};

	int counter = 0;

	/////////////////////////////////
	// functions for managing entries
	/////////////////////////////////



	inline static void free(TableEntry *entry)
	{
		str::free(entry->key);
	}

	inline bool equal(TableEntry *entry, const char* key)
	{
		return str::equal(entry->key, key);
	}
	inline bool equal(TableEntry *entry, str &key)
	{
		return str::equal(entry->key, key);
	}

	void move(TableEntry *dest, TableEntry *src)
	{
		str::move(dest->key, src->key);
		dest->value = src->value;
	}

	// empty string with allocated_bit = 0				--> empty but not deleted entry
	// empty string with allocated_bit = 1, ptr = NULL	--> empty and deleted entry
	// becasuse of str class string cant be non empty and return true for is_deleted(),
	// no need for set_deleted_flag_false()

	inline bool is_empty(TableEntry *entry)
	{
		return entry->key.empty();
	}

	bool is_deleted(TableEntry *entry)
	{
		return  entry->key.ptr == NULL && entry->key.allocated();
	}

	void set_deleted_flag_true(TableEntry *entry)
	{
		entry->key.set_allocated_bit();
		entry->key.ptr = NULL;
	}


	//////////////////
	// table functions
	//////////////////

	TableEntry* probe(TableEntry* table, u32 capacity, const char* key)
	{
		const u64 hash = string_hash(key);
		size_t index = hash % capacity;

		while (true)
		{
			TableEntry *entry = table + index;

			if (is_empty(entry) || equal(entry, key))
				return entry;

			index++;
			if (index >= capacity)
			{
				index = 0;
				counter++;
			}
		}
	}

	TableEntry* probe(TableEntry* table, u32 capacity, str &key)
	{
		const u64 hash = string_hash(key.data());
		size_t index = hash % capacity;

		while (true)
		{
			TableEntry *entry = table + index;

			if (is_empty(entry) || equal(entry, key))
				return entry;

			index++;
			if (index >= capacity)
			{
				index = 0;
				counter++;
			}
				
		}
	}
	
	void insert(const char* key, valT value)
	{
		if (capacity == 0)
			resize(INITIAL_CAPACITY);
		else if (item_and_deleted_count >= item_threshold)
			resize(capacity * GROWTH_FACTOR);

		str key_string = str::make(key);

		TableEntry *entry	= probe(table, capacity, key_string);

		entry->key	 = key_string;
		entry->value = value;
		item_count++;
		item_and_deleted_count++;
	}

	void resize(u32 new_capacity)
	{
		TableEntry* new_table = (TableEntry*)calloc(new_capacity, sizeof(TableEntry));

		for (int i = 0; i < capacity; i++)
		{
			TableEntry *spot = table + i;

			if (!is_empty(spot))
			{
				TableEntry* new_spot = probe(new_table, new_capacity, spot->key);
				move(new_spot, spot);
			}
		}

		::free(table);
		table			= new_table;
		capacity		= new_capacity;
		item_threshold	= new_capacity * LOAD_FACTOR;
		item_and_deleted_count = item_count;
	}

	void erase(const char* key)
	{
		const u64 key_hash = string_hash(key);
		size_t index = key_hash % capacity;

		//probing
		for (int i = 0; i < capacity; i++)
		{
			TableEntry *spot = table + index;

			if (is_empty(spot) && !is_deleted(spot))
				return;

			if (equal(spot, key))
			{
				if (is_deleted(spot)) // if already deleted exit
					return;
				else
				{
					free(spot);
					set_deleted_flag_true(spot);
					item_count--;

					return;
				}
			}	

			index++;
			if (index >= capacity)
			{
				index = 0;
				counter++;
			}
		}
	}

	valT find(const char* key)
	{
		const u64 key_hash = string_hash(key);
		size_t index = key_hash % capacity;

		for (int i = 0; i < capacity; i++)
		{
			TableEntry *spot = table + index;

			if (is_deleted(spot))
				goto loop_end;

			if (is_empty(spot))
				return default_return_value;

			if (equal(spot, key))
				return spot->value;

		loop_end:
			index++;
			if (index >= capacity)
			{
				index = 0;
				counter++;
			}
		}

		return default_return_value;
	}

	static void free(LinearStringMap<valT> &map)
	{
		for (int i = 0; i < map.capacity; i++)
			free(&map.table[i]);
	}

};