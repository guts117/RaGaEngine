#ifndef SIMPLE_STRING
#define SIMPLE_STRING

#include <cassert>
#include <string>

template<unsigned int size>
struct SimpleString
{
private:
	char buffer[size];
	unsigned int realSize;
public:
	inline SimpleString() noexcept = delete;
	
	inline SimpleString(const char* buf) noexcept
	{
		memset(buffer, '\0', size);
		realSize = strlen(buf);
		assert(size >= realSize, "Increase the size or reduce the size of the incoming char array");
		memcpy(buffer, buf, realSize);
	}
	inline SimpleString(std::string&& buf) noexcept
	{
		memset(buffer, '\0', size);
		realSize = buf.size();
		assert(size >= realSize, "Increase the size or reduce the size of the incoming string");
		memcpy(buffer, &buf[0], realSize);
	}

	inline SimpleString(const SimpleString& buf) noexcept
		: realSize{ buf.realSize }
	{
		assert(size >= realSize, "Increase the size or reduce the size of the SimpleString");
		memset(buffer, '\0', size);
		memcpy(buffer, buf.buffer, realSize);
	}

	inline SimpleString& operator= (const SimpleString& buf) noexcept
	{
		realSize = buf.realSize;
		assert(size >= realSize, "Increase the size or reduce the size of the SimpleString");
		memset(buffer, '\0', size);
		memcpy(buffer, buf.buffer, realSize);
		return *this;
	}

	inline SimpleString(SimpleString&& buf) noexcept
		: realSize{ std::exchange(buf.realSize, 0) }
	{
		assert(size >= realSize, "Increase the size or reduce the size of the SimpleString");
		memset(buffer, '\0', size);
		memcpy(buffer, buf.buffer, realSize);
	}

	inline SimpleString& operator= (SimpleString&& buf) noexcept
	{
		realSize = std::exchange(buf.realSize, 0);
		assert(size >= realSize, "Increase the size or reduce the size of SimpleString");
		memset(buffer, '\0', size);
		memcpy(buffer, buf.buffer, realSize);
		return *this;
	}

	inline void toString(std::string& toStr) const noexcept
	{
		toStr.resize(realSize);
		memcpy(&toStr[0], buffer, realSize);
	}

	inline const char* getBuffer() const noexcept
	{
		return buffer;
	}

	template<class Archive>
	void serialize(Archive& archive)
	{
		auto toStr = string();
		toStr.resize(realSize);
		memcpy(&toStr[0], buffer, realSize);
		archive(toStr);
	}

	inline ~SimpleString() noexcept 
	{
		memset(buffer, '\0', size);
		realSize = 0;
	}
};

#endif