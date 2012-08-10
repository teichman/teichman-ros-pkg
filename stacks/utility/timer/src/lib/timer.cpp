#include <timer/timer.h>

#define HRTCLOCK CLOCK_MONOTONIC_RAW

HighResTimer::HighResTimer(const std::string& description) :
  description_(description),
  total_us_(0),
  stopped_(true)
{
}

void HighResTimer::start()
{
  clock_gettime(HRTCLOCK, &start_);
  stopped_ = false;
}

void HighResTimer::stop()
{
  clock_gettime(HRTCLOCK, &end_);
  total_us_ += 1e6 * (end_.tv_sec - start_.tv_sec) + 1e-3 * (end_.tv_nsec - start_.tv_nsec);
  stopped_ = true;
}

void HighResTimer::reset(const std::string& description)
{
  description_ = description;
  total_us_ = 0;
  stopped_ = true;
}

void HighResTimer::reset()
{
  total_us_ = 0;
  stopped_ = true;
}

double HighResTimer::getMicroseconds() const
{
  if(stopped_)
    return total_us_;
  else { 
    timespec curr;
    clock_gettime(HRTCLOCK, &curr);
    return total_us_ + 1e6 * (curr.tv_sec - start_.tv_sec) + 1e-3 * (curr.tv_nsec - start_.tv_nsec);
  }
}

double HighResTimer::getMilliseconds() const
{
  return getMicroseconds() / 1000.;
}

double HighResTimer::getSeconds() const
{
  return getMilliseconds() / 1000.;
}

double HighResTimer::getMinutes() const
{
  return getSeconds() / 60.;
}

double HighResTimer::getHours() const
{
  return getMinutes() / 60.;
}

std::string HighResTimer::reportMicroseconds() const
{
  std::ostringstream oss; oss << description_ << ": " << getMicroseconds() << " microseconds.";
  return oss.str();
}

std::string HighResTimer::reportMilliseconds() const
{
  std::ostringstream oss; oss << description_ << ": " << getMilliseconds() << " milliseconds.";
  return oss.str();
}

std::string HighResTimer::reportSeconds() const
{
  std::ostringstream oss; oss << description_ << ": " << getSeconds() << " seconds.";
  return oss.str();
}

std::string HighResTimer::reportMinutes() const
{
  std::ostringstream oss; oss << description_ << ": " << getMinutes() << " minutes.";
  return oss.str();
}

std::string HighResTimer::reportHours() const
{
  std::ostringstream oss; oss << description_ << ": " << getHours() << " hours.";
  return oss.str();
}

std::string HighResTimer::report() const
{
  double val = getMicroseconds();
  if(val <= 1000.0)
    return reportMicroseconds();

  val /= 1000.0;
  if(val <= 1000.0 && val >= 1.0)
    return reportMilliseconds();

  val /= 1000.0;
  if(val <= 60.0 && val >= 1.0)
    return reportSeconds();
  
  val /= 60.0;
  if(val <= 60.0 && val >= 1.0)
    return reportMinutes();
  
  val /= 60.0;
  return reportHours();
}

ScopedTimer::ScopedTimer(const std::string& description) :
  hrt_(description)
{
  hrt_.start();
}

ScopedTimer::~ScopedTimer()
{
  hrt_.stop();
  std::cout << hrt_.report() << std::endl;
}


