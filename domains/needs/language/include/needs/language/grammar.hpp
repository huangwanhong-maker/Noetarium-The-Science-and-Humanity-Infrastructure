// The grammar of statements of need, in BNF over bytes, read by morphe's reader (morphe/formal/bnf.hpp): the first rule is the
// start, names beginning with '_' are hidden, and whitespace is written out, since the grammar is over bytes. The theory note's §6
// gives it in outline; this is the text the reader compiles.
//
// A document is a list of statements, one to a line (or separated by ';'), with comments after '#'. A value is a number, a word
// or a quoted string ("Hume City", with \" and \\ escaped). A level may be written as a percentage (30%), read as 0.3.
//
// A change to this text changes the reader (reader.hpp), the statement (statement.hpp) and the checks together.
#pragma once

#include <string_view>

namespace needs::language {

inline constexpr std::string_view grammar_text = R"BNF(
statements ::= _ ( need ( _sep need )* _ )? ;

_    ::= ( [ \t\n\r] | _c )* ;
_c   ::= "#" [^\n]* ;
_s   ::= [ \t]* ;
_sp  ::= [ \t]+ ;
_sep ::= _s _c? ( [\n\r;] _s _c? )+ ;

need      ::= "need" _sp domain _sp "for" _sp group ( _sp "within" _sp frame )? _sp "by" _sp _standard ;
domain    ::= word ;
frame     ::= group ;
reference ::= group ;
group     ::= everyone | condition ( _sp "and" _sp condition )* ;
everyone  ::= "everyone" ;
condition ::= attribute _s op _s value | attribute _sp "in" _s "{" _s value ( _s "," _s value )* _s "}" ;
attribute ::= word ;
op        ::= "<=" | ">=" | "!=" | "=" | "<" | ">" ;
value     ::= number | word | quoted ;
number    ::= "-"? [0-9]+ ( "." [0-9]+ )? ( [eE] [+\-]? [0-9]+ )? ;
word      ::= [A-Za-z_] [A-Za-z0-9_.\-]* ;
quoted    ::= "\"" ( [^"\\\n] | "\\" [^\n] )* "\"" ;

_standard   ::= normative | comparative | expressed | felt ;
normative   ::= "normative" _sp indicator _sp side _sp level ;
felt        ::= "felt" _sp indicator _sp side _sp level ;
comparative ::= "comparative" _sp indicator _sp direction ( _sp "than" _sp reference )? ;
expressed   ::= "expressed" _sp demand _sp "over" _sp provision ;
side        ::= "above" | "below" ;
direction   ::= "higher" | "lower" ;
level       ::= number percent? ;
percent     ::= "%" ;
indicator   ::= word ;
demand      ::= word ;
provision   ::= word ;
)BNF";

}  // namespace needs::language
