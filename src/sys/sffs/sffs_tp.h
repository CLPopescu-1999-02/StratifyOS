/*
 * sffs_tp.h
 *
 *  Created on: Apr 17, 2012
 *      Author: tgil
 */

#ifndef CL_TP_H_
#define CL_TP_H_

#ifdef CL_TEST

void sffs_tp_setfailroutine(void (*routine)(void)); //function called when failing
int sffs_tp(const char * file, int line, const char * func, float failrate, const char * desc); //this is a test point
int sffs_tp_createreport(const char * name);
int sffs_getcount(const char * desc);


#define CL_TP(failrate) sffs_tp(__FILE__, __LINE__, __func__, failrate, NULL)
#define CL_TP_DESC(failrate, desc) sffs_tp(__FILE__, __LINE__, __func__, failrate, desc)

#else
#define CL_TP(failrate)
#define CL_TP_DESC(failrate, desc)
#endif

#define NEVER_FAIL

#ifdef NEVER_FAIL
#define CL_PROB_COMMON (0.0)
#define CL_PROB_RARE (0.0)
#define CL_PROB_IMPROBABLE (0.0)
#define CL_PROB_IMPOSSIBLE (0.0)
#else
#define CL_PROB_COMMON (0.005)
#define CL_PROB_RARE (0.00001)
#define CL_PROB_IMPROBABLE (0.000001)
#define CL_PROB_IMPOSSIBLE (0.0)
#endif


#endif /* CL_TP_H_ */
