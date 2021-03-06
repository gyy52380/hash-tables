// hash tables.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include "LinearStringMapWithOverflow.inl"
#include "LinearStringMap.inl"
#include "str.h"
#include <unordered_map>
#include <map>

#include <random>
#include <time.h>

#include <iostream>
#include <chrono>

#include "ScopedTimer.h"


LinearStringMap<u64> mymap;
LinearStringMapV2<u64> mymap2;
std::unordered_map<std::string, u64> um;
std::map<std::string, u64> m;

constexpr int TEST_COUNT = 100000;

// generate pairs
struct Pair
{
	str key;
	int value;
};

Pair pairs[TEST_COUNT];

typedef std::chrono::duration<float> fsec;

struct Times
{
	u64 map;
	u64 unordered_map;
	u64 my_map;
	u64 my_map2;
};

Times	insertion,
		finding,
		finding_non_existant,
		deletion;

int main()
{
	
	for (int i = 0; i < TEST_COUNT; i++)
	{
		char buf[64];
		sprintf_s(buf, 64, "k_%d", i);

		pairs[i].key = str::make(buf);
		pairs[i].value = i;
	}


	// inserting
	printf("Inserting %d elements...\n", TEST_COUNT);

	//map
	{
		TIMED_BLOCK(insertion.map);

		for (int i = 0; i < TEST_COUNT; i++)
			m.insert(std::make_pair(pairs[i].key.data(), pairs[i].value));
	}

	// unordered map
	{
		TIMED_BLOCK(insertion.unordered_map);

		for (int i = 0; i < TEST_COUNT; i++)
			um.insert(std::make_pair(pairs[i].key.data(), pairs[i].value));
	}

	//my map
	{
		TIMED_BLOCK(insertion.my_map);

		for (int i = 0; i < TEST_COUNT; i++)
			mymap.insert(pairs[i].key.data(), pairs[i].value);
	}

	// finding

	// map
	printf("Finding %d elements...\n", TEST_COUNT);

	{
		TIMED_BLOCK(finding.map);

		for (int i = 0; i < TEST_COUNT; i++)
			volatile int temp = m.find(pairs[i].key.data())->second;
	}

	// unordered map
	{
		TIMED_BLOCK(finding.unordered_map);

		for (int i = 0; i < TEST_COUNT; i++)
			volatile int temp = um.find(pairs[i].key.data())->second;
	}

	//my map
	{
		TIMED_BLOCK(finding.my_map);

		for (int i = 0; i < TEST_COUNT; i++)
			volatile int temp = mymap.find(pairs[i].key.data());
	}

	// finding non existant elements

	// map
	printf("Finding %d non existant elements...\n", TEST_COUNT);

	{
		TIMED_BLOCK(finding_non_existant.map);

		for (int i = 0; i < TEST_COUNT; i++)
			if (m.find("abcedf") == m.end())
				volatile int temp = 0;
	}

	// unordered map
	{
		TIMED_BLOCK(finding_non_existant.unordered_map);

		for (int i = 0; i < TEST_COUNT; i++)
			if (m.find("abcedf") == m.end())
				volatile int temp = 0;
	}

	//my map
	{
		TIMED_BLOCK(finding_non_existant.my_map);

		for (int i = 0; i < TEST_COUNT; i++)
			volatile auto temp = mymap.find("abcdef");
	}

	// erasing
	printf("Erasing %d elements...\n", TEST_COUNT);

	// map
	{
		TIMED_BLOCK(deletion.map);

		for (int i = 0; i < TEST_COUNT; i++)
			m.erase(pairs[i].key.data());
	}

	// unordered map
	{
		TIMED_BLOCK(deletion.unordered_map);

		for (int i = 0; i < TEST_COUNT; i++)
			um.erase(pairs[i].key.data());
	}

	//my map
	{
		TIMED_BLOCK(deletion.my_map);

		for (int i = 0; i < TEST_COUNT; i++)
			mymap.erase(pairs[i].key.data());
	}


	{
		TIMED_BLOCK(insertion.my_map2);

		for (int i = 0; i < TEST_COUNT; i++)
			mymap2.insert(pairs[i].key.data(), pairs[i].value);
	}
	{
		TIMED_BLOCK(finding.my_map2);

		for (int i = 0; i < TEST_COUNT; i++)
			volatile int temp = mymap2.find(pairs[i].key.data());
	}
	{
		TIMED_BLOCK(finding_non_existant.my_map2);

		for (int i = 0; i < TEST_COUNT; i++)
			volatile auto temp = mymap2.find("abcdef");
	}
	{
		TIMED_BLOCK(deletion.my_map2);

		for (int i = 0; i < TEST_COUNT; i++)
			mymap2.erase(pairs[i].key.data());
	}


	printf("\n\n");

	printf("Insertion of %d elements:\n", TEST_COUNT);
	printf("    map:            %llu ms\n", insertion.map);
	printf("    unordered map:  %llu ms\n", insertion.unordered_map);
	printf("    my map:         %llu ms\n", insertion.my_map);
	printf("    my map 2:       %llu ms\n", insertion.my_map2);
	printf("\n\n");

	printf("Finding %d elements:\n", TEST_COUNT);
	printf("    map:            %llu ms\n", finding.map);
	printf("    unordered map:  %llu ms\n", finding.unordered_map);
	printf("    my map:         %llu ms\n", finding.my_map);
	printf("    my map 2:       %llu ms\n", finding.my_map2);
	printf("\n\n");

	printf("Finding %d non existant elements:\n", TEST_COUNT);
	printf("    map:            %llu ms\n", finding_non_existant.map);
	printf("    unordered map:  %llu ms\n", finding_non_existant.unordered_map);
	printf("    my map:         %llu ms\n", finding_non_existant.my_map);
	printf("    my map 2:       %llu ms\n", finding_non_existant.my_map2);
	printf("\n\n");

	printf("Erasing %d elements:\n", TEST_COUNT);
	printf("    map:            %llu ms\n", deletion.map);
	printf("    unordered map:  %llu ms\n", deletion.unordered_map);
	printf("    my map:         %llu ms\n", deletion.my_map);
	printf("    my map 2:       %llu ms\n", deletion.my_map2);
	printf("\n\n");
	 

	printf("counter: %d", mymap.counter);
	LinearStringMap<u64>::free(mymap);
	return 0;
}