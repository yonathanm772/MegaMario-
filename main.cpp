#include  <SFML/Graphics.hpp> 
#include <iostream>
#include <fstream>
#include "GameEngine.h"

#include "Profiler.hpp"

int main()
{
    PROFILE_FUNCTION();
    GameEngine g("assets.txt");
    g.run();
}


