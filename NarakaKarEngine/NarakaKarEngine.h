#ifndef NARAKA_KAR_ENGINE
#define NARAKA_KAR_ENGINE

#include <memory>

class NarakaKarEngine 
{
public:
	explicit NarakaKarEngine();

	void Init();

	void Update();
	
	bool IsEnd();

	~NarakaKarEngine();

	NarakaKarEngine(NarakaKarEngine&& rhs)						= delete;
	NarakaKarEngine& operator=(NarakaKarEngine&& rhs)			= delete;

	NarakaKarEngine(const NarakaKarEngine& rhs)					= delete;
	NarakaKarEngine& operator=(const NarakaKarEngine& rhs)		= delete;
private:
	struct Impl;

	const Impl* Pimpl() const					{ return m_Pimpl.get(); }
	Impl* Pimpl()								{ return m_Pimpl.get(); }

	std::unique_ptr<Impl> m_Pimpl				= std::make_unique<Impl>();
};
#endif