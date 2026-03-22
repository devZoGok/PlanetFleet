#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <vector>

#include "fxManager.h"

namespace battleship
{
	class Environment
	{
	public:
		enum class Detonation
		{
			EXPLOSION,
			EMP,
			CRYO
		};

		static Environment *getSingleton();
		static void explode(FxManager::Fx *, Detonation, vb01::Vector3, int = 0, float = 0);

	private:
		Environment() {}
	};
}

#endif
