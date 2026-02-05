#ifndef SHELL_H
#define SHELL_H

#include "projectile.h"

#include <util.h>

namespace battleship{
	class Shell : public Projectile{
		public:
			Shell(Unit*, int, vb01::Vector3, vb01::Quaternion);
			void update();
		private:
			vb01::s64 initTime;
			vb01::Vector3 initDir;
	};
}

#endif
