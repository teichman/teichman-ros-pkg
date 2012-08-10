#include <timer/timer.h>
#include <timer/timer.h>
#include <gtest/gtest.h>

using namespace std;

TEST(HighResTimer, HighResTimer)
{
  timespec t;
  t.tv_sec = 0;
  t.tv_nsec = 345098;
  
  HighResTimer hrt;
  hrt.start();
  nanosleep(&t, NULL);
  hrt.stop();
  cout << hrt.report() << endl;
}

TEST(ScopedTimer, ScopedTimer)
{
  HighResTimer hrt;
  timespec t;
  t.tv_sec = 0;
  t.tv_nsec = 1e6;

  {
    ScopedTimer st;
    hrt.start();
    nanosleep(&t, NULL);
  }
  hrt.stop();    
  cout << hrt.report() << endl;
}

TEST(HighResTimer, AllocationTime)
{
  int num = 640 * 480;
  int reps = 100;
  HighResTimer hrt;
  hrt.start();
  for(int i = 0; i < reps; ++i) {
    double data[num];
    data[13] = 1.13;  // Make sure that the compiler doesn't optimize this out?
  }
  hrt.stop();
  cout << "Time to allocate " << num << " doubles on the stack: " << hrt.getMicroseconds() / (double)reps << " us." << endl;

  hrt.reset();
  hrt.start();
  vector<double*> vd(reps);
  for(int i = 0; i < reps; ++i) {
    vd[i] = new double[num];
    vd[i][13] = 1.14;
  }
  hrt.stop();
  cout << "Time to allocate " << num << " doubles on the heap: " << hrt.getMicroseconds() / (double)reps << " us." << endl;

  for(int i = 0; i < reps; ++i)
    delete[] vd[i];
}

int main(int argc, char** argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
