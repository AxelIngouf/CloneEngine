#pragma once

namespace Core
{
	class  MemoryMappedFile
	{
	public:
		MemoryMappedFile() = delete;
		MemoryMappedFile(const char* file);
		~MemoryMappedFile();

		bool IsValid() const;
		char const* GetData() const { return m_data; }
		size_t GetSize() const { return m_size; }

	private:
		void ManageError();

		void* m_fileHandle;
		void* m_mappingHandle;
		void* m_fileView;

		char const*		m_data = nullptr;
		size_t			m_size = 0;
	};
}