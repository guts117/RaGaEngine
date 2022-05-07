#include "NarakaKarEngine.h"
int main()
{
	NarakaKarEngine NarakaKarEngine;
	NarakaKarEngine.Init();

	do 
	{
		NarakaKarEngine.Update();		
	} while (!NarakaKarEngine.mainWindow->getShouldClose());

	return 0;
}