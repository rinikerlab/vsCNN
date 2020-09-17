/*

MIT License

Copyright (c) 2020, R. Gregor Weiß, Benjamin Ries

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE
*/

#ifndef PARSER_ARGPARSE_H
#define PARSER_ARGPARSE_H

#include <iostream>

#include <string>
#include <vector>
#include <map>
#include <boost/variant/variant.hpp>
#include <boost/variant/get.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/any.hpp>

#include "Argument.h"

using namespace std;

/////////////////////////////////////////////////////////////////////////////////////
typedef boost::variant< boost::shared_ptr< Argument<string> >,
        boost::shared_ptr< Argument<bool> >,
        boost::shared_ptr< Argument<int> >,
        boost::shared_ptr< Argument<unsigned int> >,
        boost::shared_ptr< Argument<size_t> >,
        boost::shared_ptr< Argument<double> >,
        boost::shared_ptr< Argument<float> > > FlexArgument;
typedef std::map< std::string, FlexArgument > FlexMap;
//////////////////////////////////////////////////////////////////////////////////////

class ArgParse {

public:

    // Constructor
    ArgParse(int argc, char* argv[] );

    // Destructor
    ~ArgParse() {
        for ( int i = 0; i < _argc; i++ )
        { delete[] _argv[i]; }
        delete[] _argv;
    }

    // routines to handle user flags
    template <typename PT> void insert_flag( const string, const PT, int const );
    template <typename PT> void insert_flag( const string, const string, const PT, int const );
    template <typename PT> PT flag( const string& );
    template <typename PT> PT flag( const unsigned, const string& );
    template <typename PT> PT flag( const string&, const PT );
    template <typename PT> PT flag( const string&, const string&, const PT );
    template <typename PT> PT flag( const unsigned, const string&, const PT, int const );
    template <typename PT> bool is_parsed( const string& );
    template <typename PT> int flag_size( const string );

private:

    template <typename PT> void insert_flag( boost::shared_ptr< Argument<PT> > );
    template <typename PT> void insert_flag( const string, const PT, int const,  int argc,  char* argv[] );
    template <typename PT> void insert_flag( const string, const string, const PT, int const,  int argc,  char* argv[] );

    int _argc;
    char** _argv;

    FlexMap iMap; // contains the flags
};


/*
* PUBLIC
* routines to handle user flags
*/

// inserts and defines a new object of class type Argument<PT> into iMap
template <typename PT> void ArgParse::insert_flag( boost::shared_ptr< Argument<PT> > newflag )
{
    if ( iMap.count( newflag->id(0) ) == 0 ) {
        FlexArgument FP = newflag;
        iMap[ newflag->id(0) ] = FP;
    } else {
        newflag.reset();
    }
}

// inserts and defines a new object of class type Argument<PT> into iMap
template <typename PT> void ArgParse::insert_flag( const string id, const PT val, int const size,
        int argc, char* argv[] ) {
    boost::shared_ptr< Argument<PT> > P( new Argument<PT>( id, val, size ) );
    P->parse( argc, argv );
    insert_flag<PT>( P );
}

// inserts and defines a new object of class type Argument<PT> into iMap
template <typename PT> void ArgParse::insert_flag( const string id1, const string id2, const PT val, int const size,
        int argc, char* argv[] ) {
    boost::shared_ptr< Argument<PT> > P( new Argument<PT>( id1, id2, val, size ) );
    P->parse( argc, argv );
    insert_flag<PT>( P );
}

// inserts and defines a new object of class type Argument<PT> into iMap
template <typename PT> void ArgParse::insert_flag( const string id, const PT val, int const size ) {
    insert_flag<PT>(id, val, size, _argc, _argv );
}

// inserts and defines a new object of class type Argument<PT> into iMap
template <typename PT> void ArgParse::insert_flag( const string id1, const string id2, const PT val, int const size ) {
    insert_flag<PT>(id1, id2, val, size, _argc, _argv );
}

// obtains the value parsed with flag "key"
template <typename PT> PT ArgParse::flag( const string& key ) {
    return boost::get< boost::shared_ptr< Argument<PT> > >( iMap.at(key) )->val(0);
}

// obtains the value parsed into with flag "key" at position "n"
template <typename PT> PT ArgParse::flag( const unsigned n,  const string& key) {
    return boost::get< boost::shared_ptr< Argument<PT> > >( iMap.at(key) )->val(n);
}

// obtains the value parsed with flag "key"
template <typename PT> PT ArgParse::flag( const string& key, const PT val ) {
    insert_flag<PT>( key, val, 1 );
    return flag<PT>(key);
}

// obtains the value parsed with flag "key"
template <typename PT> PT ArgParse::flag( const string& key1, const string& key2, const PT val ) {
    insert_flag<PT>( key1, key2, val, 1 );
    return flag<PT>(key1);
}

// obtains the value parsed into with flag "key" at position "n"
template <typename PT> PT ArgParse::flag( const unsigned n, const string& key, const PT val, int const size ) {
    insert_flag<PT>( key, key, val, size );
    return flag<PT>( key, n );
}

// checks whether the flag "key" was ever parsed by the user or whether it carries the default value
template <typename PT> bool ArgParse::is_parsed( const string& key ) {
    return boost::get< boost::shared_ptr< Argument<PT> > >( iMap.at(key) )->is_parsed();
}

// checks the size of the parsed array corresponding to the flag "key"
template <typename PT> int ArgParse::flag_size( const string key ) {
    return boost::get< boost::shared_ptr< Argument<PT> > >( iMap.at(key) )->size();
}

//#include "ArgParse.cpp"

#endif //PARSER_ARGPARSE_H
