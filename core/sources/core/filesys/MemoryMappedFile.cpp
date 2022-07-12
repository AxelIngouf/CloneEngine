#include "MemoryMappedFile.h"

#include <filesystem>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "core/CLog.h"

namespace Core
{
	MemoryMappedFile::MemoryMappedFile(const char* path) :
		m_fileHandle(INVALID_HANDLE_VALUE),
		m_mappingHandle(NULL),
		m_fileView(NULL)
	{
		m_fileHandle = CreateFileA(path,
			GENERIC_READ,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
			NULL);
		if (m_fileHandle == INVALID_HANDLE_VALUE)
		{
			ManageError();
		}

		m_mappingHandle = CreateFileMapping(m_fileHandle,
			NULL,
			PAGE_READONLY,
			NULL,
			NULL,
			NULL);
		if (m_mappingHandle == NULL)
		{
			ManageError();
		}

		m_fileView = MapViewOfFile(m_mappingHandle,
			FILE_MAP_READ,
			NULL,
			NULL,
			0);
		if (m_fileView == NULL)
		{
			ManageError();
		}

		LARGE_INTEGER size;
		size.QuadPart = 0;
		GetFileSizeEx(m_fileHandle, &size);

		m_data = (char const*)m_fileView;
		m_size = (size_t)size.QuadPart;
	}

	MemoryMappedFile::~MemoryMappedFile()
	{
		if (m_fileView != NULL)
			UnmapViewOfFile(m_fileView);
		if (m_mappingHandle != NULL)
			CloseHandle(m_mappingHandle);
		if (m_fileHandle != INVALID_HANDLE_VALUE)
			CloseHandle(m_fileHandle);
	}

	bool Core::MemoryMappedFile::IsValid() const
	{
		return m_fileView != NULL;
	}

	void MemoryMappedFile::ManageError()
	{
		char buf[256];
		FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			buf, (sizeof(buf) / sizeof(wchar_t)), NULL);
		LOG(LOG_ERROR, buf); // tolog: use channel for in/out
	}
}
