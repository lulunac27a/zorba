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
#pragma once
#include <zorba/zorba.h>
#include <zorba/item_sequence.h>
#include <zorba/static_context.h>
#include <zorbatypes/zstring.h>

namespace zorba {
  class HttpStream {
  public:
    HttpStream(const zstring& uri);
    ~HttpStream();
  public:
    std::istream& getStream();
    void init();
    bool didInit() { return theDidInit; }
  private:
    ItemSequence_t theItemSequence;
    StaticContext_t theStaticContext;
    Item theStreamableString;
    Iterator_t theIterator;
    bool theDidInit;
    const zstring& theUri;
  };
}
