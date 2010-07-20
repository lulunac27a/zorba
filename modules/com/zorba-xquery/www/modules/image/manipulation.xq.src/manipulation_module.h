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

#ifndef ZORBA_IMAGEMODULE_MANIPULATIONMODULE_MANIPULATIONMODULE_H
#define ZORBA_IMAGEMODULE_MANIPULATIONMODULE_MANIPULATIONMODULE_H 

#include <map>

#include <zorba/zorba.h>
#include <zorba/external_module.h>

namespace zorba { namespace imagemodule { namespace manipulationmodule {

class ManipulationModule : public ExternalModule
{
private:
  static ItemFactory* theFactory;

protected:
  class ltstr
  {
  public:
    bool operator()(const String& s1, const String& s2) const
    {
      return s1.compare(s2) < 0;
    }
  };
  
  typedef std::map<String, StatelessExternalFunction*, ltstr> FuncMap_t;

  mutable FuncMap_t theFunctions;
  
public:
  virtual ~ManipulationModule();
  
  virtual String
  getURI() const { return "http://www.zorba-xquery.com/modules/image/manipulation"; }
  
  virtual StatelessExternalFunction*
  getExternalFunction(String aLocalname) const;

  virtual void
  destroy();

  static ItemFactory*
  getItemFactory()
  {
    if(!theFactory)
    {
      theFactory = Zorba::getInstance(0)->getItemFactory();
    }
    
    return theFactory;
  }
};


} /* namespace manipulationmodule */ 
} /* namespace imagemodule */
} /* namespace zorba */

#endif /* ZORBA_IMAGEMODULE_MANIPULATIONMODULE_MANIPULATIONMODULE_H  */

/*
 * Local variables:
 * mode: c++
 * End:
 */
