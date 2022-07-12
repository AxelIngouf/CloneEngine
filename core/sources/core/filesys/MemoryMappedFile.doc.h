#pragma once

namespace Core
{
	/**
	* MemoryMappedFile is a class use to seemlessly access file on disk.
	*
	* @author		Francis Blanchet
	* @version		1.0
	* @since		1.0
	*/
	class  MemoryMappedFile
	{
	public:
		MemoryMappedFile() = delete;
		/**
		* Constructor opening the file pointed by the given path and mapping it to the
		* m_fileView pointer for further use.
		*
		* @param file		Path of the source file.
		*/
		MemoryMappedFile(const char* file);
		MemoryMappedFile(const MemoryMappedFile& file) = delete;
		MemoryMappedFile(MemoryMappedFile&& file) = delete;
		~MemoryMappedFile();

		MemoryMappedFile& operator=(const MemoryMappedFile&) = delete;
		MemoryMappedFile& operator=(MemoryMappedFile&&) = delete;

		/**
		* Test if the file was correctly updloaded to memory or not.
		*
		* @return			Whether the GetData() function can be use safely or not.
		*/
		bool IsValid() const;
		/**
		* Get the file content.
		*
		* @return			A char pointer to the file content.
		*/
		char const* GetData() const { return m_data; }
		/**
		* Get the file size.
		*
		* @return			How many character are pointed to by the GetData() pointer.
		*/
		size_t GetSize() const { return m_size; }

	private:
		/**
		* Log error string given by the operating system if an operation fail.
		*/
		void ManageError();

		void* m_fileHandle;
		void* m_mappingHandle;
		void* m_fileView;

		char const* m_data = nullptr;
		size_t			m_size = 0;
	};
}