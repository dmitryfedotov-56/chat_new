	#pragma once
	/***********************************************************/

	const char* allocate_Key(size_t n);

	enum class Status {
		vacant,
		engaged,
		deleted
	};

	struct Pair
	{
		size_t link = SIZE_MAX;
		Status status = Status::vacant;
	};

	/***********************************************************/

	typedef Pair Ptable[8];	// debugging

	class Hash_Table
	{
	public:
		unsigned table_Size();
		unsigned table_Cntr();
		static unsigned memory_Size(unsigned size);
		void* table_Pointer();
		size_t find_Link(const char* key);
		void add_Link(size_t link, const char* key);
		size_t init_Link(unsigned* nptr);
		size_t next_Link(unsigned* nptr);
		void set_Data(unsigned _size, unsigned _cntr, void* _table);

		Hash_Table();
		~Hash_Table();

	private:
		void resize();
		unsigned hash(const char* key);
		unsigned probe(unsigned index, unsigned offset);
		Pair* table;
		unsigned size = 0;
		unsigned cntr = 0;
		double alpha;
		Ptable* ptable;
	};

	/***********************************************************/



