#ifndef LOADING_APP_STATE_H
#define LOADING_APP_STATE_H

#include <abstractAppState.h>
#include <util.h>

namespace battleship{
	class LoadingAppState : public gameBase::AbstractAppState{
		public:
			LoadingAppState(gameBase::AbstractAppState*, std::string);
			~LoadingAppState(){}
			void onAttached(){}
			void onDettached();
			void update();
			inline void setLoadableAssets(std::vector<std::string> ls){
				this->loadableAssets = ls;
				initNumAssets = ls.size();
			}
		private:
			int initNumAssets;
			std::vector<std::string> loadableAssets;
			gameBase::AbstractAppState *newState = nullptr;
			std::string newScreen = "";
			vb01::s64 lastUpdateTime = 0;
	};
}

#endif
