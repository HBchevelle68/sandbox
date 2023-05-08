/**
 * @file This is where instructor code should go. This solution
 * is not intended to be perfect or usable for all situations.
 * Merely it should be written well enough to assist future
 * instructors  in teaching as well as students to understand
 * the material that is WITHIN the scope of the class.
 *
 * To prevent any leakage, lets keep to some guidlines:
 * - ALL functions must follow the formate "instructor_<your function>"
 * - ALL functions with the exception of instructor_load and instructor_jump
 * should be static functions. This prevents accidental usage outside
 * their intended bounds
 *
 */
#include <stdio.h>
#include <errno.h>
#include <stdint.h>
#include <elf.h>

uint64_t instructor_load(void *fdata, size_t size)
{

    return 0;
}

int instructor_jump(uint64_t)
{

    return 0;
}