#pragma once

#include "typedef.h"

// header:
// 0b |00000000 00000000 00000000 00000000 00000000 00000000 00000000 | 0000000|0 -> allocated (on heap) bool
//    |->length when allocated on the heap                            |-> length on stack

class str
{
public:
	static constexpr u32 BUFFER_ELEMENT_COUNT = 1000; 

	union
	{
		struct
		{
			char _padding;
			char buf[BUFFER_ELEMENT_COUNT]; //can store string of len 14 + \0
		};
		struct
		{
			u64 header;
			char* ptr;
		};

		char _for_initialization_only[BUFFER_ELEMENT_COUNT + 1] = {};
	};
	
	inline bool 	allocated() { return header & 1; }
	inline char* 	data() 		{ return allocated() ? ptr : buf; }
	inline u32 		len()		{ return allocated() ? header >> 1 : (header & 0xff) >> 1; }
	inline bool 	empty() 	{ return !len(); }

	static str make(const char* s);
	static str make(const char* s, u32 str_len);
	static void free(str &s);
	static void move(str &dest, str &src);
	static void copy(str &dest, str &src);
	static bool equal(str &s1, str &s2);
	static bool equal(str &s1, const char* s2);
	static bool equal(str &s1, const char* s2, u32 s2_len);

	inline void set_allocated_bit() 	{ header |= 1; }
	inline void clear_allocated_bit()	{ header = (header >> 1) << 1; }
	inline void set_len(u32 new_len) 	{ header = allocated() ? (new_len << 1) | 1 : ((header >> 8) << 8) | (new_len << 1); } //preserve allocated() bit
};