/*
 *	Copyright 2006-2007 FLWOR Foundation.
 *  Authors: Tim Kraska, David Graf
 */

#include "system/globalenv.h"
#include "functions/Boolean.h"
#include "runtime/booleans/BooleanImpl.h"
#include "runtime/core/item_iterator.h"
#include "util/zorba.h"

namespace xqp
{

	/* begin class GenericOpComparison */
	GenericOpComparison::GenericOpComparison ( const signature& sig) 
		: function ( sig ) {}

	PlanIter_t
	GenericOpComparison::operator() ( const yy::location& loc, vector<PlanIter_t>& argv ) const
	{
		return this->createIterator(loc, argv);
	}

	TypeSystem::xqtref_t
	GenericOpComparison::type_check ( signature& sig ) const
	{
		return GENV_TYPESYSTEM.BOOLEAN_TYPE_ONE;
	}

	bool
	GenericOpComparison::validate_args ( vector<PlanIter_t>& argv ) const
	{
		return ( argv.size() == 2 );
	}
	/* end class GenericOpComparison */
	
	/* begin class op_equal */
	op_equal::op_equal( const signature& sig) : GenericOpComparison(sig) {}
	PlanIter_t 
	op_equal::createIterator ( const yy::location& loc, std::vector<PlanIter_t>& argv ) const
	{
		return new CompareIterator ( loc, argv[0], argv[1], CompareIterator::GENERAL_EQUAL );
	}
	/* end class op_equal */
	
	/* begin class op_not_equal */
	op_not_equal::op_not_equal( const signature& sig) : GenericOpComparison(sig) {}
	PlanIter_t 
	op_not_equal::createIterator ( const yy::location& loc, std::vector<PlanIter_t>& argv ) const
	{
		return new CompareIterator ( loc, argv[0], argv[1], CompareIterator::GENERAL_NOT_EQUAL );
	}
	/* end class op_not_equal */
	
	/* begin class op_greater */
	op_greater::op_greater( const signature& sig) : GenericOpComparison(sig) {}
	PlanIter_t 
	op_greater::createIterator ( const yy::location& loc, std::vector<PlanIter_t>& argv ) const
	{
		return new CompareIterator ( loc, argv[0], argv[1], CompareIterator::GENERAL_GREATER );
	}
	/* end class op_greater */
	
	/* begin class op_greater_equal */
	op_greater_equal::op_greater_equal( const signature& sig) : GenericOpComparison(sig) {}
	PlanIter_t 
	op_greater_equal::createIterator ( const yy::location& loc, std::vector<PlanIter_t>& argv ) const
	{
		return new CompareIterator ( loc, argv[0], argv[1], CompareIterator::GENERAL_GREATER_EQUAL );
	}
	/* end class op_greater_equal */
	
	/* begin class op_less */
	op_less::op_less( const signature& sig) : GenericOpComparison(sig) {}
	PlanIter_t 
	op_less::createIterator ( const yy::location& loc, std::vector<PlanIter_t>& argv ) const
	{
		return new CompareIterator ( loc, argv[0], argv[1], CompareIterator::GENERAL_LESS );
	}
	/* end class op_less */
	
	/* begin class op_less_equal */
	op_less_equal::op_less_equal( const signature& sig) : GenericOpComparison(sig) {}
	PlanIter_t 
	op_less_equal::createIterator ( const yy::location& loc, std::vector<PlanIter_t>& argv ) const
	{
		return new CompareIterator ( loc, argv[0], argv[1], CompareIterator::GENERAL_LESS_EQUAL );
	}
	/* end class op_less_equal */

		/* begin class op_value_equal */
	op_value_equal::op_value_equal( const signature& sig) : GenericOpComparison(sig) {}
	PlanIter_t 
	op_value_equal::createIterator ( const yy::location& loc, std::vector<PlanIter_t>& argv ) const
	{
		return new CompareIterator ( loc, argv[0], argv[1], CompareIterator::VALUE_EQUAL );
	}
	/* end class op_value_equal */
	
	/* begin class op_value_not_equal */
	op_value_not_equal::op_value_not_equal( const signature& sig) : GenericOpComparison(sig) {}
	PlanIter_t 
	op_value_not_equal::createIterator ( const yy::location& loc, std::vector<PlanIter_t>& argv ) const
	{
		return new CompareIterator ( loc, argv[0], argv[1], CompareIterator::VALUE_NOT_EQUAL );
	}
	/* end class op_value_not_equal */
	
	/* begin class op_value_greater */
	op_value_greater::op_value_greater( const signature& sig) : GenericOpComparison(sig) {}
	PlanIter_t 
	op_value_greater::createIterator ( const yy::location& loc, std::vector<PlanIter_t>& argv ) const
	{
		return new CompareIterator ( loc, argv[0], argv[1], CompareIterator::VALUE_GREATER );
	}
	/* end class value_op_greater */
	
	/* begin class op_value_greater_equal */
	op_value_greater_equal::op_value_greater_equal( const signature& sig) : GenericOpComparison(sig) {}
	PlanIter_t 
	op_value_greater_equal::createIterator ( const yy::location& loc, std::vector<PlanIter_t>& argv ) const
	{
		return new CompareIterator ( loc, argv[0], argv[1], CompareIterator::VALUE_GREATER_EQUAL );
	}
	/* end class op_greater_equal */
	
	/* begin class op_value_less */
	op_value_less::op_value_less( const signature& sig) : GenericOpComparison(sig) {}
	PlanIter_t 
	op_value_less::createIterator ( const yy::location& loc, std::vector<PlanIter_t>& argv ) const
	{
		return new CompareIterator ( loc, argv[0], argv[1], CompareIterator::VALUE_LESS );
	}
	/* end class op_value_less */
	
	/* begin class op_value_less_equal */
	op_value_less_equal::op_value_less_equal( const signature& sig) : GenericOpComparison(sig) {}
	PlanIter_t 
	op_value_less_equal::createIterator ( const yy::location& loc, std::vector<PlanIter_t>& argv ) const
	{
		return new CompareIterator ( loc, argv[0], argv[1], CompareIterator::VALUE_LESS_EQUAL );
	}
	/* end class op_value_less_equal */
	
	/* start class op_and */
	op_and::op_and ( const signature& sig ) : function ( sig ) {}

	PlanIter_t
	op_and::operator() (
	    const yy::location& loc,
	    vector<PlanIter_t>& argv ) const
	{
		return new LogicIterator ( loc, argv[0], argv[1], LogicIterator::AND );
	}

	TypeSystem::xqtref_t
	op_and::type_check (
	    signature& sig ) const
	{
		return GENV_TYPESYSTEM.BOOLEAN_TYPE_ONE;
	}

	bool
	op_and::validate_args ( vector<PlanIter_t>& argv ) const
	{
		return ( argv.size() == 2 );
	}
	/* end class op_and */
	
	/*start class op_or */
	op_or::op_or ( const signature& sig ) : function ( sig ) {}

	PlanIter_t
	op_or::operator() (
	    const yy::location& loc,
	    vector<PlanIter_t>& argv ) const
	{
		return new LogicIterator ( loc, argv[0], argv[1], LogicIterator::OR );
	}

	TypeSystem::xqtref_t
	op_or::type_check (
	    signature& sig ) const
	{
		return GENV_TYPESYSTEM.BOOLEAN_TYPE_ONE;
	}

	bool
	op_or::validate_args ( vector<PlanIter_t>& argv ) const
	{
		return ( argv.size() == 2 );
	}
	/* end class op_or */

	/* start class fn_true */
	fn_true::fn_true ( const signature& sig ) :function ( sig ) {}

	PlanIter_t
	fn_true::operator() (
	    const yy::location& loc,
	    vector<PlanIter_t>& argv ) const
	{
		return new SingletonIterator ( loc, zorba::getItemFactory()->createBoolean(true) );
	}

	TypeSystem::xqtref_t
	fn_true::type_check (
	    signature& sig ) const
	{
		return GENV_TYPESYSTEM.BOOLEAN_TYPE_ONE;
	}

	bool
	fn_true::validate_args ( vector<PlanIter_t>& argv ) const
	{
		return ( argv.size() == 0 );
	}
	/* end class fn_true */
	
	/* start class fn_false */
	fn_false::fn_false ( const signature& sig ) :function ( sig ) {}

	PlanIter_t
	fn_false::operator() (
	    const yy::location& loc,
	    vector<PlanIter_t>& argv ) const
	{
		return new SingletonIterator ( loc, zorba::getItemFactory()->createBoolean(false) );
	}

	TypeSystem::xqtref_t
	fn_false::type_check (
	    signature& sig ) const
	{
		return GENV_TYPESYSTEM.BOOLEAN_TYPE_ONE;
	}

	bool
	fn_false::validate_args ( vector<PlanIter_t>& argv ) const
	{
		return ( argv.size() == 0 );
	}	
	/* end class fn_false */
	
	/* begin class fn_not */
	fn_not::fn_not ( const signature& sig ) :function ( sig ) {}

	PlanIter_t
	fn_not::operator() (
	    const yy::location& loc,
	    vector<PlanIter_t>& argv ) const
	{
		return new FnBooleanIterator(loc, argv[0], true );
	}

	TypeSystem::xqtref_t
	fn_not::type_check (
	    signature& sig ) const
	{
		return GENV_TYPESYSTEM.BOOLEAN_TYPE_ONE;
	}

	bool
	fn_not::validate_args ( vector<PlanIter_t>& argv ) const
	{
		return ( argv.size() == 1 );
	}	
	/* end class fn_not */

	/* start class fn_boolean */
	fn_boolean::fn_boolean ( const signature& sig ) :function ( sig ) {}

	PlanIter_t
	fn_boolean::operator() (
	    const yy::location& loc,
	    vector<PlanIter_t>& argv ) const
	{
		return new FnBooleanIterator ( loc, argv[0] );
	}

	TypeSystem::xqtref_t
	fn_boolean::type_check (
	    signature& sig ) const
	{
		return GENV_TYPESYSTEM.BOOLEAN_TYPE_ONE;
	}

	bool
	fn_boolean::validate_args ( vector<PlanIter_t>& argv ) const
	{
		return ( argv.size() == 1 );
	}
	/* end class fn_boolean */
}
