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

#include "file_function.h"

#include <cctype>
#include <sstream>

#include <zorba/empty_sequence.h>
#include <zorba/diagnostic_list.h>
#include <zorba/serializer.h>
#include <zorba/store_consts.h>
#include <zorba/user_exception.h>
#include <zorba/util/fs_util.h>
#include <zorba/xquery_functions.h>
#include <zorba/singleton_item_sequence.h>
#include <zorba/zorba.h>

#include "file_module.h"

#include <cassert>

#include <stdlib.h>
#include <stdio.h>
#ifdef WIN32
#include <Windows.h>
#include <direct.h>
#else
#include <unistd.h>
#endif

namespace zorba { namespace filemodule {

FileFunction::FileFunction(const FileModule* aModule, char const *local_name ) :
  theModule(aModule),
  local_name_( local_name )
{
}

String FileFunction::getLocalName() const {
  return local_name_;
}

int
FileFunction::raiseFileError(
  char const *aQName,
  char const *aMessage,
  String const &aPath ) const
{
  Item const lQName(
    theModule->getItemFactory()->createQName( getURI(), "file", aQName )
  );
  std::ostringstream lErrorMessage;
  lErrorMessage << '"' << aPath << "\": " << aMessage;
  throw USER_EXCEPTION( lQName, lErrorMessage.str() );
}

String
FileFunction::getURI() const
{
  return theModule->getURI();
}

String
FileFunction::getEncodingArg(
  const ExternalFunction::Arguments_t& aArgs,
  unsigned int aPos) const
{
  String encoding( getStringArg( aArgs, aPos ) );
  if ( encoding.empty() )
    encoding = "UTF-8";                 // the default file encoding
  return encoding;
}

String
FileFunction::getPathArg(
  const ExternalFunction::Arguments_t& aArgs,
  unsigned int aPos) const
{
  String const path( getStringArg( aArgs, aPos ) );
  if ( path.empty() )
    return path;
  try {
    return fs::normalize_path( path, fs::curdir() );
  }
  catch ( std::invalid_argument const &e ) {
    throw raiseFileError( "FOFL9999", e.what(), path );
  }
}

String
FileFunction::getStringArg(
  const ExternalFunction::Arguments_t& aArgs,
  unsigned int aPos) const
{
  String str;
  if ( aPos < aArgs.size() ) {
    Iterator_t i( aArgs[ aPos ]->getIterator() );
    i->open();
    Item item;
    if ( i->next( item ) )
      str = item.getStringValue();
    i->close();
  }
  return str;
}

String FileFunction::pathToFullOSPath(const String& aPath) const {
  try {
    return fs::normalize_path( aPath );
  }
  catch ( std::invalid_argument const &e ) {
    throw raiseFileError( "FOFL9999", e.what(), aPath );
  }
}

String FileFunction::pathToOSPath(const String& aPath) const {
  try {
    return fs::normalize_path( aPath );
  }
  catch ( std::invalid_argument const &e ) {
    throw raiseFileError( "FOFL9999", e.what(), aPath );
  }
}

String FileFunction::pathToUriString(const String& aPath) const {
  if ( fn::starts_with( aPath,"file://" ) ) {
    std::stringstream msg;
    msg << '"' << aPath << "\": path must not be a URI";
    Item const lQName(
      theModule->getItemFactory()->createQName(
        "http://www.w3.org/2005/xqt-errors", "err", "XPTY0004"
      )
    );
    throw USER_EXCEPTION( lQName, msg.str() );
  }

  String const uri( aPath );
  return uri;
}

#ifdef WIN32
bool FileFunction::isValidDriveSegment( String& aString ) {
  aString = fn::upper_case( aString );
  // the drive segment has one of the forms: "C:", "C%3A"
  if ( (aString.length() != 2 && aString.length() != 4) ||
       (aString.length() == 2 && !fn::ends_with( aString,":" ) ) ||
       (aString.length() == 4 && !fn::ends_with( aString,"%3A" ) ) ) {
    return false;
  }
  return isalpha( aString[0] );
}
#endif

//*****************************************************************************

WriterFileFunction::WriterFileFunction( FileModule const *aModule, char const *local_name) :
  FileFunction( aModule, local_name )
{
}

ItemSequence_t
WriterFileFunction::evaluate(
  const ExternalFunction::Arguments_t& aArgs,
  const StaticContext*,
  const DynamicContext* ) const
{
  String const lFileStr( getPathArg(aArgs, 0) );

  fs::type const fs_type = fs::get_type( lFileStr );
  if ( fs_type && fs_type != fs::file )
    raiseFileError( "FOFL0004", "not a plain file", lFileStr );

  bool const lBinary = isBinary();

  std::ios_base::openmode mode = std::ios_base::out
    | (isAppend() ? std::ios_base::app : std::ios_base::trunc);
  if ( lBinary )
    mode |= std::ios_base::binary;

  std::ofstream lOutStream( lFileStr.c_str(), mode );
  if ( !lOutStream ) {
    std::ostringstream oss;
    oss << '"' << lFileStr << "\": can not open file for writing";
    raiseFileError( "FOFL9999", oss.str().c_str(), lFileStr );
  }

  // if this is a binary write
  if (lBinary)
  {
    Item lBinaryItem;
    Iterator_t lContentSeq = aArgs[1]->getIterator();
    lContentSeq->open();
    while (lContentSeq->next(lBinaryItem))
    {
      if (lBinaryItem.isStreamable() && !lBinaryItem.isEncoded())
      {
        lOutStream << lBinaryItem.getStream().rdbuf();
      }
      else
      {
        Zorba_SerializerOptions lOptions;
        lOptions.ser_method = ZORBA_SERIALIZATION_METHOD_BINARY;
        Serializer_t lSerializer = Serializer::createSerializer(lOptions);
        SingletonItemSequence lSeq(lBinaryItem);
        lSerializer->serialize(&lSeq, lOutStream);
      }

    }
  }
  // if we only write text
  else
  {
    Item lStringItem;
    Iterator_t lContentSeq = aArgs[1]->getIterator();
    lContentSeq->open();
    // for each item (string or base64Binary) in the content sequence
    while (lContentSeq->next(lStringItem)) {
      // if the item is streamable make use of the stream
      if (lStringItem.isStreamable()) {
        std::istream& lInStream = lStringItem.getStream();
        char lBuf[4096];
        while (!lInStream.eof()) {
          lInStream.read(lBuf, sizeof lBuf);
          lOutStream.write(lBuf, lInStream.gcount());
        }
      }
      // else write the string value
      else {
        zorba::String const lString( lStringItem.getStringValue() );
        lOutStream.write(lString.data(), lString.size());
      }
    }
    lContentSeq->close();
  }

  // close the file stream
  lOutStream.close();

  return ItemSequence_t(new EmptySequence());
}

} // namespace filemodule
} // namespace zorba
/* vim:set et sw=2 ts=2: */
