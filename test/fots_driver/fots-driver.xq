(:
 : Copyright 2006-2011 The FLWOR Foundation.
 :
 : Licensed under the Apache License, Version 2.0 (the "License");
 : you may not use this file except in compliance with the License.
 : You may obtain a copy of the License at
 :
 : http://www.apache.org/licenses/LICENSE-2.0
 :
 : Unless required by applicable law or agreed to in writing, software
 : distributed under the License is distributed on an "AS IS" BASIS,
 : WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 : See the License for the specific language governing permissions and
 : limitations under the License.
 :)

(:~
 : Zorba FOTS driver
 : @author Sorin Nasoi
 :)

module namespace driver =
  "http://www.zorba-xquery.com/fots-driver";

import module namespace functx =
  "http://www.functx.com/";

import module namespace datetime  =
  "http://www.zorba-xquery.com/modules/datetime";

import module namespace eval =
  "http://www.zorba-xquery.com/fots-driver/evaluate" at "evaluate.xq";

import module namespace feedback =
  "http://www.zorba-xquery.com/fots-driver/feedback" at "feedback.xq";

import module namespace execute =
  "http://www.zorba-xquery.com/fots-driver/execute" at "execute.xq";

import module namespace env =
  "http://www.zorba-xquery.com/fots-driver/environment" at "environment.xq";

import module namespace util =
  "http://www.zorba-xquery.com/fots-driver/util" at "util.xq";

import module namespace fots-err =
  "http://www.zorba-xquery.com/fots-driver/errors" at "errors.xq";


declare namespace err =
  "http://www.w3.org/2005/xqt-errors";

declare namespace fots =
  "http://www.w3.org/2010/09/qt-fots-catalog";

declare namespace ann =
  "http://www.zorba-xquery.com/annotations";

(:~
 : Returns the names of all qualifying test-sets.
 :
 : A test-set qualifies if its name starts with one of the prefixes listed in
 : $testSetPrefixes. If $testSetPrefixes is the empty sequence, then all test-sets
 : qualify.
 :
 : @param $fotsPath path to the FOTS catalog file.
 : @param $testSetPrefixes name criteria for the test-sets
 :        (empty sequence means all).
 : @return names of qualifying FOTS test-sets.
 :)
declare %ann:nondeterministic function driver:list-test-sets(
  $fotsPath         as xs:string,
  $testSetPrefixes  as xs:string*
) as xs:string*
{
  let $doc := doc(resolve-uri($fotsPath))
  return
    if (empty($testSetPrefixes))
    then
      for $testSet in $doc/fots:catalog/fots:test-set
      return data($testSet/@name)
    else
      for $prefix in $testSetPrefixes
      for $testSet in $doc/fots:catalog/fots:test-set[starts-with(@name, $prefix)]
      return data($testSet/@name)
};


(:~
 : Returns the names of all qualifying test-cases.
 :
 : A test-case qualifies if (a) it belongs to a qualifying test-set, and (b)
 : its applicable dependencies include a user-provided dependency, or no
 : dependency was provided by the user, and (c) its expected-result assertions
 : include at least one of the assertions in a user-provided set of assertions,
 : or no expected-result assertions were provided by the user.
 :
 : A test-set qualifies if its name is specified in $testSetNames.
 : If $testSetNames is the empty sequence, then all test-sets qualify.
 :
 : The filtering dependency is given as a string, which may be empty (signifying
 : no dependency) or of the form "depValue_depSatisfied" (e.g., "XQ30+_true"),
 : or just "depValue" (in which case "true" is assumed as the value of the
 : satisfied attribute). A test-case qualifies only if it has at least one
 : dependency whose @value and @satisfied attributes are equal to the filtering
 : depValue and depSatisfied.
 :
 : A filtering set of assertions is given as a sequence of strings. A test-case
 : qualifies if there is at least one element node under the <result> node of
 : the <test-case> whose local name is equal to one of the strings in the
 : filtering set.
 :
 : @param $fotsPath path to the FOTS catalog file.
 : @param $testSetNames name criteria for the test-sets
 :        (empty sequence means all test sets).
 : @param $dependency dependency used to filter test-cases.
 :        (empty string means no filtering).
 : @param $assert set of expected-result assertions used to filter test-cases.
 :        (empty sequence means no filtering).
 : @return names of qualifying FOTS test-cases.
 :)
declare %ann:nondeterministic function driver:list-test-cases(
  $fotsPath     as xs:string,
  $testSetNames as xs:string*,
  $dependency   as xs:string,
  $assert       as xs:string*
) as xs:string*
{
  let $doc := doc(resolve-uri($fotsPath))
  let $baseUri:= resolve-uri(util:parent-folder($fotsPath))
  return
    if (empty($testSetNames))
    then
      for $testSet in $doc/fots:catalog/fots:test-set
      let $testSetDoc := doc(resolve-uri($testSet/@file, $baseUri))
      return driver:list-test-cases($testSetDoc, $dependency, $assert)
    else
      for $testSet in $doc/fots:catalog/fots:test-set
      let $testSetDoc := doc(resolve-uri($testSet/@file, $baseUri))
      where exists(index-of($testSetNames, $testSet/@name))
      return driver:list-test-cases($testSetDoc, $dependency, $assert)
};


(:~
 : Helper function. Returns the names of all qualifying test-cases within a
 : given test-set. A test-case qualifies if (a) its applicable dependencies
 : include a user-provided dependency, or no dependency was provided by the
 : user, and (b) its expected-result assertions include at least one of the
 : assertions in a user-provided set of assertions, or no expected-result
 : assertions were provided by the user.
 :
 : @param $testSetDoc root node of the xml document the specifies the test-set.
 : @param $dependency dependency used to filter test-cases.
 :        (empty string means no filtering).
 : @param $assert set of expected-result assertions used to filter test-cases.
 :        (empty sequence means no filtering).
 : @return names of qualifying FOTS test-cases.
 :)
declare %private function driver:list-test-cases(
  $testSetDoc as document-node(),
  $dependency as xs:string,
  $assert     as xs:string*
) as xs:string*
{
  if ($dependency eq '' and empty($assert)) then
  {
    $testSetDoc//fots:test-case/@name
  }
  else if ($dependency eq '') then
  {
    for $testCase in $testSetDoc//fots:test-case
    where functx:value-intersect(driver:list-assertions($testCase), $assert)
    return $testCase/@name
  }
  else
  {
    for $testCase in $testSetDoc//fots:test-case

    where
      driver:matches-dependency(($testSetDoc/fots:test-set/fots:dependency,
                                 $testCase/fots:dependency),
                                $dependency)
      and
      (empty($assert) or
       functx:value-intersect(driver:list-assertions($testCase), $assert))

    return $testCase/@name
  }
};


(:~
 : Helper function to check whether a user-specified dependency (given as an
 : encoded string) "matches" at least one of the dependencies in a given list
 : of dependencies (given as a sequence of <dependency> elements)
 :)
declare %private function driver:matches-dependency(
  $dependencies as element(fots:dependency)*,
  $filter       as xs:string
) as xs:boolean
{
  if ($filter eq '')
  then
    fn:true()
  else
    let $filter := tokenize($filter, '_')
    let $depValue := $filter[1]
    let $depSatisfied := if ($filter[2] eq 'true' or $filter[2] eq 'false')
                         then $filter[2]
                         else 'true'
    return
      if ($depSatisfied)
      then
        $dependencies[@value eq $depValue and @satisfied eq $depSatisfied] or
        $dependencies[@value eq $depValue and empty(@satisfied)]
      else
        $dependencies[@value eq $depValue and @satisfied eq $depSatisfied]
};


(:~
 : Helper function to return the local names of all the nodes under a <result>
 : child of a given test-case.
 :)
declare %private function driver:list-assertions(
  $case as element(fots:test-case)
) as xs:string*
{
  distinct-values(for $assert in $case/fots:result/descendant-or-self::*
                  return local-name-from-QName(node-name($assert)))
};


(:~
 : For each qualifying test-case, this functions returns a string containing
 : the test-case name and the full file path of its associated test-set file.
 :
 : A test-case qualifies if its <test> node (which contains the query text)
 : matches the given pattern using the given flags.
 :
 : @param $fotsPath path to the FOTS catalog file.
 : @param $pattern pattern.
 : @param $flags flags.
 : @return names of qualifying test-case and the file path of their
 :         containing test-set files.
 :)
declare %ann:nondeterministic function driver:list-matching-test-cases(
  $fotsPath as xs:string,
  $pattern  as xs:string,
  $flags    as xs:string?
) as xs:string*
{
  let $doc := doc(resolve-uri($fotsPath))
  let $baseUri:= resolve-uri(util:parent-folder($fotsPath))
  return
    for $testSet in $doc/fots:catalog/fots:test-set
    let $uri := resolve-uri($testSet/@file, $baseUri)
    let $testSetDoc := doc($uri)
    for $testCase in $testSetDoc//fots:test-case
    where matches(util:get-value($testCase, util:parent-folder($uri), "test"),
                  $pattern,
                  $flags)
    return
      concat(resolve-uri($testSet/@file, $baseUri),
             ", test name:",
             data($testCase/@name))
};


(:~
 : This function is just a thin wrapper over the driver:run() function. See
 : driver:run() for more info.
 :
 : Process a specified subset of all test-cases and report the outcome for
 : each such test-case.
 :
 : The subset of test-cases to process is specified via the $exceptedTestSets,
 : $testSetPrefixes, $testCasePrefixes, $dependency, and assertions parameters.
 : A test-case will be processed only if it satisfies all of the following
 : conditions:
 : (a) it belongs to a test-set whose name is not among the ones listed in the
 :     $exceptedTestSets parameter,
 : (b) $testSetPrefixes is the empty sequence, or the test-case belongs to a
 :     test-set whose name starts with a prefix listed in $testSetPrefixes,
 : (c) $testCasePrefixes is the empty sequence, or the name of the test-case
 :     starts with a prefix listed in $testCaseNames.
 : (d) $dependency is equal to the empty string or the dependencies applicable
 :     to the test-case include a user-provided dependency
 : (e) $assertions is empty or the expected-result assertions of the test-case
 :     include at least one of the assertions in $assertions
 :
 : @param $fotsPath path to the FOTS catalog file.
 : @param $zorbaManifestPath the path to the file that describes optional
 :        features and implementation-defined items in Zorba.
 : @param $testSetPrefixes prefixes to filter the test sets to process
 :        (empty seq means no filtering).
 : @param $exceptedTestSets names of test sets that should NOT be precessed.
 : @param $testCasePrefixes prefixes to filter the test cases to process
 :        (empty seq means no filtering).
 : @param $dependency dependency used to filter test-cases.
 :        (empty string means no filtering).
 : @param $assertions set of expected-result assertions used to filter test-cases.
 :        (empty sequence means no filtering).
 : @param $verbose if true, the resulting XML tree will contain more details
 :        about each processed test-case.
 : @param $expectedFailuresPath the path to the FOTSExpectedFailures.xml.
 : @param $cliMode the cli command.
 : @param $usePlanSerializer if true the plan serializer is used.
 : @return an XML tree containing info about all the processed tests-cases
 :)
declare %ann:sequential function driver:run-fots(
  $fotsPath               as xs:string,
  $zorbaManifestPath      as xs:string,
  $testSetPrefixes        as xs:string*,
  $exceptedTestSets       as xs:string*,
  $testCasePrefixes       as xs:string*,
  $dependency             as xs:string,
  $assertions             as xs:string*,
  $verbose                as xs:boolean,
  $expectedFailuresPath   as xs:string,
  $cliMode                as xs:string,
  $usePlanSerializer      as xs:boolean
) as element(fots:test-cases)
{
  trace($fotsPath, "Path to FOTS catalog.xml set to:");
  trace($expectedFailuresPath, "Path to FOTSExpectedFailures.xml set to:");
  trace($zorbaManifestPath, "Path to FOTSZorbaManifest set to :");  
  trace($usePlanSerializer, "'usePlanSerializer' set to:");

  try
  {
    let $FOTSCatalog := doc(resolve-uri($fotsPath))
    let $zorbaManifest := doc(resolve-uri($zorbaManifestPath))
    let $expectedFailures := if($expectedFailuresPath ne '')
                             then doc(resolve-uri($expectedFailuresPath))
                             else ()

    let $testSetNames :=
    if(($cliMode = 'run-test-case') ||
       ($cliMode = 'run-test-set'))
    then $testSetPrefixes
    else
    {
      if (empty($testSetPrefixes) and empty($exceptedTestSets)) then
      {
        ()
      }
      else
      {
        let $testSetNames := driver:list-test-sets($fotsPath, $testSetPrefixes)
        return
          if (empty($testSetNames)) then
          {
            exit returning <fots:test-cases/>;
            ()
          }
          else
          {
            functx:value-except($testSetNames, $exceptedTestSets)
          }
      }
    }

    let $testCaseNames :=
    {
      if (empty($testCasePrefixes) and $dependency eq '' and empty($assertions)) then
      {
        ()
      }
      else
      {
        let $testCaseNames := driver:list-test-cases($fotsPath,
                                                     $testSetNames,
                                                     $dependency,
                                                     $assertions)
        return
          if (empty($testCaseNames)) then
          {
            exit returning <fots:test-cases/>;
            ()
          }
          else if (empty($testCasePrefixes)) then
          {
            $testCaseNames
          }
          else
          {
            for $name in $testCaseNames
            where some $prefix in $testCasePrefixes satisfies starts-with($name, $prefix)
            return $name
          }
      }
    }

    return driver:check-FOTS($FOTSCatalog,
                             resolve-uri(util:parent-folder($fotsPath)),
                             $zorbaManifest,
                             $testSetNames,
                             $testCaseNames,
                             $expectedFailures,
                             $verbose,
                             $cliMode,
                             $usePlanSerializer)
  }
  catch err:FODC0002
  {
    error($err:code,
          $err:description)
  }
};


(:~
 : Process a specified test set and report the outcome for each containing
 : test case.
 :
 : Processing a test-case results in one of the following outcomes:
 : 1. Pass: the query was evaluated and its result matched the expected result.
 : 2. Fail: the query was evaluated and its result did not match the expected
 :    result.
 : 3. Non-applicable: the query was not evaluated because at least one of its
 :    applicable dependencies was violated.
 : 4. Not-run: the query was not evaluated because the name of the test-case
 :    appears in the $exceptedTestCases parameter.
 :
 : The subset of test-cases to process is specified via the $exceptedTestSets,
 : $testSetName, $exceptedTestCases, $testCaseName, $dependency,
 : and $assertions parameters.
 : A test-case will be processed only if it satisfies all of the following
 : conditions:
 : (a) it belongs to a test-set whose name is not among the ones listed in the
 :     $exceptedTestSets parameter,
 : (b) the test-case belongs to a test-set whose name is $testSetName,
 : (c) the name of the test-case is $testCaseName.
 : (d) $dependency is equal to the empty string or the dependencies applicable
 :     to the test-case include a user-provided dependency.
 : (e) $assertions is empty or the expected-result assertions of the test-case
 :     include at least one of the assertions in $assertions.
 :
 : @param $fotsPath path to the FOTS catalog file.
 : @param $zorbaManifestPath the path to the file that describes optional
 :        features and implementation-defined items in Zorba.
 : @param $testSetName exact name of the test-set.
 : @param $exceptedTestSets names of test sets that should NOT be precessed.
 : @param $testCasePrefixes prefixes to filter the test cases to process
 :        (empty seq means no filtering).
 : @param $dependency dependency used to filter test-cases.
 :        (empty string means no filtering).
 : @param $assertions set of expected-result assertions used to filter test-cases.
 :        (empty sequence means no filtering).
 : @param $verbose if true, the resulting XML tree will contain more details
 :        about each processed test-case.
 : @param $expectedFailuresPath the path to the FOTSExpectedFailures.xml.
 : @param $cliMode the cli command.
 : @param $usePlanSerializer if true the plan serializer is used.
 : @return an XML tree containing info about all the processed tests-cases
 :)
declare %ann:sequential function driver:run-test-set(
  $fotsPath               as xs:string,
  $zorbaManifestPath      as xs:string,
  $testSetName            as xs:string*,
  $exceptedTestSets       as xs:string*,
  $testCasePrefixes       as xs:string*,
  $dependency             as xs:string,
  $assertions             as xs:string*,
  $verbose                as xs:boolean,
  $expectedFailuresPath   as xs:string,
  $cliMode                as xs:string,
  $usePlanSerializer      as xs:boolean
) as element(fots:test-cases)
{
  trace($fotsPath, "Path to FOTS catalog.xml set to:");
  trace($expectedFailuresPath, "Path to FOTSExpectedFailures.xml set to:");
  trace($zorbaManifestPath, "Path to FOTSZorbaManifest set to :");
  trace($usePlanSerializer, "'usePlanSerializer' set to:");
  
  try
  {
    let $FOTSCatalog := doc(resolve-uri($fotsPath))
    let $zorbaManifest := doc(resolve-uri($zorbaManifestPath))
    let $expectedFailures := doc(resolve-uri($expectedFailuresPath))
    
    let $testSetNames :=
    if(empty($testSetName))
    then
    {
      if(empty($exceptedTestSets))
      then ()
      else for $testSet in $FOTSCatalog//fots:test-set
           where empty(index-of($testSet/@name, $exceptedTestSets))
           return xs:string($testSet/@name)
    }
    else
    {
      if(empty($exceptedTestSets))
      then $testSetName
      else functx:value-except($testSetName, $exceptedTestSets)
    }

    let $testCaseNames :=
    {
      if (empty($testCasePrefixes) and $dependency eq '' and empty($assertions)) then
      {
        ()
      }
      else
      {
        let $testCaseNames := driver:list-test-cases($fotsPath,
                                                     $testSetNames,
                                                     $dependency,
                                                     $assertions)
        return
          if (empty($testCaseNames)) then
          {
            exit returning <fots:test-cases/>;
            ()
          }
          else if (empty($testCasePrefixes)) then
          {
            $testCaseNames
          }
          else
          {
            for $name in $testCaseNames
            where some $prefix in $testCasePrefixes satisfies starts-with($name, $prefix)
            return $name
          }
      }
    }

    return driver:check-FOTS($FOTSCatalog,
                           resolve-uri(util:parent-folder($fotsPath)),
                           $zorbaManifest,
                           $testSetNames,
                           $testCaseNames,
                           $expectedFailures,
                           $verbose,
                           $cliMode,
                           $usePlanSerializer)
  }
  catch err:FODC0002
  {
    error($err:code,
          $err:description)
  }
};


(:~
 : A helper function to process a specified subset of all test-cases and
 : report the outcome for each such test-case.
 :
 : The subset of test-cases to process is specified via the $testSetNames
 : and $testCaseNames parameters. A test-case will be processed only if it
 : satisfies all of the following conditions:
 : (a) $testSetNames is the empty sequence, or the test-case belongs to a
 :     test-set whose name is listed in $testSetNames,
 : (b) $testCaseNames is the empty sequence, or the name of the test-case
 :     appears in $testCaseNames.
 :
 : @param $FOTSCatalog the root node of the FOTS catalog document.
 : @param $catalogBaseURI the URI of the directory containing the FOTS catalog file.
 : @param $FOTSZorbaManifest the root node of the doc that describes optional
 :        features and implementation-defined items in Zorba.
 : @param $testSetNames names of the test-sets to run (empty seq means all)
 : @param $testCaseNames names of the test-cases to run (empty seq means all)
 : @param $expectedFailures the root node of the ExpectedFailures.xml file.
 : @param $verbose if true, the resulting XML tree will contain more details
 :        about each processed test-case.
 : @param $cliMode the cli command.
 : @param $usePlanSerializer if true the plan serializer is used.
 : @return an XML tree containing info about all the processed test-cases.
 :)
declare %private %ann:sequential function driver:check-FOTS(
  $FOTSCatalog        as document-node(),
  $catalogBaseURI     as xs:anyURI,
  $FOTSZorbaManifest  as document-node(),
  $testSetNames       as xs:string*,
  $testCaseNames      as xs:string*,
  $expectedFailures   as document-node()?,
  $verbose            as xs:boolean,
  $cliMode            as xs:string,
  $usePlanSerializer  as xs:boolean
) as element(fots:test-cases)
{
  <fots:test-cases>{
  let $testSets := if (empty($testSetNames))
                   then $FOTSCatalog//fots:test-set
                   else $FOTSCatalog//fots:test-set[@name = $testSetNames]

  for $testSet in $testSets
  let $testSetName := $testSet/@name
  let $testSetURI := resolve-uri($testSet/@file, $catalogBaseURI)
  let $tsDoc := doc($testSetURI)
  return
    driver:check-test-set($FOTSCatalog/fots:catalog/fots:environment,
                          $catalogBaseURI,
                          $FOTSZorbaManifest,
                          $tsDoc,
                          $testSetURI,
                          $expectedFailures/failures/TestSet[@name eq $testSetName],
                          $testCaseNames,
                          $verbose,
                          $cliMode,
                          $usePlanSerializer)
  }</fots:test-cases>
};


(:~
 : Checks a single test-set.
 :
 : @param $catalogEnvironments all the 'environments' defined in the FOTS catalog.
 : @param $catalogBaseURI the URI of the directory that contains the FOTS catalog.
 : @param $FOTSZorbaManifest the root node of the doc that describes optional
 :        features and implementation-defined items in Zorba.
 : @param $tsDoc the root node of the test-set document.
 : @param $tsBaseURI the URI of the directory that contains the file of the
 :        associated test-set.
 : @param $expFailuresTS the 'TestSet' element from FOTSExpectedFailures.xml.
 : @param $testCaseNames names of the test-cases to run (empty seq means all).
 : @param $verbose if true, the resulting XML tree will contain more details
 :        about the test-case.
 : @param $cliMode the cli command.
 : @param $usePlanSerializer if true the plan serializer is used.
 : @return an XML tree containing the result of checking/running the test-cases.
 :)
declare %private %ann:sequential function driver:check-test-set(
  $catalogEnvironments  as element(fots:environment)*,
  $catalogBaseURI       as xs:anyURI,
  $FOTSZorbaManifest    as document-node(),
  $tsDoc                as document-node(),
  $tsBaseURI            as xs:anyURI,
  $expFailuresTS        as element(TestSet)?,
  $testCaseNames        as xs:string*,
  $verbose              as xs:boolean,
  $cliMode              as xs:string,
  $usePlanSerializer    as xs:boolean
) as element(fots:test-set)
{ 
  let $testSetEnvironments := $tsDoc/fots:test-set//fots:environment/@name
  let $tsDepMet := env:check-dependencies($tsDoc/fots:test-set/fots:dependency,
                                          $FOTSZorbaManifest)
  let $tsName := $tsDoc/fots:test-set/@name
  return
  <fots:test-set name="{$tsName}">{
  for $testCase in $tsDoc/fots:test-set/fots:test-case
  where empty($testCaseNames) or $testCaseNames[. eq $testCase/@name]
  return
    driver:check-test-case($expFailuresTS/Test[@name = $testCase/@name],
    
                           $testCase,
                           
                           $tsName,
                           
                           $tsBaseURI,
                           
                           $tsDepMet,
                           
                           $tsDoc/fots:test-set/fots:dependency,
                           
                           $FOTSZorbaManifest,
                           
                           if(empty($testCase/fots:environment/@ref))
                           then ()
                           else if (exists($catalogEnvironments/following-sibling::*[@name eq $testCase/fots:environment/@ref]))
                           then $catalogEnvironments/following-sibling::*[@name eq $testCase/fots:environment/@ref]
                           else $tsDoc/fots:test-set/fots:environment[@name eq $testCase/fots:environment/@ref],
                           
                           if(empty($testCase/fots:environment/@ref))
                           then ()
                           else if (exists($catalogEnvironments/following-sibling::*[@name eq $testCase/fots:environment/@ref]))
                           then $catalogBaseURI
                           else $tsBaseURI,
                           
                           exists(index-of(('fn-id', 'fn-idref', 'app-FunctxFunctx'), $tsName)),
                           
                           $verbose,
                           
                           $cliMode,
                           
                           $usePlanSerializer)
  }
  </fots:test-set>
};

(:~
 : Checks a single test-case.
 :
 : @param $expFailuresTC the 'Test' element from FOTSExpectedFailures.xml
 :        for the parent test-set.
 : @param $testCase the test-case to run.
 : @param $tsName the name of the parent test-set.
 : @param $tsBaseURI the URI of the directory that contains the file of the
 :        associated test-set.
 : @param $deps the result of checking the test-set level dependencies.
 : @param $tsDependencies  the test-set level dependencies.
 : @param $FOTSZorbaManifest the root node of the doc that describes optional
 :        features and implementation-defined items in Zorba.
 : @param $env the non-local environment to use, if any. It is an environment
 :        specified either at the test-set level or at the catalog level and
 :        is referenced by the test-case.
 : @param $envBaseURI the URI of the directory containing the file where the
 :        environment is specified in. It is used to calculate the full URI
 :        for the different children of the <environment> that have a @file
 :        attribute.
 : @param $mayNeedDTDValidation true if the test case may need DTD validation
 :        for the document bound as context item.
 : @param $verbose if true, the resulting XML tree will contain more details
 :        about the test-case.
 : @param $cliMode the cli command.
 : @param $usePlanSerializer if true the plan serializer is used.
 : @return an XML tree containing the result of checking/running the test-case.
 :)
declare %ann:sequential function driver:check-test-case(
  $expFailureTC         as element(Test)?,
  $testCase             as element(fots:test-case),
  $tsName               as xs:string,
  $tsBaseURI            as xs:anyURI,
  $tsDepMet             as xs:string*,
  $tsDependencies       as element(fots:dependency)*,
  $FOTSZorbaManifest    as document-node(),
  $env                  as element(fots:environment)?,
  $envBaseURI           as xs:anyURI?,
  $mayNeedDTDValidation as xs:boolean,
  $verbose              as xs:boolean,
  $cliMode              as xs:string,
  $usePlanSerializer    as xs:boolean
) as element(fots:test-case)?
{
  let $tcDepMet := env:check-dependencies($testCase/fots:dependency,
                                          $FOTSZorbaManifest)
  return
  if (exists($expFailureTC[@notRun="true"]))
  then
  {
(:  
  concat("Test case was not run because it is marked as EXPECTED_FOTS_FAILURE ",
         $status,
         " in test/fots/CMakeLists.txt.",
         " For details please check out https://bugs.launchpad.net/zorba/+bug/",
         $bugNo)
:)

    if($expFailureTC/@finalStatus = "pass")
    then
      feedback:pass($testCase,
                    (),
                    (),
                    concat("Test case was not run because it is marked as",
                           " EXPECTED_FOTS_FAILURE SLOW in test/fots/CMakeLists.txt.",
                           "For details please check out https://bugs.launchpad.net/zorba/+bug/",
                           $expFailureTC/@bug))
    else
      feedback:fail($testCase,
                    (),
                    '',
                    $tsName,
                    (),
                    xs:dayTimeDuration("PT0S"),
                    $verbose,
                    fn:false(),
                    (),
                    concat("Test case was not run because it is marked as",
                           " EXPECTED_FOTS_FAILURE CRASH in test/fots/CMakeLists.txt.",
                           "For details please check out https://bugs.launchpad.net/zorba/+bug/",
                           $expFailureTC/@bug))
  }
  else if (exists($tsDepMet) or exists($tcDepMet))
  then
  {
    feedback:not-applicable($testCase,
                            string-join(($tsDepMet, $tcDepMet), ''))
  }
  else
  {
    driver:run-test-case($testCase,
                         $env,
                         $envBaseURI,
                         ($testCase/fots:dependency,
                          $tsDependencies),
                         $tsName,
                         $tsBaseURI,
                         $verbose,
                         $expFailureTC,
                         $cliMode,
                         $mayNeedDTDValidation,
                         $usePlanSerializer)
  }
};

(:~
 : Runs a single test-case.
 :
 : @param $case the test-case to run.
 : @param $env the non-local environment to use, if any. It is an environment
 :        specified either at the test-set level or at the catalog level and
 :        is referenced by the test-case.
 : @param $envBaseURI the URI of the directory containing the file where the
 :        environment is specified in. It is used to calculate the full URI
 :        for the different children of the <environment> that have a @file
 :        attribute.
 : @param $deps the dependencies that should be checked for given test-case.
 :        These may be defined at test-set level and/or test-case level.
 : @param $testSetName the name of the test-set.
 : @param $testSetBaseURI the URI of the directory that contains the file of the
          associated test-set.
 : @param $verbose if true, the resulting XML tree will contain more details
 :        about the test-case.
 : @param $expFailureTC the 'Test' element from the ExpectedFailures.xml file.
 : @param $cliMode the cli command.
 : @param $mayNeedDTDValidation true if the test-case may need DTD validation
 :        for the document bound as context item.
 : @param $usePlanSerializer if true the plan serializer is used.
 : @return an XML tree containing the result of running the test-case.
 :)
declare %private %ann:sequential function driver:run-test-case(
  $case                 as element(fots:test-case),
  $env                  as element(fots:environment)?,
  $envBaseURI           as xs:anyURI?,
  $deps                 as element(fots:dependency)*,
  $testSetName          as xs:string,
  $testSetBaseURI       as xs:anyURI,
  $verbose              as xs:boolean,
  $expFailureTC         as element(Test)?,
  $cliMode              as xs:string,
  $mayNeedDTDValidation as xs:boolean,
  $usePlanSerializer    as xs:boolean
) as element(fots:test-case)?
{
(:TODO Cover the "(:%VARDECL%:)"when there are tests in FOTS that use it:)
  try
  {
  {
    variable $trace := concat("processing test case : ", $case/@name,
                              " in test set : ", $testSetName);

    trace($trace,"");

    variable $queryName := $case/@name;

    variable $test as xs:string := util:get-value($case, $testSetBaseURI, "test");

    variable $envCase := $case/fots:environment;

    variable $query :=
      string-join
      (
      (
        env:add-xquery-version-decl(($deps, $case//fots:dependency), $test),

        env:decl-base-uri($env, $envCase),

        env:decl-def-elem-namespace($env, $envCase),

        env:decl-namespaces($env, $envCase, $test),

        env:decl-decimal-formats(($env/fots:decimal-format,
                                  $envCase/fots:decimal-format)),

        env:add-var-decl($env, $envCase, $envBaseURI, $testSetBaseURI),

        $test
      ),
      "&#xA;"
      );

   variable $needsDTDValidation :=
    if (not($mayNeedDTDValidation))
    then fn:false()
    else if (($testSetName = 'app-FunctxFunctx') and
             (xs:string($case/@name) = 'functx-functx-id-from-element-1'))
    then fn:true()
    else if (($testSetName = 'fn-id') and
             starts-with(xs:string($case/@name), 'fn-id-dtd-'))
    then fn:true()
    else (($testSetName = 'fn-idref') and
         starts-with(xs:string($case/@name), 'fn-idref-dtd-'));

    variable $xqxqQuery := driver:create-XQXQ-query($query,
                                                    $case,
                                                    $env,
                                                    $envBaseURI,
                                                    $testSetBaseURI,
                                                    $needsDTDValidation);

    (:if $verbose then print the query to a file:)
    if ($verbose and
        ($cliMode eq "run-test-case"))
    then util:write-query-to-file($xqxqQuery, $queryName);
    else ();

    variable $startDateTime := datetime:current-dateTime();

    variable $result := execute:xqxq-invoke($xqxqQuery,
                                           $case,
                                           $verbose,
                                           $testSetBaseURI,
                                           $usePlanSerializer);

    variable $duration := (datetime:current-dateTime() - $startDateTime);
    
    variable $prerequisitesError as xs:string? := env:check-prerequisites($case, $env);

    if (feedback:check-pass($result, $queryName, $testSetName, $expFailureTC))
    then
      feedback:pass($case,
                    $result,
                    $xqxqQuery,
                    $env,
                    $duration,
                    $verbose,
                    exists($expFailureTC))
    (: 
      If the test case did not pass, we check to see if the failure is caused
      by a environment that requires setting of a COLLATION or COLLECTION.
      There are over 130 test cases that are using an environment that requires
      setting a COLLATION or COLLECTION but they still PASS even if this setting
      is not done. That is why we first run the test case.
     :)
    else if(exists($prerequisitesError))
    then
      feedback:not-run($case, $prerequisitesError)
    else
      feedback:fail($case,
                    $result,
                    $xqxqQuery,
                    $testSetName,
                    $env,
                    $duration,
                    $verbose,
                    exists($expFailureTC),
                    (),
                    ())
  }
  }
  catch *
  {
    feedback:fail($case,
                  eval:error((),
                             $case/fots:result/*,
                             $err:code,
                             $err:description,
                             $testSetBaseURI),
                  "fots-driver.xq:driver:test catch",
                  $testSetName,
                  $env,
                  xs:dayTimeDuration("PT0S"),
                  $verbose,
                  exists($expFailureTC),
                  (),
                  ())
  }
};


(:~
 : Creates the text for the complete query FQ that will be evaluated via XQXQ
 : by the fots test driver. The actual test-case query TQ will be evaluated as
 : a nested XQXQ query within FQ. FQ may contain additional nested XQXQ queries,
 : for example to compute values for external variables declared in TQ.
 :
 : @param $queryText the text for the test-case query TQ. It is content of
 :        <test-case>/test augmented with all the necessary prolog statements.
 : @param $case the test case.
 : @param $env the environment.
 : @param $envBaseURI URI of the environment.
 : @param $testSetBaseURI the URI of the directory that contains the file of the
          associated test set.
 : @param $needsDTDValidation true if the test case needs DTD validation
 :        for the document bound as context item.
 : @return the query that will be evaluated.
 :)
declare %private function driver:create-XQXQ-query(
  $queryText          as xs:string,
  $case               as element(fots:test-case),
  $env                as element(fots:environment)?,
  $envBaseURI         as xs:anyURI?,
  $testSetBaseURI     as xs:anyURI,
  $needsDTDValidation as xs:boolean
) as xs:string
{
  let $resolver as xs:string? := env:resolver($case,
                                              $env,
                                              $envBaseURI,
                                              $testSetBaseURI)
  let $mapper as xs:string? := env:mapper($case,
                                          $env,
                                          $envBaseURI,
                                          $testSetBaseURI)
  return
    string-join
    (
    (
    "",
    "import module namespace xqxq = 'http://www.zorba-xquery.com/modules/xqxq';",
    
    if ($needsDTDValidation) 
    then ("import module namespace zorba-xml = 'http://www.zorba-xquery.com/modules/xml#2.1';",
          "import schema namespace opt       = 'http://www.zorba-xquery.com/modules/xml-options';")
    else (),
    
    if (exists($resolver))
    then "declare namespace resolver = 'http://www.zorba-xquery.com/modules/xqxq/url-resolver';"
    else (),

    if (exists($mapper))
    then "declare namespace mapper = 'http://www.zorba-xquery.com/modules/xqxq/uri-mapper';"
    else (),

    "",

    if (exists($resolver)) then ($resolver, "") else (),

    if (exists($mapper)) then ($mapper, "") else (),

    (:
     : We want to put the input query into a new XQuery as a string literal
     : and compile this new XQuery. In order to do so, we must escape some
     : things. The StringLiteral production in the XQuery grammar reads:
     :
     :  StringLiteral ::=
     :      "'" (PredefinedEntityRef | CharRef | EscapeApos | [^'&])* "'"
     :
     : for string literals delimited by single quotes. Therefore, we need
     : to manipulate the input query in two ways:
     :   1. Replace ' with '' (EscapeApos)
     :   2. Replace & with &amp; (which will block any PredefinedEntityRefs
     :      or CharRefs in the input query from being expanded in the new
     :      XQuery).
     : The replacement happens here. (2) looks weird because there's one
     : level of replacement being done right *here* when fots-driver.xq
     : itself is compiled. Escaping sucks.
     :)
    let $escAposQueryText := replace($queryText,"'","''")
    let $escAmpQueryText  := replace($escAposQueryText, '&amp;', '&amp;amp;')
    return concat(
           "variable $queryID := xqxq:prepare-main-module&#xA;(",
           "&#xA;",
           "'",
           "&#xA;",
           $escAmpQueryText,
           "&#xA;",
           "'",
           "&#xA;",
           if (exists($resolver))
           then if(exists($mapper))
                then ", resolver:url-resolver#2, mapper:uri-mapper#2&#xA;);"
                else ", resolver:url-resolver#2, ()&#xA;);"
           else if(exists($mapper))
                then ", (), mapper:uri-mapper#2&#xA;);"
                else "&#xA;);"),

    env:set-context-item($env, $envBaseURI, $needsDTDValidation),
    env:set-context-item($case/fots:environment, $testSetBaseURI, $needsDTDValidation),
    env:set-variables($env, $envBaseURI),
    env:set-variables($case/fots:environment, $testSetBaseURI),

    "xqxq:evaluate($queryID),",
    "",
    "xqxq:delete-query($queryID)",
    "        "
    ),
    "&#xA;"
    )
};
