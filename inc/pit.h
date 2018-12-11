/*
 * pit.h
 *
 *  Created on: 16. 11. 2018
 *      Author: root
 */

#ifndef INC_PIT_H_
#define INC_PIT_H_


#include <sys/time.h>

namespace pit
{

	void SetPIT(__time_t _sec, __suseconds_t _usec);

}


#endif /* INC_PIT_H_ */
