#ifndef __COLORS_H__
#define __COLORS_H__

#define ESC string("\033[")

#define DEFAULT (ESC+"0m").c_str()
#define RED (ESC+"31m").c_str()
#define GREEN (ESC+"32m").c_str()
#define YELLOW (ESC+"33m").c_str()
#define BLUE (ESC+"34m").c_str()
#define PURPLE (ESC+"35m").c_str()
#define CYAN (ESC+"36m").c_str()


#endif
