#pragma once
#ifndef UTIL_BATTLESHIP_H
#define UTIL_BATTLESHIP_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>

#include <vector.h>

namespace battleship{
    typedef unsigned char u8;
    typedef unsigned short u16;
    typedef unsigned int u32;
    typedef unsigned long long u64;
    typedef char s8;
    typedef short s16;
    typedef int s32;
    typedef long long s64;

	enum AppStateType{GUI_STATE, IN_GAME_STATE, ACTIVE_STATE, MAP_EDITOR, LOADING_STATE};
    
    class GuiAppState;
    class LoadingAppState;
    
	void handleLoadingGui(LoadingAppState*);
	vb01::Vector3 getVecToPlane(vb01::Vector3, vb01::Vector3, vb01::Vector3);
	std::vector<std::string> readDir(std::string, bool);
	vb01::Vector2 spaceToScreen(vb01::Vector3);
	vb01::Vector3 spaceToScreen3d(vb01::Vector3);
	vb01::Vector3 screenToSpace(vb01::Vector2);
}

#endif
