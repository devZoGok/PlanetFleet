#include "defConfigs.h"

namespace battleship{
		namespace configData{
			int calcSumStaticBinds(int id, bool calcPrev){
					int numBinds = 0;

					if(calcPrev)
						for(int i = 0; i < id; i++)
								numBinds += numStaticBinds[i];
					else
						numBinds = numStaticBinds[id];

					return numBinds;
			}

			int calcSumConfBinds(int id, bool calcPrev){
					int numBinds = 0;

					if(calcPrev)
						for(int i = 0; i < id; i++)
								numBinds += numConfBinds[i];
					else
						numBinds = numConfBinds[id];

					return numBinds;
			}

			int calcSumBinds(int id, bool calcPrev){
					return calcSumStaticBinds(id, calcPrev) + calcSumConfBinds(id, calcPrev);
			}
		}
}
