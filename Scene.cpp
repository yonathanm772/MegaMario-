#include "Scene.h"
#include "GameEngine.h"
#include <iostream>

void Scene::setPaused(bool paused)
{
	m_paused = false;
}

Scene::Scene()
{
}

Scene::Scene(GameEngine* game)
{
	m_game = game;  // Set m_game properly

}

void Scene::doAction(const Action& action)
{	
	std::cout << "===============DOING ACTION================\n";
	sDoAction(action);
}

void Scene::simulate(const size_t frames)
{
}

void Scene::registerAction(int inputKey, const std::string& actionName)
{
	m_actionMap[inputKey] = actionName;
}

size_t Scene::width() const
{
	return m_game->window().getSize().x;
}

size_t Scene::height() const
{
	return m_game->window().getSize().y;
}

size_t Scene::currentFrame() const
{
	return m_currentFrame;
}

bool Scene::hasEnded() const
{
	return m_hasEnded;
}

const ActionMap& Scene::getActionMap() const
{
	return m_actionMap;
}

void Scene::drawLine(const Vec2& p1, const Vec2& p2)
{
	sf::Vertex line[] = { sf::Vector2f(p1.x,p1.y),sf::Vector2f(p2.x,p2.y) };
	m_game->window().draw(line, 2, sf::Lines);
}
