/*
 *	Copyright 2006-2007 FLWOR Foundation.
 *  Author: David Graf (david.graf@28msec.com)
 *
 */

#ifndef XQP_SIMPLE_TEMP_SEQ_H
#define XQP_SIMPLE_TEMP_SEQ_H

#include <vector>

#include "store/api/temp_seq.h"
#include "runtime/base/iterator.h"

namespace xqp
{

/**
 * Very simple implementation of Temp Sequence. It saves the resulting items
 * of an iterator eager in a vector.
 */
typedef rchandle<class SimpleTempSeq> SimpleTempSeq_t;

class SimpleTempSeq : public TempSeq
{
private:
  std::vector<Item_t> theItems;

public:
  SimpleTempSeq() { }

  SimpleTempSeq(const std::vector<Item_t>& items) : theItems(items) {}

  SimpleTempSeq(Iterator_t iter, bool lazy = true);

  virtual ~SimpleTempSeq();

  virtual Iterator_t getIterator();

  virtual Iterator_t getIterator(
        int32_t startPos,
        int32_t endPos,
        bool streaming = false);

  virtual Iterator_t getIterator(
        int32_t startPos,
        Iterator_t function,
        const std::vector<var_iterator>& var,
        bool streaming = false );

  virtual Iterator_t getIterator(
        const std::vector<int32_t>& positions,
        bool streaming = false);

  virtual Iterator_t getIterator(
        Iterator_t positions,
        bool streaming = false);

  virtual Item_t getItem(int32_t position);
  virtual bool containsItem(int32_t position);

  virtual void purge();
  virtual void purgeUpTo(int32_t upTo );
  virtual void purgeItem(const std::vector<int32_t>& positions );
  virtual void purgeItem(int32_t position );
  virtual bool empty();
  
  Item_t operator[](int32_t aIndex);
  int32_t getSize();

  class SimpleTempSeqIter : public Iterator
	{
  private:
    enum BorderType
    {
      none,
      startEnd,
      specificPositions
    };

    SimpleTempSeq_t            theTempSeq;
    BorderType                 theBorderType;

    int32_t                    theCurPos;
    int32_t                    theStartPos;
    int32_t                    theEndPos;
    std::vector<int32_t>       thePositions;
    
  public:
    SimpleTempSeqIter(SimpleTempSeq_t aTempSeq);
    SimpleTempSeqIter(SimpleTempSeq_t aTempSeq, int startPos, int endPos);
    SimpleTempSeqIter(SimpleTempSeq_t aTempSeq, const std::vector<int32_t>& positions);
    virtual ~SimpleTempSeqIter();

    Item_t next();
    void reset();
    void close();
  };

}; /* class SimpleTempSeq */

} /* namespace xqp */

#endif /* XQP_SIIMPLE_TEMP_SEQ_H */
