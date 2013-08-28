#include "Global.hpp"

int main(int argc, char *argv[]) {
    InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

/*! PLANS:
 * futures!
 *
 */

/*! Features:
 * fast
 * asynchronous
 * thread-safe
 * full tested
 * minimum dependencies
 * convenient
 */

/*!
 * 1. raw callbacks
 * 2. futures (async).  get - block.
 * 3. deferred (async). addCallback - attach own callbacks.
 *                      then - add callback to the pipeline
 */
