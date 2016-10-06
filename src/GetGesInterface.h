// interface class for integrating with unity3D

#include "GestureGetter.h"


using namespace GetGes;

GestureGetter gg;

extern "C" __declspec(dllexport) int init() {
	return gg.init();
}

extern "C" __declspec(dllexport) int test(){
	return gg.test();
}
