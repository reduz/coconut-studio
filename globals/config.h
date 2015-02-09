//
// C++ Interface: config
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef CONFIG_H
#define CONFIG_H

#include <stddef.h>


#define VERSION_MAJOR 1
#define VERSION_MINOR 4
#define VERSION_REV ".2"


#include <SDL/SDL.h>

#ifndef MIN
#define MIN(m_left,m_right) ( (m_left)<(m_right)?(m_left):(m_right) )
#endif

#ifndef MAX
#define MAX(m_left,m_right) ( (m_left)>(m_right)?(m_left):(m_right) )
#endif


#endif  /* Config_h */
