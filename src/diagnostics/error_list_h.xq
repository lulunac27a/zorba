import module namespace util = "http://www.zorba-xquery.com/diagnostic/util" at "diagnostic_util.xq";

declare function local:strip-ws( $comment ) as element(comment)
{
  copy $copy := $comment
  modify
  for $n in $copy/descendant-or-self::text()[ contains(., "^\n+") or contains(., "\n+$") ]
  return replace value of node $n with replace( replace( $n, '^\n+', '' ), '\n+$', '' )
  return $copy
};

declare function local:reformat-line($line as xs:string) as xs:string
{
  concat( $util:newline, ' * ', substring($line, 7) )
};


declare function local:add-stars( $comment )
{
 copy $copy := $comment
 modify
 for $n in $copy/text()
 return replace value of node $n with
   let $tokens := fn:tokenize( $n, '\n' )
   return (
     if ( $n/preceding-sibling::node()[1] instance of element() )
     then
       fn:head( $tokens )
     else
        local:reformat-line( fn:head( $tokens ) ),
     for $line in fn:tail( $tokens )
     return local:reformat-line( $line )
   )
 return $copy/node()
};


declare function local:make-doxygen-comment( $diagnostic ) as xs:string*
{
  string-join(
    ( '/**',
      let $comment := $diagnostic/comment
      return
        if ( $comment )
        then
          let $stripped-comment := local:strip-ws( $comment )
          return local:add-stars( $comment )
        else
          "",
      ' */',
      ''
    ),
    $util:newline
  )
};

declare function local:declare-diagnostics( $doc ) as xs:string*
{
  for $namespace in $doc/diagnostic-list/namespace
  let $class :=
    switch ( data( $namespace/@prefix ) )
    case "err" return "XQueryErrorCode"
    case "zerr" return "ZorbaErrorCode"
    case "zwarn" return "ZorbaWarningCode"
    default return error()
  return
    string-join(
      (
        concat( $util:newline, "namespace ", $namespace/@prefix, " {" ),
        for $diagnostic in $namespace/diagnostic
        return 
          concat (
            (:local:make-doxygen-comment( $diagnostic ),:)
              "",
             util:begin_guard( $diagnostic ),
             "extern ZORBA_DLL_PUBLIC ", $class, " ", $diagnostic/@code,
             if ( $diagnostic/@name )
             then
              concat( "_", $diagnostic/@name, ";" )
             else
              ";",
             $util:newline,
             util:end_guard( $diagnostic )
          ),
        concat( "} // namespace ", $namespace/@prefix )
      ),
      $util:newline
    )
};


declare variable $input external;

string-join(
  ( util:copyright(), 
    '#ifndef ZORBA_ERROR_LIST_API_H',
    '#define ZORBA_ERROR_LIST_API_H',
    '',
    '#include &lt;zorba/config.h>',
    '#include &lt;zorba/error.h>',
    '#include &lt;zorba/xquery_warning.h>',
    '',
    'namespace zorba {',
    '',
    local:declare-diagnostics( $input ),
    '} // namespace zorba',
    '#endif',
    '/*',
    ' * Local variables:',
    ' * mode: c++',
    ' * End:',
    ' */'
  ),
  $util:newline
),
$util:newline

(: vim:set syntax=xquery et sw=2 ts=2: :)

