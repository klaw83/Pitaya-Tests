/**
 * $Id: $
 *
 * @brief Red Pitaya Applications library common module interface
 *
 * @Author Luka Golinar, RedPitaya
 *
 * (c) Red Pitaya  http://www.redpitaya.com
 *
 * This part of code is written in C programming language.
 * Please visit http://en.wikipedia.org/wiki/C_(programming_language)
 * for more details on the language used herein.
 */

 #ifndef UTILS_H_
 #define UTILS_H_


#include <sys/syslog.h>
#include "rp.h"
#include "lcrApp.h"

#define ECHECK_LCR_APP(x) { \
    int retval = (x); \
    if (retval != RP_OK) { \
        ERROR("%s returned \"%s\"", #x, lcrApp_LcrGetError(static_cast<lcr_error_t>(retval))); \
        return retval; \
    } \
}


#define ECHECK_LCR_APP_NO_RET(x) { \
    int retval = (x); \
    if (retval != RP_OK) { \
        ERROR("%s returned \"%s\"", #x, lcrApp_LcrGetError(static_cast<lcr_error_t>(retval))); \
    } \
}


void lcr_getDecimationValue(float frequency,int *dec_val,uint32_t adc_rate);

#endif //UTILS_H_
