#include  <SFML/Graphics.hpp> 
#include <iostream>
#include <fstream>

#include "Game.h"

int main()
{
    Game g("config.txt");
    g.run();
}


