
#ifndef FILESTREAM_HPP
#define FILESTREAM_HPP

#include <iostream>
#include "SFML/Graphics.hpp"
#include "physfs.h"

namespace ce
{
	class FileStream : public sf::InputStream
	{
	public:
		FileStream(const char* filename = nullptr) : file(nullptr)
		{
			if (filename)
				open(filename);
		}
		virtual ~FileStream()
		{
			close();
		}

		bool isOpen() const
		{
			return file != nullptr;
		}

		bool open(const char* filename)
		{
			close();
			file = PHYSFS_openRead(filename);
			return isOpen();
		}

		void close()
        {
			if (isOpen())
				PHYSFS_close(file);
			file = nullptr;
		}

		sf::Int64 read(void* data, const sf::Int64 size) override
		{
			if (!isOpen())
				return -1;

			// PHYSFS_read returns the number of 'objects' read or -1 on error.
			// We assume our objects are single bytes and request to read size
			// amount of them.
			return PHYSFS_readBytes(file, data, static_cast<PHYSFS_uint32>(size));
		}

		sf::Int64 seek(const sf::Int64 position) override
		{
			if (!isOpen())
				return -1;

			// PHYSFS_seek return 0 on error and non zero on success
			if (PHYSFS_seek(file, position))
				return tell();
			else
				return -1;
		}

		sf::Int64 tell() override
		{
			if (!isOpen())
				return -1;

			// PHYSFS_tell returns the offset in bytes or -1 on error just like SFML wants.
			return PHYSFS_tell(file);
		}

		sf::Int64 getSize() override
		{
			if (!isOpen())
				return -1;

			// PHYSFS_fileLength also the returns length of file or -1 on error just like SFML wants.
			return PHYSFS_fileLength(file);
		}

	private:
		PHYSFS_File* file;
	};
}

#endif // FILESTREAM_HPP