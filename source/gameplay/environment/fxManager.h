#ifndef FX_MANAGER_H
#define FX_MANAGER_H

#include <util.h>

#include <solUtil.h>

namespace vb01{
	class Node;
}

namespace battleship{
	class FxManager{
		public:
			struct Fx {
				struct Component{
					vb01::s64 duration, initTime = 0, offsetTime = 0;
					vb01::Vector3 pos = vb01::Vector3::VEC_ZERO;
					bool vfx, active = false;
					void *comp = nullptr;

					Component(void*, bool, vb01::s64, vb01::Vector3 = vb01::Vector3::VEC_ZERO, vb01::s64 = 0);
				};

				bool reuse;
				std::vector<Component> components;
			
				Fx(std::vector<Component>, bool = false);
				void toggleComponents(bool);
			};

			static FxManager* getSingleton();
			FxManager::Fx* initFx(sol::table, vb01::Node*, bool, vb01::Vector3 = vb01::Vector3::VEC_ZERO);
			void update();
			void removeFx(Fx*);
			inline void addFx(Fx *fx){fxs.push_back(fx);}
		private:
			FxManager(){}
			void destroyFxComponent(int, int);
			std::vector<Fx*> fxs;
	};
}

#endif
