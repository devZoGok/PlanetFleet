#ifndef BUILDABLE_UNIT_H
#define BUILDABLE_UNIT_H

namespace battleship{
	struct BuildableUnit{
		int id;
		bool buildable;

		BuildableUnit(int i, bool build) : id(i), buildable(build){}
	};
}

#endif
