#include "Assets.h"
#include <iostream>
#include <fstream>
#include <iostream>
#include <fstream>


Assets::Assets()
{
}

Assets::~Assets()
{
}


void Assets::addTexture(std::string& name, std::string& path, bool wantRepeated)
{
    sf::Texture texture;

    if (texture.loadFromFile(path))
    {
        texture.setRepeated(wantRepeated);
        this->m_texturesMap[name] = texture;
    }
}

void Assets::addAnimation(std::string& name, Animation& animation)
{

}

void Assets::addFont(std::string& name, std::string& path)
{
    sf::Font font;

    if (font.loadFromFile(path))
    {
        this->m_fontsMap[name] = font;
    }
}

const sf::Texture& Assets::getTexture(std::string name) const
{
    return m_texturesMap.at(name);
}

const Animation& Assets::getAnimation(std::string name) const
{
    return m_animationsMap.at(name);
}

const sf::Font& Assets::getFont(std::string name) const
{
    return m_fontsMap.at(name);
}

void Assets::loadFromFile(const std::string& path)
{
    // Reads in config file here
    
    std::cout << "Loading From File" << "\n";

    std::ifstream fin(path);
    if (!fin) {
        std::cerr << "Error opening file!" << std::endl;
    }
    std::string assetType, asset, assetPath, animationTex;
    size_t animationFrameCount, animationSpeed;
    while (!fin.eof())
    {
        fin >> assetType;
        if (assetType == "Texture") {
            fin >> asset >> assetPath;
            
            sf::Texture texture;
            if (!texture.loadFromFile(assetPath))
            {
                std::cout << "ERROR LOADING TEXTURE" << std::endl;
            }
            m_texturesMap.insert({ asset,texture });
            std::cout << "Loaded Textures:" << assetPath << "\n";
            
        }
        else if (assetType == "Animation") {
            fin >> asset >> animationTex >> animationFrameCount >> animationSpeed;
            Animation animation(asset, m_texturesMap.at(animationTex), animationFrameCount, animationSpeed);
            m_animationsMap.insert({ asset,animation });

            //std::cout << "Loaded Animation:" << assetPath << "\n";
        }
        else if (assetType == "Font") {
            fin >> asset >> assetPath;

            sf::Font font;
            if (!font.loadFromFile(assetPath))
            {
                std::cout << "ERROR LOADING TEXTURE" << std::endl;
            }
            m_fontsMap.insert({ asset,font });
            std::cout << "Loaded Font:" << assetPath << "\n";

 
        }
    }
    fin.close();
    std::cout << "Finished Loading From File" << "\n";
}

