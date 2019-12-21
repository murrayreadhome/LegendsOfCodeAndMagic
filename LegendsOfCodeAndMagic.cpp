// LegendsOfCodeAndMagic.cpp : Defines the entry point for the application.
//

#include "LegendsOfCodeAndMagic.h"
#include "GameController.h"

using namespace std;

int main()
{
    Player p1, p2;
    GameController gc(1);
    gc.play(p1, p2);
	return 0;
}
