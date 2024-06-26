#pragma once

#include <QReadWriteLock>

class FlashLocker
{
  QReadWriteLock* lock;

public:
  enum Mode
  {
    Read,
    Write
  };
  bool has_locked;
  FlashLocker(QReadWriteLock* lock, Mode);
  bool hasLocked();
  virtual ~FlashLocker();
};
