/**
 *
 * @copyright
 * ========================================================================
 *	Copyright 2007 FLWOR Foundation
 *
 *	Licensed under the Apache License, Version 2.0 (the "License");
 *	you may not use this file except in compliance with the License.
 *	You may obtain a copy of the License at
 *
 *		http://www.apache.org/licenses/LICENSE-2.0
 *
 *	Unless required by applicable law or agreed to in writing, software
 *	distributed under the License is distributed on an "AS IS" BASIS,
 *	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *	See the License for the specific language governing permissions and
 *	limitations under the License.
 * ========================================================================
 *
 * @author David Graf (david.graf@28msec.com)
 * @file types/casting.h
 *
 */

#ifndef XQP_CASTING_H
#define XQP_CASTING_H

#include "util/rchandle.h"
#include "types/typesystem.h"

namespace xqp
{
	typedef rchandle<class Item> Item_t;
	
	/**
	 * Class which implements casting of items.
	 */
	class GenericCast
	{
		private:
			GenericCast();
    
    public:
			~GenericCast() {}
			
      static GenericCast* instance();

      /**
       * Tries to cast the passed string to an Item of the passed target type.
       * @param aString 
       * @param aTargateType
       * @return Created Item or 0 if the parsing was not possible
       */
      Item_t stringSimpleCast(const xqpString& aString, const TypeSystem::xqtref_t& aTargetType) const;
      
			/**
			 * Executes the casting of the passed item. If the passed item has the same type or a subtype
       * of the passed targetType, the passed item is directly returned.
			 * @param aItem 
       * @param aTargetType
			 * @return resutling item
			 */
      Item_t cast ( Item_t aItem, const TypeSystem::xqtref_t& aTargetType ) const;

      /**
       * Checks if the passed item would be castable to the passed target type.
       * @param aItem
       * @param aTargetType
       * @return true if castable, else false
       */
      bool isCastable( Item_t aItem, const TypeSystem::xqtref_t& aTargetType ) const; 

	}; /* class GenericCast */

} /* namespace xqp */

#endif	/* XQP_CASTING_H */
