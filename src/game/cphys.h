#pragma once

#include "naphy/physbody.h"

struct CPhys {
    PhysBody* body;

    CPhys() {
        body = NULL;
    }
    
    CPhys(PhysBody* pointer_to_the_body) {
        body = pointer_to_the_body;
    }
};