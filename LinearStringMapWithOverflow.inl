#pragma once

#include "typedef.h"
#include "str.h"


u64 string_hash(const char *str, u32 *str_len_ptr = NULL)
{
	u64 hash = 5381;
	int c;

	if (str_len_ptr == NULL)
	{
		while (c = *str++)
			hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
	}
	else
	{
		while (c = *str++)
		{
			hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
			(*str_len_ptr)++;
		}
	}
	

	return hash;
}

template <typename T>
struct LinearStringMapV2
{
	typedef u64 hashT;

	static constexpr u32 INITIAL_CAPACITY = 32;
	static constexpr float OVERFLOW_CAPACITY_FACTOR = 0.125f;
	static constexpr float GROWTH_FACTOR = 2.0f;
	static constexpr float LOAD_FACTOR = 0.6f;

	struct TableEntry
	{
		str key;
		hashT hash;
		T value;
	};

	T default_value = {};

	TableEntry *table;
	//TableEntry *overflow;

	u32 capacity;
	u32 item_count;
	u32 item_threshold;
	u32 overflow_capacity;
	u32 overflow_count;


	inline bool empty(TableEntry *entry)
	{
		return entry->key.empty();
	}

	inline bool equal(TableEntry *entry, hashT hash, str &key)
	{
		return entry->hash == hash ? str::equal(entry->key, key) : false;
	}

	inline bool equal(TableEntry *entry, hashT hash, const char* key, size_t key_len)
	{
		return entry->hash == hash ? str::equal(entry->key, key, key_len) : false;
	}

	TableEntry* probe(TableEntry *table, u32 capacity, hashT hash, str &key)
	{
		size_t index = hash % capacity;

		while (true)
		{
			TableEntry *entry = table + index;

			if (empty(entry) || equal(entry, hash, key))
				return entry;

			index++; // no need for looping around, overflow will never be filled
		}
	}

	TableEntry* probe(TableEntry *table, u32 capacity, hashT hash, const char* key, size_t key_len)
	{
		size_t index = hash % capacity;

		while (true)
		{
			TableEntry *entry = table + index;

			if (empty(entry) || equal(entry, hash, key, key_len))
				return entry;

			index++; // no need for looping around, overflow will never be filled
		}
	}


	void insert(const char* key, T value)
	{
		if (capacity == 0)
			resize(INITIAL_CAPACITY);
		if (item_count >= item_threshold)
			resize(capacity * GROWTH_FACTOR);

		u32 str_len 	= 0;
		hashT key_hash 	= string_hash(key, &str_len);
		str key_str 	= str::make(key, str_len);

		TableEntry *spot = probe(table, capacity, key_hash, key_str);

		spot->key 	= key_str;
		spot->hash 	= key_hash;
		spot->value = value;

		item_count++;

		if (spot - table >= capacity)
		{
			overflow_count++;
			if (overflow_count >= overflow_capacity)
				resize(capacity * GROWTH_FACTOR);
		}
	}

	T find(const char* key)
	{
		u32 key_len 	= 0;
		u64 key_hash 	= string_hash(key, &key_len);

		TableEntry *entry = probe(table, capacity, key_hash, key, key_len);

		if (empty(entry))
			return default_value;
		else
			return entry->value;
	}


	void move(TableEntry *dest, TableEntry *src)
	{
		str::move(dest->key, src->key);
		dest->hash 	= src->hash;
		dest->value = src->value;
	}

	void erase(const char* key)
	{
		u32 key_len 	= 0;
		u64 key_hash 	= string_hash(key, &key_len);

		// probing
		size_t index = key_hash % capacity;
		TableEntry *entry;

		while (true)
		{
			entry = table + index;

			if (empty(entry))
				return;

			if (equal(entry, key_hash, key, key_len))
				break;

			index++; // no need for looping around, overflow will never be filled
		}

		// empty found entry
		str::free(entry->key);
		item_count--;

		// look for next empty cell
		size_t empty_index 	= index;
		size_t pivot_index 	= empty_index + 1; 

		while (pivot_index < capacity)
		{
			TableEntry *pivot = table + pivot_index;

			if (empty(pivot))
				return;

			size_t pivot_hashed_index = pivot->hash % capacity;
			if (pivot_hashed_index <= empty_index)
			{
				TableEntry *empty = table + empty_index;
				move(empty, pivot);
				empty_index = pivot_index;
			}

			pivot_index++;
		}

		// handle one possible overflow move
		if (overflow_count == 0)
			return;

		TableEntry *last_overflowed = table + capacity + overflow_count - 1;
		TableEntry *empty 			= table + empty_index;
		move(empty, last_overflowed);
		overflow_count--;
	}

	void resize(u32 new_capacity)
	{
		u32 new_overflow_capacity = new_capacity * OVERFLOW_CAPACITY_FACTOR;

		TableEntry *new_table = (TableEntry*) calloc(new_capacity + new_overflow_capacity, sizeof(TableEntry));

		u32 new_overflow_count = 0;

		for (int i = 0; i < capacity + overflow_capacity; i++)
		{
			TableEntry *entry = table + i;

			if (!empty(entry))
			{
				TableEntry *new_spot = probe(new_table, new_capacity, entry->hash, entry->key);
				move(new_spot, entry);

				if (new_table - new_spot >= new_capacity)
				{
					new_overflow_count++;
					if (new_overflow_count >= new_overflow_capacity)
						resize(new_capacity * GROWTH_FACTOR);
				}
			}
		}

		free(table);
		table 				= new_table;
		capacity 			= new_capacity;
		overflow_capacity 	= new_overflow_capacity;
		overflow_count 		= new_overflow_count;
		item_threshold 		= new_capacity * LOAD_FACTOR;
	}
};