/*
 * Copyright 2006-2016 zorba.io
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



#include "stdafx.h"
#include "runtime/csv/csv.h"
#include "functions/func_csv.h"


namespace zorba{



PlanIter_t fn_zorba_csv_parse::codegen(
  CompilerCB*,
  static_context* sctx,
  const QueryLoc& loc,
  std::vector<PlanIter_t>& argv,
  expr& ann) const
{
  return new CsvParseIterator(sctx, loc, argv);
}

PlanIter_t fn_zorba_csv_serialize::codegen(
  CompilerCB*,
  static_context* sctx,
  const QueryLoc& loc,
  std::vector<PlanIter_t>& argv,
  expr& ann) const
{
  return new CsvSerializeIterator(sctx, loc, argv);
}

void populate_context_csv(static_context* sctx)
{


      {
    DECL_WITH_KIND(sctx, fn_zorba_csv_parse,
        (createQName("http://zorba.io/modules/json-csv","","parse"), 
        GENV_TYPESYSTEM.STRING_TYPE_ONE, 
        GENV_TYPESYSTEM.JSON_OBJECT_TYPE_ONE, 
        GENV_TYPESYSTEM.JSON_OBJECT_TYPE_STAR),
        FunctionConsts::FN_ZORBA_CSV_PARSE_2);

  }




      {
    DECL_WITH_KIND(sctx, fn_zorba_csv_serialize,
        (createQName("http://zorba.io/modules/json-csv","","serialize"), 
        GENV_TYPESYSTEM.JSON_OBJECT_TYPE_STAR, 
        GENV_TYPESYSTEM.JSON_OBJECT_TYPE_ONE, 
        GENV_TYPESYSTEM.STRING_TYPE_STAR),
        FunctionConsts::FN_ZORBA_CSV_SERIALIZE_2);

  }

}


}



