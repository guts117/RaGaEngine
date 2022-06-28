#ifndef NARAKA_KAR_ENGINE
#define NARAKA_KAR_ENGINE

#include "pch.h"
//#include "spimpl.h"

class NarakaKarEngine 
{
public:
	explicit NarakaKarEngine();

	void Init();

	void Update();
	
	bool IsEnd();

	~NarakaKarEngine();

	NarakaKarEngine(NarakaKarEngine&& rhs) noexcept						= delete;
	NarakaKarEngine& operator=(NarakaKarEngine&& rhs) noexcept			= delete;

	NarakaKarEngine(const NarakaKarEngine& rhs) noexcept				= delete;
	NarakaKarEngine& operator=(const NarakaKarEngine& rhs) noexcept		= delete;
private:
	struct Impl;

	const Impl* Pimpl() const					{ return m_pImpl.get(); }
	Impl* Pimpl()								{ return m_pImpl.get(); }

	std::unique_ptr<Impl> m_pImpl				= std::make_unique<Impl>();
};
#endif