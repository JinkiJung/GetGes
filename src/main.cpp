// Copyright (C) 2013-2014 Thalmic Labs Inc.
// Distributed under the Myo SDK license agreement. See LICENSE.txt for details.


#include "GestureGetter.h"

int main(int argc, char** argv)
{
    
	GetGes::GestureGetter gg;
	if (gg.init() < 0)
		exit(1);
	
    while (1) {
		if (gg.update() < 0)
			exit(1);
    }
    
	return 0;
}

//*/