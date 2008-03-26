#ifndef ZORBA_STORE_LATCH_H
#define ZORBA_STORE_LATCH_H

#include "common/common.h"

#ifdef WIN32
#include "util/rwlock.h"
#endif

namespace zorba { namespace store {

#ifndef ZORBA_FOR_ONE_THREAD_ONLY

/*******************************************************************************

********************************************************************************/
class Latch
{
public:

  enum Mode { READ, WRITE };

protected:

#ifdef HAVE_PTHREAD_H
  pthread_rwlock_t  theLatch;
#elif WIN32
  rwlock  theLatch;
  bool    rlocked;
  bool    wlocked;
#endif

public:
  Latch();

  ~Latch();
    
  void rlock();
  void wlock();

  void unlock();

 private:
  Latch(const Latch &);
  void operator=(const Latch &);
};


/*******************************************************************************

********************************************************************************/
class AutoLatch
{
private:
  Latch & theLatch;

public:
  AutoLatch(Latch& l, Latch::Mode m) : theLatch(l)
  {
    if (m == Latch::READ)
      theLatch.rlock();
    else
      theLatch.wlock();
  }
    
  ~AutoLatch()
  {
    theLatch.unlock();
  }
};

#endif // ZORBA_FOR_ONE_THREAD_ONLY

} // namespace store
} // namespace zorba


#endif
