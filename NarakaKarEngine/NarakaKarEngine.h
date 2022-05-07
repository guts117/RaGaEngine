#ifndef NARAKA_KAR_ENGINE
#define NARAKA_KAR_ENGINE

#include <memory>

class NarakaKarEngine 
{
public:
	NarakaKarEngine();

	void Init();

	void Update();
	
	bool IsEnd();

	~NarakaKarEngine();
private:
	struct Impl;
	std::unique_ptr<Impl> pimpl = std::make_unique<Impl>();
};
#endif