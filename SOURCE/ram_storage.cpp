
	/***********************************************************/
	/*                         RAM Storage                     */
	/***********************************************************/

	#include <string.h>
	#include "storage_access.h"

	/***********************************************************/

	class RAM_Storage : public Storage_Access {
	public:
		virtual BLOCK new_Data(unsigned size, void* src);

		virtual void del_Data(BLOCK id);

		virtual void get_Data(BLOCK id, unsigned size, void* dst);

		virtual void put_Data(BLOCK id, unsigned size, void* src);

		virtual BLOCK new_Line(const char* src);

		virtual const char* allocate_Line(BLOCK id);

		virtual void open_Storage();

		virtual void close_Storage();

		virtual bool empty_Storage();
	};

	/***********************************************************/

	BLOCK RAM_Storage::new_Data(unsigned size, void* src) 
	{
		void* ptr = malloc(size);
		if (ptr == nullptr)throw STORAGE_ERROR;
		if(ptr != nullptr) memcpy(ptr, src, size);
		return (size_t)ptr;
	};

	void RAM_Storage::del_Data(BLOCK id) { free((void*)id); };

	/***********************************************************/

	void RAM_Storage::get_Data(BLOCK id, unsigned size, void* dst)
	{ 
		memcpy(dst, (void*)id, size); 
	};

	/***********************************************************/

	void RAM_Storage::put_Data(BLOCK id, unsigned size, void* src)
	{	
		memcpy((void*)id, src, size); 
	};

	/***********************************************************/

	BLOCK RAM_Storage::new_Line(const char* src) 
	{
		return new_Data(strlen(src) + 1, (void*)src);
	};

	/***********************************************************/

	const char* RAM_Storage::allocate_Line(BLOCK id) 
	{
		unsigned size = strlen((char*)id) + 1;
		void* ptr = (char*)malloc(size);
		if (ptr == nullptr)throw STORAGE_ERROR;
		if (ptr != nullptr)memcpy(ptr, (void*)id, size);
		return (const char*)ptr;
	};

	/***********************************************************/

	RAM_Storage ram_storage;

	Storage_Access* open_Storage() { return &ram_storage; };

	void RAM_Storage::open_Storage() {};

	void RAM_Storage::close_Storage() {};

	bool RAM_Storage::empty_Storage() { return true; };

	/***********************************************************/
