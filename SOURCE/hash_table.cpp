
	/***********************************************************/

	#include <string.h>
	#include <stdlib.h>
	#include <math.h>
	#include <cstdint>
	#include "hash_table.h"

	#define INIT_SIZE	2

	/***********************************************************/

	unsigned Hash_Table::table_Size() { return size; };

	unsigned Hash_Table::table_Cntr() { return cntr; };

	unsigned Hash_Table::memory_Size(unsigned size) 
	{ 
		return size * sizeof(Pair); 
	};

	void* Hash_Table::table_Pointer() { return (void*)table; };

	/***********************************************************/

	unsigned  Hash_Table::hash(const char* key) 
	{
		size_t sum = 0;
		for (unsigned i = 0; i < strlen(key); i++)sum += key[i];
		return int(size * (alpha * sum - int(alpha * sum)));
	};

	unsigned Hash_Table::probe(unsigned index, unsigned offset) 
	{
		return (index + offset) % size;
	};

	/***********************************************************/

	size_t Hash_Table::find_Link(const char* key)
	{
		unsigned n;
		const char* pkey;
		size_t link;
		unsigned index = hash(key);
		for (unsigned i = 0; i < size; i++)
		{
			n = probe(index, i);
			if (table[n].status == Status::vacant) return SIZE_MAX;
			if (table[n].status == Status::engaged) 
			{
				link = table[n].link;
				pkey = allocate_Key(link);
				if (strcmp(pkey, key) == 0) 
				{ 
					free((void*)pkey);
					return link; 
				};
				free((void*)pkey);
			};
		};
		return SIZE_MAX;
	};

	/***********************************************************/

	void Hash_Table::add_Link(size_t link, const char* key)
	{
		unsigned n;
		unsigned index = hash(key);
		for (unsigned i = 0; i < size; i++) 
		{
			n = probe(index, i);
			if (table[n].status == Status::vacant) 
			{
				table[n].link = link;
				table[n].status = Status::engaged;
				cntr++;
				return;
			};
		};
		resize();
		add_Link(link, key);
	};

	/***********************************************************/

	size_t Hash_Table::init_Link(unsigned* nptr)
	{
		*nptr = 0;
		return next_Link(nptr);
	};

	/***********************************************************/

	size_t Hash_Table::next_Link(unsigned* nptr)
	{
		if (*nptr == size)return SIZE_MAX;
		for (unsigned i = *nptr; i < size; i++)
		{
			if (table[i].status == Status::engaged)
			{
				*nptr = i + 1;
				return table[i].link;
			};
		};
		return SIZE_MAX;
	};

	/***********************************************************/

	void Hash_Table::set_Data(unsigned _size, unsigned _cntr, void* _table)
	{
		size = _size;
		cntr = _cntr;
		delete[] table;
		table = (Pair*)_table;
		ptable = (Ptable*)table;
	};

	/***********************************************************/

	Hash_Table::Hash_Table()
	{
		size = INIT_SIZE;
		cntr = 0;

		alpha = (sqrt(5) - 1) / 2;

		table = new Pair[size];
		for (unsigned i = 0; i < size; i++) 
		{ 
			table[i].link = SIZE_MAX; 
			table[i].status = Status::vacant;
		};
		ptable = (Ptable*)table;
	};

	Hash_Table::~Hash_Table()
	{
		delete[] table;
	};

	/***********************************************************/

	void Hash_Table::resize()
	{
		size_t link;
		const char* pkey;
		Pair pair;
		Pair* t;
		
		ptable = (Ptable*)table;

		// save old table
		t = new Pair[size];
		for (unsigned i = 0; i < size; i++)t[i] = table[i];

		// create a new table
		unsigned init = size;
		size = 2 * size;
		table = new Pair[size];
		ptable = (Ptable*)table;

		// add all the links to the new table
		for (unsigned i = 0; i < init; i++) 
		{
			pair = t[i];
			if (pair.status == Status::engaged) 
			{
				link = pair.link;
				pkey = allocate_Key(link);
				add_Link(link, pkey);
				free((void*)pkey);
			};
		};

		delete[] t;
		ptable = (Ptable*)table;
	};

	/***********************************************************/


