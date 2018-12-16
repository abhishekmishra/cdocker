/*
 * minunit.h
 *
 * see http://www.jera.com/techinfo/jtns/jtn002.html
 */

#ifndef TEST_MINUNIT_H_
#define TEST_MINUNIT_H_

/* file: minunit.h */
#define mu_assert(message, test) do { if (!(test)) return message; } while (0)
#define mu_run_test(test) do { char *message = test(); tests_run++; \
                                if (message) return message; } while (0)
extern int tests_run;

#endif /* TEST_MINUNIT_H_ */
