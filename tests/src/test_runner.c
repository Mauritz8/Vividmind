#include <CUnit/Basic.h>

#include "test_knight_moves.h"
#include "test_bishop_moves.h"
#include "test_rook_moves.h"
#include "test_move_generator.h"


int main(void) {
    if (CUE_SUCCESS != CU_initialize_registry()) {
        return CU_get_error(); 
    }

    if (0 != knight_moves_tests()) {
        return CU_get_error();
    }
    if (0 != bishop_moves_tests()) {
        return CU_get_error();
    }
    if (0 != rook_moves_tests()) {
        return CU_get_error();
    }
    if (0 != move_generator_tests()) {
        return CU_get_error();
    }


    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();

    return CU_get_error();
}
