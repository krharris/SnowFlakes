#ifndef _GUILDHALL_TYPES_H_
#define _GUILDHALL_TYPES_H_

#include <stdint.h>

namespace guildhall {

typedef int32_t status;

const status STATUS_OK = 0;
const status STATUS_ERROR = -1;
const status STATUS_EXIT = -2;

}
#endif // _GUILDHALL_TYPES_H_
