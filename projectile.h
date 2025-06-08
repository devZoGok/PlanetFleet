#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "gameObject.h"
#include "fxManager.h"

#include <SFML/Audio.hpp>

namespace vb01{
	class Node;
}

namespace battleship {
	class Unit;

	enum class ProjectileClass{SHELL, CRUISE_MISSILE, MISSILE, TORPEDO};

    class Projectile : public GameObject{
    public:
        Projectile(Unit*, int, vb01::Vector3, vb01::Quaternion);
        virtual ~Projectile();
        virtual void update();
        virtual void debug();
	private:
		void initProperties();
		void detonate(Unit *u = nullptr);
    protected:
		virtual void reinit();
		virtual void checkUnitCollision();
		virtual void checkSurfaceCollision();
		void checkCollision();

		vb01::Vector3 initPos;
		Unit *unit = nullptr;
		FxManager::Fx *explosionFx = nullptr;
        float speed, rayLength, explosionRadius, rotAngle;
        int directHitDamage, explosionDamage;
        sf::SoundBuffer *shotSfxBuffer, *explosionSfxBuffer;
        sf::Sound *shotSfx = nullptr, *explosionSfx = nullptr;
    };
}

#endif
