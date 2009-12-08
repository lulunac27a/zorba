/*
 * Copyright 2006-2008 The FLWOR Foundation.
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

#include "system/globalenv.h"

//#include "compiler/expression/expr.h"
#include "compiler/semantic_annotations/tsv_annotation.h"
#include "compiler/semantic_annotations/annotation_keys.h"

#include "functions/function_impl.h"

#include "util/hashmap32.h"

#include "types/typeops.h"

using namespace std;

namespace zorba {

SERIALIZABLE_CLASS_VERSIONS(function)
END_SERIALIZABLE_CLASS_VERSIONS(function)


/*******************************************************************************

********************************************************************************/
function::function(const signature& sig) 
  :
  theSignature(sig),
  theKind(FunctionConsts::FN_UNKNOWN),
  theFlags(0)
{
  if(getName()->getNamespace()->byteEqual(XQUERY_FN_NS, sizeof(XQUERY_FN_NS)-1))
    setFlag(FunctionConsts::hasFnNamespace);

  zorba::serialization::Archiver& ar = *::zorba::serialization::ClassSerializer::getInstance()->getArchiverForHardcodedObjects();

  if(ar.is_loading_hardcoded_objects())
  {
    //register this hardcoded object to help plan serialization
    function* this_ptr = this;
    ar & this_ptr;
  }
}


/*******************************************************************************

********************************************************************************/
function::function(const signature& sig, FunctionConsts::FunctionKind kind) 
  :
  theSignature(sig),
  theKind(kind),
  theFlags(0)
{
  if(getName()->getNamespace()->byteEqual(XQUERY_FN_NS, sizeof(XQUERY_FN_NS)-1))
    setFlag(FunctionConsts::hasFnNamespace);

  zorba::serialization::Archiver& ar = *::zorba::serialization::ClassSerializer::getInstance()->getArchiverForHardcodedObjects();

  if(ar.is_loading_hardcoded_objects())
  {
    //register this hardcoded object to help plan serialization
    function* this_ptr = this;
    ar & this_ptr;
  }
}


/*******************************************************************************

********************************************************************************/
void function::serialize(::zorba::serialization::Archiver& ar)
{
  ar & theSignature;
  SERIALIZE_ENUM(FunctionConsts::FunctionKind, theKind);
  ar & theFlags;
}


/*******************************************************************************

********************************************************************************/
xqtref_t function::return_type(const std::vector<xqtref_t> &) const 
{
  return theSignature.return_type();
}


/*******************************************************************************

********************************************************************************/
bool function::validate_args(std::vector<PlanIter_t>& argv) const 
{
  uint32_t n = theSignature.arg_count ();
  return n == VARIADIC_SIG_SIZE || argv.size() == theSignature.arg_count();
}



/*******************************************************************************
  Propagate a property of this function down to its inputs, or up to its caller.
********************************************************************************/
void function::compute_annotation(
    AnnotationHolder* parent,
    std::vector<AnnotationHolder *>& kids,
    Annotations::Key k) const 
{
  switch (k) 
  {
  case Annotations::IGNORES_SORTED_NODES:
  case Annotations::IGNORES_DUP_NODES:
  {
    for (unsigned src = 0; src < kids.size(); ++src)
    {
      if (kids[src] != NULL)
      {
        // Unless this method is redefined by a specific function, a function
        // is considered as "potentially interested" in sorted and/or duplicate
        // nodes. In this case, each of its input exprs must also be marked as
        // "potentially interested", even if the input expr by itself is not
        // interested. 
        TSVAnnotationValue::update_annotation(kids[src],
                                              k,
                                              TSVAnnotationValue::MAYBE_VAL);
      }
    }
    break;
  }
  default:
    break;
  }
}


/*******************************************************************************
  Check whether this function produces, preserves, or eliminates duplicate nodes.
********************************************************************************/
FunctionConsts::AnnotationValue function::producesDuplicates() const 
{
  xqtref_t rt = theSignature.return_type();

  if (TypeOps::type_max_cnt(*rt) <= 1 || TypeOps::is_builtin_simple(*rt))
    return FunctionConsts::NO;

  return FunctionConsts::YES;
}


/*******************************************************************************
  Check whether this function produces nodes in document order, preserves the
  doc order of its input, or produces nodes out of doc order.
********************************************************************************/
FunctionConsts::AnnotationValue function::producesNodeIdSorted() const 
{
  xqtref_t rt = theSignature.return_type();

  if (TypeOps::type_max_cnt(*rt) <= 1 || TypeOps::is_builtin_simple(*rt))
    return FunctionConsts::YES;

  return FunctionConsts::NO;
}


}

/* vim:set ts=2 sw=2: */
