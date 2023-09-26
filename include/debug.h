#ifndef __BI_H_
#define __BI_H_

#include <iostream>

#define ANSI_FG_BLACK   "\33[1;30m"
#define ANSI_FG_RED     "\33[1;31m"
#define ANSI_FG_GREEN   "\33[1;32m"
#define ANSI_FG_YELLOW  "\33[1;33m"
#define ANSI_FG_BLUE    "\33[1;34m"
#define ANSI_NONE       "\33[0m"

#define INFO(content) \
  std::cout << ANSI_FG_BLUE \
  << "[" << __FILE__ << ":" << __LINE__ << " " << __func__ << "] " \
  << content << ANSI_NONE << std::endl

#define ERROR(content) \
  std::cout << ANSI_FG_RED \
  << "[" << __FILE__ << ":" << __LINE__ << " " << __func__ << "] " \
  << content << ANSI_NONE << std::endl

#endif
