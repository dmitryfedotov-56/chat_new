	#pragma once
	/***********************************************************/
	/*                        Storage Access                   */
	/***********************************************************/

	#include <stdlib.h>

	#define BLOCK		size_t

	#define STORAGE_ERROR		10

	class Storage_Access {
	public:
		virtual BLOCK new_Data(unsigned size, void* src) = 0;

		virtual void del_Data(BLOCK id) = 0;

		virtual void get_Data(BLOCK id, unsigned size, void* dst) = 0;

		virtual void put_Data(BLOCK id, unsigned size, void* src) = 0;

		virtual BLOCK new_Line(const char* src) = 0;

		virtual const char* allocate_Line(BLOCK id) = 0;

		virtual void close_Storage() = 0;

		virtual bool empty_Storage() = 0;

		virtual ~Storage_Access() {};
	};

	Storage_Access* open_Storage();

	/***********************************************************/


