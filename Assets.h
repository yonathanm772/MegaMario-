#pragma once
#pragma once
#include "Animation.h"

#include <map> 
typedef std::map<std::string, sf::Texture> TexturesMap;
typedef std::map<std::string, Animation> AnimationsMap;
//typedef std::map<std::string, sf::Sound> SoundsMap;
typedef std::map<std::string, sf::Font> FontsMap;

class GameEngine;

class Assets
{
	TexturesMap m_texturesMap;
	AnimationsMap m_animationsMap;
	FontsMap m_fontsMap;

public:
	Assets();
	~Assets();
	void addTexture(std::string& name, std::string& path, bool wantRepeated = false);
	void addAnimation(std::string& name, Animation& animation);
	//void addSound(std::string &name, std::string &path);
	void addFont(std::string& name, std::string& path);

	const sf::Texture& getTexture(std::string name) const;
	const Animation& getAnimation(std::string name) const;
	//sf::Sound& getTexture(std::string& name);
	const sf::Font& getFont(std::string name) const;
	void loadFromFile(const std::string& path);
};

