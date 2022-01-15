#pragma once

#include "naphy/physbody.h"

enum CPhysLayer{ // Use for PhysBody::layer.
	LAYER_DRIFTER,
	LAYER_SOLID,
	LAYER_PIT,
};

struct CPhys {
    PhysBody* body;

    CPhys() {
        body = NULL;
    }
    
    CPhys(PhysBody* pointer_to_the_body) {
        body = pointer_to_the_body;
    }
};