#include "NarakaKarEngine.h"
#include  <memory>

int main()
{
	std::unique_ptr<NarakaKarEngine> engine = std::make_unique<NarakaKarEngine>();
	
	engine->Init();

	do 
	{
		engine->Update();
	} while (!engine->IsEnd());

	return 0;
}