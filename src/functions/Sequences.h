#ifndef ZORBA_SEQUENCES_H
#define ZORBA_SEQUENCES_H

#include <vector>
#include "common/shared_types.h"
#include "functions/function.h"
#include "functions/single_seq_func.h"

namespace zorba {

class zorba;

/*______________________________________________________________________
|  
| 15.1 General Functions and Operators on Sequences
|_______________________________________________________________________*/


//15.1.1 fn:boolean (effective boolean value)
//-----------------


//15.1.2 op:concatenate
//---------------------
class op_concatenate : public function
{
public:
  op_concatenate(const signature&);

public:
  PlanIter_t codegen (const QueryLoc& loc, std::vector<PlanIter_t>& argv, AnnotationHolder &ann) const;
  xqtref_t return_type (const std::vector<xqtref_t> &arg_types) const;
  void compute_annotation (AnnotationHolder *parent, std::vector<AnnotationHolder *> &kids, Annotation::key_t k) const;
};


//15.1.3 fn:index-of
//------------------
class fn_index_of : public function
{
public:
  fn_index_of(const signature&);

public:
  PlanIter_t codegen (const QueryLoc& loc, std::vector<PlanIter_t>& argv, AnnotationHolder &ann) const;
};


//15.1.4 fn:empty
//---------------
class fn_empty : public function
{
public:
  fn_empty(const signature&);

public:
  PlanIter_t codegen (const QueryLoc& loc, std::vector<PlanIter_t>& argv, AnnotationHolder &ann) const;
};


//15.1.5 fn:exists
//----------------
class fn_exists : public function
{
public:
  fn_exists(const signature&);

public:
  PlanIter_t codegen (const QueryLoc& loc, std::vector<PlanIter_t>& argv, AnnotationHolder &ann) const;
};


//15.1.6 fn:distinct-values
//-------------------------
class fn_distinct_values_1 : public single_seq_function
{
public:
  fn_distinct_values_1(const signature& sig) : single_seq_function (sig) {}

public:
  PlanIter_t codegen (const QueryLoc& loc, std::vector<PlanIter_t>& argv, AnnotationHolder &ann) const;
};

class fn_distinct_values_2 : public fn_distinct_values_1
{
public:
  fn_distinct_values_2(const signature& sig)
    : fn_distinct_values_1(sig) { }
};


//15.1.7 fn:insert-before
//-----------------------
class fn_insert_before : public function
{
public:
  fn_insert_before(const signature&);

public:
  PlanIter_t codegen (const QueryLoc& loc, std::vector<PlanIter_t>& argv, AnnotationHolder &ann) const;
};


//15.1.8 fn:remove
//----------------
class fn_remove : public single_seq_function
{
public:
  fn_remove(const signature& sig) : single_seq_function (sig) {}

public:
  PlanIter_t codegen (const QueryLoc& loc, std::vector<PlanIter_t>& argv, AnnotationHolder &ann) const;
};


//15.1.9 fn:reverse
//-----------------
class fn_reverse : public single_seq_function
{
public:
  fn_reverse(const signature& sig) : single_seq_function (sig) {}

public:
  PlanIter_t codegen (const QueryLoc& loc, std::vector<PlanIter_t>& argv, AnnotationHolder &ann) const;
};


//15.1.10 fn:subsequence
//----------------------
// subsequence with 2 arguments
class fn_subsequence_2 : public single_seq_function
{
public:
  fn_subsequence_2(const signature&sig) : single_seq_function (sig) {}

public:
  PlanIter_t codegen (const QueryLoc& loc, std::vector<PlanIter_t>& argv, AnnotationHolder &ann) const;
};

// subsequence with a third length argument
class fn_subsequence_3 : public fn_subsequence_2
{
public:
  fn_subsequence_3(const signature& sig)
    : fn_subsequence_2(sig) { }
};


//15.1.11 fn:unordered
//--------------------


/*______________________________________________________________________
|  
| 15.2 Functions That Test the Cardinality of Sequences
|_______________________________________________________________________*/

//15.2.1 fn:zero-or-one
class fn_zero_or_one : public function
{
public:
  fn_zero_or_one(const signature&);

public:
  PlanIter_t codegen (const QueryLoc& loc, std::vector<PlanIter_t>& argv, AnnotationHolder &ann) const;
};

//15.2.2 fn:one-or-more
class fn_one_or_more : public function
{
public:
  fn_one_or_more(const signature&);

public:
  PlanIter_t codegen (const QueryLoc& loc, std::vector<PlanIter_t>& argv, AnnotationHolder &ann) const;
};

//15.2.3 fn:exactly-one
class fn_exactly_one_noraise : public function
{
public:
  fn_exactly_one_noraise(const signature&);

public:
  PlanIter_t codegen (const QueryLoc& loc, std::vector<PlanIter_t>& argv, AnnotationHolder &ann) const;

protected:
  bool raise_err;
};

class fn_exactly_one : public fn_exactly_one_noraise {
public:
  fn_exactly_one (const signature& sig) : fn_exactly_one_noraise (sig)
  { raise_err = true; }
};

/*______________________________________________________________________
|
| 15.3 Equals, Union, Intersection and Except
|_______________________________________________________________________*/

//15.3.1 fn:deep-equal
class fn_deep_equal : public function
{
public:
  fn_deep_equal(const signature&);
  PlanIter_t codegen (const QueryLoc& loc, std::vector<PlanIter_t>& argv, AnnotationHolder &ann) const;
};


//15.3.2 op:union
class fn_union : public function
{
public:
  fn_union(const signature&);

public:
  PlanIter_t codegen (const QueryLoc& loc, std::vector<PlanIter_t>& argv, AnnotationHolder &ann) const;
};

//15.3.3 op:intersect
class fn_intersect : public function
{
public:
  fn_intersect(const signature&);

public:
  PlanIter_t codegen (const QueryLoc& loc, std::vector<PlanIter_t>& argv, AnnotationHolder &ann) const;
};

//15.3.4 op:except
class fn_except: public function
{
public:
  fn_except(const signature&);

public:
  PlanIter_t codegen (const QueryLoc& loc, std::vector<PlanIter_t>& argv, AnnotationHolder &ann) const;
};

/*______________________________________________________________________
|
| 15.4 Aggregate Functions
|_______________________________________________________________________*/

//15.4.1 fn:count
class fn_count: public function
{
public:
  fn_count(const signature&);

public:
  PlanIter_t codegen (const QueryLoc& loc, std::vector<PlanIter_t>& argv, AnnotationHolder &ann) const;
};

//15.4.2 fn:avg
class fn_avg: public function
{
public:
  fn_avg(const signature&);

public:
  PlanIter_t codegen (const QueryLoc& loc, std::vector<PlanIter_t>& argv, AnnotationHolder &ann) const;
};

//15.4.3 fn:max
class fn_max_1 : public function
{
public:
  fn_max_1(const signature&);

public:
  PlanIter_t codegen (const QueryLoc& loc, std::vector<PlanIter_t>& argv, AnnotationHolder &ann) const;
};

class fn_max_2 : public fn_max_1
{
public:
  fn_max_2(const signature& sig)
    : fn_max_1(sig) { }
};


//15.4.4 fn:min
//-------------
class fn_min_1 : public function
{
public:
  fn_min_1(const signature&);

public:
  PlanIter_t codegen (const QueryLoc& loc, std::vector<PlanIter_t>& argv, AnnotationHolder &ann) const;
};

class fn_min_2 : public fn_min_1
{
public:
  fn_min_2(const signature& sig)
    : fn_min_1(sig) { }
};

//15.4.5 fn:sum
class fn_sum_1 : public function
{
public:
  fn_sum_1(const signature&);

public:
  PlanIter_t codegen (const QueryLoc& loc, std::vector<PlanIter_t>& argv, AnnotationHolder &ann) const;
};

class fn_sum_2 : public fn_sum_1
{
public:
  fn_sum_2(const signature& sig)
    : fn_sum_1(sig) { }
};


/*______________________________________________________________________
|
| 15.5 Functions and Operators that Generate Sequences
|_______________________________________________________________________*/

//15.5.1 op:to
class op_to : public function
{
public:
  op_to(const signature&);

public:
  PlanIter_t codegen (const QueryLoc& loc, std::vector<PlanIter_t>& argv, AnnotationHolder &ann) const;
};


//15.5.2 fn:id
class fn_id : public function
{
public:
  fn_id(const signature&);

public:
  PlanIter_t codegen (const QueryLoc& loc, std::vector<PlanIter_t>& argv, AnnotationHolder &ann) const;
};

//15.5.3 fn:idref
class fn_id_ref : public function
{
  public:
    fn_id_ref(const signature&);

  public:
    PlanIter_t codegen (const QueryLoc& loc, std::vector<PlanIter_t>& argv, AnnotationHolder &ann) const;
};

//15.5.4 fn:doc
class fn_doc_func : public function
{
public:
  fn_doc_func(const signature&);

public:
  PlanIter_t codegen (const QueryLoc& loc, std::vector<PlanIter_t>& argv, AnnotationHolder &ann) const;
  virtual bool isSource() const { return true; }
  bool requires_dyn_ctx () const { return true; }  // TODO: rename to unfoldable()
};


//15.5.5 fn:doc-available
class fn_doc_available_func : public function
{
public:
  fn_doc_available_func(const signature&);

public:
  PlanIter_t codegen (const QueryLoc& loc, std::vector<PlanIter_t>& argv, AnnotationHolder &ann) const;
};


//15.5.6 fn:collection


/*______________________________________________________________________
|
| Extensions
|_______________________________________________________________________*/

// internal distinct-nodes function

class op_node_sort_distinct : public single_seq_function {
public:
  typedef enum { NODES, ATOMICS, MIXED } nodes_or_atomics_t;

  op_node_sort_distinct(const signature& sig) : single_seq_function (sig) {}
  // (sort?, atomics?, distinct?, ascending?)
  virtual const bool *action () const = 0;
  static const function *op_for_action (const static_context *sctx, const bool *a, const AnnotationHolder *parent, const AnnotationHolder *child, nodes_or_atomics_t noa);
  virtual const function *min_action (const static_context *sctx, const AnnotationHolder *self, const AnnotationHolder *child, nodes_or_atomics_t noa) const;

public:
  PlanIter_t codegen (const QueryLoc& loc, std::vector<PlanIter_t>& argv, AnnotationHolder &ann) const;
  void compute_annotation (AnnotationHolder *parent, std::vector<AnnotationHolder *> &kids, Annotation::key_t k) const;
};

class op_either_nodes_or_atomics : public op_node_sort_distinct {
public:
  op_either_nodes_or_atomics (const signature& sig) : op_node_sort_distinct (sig) {}
  const bool *action () const {
    static const bool a [] = { false, true, false, false };
    return a;
  }
};

class op_distinct_nodes : public op_node_sort_distinct
{
public:
  op_distinct_nodes(const signature& sig) : op_node_sort_distinct (sig) {}
  const bool *action () const {
    static const bool a [] = { false, false, true, false };
    return a;
  }
};

// internal distinct-nodes-or-atomics function
/**
 * Similar to op-distinct, but it allows a sequences of atomic items as input 
 * (but no mixture of atomic and node items). In this case, the result is
 * equal to the input
 */
class op_distinct_nodes_or_atomics : public op_node_sort_distinct {
public:
  op_distinct_nodes_or_atomics(const signature& sig) : op_node_sort_distinct (sig) {}
  const bool *action () const {
    static const bool a [] = { false, true, true, false };
    return a;
  }
};

// internal sort-nodes function sorting in document order
class op_sort_nodes_ascending : public op_node_sort_distinct
{
public:
  op_sort_nodes_ascending(const signature& sig) : op_node_sort_distinct (sig) {}
  const bool *action () const {
    static const bool a [] = { true, false, false, true };
    return a;
  }
};

// internal sort-nodes-asc-or-atomics function
/**
 * Similar to op-sort-ascending, but it allows a sequences of atomic items as input 
 * (but no mixture of atomic and node items). In this case, the result is
 * equal to the input
 */
class op_sort_nodes_asc_or_atomics : public op_node_sort_distinct {
public:
  op_sort_nodes_asc_or_atomics(const signature& sig) : op_node_sort_distinct (sig) {}
  const bool *action () const {
    static const bool a [] = { true, true, false, true };
    return a;
  }
};

// internal sort-nodes function sorting in reverse document order
class op_sort_nodes_descending : public op_node_sort_distinct
{
public:
  op_sort_nodes_descending(const signature& sig) : op_node_sort_distinct (sig) {}
  const bool *action () const {
    static const bool a [] = { true, false, false, false };
    return a;
  }
};

// internal sort-nodes-desc-or-atomics function
/**
* Similar to op-sort-descending, but it allows a sequences of atomic items as input 
* (but no mixture of atomic and node items). In this case, the result is
* equal to the input
*/
class op_sort_nodes_desc_or_atomics : public op_node_sort_distinct
{
public:
  op_sort_nodes_desc_or_atomics(const signature& sig) : op_node_sort_distinct (sig) {}
  const bool *action () const {
    static const bool a [] = { true, true, false, false };
    return a;
  }
};



// internal function for sort-nodes in document order and doing distinct-nodes in one run
class op_sort_distinct_nodes_ascending : public op_node_sort_distinct
{
public:
  op_sort_distinct_nodes_ascending(const signature& sig) : op_node_sort_distinct (sig) {}
  const bool *action () const {
    static const bool a [] = { true, false, true, true };
    return a;
  }
};

// internal sort-distinct-nodes-asc-or-atomics
/**
* Similar to sort-distinct-nodes-ascending, but it allows a sequences of atomic items as input 
* (but no mixture of atomic and node items). In this case, the result is
* equal to the input
*/
class op_sort_distinct_nodes_asc_or_atomics : public op_node_sort_distinct
{
public:
  op_sort_distinct_nodes_asc_or_atomics(const signature& sig) : op_node_sort_distinct (sig) {}
  const bool *action () const {
    static const bool a [] = { true, true, true, true };
    return a;
  }
};


// internal function for sort-nodes in reverse document order and doing distinct-nodes in one run
class op_sort_distinct_nodes_descending : public op_node_sort_distinct
{
public:
  op_sort_distinct_nodes_descending(const signature& sig) : op_node_sort_distinct (sig) {}
  const bool *action () const {
    static const bool a [] = { true, false, true, false };
    return a;
  }
};

class fn_unordered : public single_seq_function 
{
public:
  fn_unordered(const signature& sig) : single_seq_function (sig) {}
  PlanIter_t codegen (const QueryLoc& loc, std::vector<PlanIter_t>& argv, AnnotationHolder &ann) const;
  void compute_annotation (AnnotationHolder *parent, std::vector<AnnotationHolder *> &kids, Annotation::key_t k) const;
};

} /* namespace zorba */
#endif /* ZORBA_SEQUENCES_H */

/*
 * Local variables:
 * mode: c++
 * End:
 */
