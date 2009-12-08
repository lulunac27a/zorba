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
#ifndef ZORBA_FUNCTIONS_FUNCTION_H
#define ZORBA_FUNCTIONS_FUNCTION_H

#include <vector>

#include "common/shared_types.h"

#include "functions/function_enum.h"
#include "functions/signature.h"

#include "compiler/parser/parse_constants.h"
#include "compiler/parser/query_loc.h"
#include "compiler/semantic_annotations/annotation_keys.h"
#include "compiler/expression/expr_consts.h"


namespace zorba 
{


class fo_expr;
class CompilerCB;
class AnnotationHolder;


#define FUNCTION_PRODUCES_SORTED_NODES                       \
FunctionConsts::AnnotationValue producesNodeIdSorted() const \
{                                                            \
  return FunctionConsts::YES;                                \
}


#define FUNCTION_PRODUCES_DISTINCT_NODES                     \
FunctionConsts::AnnotationValue producesDuplicates() const   \
{                                                            \
  return FunctionConsts::NO;                                 \
}


#define FUNCTION_PROPAGATES_I2O                              \
bool propagatesInputToOutput(uint32_t aProducer) const       \
{                                                            \
  return true;                                               \
}


#define FUNCTION_PROPAGATES_ONE_I2O( n )                     \
bool propagatesInputToOutput(uint32_t aProducer) const       \
{                                                            \
  return n == aProducer;                                     \
}


/*******************************************************************************

********************************************************************************/
class function_typechecker 
{
public:
  function_typechecker() {}

  virtual ~function_typechecker() {}
  
  virtual xqtref_t return_type(const fo_expr *fo) = 0;
};


/*******************************************************************************

********************************************************************************/
class function_codegenerator 
{
public:
  function_codegenerator() {}

  virtual ~function_codegenerator() {}

  virtual PlanIter_t codegen(const fo_expr *fo) = 0;
};



/*******************************************************************************

********************************************************************************/
class function : public SimpleRCObject 
{
protected:
	signature                    theSignature;
  FunctionConsts::FunctionKind theKind;
  uint32_t                     theFlags;

public:
  SERIALIZABLE_ABSTRACT_CLASS(function)
  SERIALIZABLE_CLASS_CONSTRUCTOR3(function, SimpleRCObject, theSignature)
  void serialize(::zorba::serialization::Archiver& ar);

public:
	function(const signature& sig);

	function(const signature& sig, FunctionConsts::FunctionKind kind);

	virtual ~function() {}

  FunctionConsts::FunctionKind getKind() const { return theKind; }

	const store::Item* getName() const { return theSignature.get_name(); }

	void set_signature(signature& sig) { theSignature = sig; }

  const signature& get_signature() const { return theSignature; }

  int get_arity() const { return theSignature.arg_count(); }

  virtual xqtref_t return_type(const std::vector<xqtref_t>& arg_types) const;

  void setFlag(FunctionConsts::AnnotationFlags flag) 
  {
    theFlags |= flag;
  }

  void resetFlag(FunctionConsts::AnnotationFlags flag)
  {
    theFlags &= ~flag;
  }

  bool testFlag(FunctionConsts::AnnotationFlags flag) const 
  {
    return (theFlags & flag) != 0;
  }

	virtual bool validate_args(std::vector<PlanIter_t>& argv) const;

  virtual bool requires_dyn_ctx() const { return false; }

  virtual bool isSource() const { return false; }

  virtual bool isDeterministic() const { return true; }

  virtual expr_script_kind_t getUpdateType() const { return SIMPLE_EXPR; }

  bool isUpdating() const { return getUpdateType() == UPDATE_EXPR; }

  bool isSequential() const { return getUpdateType() == SEQUENTIAL_EXPR; }

  virtual function* specialize(
        static_context* sctx,
        const std::vector<xqtref_t>& argTypes) const
  {
    return NULL;
  }

  virtual bool specializable() const { return false; }

  virtual bool isArithmeticFunction() const { return false; }

  virtual ArithmeticConsts::OperationKind arithmetic_kind() const 
  {
    return ArithmeticConsts::UNKNOWN;
  }

  virtual bool isComparisonFunction() const { return false; }

  virtual bool isValueComparisonFunction() const { return false; }

  virtual bool isGeneralComparisonFunction() const { return false; }

  virtual CompareConsts::CompareType comparison_kind() const 
  {
    return CompareConsts::UNKNOWN;
  }

  virtual bool isFnError() const { return false; }

  virtual bool isNodeSortFunction() const { return false; }

  virtual bool isNodeDistinctFunction() const { return false; }

  virtual bool propagatesInputToOutput(uint32_t aProducer) const { return false; }

  virtual FunctionConsts::AnnotationValue producesDuplicates() const;

  virtual FunctionConsts::AnnotationValue producesNodeIdSorted() const;

  virtual void compute_annotation(
        AnnotationHolder* foExpr,
        std::vector<AnnotationHolder*>& args,
        Annotations::Key) const;

  virtual PlanIter_t codegen(
        CompilerCB* cb,
        static_context* sctx,
        const QueryLoc& loc,
        std::vector<PlanIter_t>& argv,
        AnnotationHolder& ann) const = 0;
};




} /* namespace zorba */
#endif
/* vim:set ts=2 sw=2: */

/*
 * Local variables:
 * mode: c++
 * End:
 */
