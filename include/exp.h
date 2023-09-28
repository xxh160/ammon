#ifndef __EXP_H_
#define __EXP_H_

#include <exception>

class InvalidConsumerExp : public std::exception {
  const char *what() const noexcept override;
};

class NoMoreConsumerExp : public std::exception {
  const char *what() const noexcept override;
};

#endif
