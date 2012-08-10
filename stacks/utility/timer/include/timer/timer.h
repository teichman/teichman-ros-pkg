#ifndef HIGH_RES_TIMER_H
#define HIGH_RES_TIMER_H

#include <time.h>
#include <string>
#include <sstream>
#include <cstddef>
#include <iostream>

//! CLOCK_MONOTONIC_RAW will not be adjusted by NTP.
//! See man clock_gettime.
class HighResTimer {
public:
  std::string description_;

  HighResTimer(const std::string& description = "HighResTimer");
  void start();
  void stop();
  void reset(const std::string& description);
  void reset();
  double getMicroseconds() const;
  double getMilliseconds() const;
  double getSeconds() const;
  double getMinutes() const;
  double getHours() const;

  std::string report() const;
  std::string reportMicroseconds() const;
  std::string reportMilliseconds() const;
  std::string reportSeconds() const;
  std::string reportMinutes() const;
  std::string reportHours() const;
  
private:
  double total_us_;
  timespec start_;
  timespec end_;
  bool stopped_;
};

class ScopedTimer
{
public:
  HighResTimer hrt_;
  ScopedTimer(const std::string& description = "ScopedTimer");
  ~ScopedTimer();
};

#endif // HIGH_RES_TIMER_H
