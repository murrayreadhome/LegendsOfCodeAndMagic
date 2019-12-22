// LegendsOfCodeAndMagic.cpp : Defines the entry point for the application.
//

#include "LegendsOfCodeAndMagic.h"
#include "GameController.h"

using namespace std;

int main()
{
    for (int i = 0; i < 1000; i++)
    {
        Player p1, p2;
        GameController gc(i);
        int winner = gc.play(p1, p2);
        cout << winner;
    }
	return 0;
}
