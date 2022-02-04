#pragma once

#include "cphys.h"
#include "utility/input.h"

struct CJumpNRun {
    CJumpNRun() {};
    void jump_n_run(CPhys* phys, const Input* input);
};