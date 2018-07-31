#include "str.h"

#include <stdlib.h>
#include <string.h>

#include <stdio.h>

void str::free(str &s)
{
	if (s.allocated())
		::free(s.ptr);
	s.clear_allocated_bit();
	s.set_len(0);
}

str str::make(const char* s)
{
	const size_t str_len = strlen(s);
	return str::make(s, str_len);
}

str str::make(const char* s, u32 str_len)
{
	str ret;
	
	if (str_len < BUFFER_ELEMENT_COUNT)
	{
		memcpy(ret.buf, s, str_len + 1);
		ret.clear_allocated_bit();
	}

	else
	{
		ret.ptr = _strdup(s);
		ret.set_allocated_bit();
	}

	ret.set_len(str_len);

	return ret;
}

void str::copy(str &dest, str &src)
{
	str::free(dest);
	dest = str::make(src.data());
}

void str::move(str &dest, str &src)
{
	//printf("src allocated: %d, dest: %d\n", src.allocated(), dest.allocated());

	str::free(dest);

	if (!src.allocated())
	{
		dest = str::make(src.data());
	}
	else
	{
		dest.ptr = src.ptr;
		dest.set_allocated_bit();
		dest.set_len(src.len());

		src.ptr = NULL;
		src.clear_allocated_bit();
		src.set_len(0);
	}

	//str::free(dest);
	//str::copy(dest, src);
	//str::free(src);
}

bool str::equal(str &s1, str &s2)
{
	return s1.len() == s2.len() ? !strcmp(s1.data(), s2.data()) : false;
}

bool str::equal(str &s1, const char* s2)
{
	return  s1.len() == strlen(s2) ? !strcmp(s1.data(), s2) : false;
}

bool str::equal(str & s1, const char * s2, u32 s2_len)
{
	return  s1.len() == s2_len ? !strcmp(s1.data(), s2) : false;
}
