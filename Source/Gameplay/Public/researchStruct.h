#ifndef RESEARCH_STRUCT_H
#define RESEARCH_STRUCT_H

#include "structure.h"

namespace vb01{
	class Node;
}

namespace battleship{
	class ResearchStruct : public Structure{
		public:
			ResearchStruct(Player*, int, vb01::Vector3, vb01::Quaternion, int = 0, Unit::State = Unit::State::STAND_GROUND);
			~ResearchStruct();
			void update();
			inline void appendToQueue(int tid){researchQueue.push_back(tid);}
			inline std::vector<int> getQueue(){return researchQueue;}
		private:
			vb01::s64 lastUpdateTime = 0;
			int researchCost, generationRate, generationSpeed, researchStatus = 0;
			std::vector<int> researchQueue;
			vb01::Node *researchStatusForeground = nullptr, *researchStatusBackground = nullptr;

			bool canUpdateResearch(){return vb01::getTime() - lastUpdateTime > generationRate;}
			void generateResearch();
	};
}

#endif
