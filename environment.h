#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <vector.h>

namespace sf{
	class Sound;
}

namespace battleship{
	class Environment{
		public:
			static Environment* getSingleton();
			static void explode(vb01::Vector3, int, float, sf::Sound*);
		private:
			Environment(){}
	};
}

#endif
