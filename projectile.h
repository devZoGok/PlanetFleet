#ifndef PROJECTILE_H
#define PROJECTILE_H

#include <SFML/Audio.hpp>

#include "gameObject.h"

#include <algorithm>

namespace vb01{
	class Node;
}

namespace battleship {
	enum class ProjectileClass{SHELL, CRUISE_MISSILE, TORPEDO};
	class Unit;

    class Projectile : public GameObject{
    public:
        Projectile(Unit*, int, vb01::Vector3, vb01::Quaternion);
        virtual ~Projectile();
        virtual void update();
        virtual void debug();
	private:
		void initProperties();
		void initSound();
    protected:
		virtual void reinit();
		virtual void checkUnitCollision();
		virtual void checkSurfaceCollision();
		void checkCollision();

        float speed, rayLength, explosionRadius, rotAngle;
        int directHitDamage, explosionDamage;
		vb01::Vector3 initPos;
		Unit *unit = nullptr;
        sf::SoundBuffer *shotSfxBuffer, *explosionSfxBuffer;
        sf::Sound *shotSfx = nullptr, *explosionSfx = nullptr;
    };
}

#endif
