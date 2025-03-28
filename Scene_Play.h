#pragma once
//#include "Vec2.h"
#include <memory>
#include <map>
//#include "Entity.h"
#include "Scene.h"

#include "EntityManager.h"

class Scene_Play : public Scene
{
	struct PlayerConfig {
		float X, Y, CX, CY, SPEED, MAXSPEED, JUMP, GRAVITY;
		std::string WEAPON;
	};

	struct EnemyConfig {
		float X, Y, CX, CY, SPEED, MAXSPEED, GRAVITY;
	};



protected:

	std::shared_ptr<Entity>		m_player;
	std::string					m_levelPath;
	PlayerConfig				m_playerConfig;
	EnemyConfig					m_enemyConfig;
	bool						m_drawTextures = true;
	bool						m_drawCollision = false;
	bool						m_drawGrid = false;
	const Vec2					m_gridSize = { 64, 64 };
	sf::Text					m_gridText;

	void init(const std::string& levelPath);
	Vec2 gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity);

	void loadLevel(const std::string& filename);

	void spawnPlayer();

	void spawnEnemy();

	void SpawnBullet(std::shared_ptr<Entity> entity);

	void sMovement();

	void sLifespan();

	void sCollision();

	void sDoAction(const Action& action);

	void sAnimation();

	void onEnd();

	void sRender();
public:
	Scene_Play();
	Scene_Play(GameEngine* gameEngine, const std::string& levelPath);
	void update();

};

