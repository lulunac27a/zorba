/*
 * Copyright 2006-2012 2006-2016 zorba.io.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
// ******************************************
// *                                        *
// * THIS IS A GENERATED FILE. DO NOT EDIT! *
// * SEE .xml FILE WITH SAME NAME           *
// *                                        *
// ******************************************

#ifndef ZORBA_RUNTIME_FETCH_FETCH_H
#define ZORBA_RUNTIME_FETCH_FETCH_H


#include "common/shared_types.h"



#include "runtime/base/narybase.h"


namespace zorba {

/**
 * 
 * Author: Matthias Brantner
 */
class FetchContentIterator : public NaryBaseIterator<FetchContentIterator, PlanIteratorState>
{ 
public:
  SERIALIZABLE_CLASS(FetchContentIterator);

  SERIALIZABLE_CLASS_CONSTRUCTOR2T(FetchContentIterator,
    NaryBaseIterator<FetchContentIterator, PlanIteratorState>);

  void serialize( ::zorba::serialization::Archiver& ar);

  FetchContentIterator(
    static_context* sctx,
    const QueryLoc& loc,
    std::vector<PlanIter_t>& children)
    : 
    NaryBaseIterator<FetchContentIterator, PlanIteratorState>(sctx, loc, children)
  {}

  virtual ~FetchContentIterator();

  zstring getNameAsString() const;

public:
  static void destroyStream(std::istream& aStream);
  void accept(PlanIterVisitor& v) const;

  bool nextImpl(store::Item_t& result, PlanState& aPlanState) const;
};


/**
 * 
 * Author: Matthias Brantner
 */
class FetchContentBinaryIterator : public NaryBaseIterator<FetchContentBinaryIterator, PlanIteratorState>
{ 
public:
  SERIALIZABLE_CLASS(FetchContentBinaryIterator);

  SERIALIZABLE_CLASS_CONSTRUCTOR2T(FetchContentBinaryIterator,
    NaryBaseIterator<FetchContentBinaryIterator, PlanIteratorState>);

  void serialize( ::zorba::serialization::Archiver& ar);

  FetchContentBinaryIterator(
    static_context* sctx,
    const QueryLoc& loc,
    std::vector<PlanIter_t>& children)
    : 
    NaryBaseIterator<FetchContentBinaryIterator, PlanIteratorState>(sctx, loc, children)
  {}

  virtual ~FetchContentBinaryIterator();

  zstring getNameAsString() const;

public:
  static void destroyStream(std::istream& aStream);
  void accept(PlanIterVisitor& v) const;

  bool nextImpl(store::Item_t& result, PlanState& aPlanState) const;
};


/**
 * 
 * Author: Matthias Brantner
 */
class FetchContentTypeIterator : public NaryBaseIterator<FetchContentTypeIterator, PlanIteratorState>
{ 
public:
  SERIALIZABLE_CLASS(FetchContentTypeIterator);

  SERIALIZABLE_CLASS_CONSTRUCTOR2T(FetchContentTypeIterator,
    NaryBaseIterator<FetchContentTypeIterator, PlanIteratorState>);

  void serialize( ::zorba::serialization::Archiver& ar);

  FetchContentTypeIterator(
    static_context* sctx,
    const QueryLoc& loc,
    std::vector<PlanIter_t>& children)
    : 
    NaryBaseIterator<FetchContentTypeIterator, PlanIteratorState>(sctx, loc, children)
  {}

  virtual ~FetchContentTypeIterator();

  zstring getNameAsString() const;

  void accept(PlanIterVisitor& v) const;

  bool nextImpl(store::Item_t& result, PlanState& aPlanState) const;
};


}
#endif
/*
 * Local variables:
 * mode: c++
 * End:
 */ 
