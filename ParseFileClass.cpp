#include "ParseFileClass.h"

  const char ParseFileClass_VERSION[]= "2.0.1";
/*
 * Programmer:	Pepperdirt
 * github:	github.com/pepperdirt
 *
         Latest update 2017/12/07 - Version 2.0.1
				    Fixed Bug: doesn't crash for passing nulled string in constructor; 
				    Version 2.0.0
                                    Modified argList type findPos to accept unsigned chr;
                                    Added rawPtr access; 
                                    Fixed Bug:
                                        - logic err in findPos() returning 0 
                                          if int paramLen is smaller than strlen;
                                        - possible buffer overflow fix 
                                          MAIN LOOP: fileLength-strLen; 
                                        - getline starts by giving return string a
                                          default value: retStr[0]='\0';
                                  - Version 1.0.2
                                    Modified VERSION variable to be unique; 
                       11/23/2016 - Version 1.0.1
                                    Added getLineMultDeliminators()
                                        * Same function as getLine(),
                                          however, last parameter(3rd) is a string
                                          that contains ALL deliminators to 
                                          search for while terminating a getLine();
                                          Singe chars are deliminators from arg3
                                    FIXED getLine()
                                        * Line 134? off-by-one in main for loop
                                          Changed < to <=; Allowing final char
                                          to be searched ( inf.Loop );
                       11/10/2016 - Version 1.0.0
                                  - Modified findPos()
                                        * does not change pointerPos on exit
                                          Returns pos of arg1 + 1
                                  - Modified private member: file;
                                        * converted from const unsigned char * const
                                          to unsigned char *
                                          will be able to modify this pointer now;

                       11/??//201 - Version 0.0.0
                                  - Original Version
                                        * all actions change pointerPos;
                                          changing this in future versions;6
*/

ParseFileClass::ParseFileClass(const char fName[]) 
: fileLength(getFileLengthInConstructor(fName) ),
  file(getFileInConstrutor(fName) ),  
  ptrPosition(0)
{ }

ParseFileClass::~ParseFileClass() 
{
    if( file ) { delete [] file; } 
    file = 0;
}

// ret pos of agr1 in file; 
// if arg3, then returns pos after arg3(if found), but does not add to retString
std::size_t ParseFileClass::findPos(const unsigned char paramSearchString[],
                                           const unsigned char endDelimString[]=(const unsigned char *)"\0",
                                           const int paramLen =0,
                                           const int searchDirection =0 ) const // 0 == positive direction; 1 == negative; 
{
    if( !paramSearchString || ptrPosition == fileLength) { return 0; }
    int iterator= 1;
    if( searchDirection ) { iterator = -1; }

    int strLen = paramLen;
    if( strLen == 0 ) { 
        while( paramSearchString[strLen] ) { strLen++; }
    }
    if( strLen == 0 ) { return 0; }

    if( paramLen ) {  // checks for out of bounds arg3;
        int i = 0;
        while( paramSearchString[i] ) { i++; }
        if( i < paramLen ) { return 0; } // ParamLen exceeds buffer!
    }

    // creates unsigned copy of endDelim;
    unsigned int count = 0;; 
    int delimLen = 0; while( endDelimString[ delimLen ] ) { delimLen++; }

    std::size_t pos = ptrPosition;
    int match =0;
    while( pos <= fileLength-strLen ) { // even if -iterator, numWrap will exceed fileLen;
        match =0;
        while( match != strLen ) {
            if( file[pos+match] == paramSearchString[match] ) { match++; } 
            else break;
        }

        if( match == strLen ) { return pos; }
        
        match = 0;
        while( match != delimLen ) {
            if( file[pos+match] == endDelimString[match] ) { match++; }
            else break;
        }
        
        if( match!= 0 && match == delimLen ) { return 0; } // match not found before delim was; 
        pos+=iterator;
    }
    
    
    return 0;
}

// ret pos after recieved text. 
// if arg3, then returns pos OF arg3(if found)
// RetStr[] will not contain *endString;
// ptrPosition modified; == pos After match; if !Match, ptrPosition is unchanged;
std::size_t ParseFileClass::getLine(unsigned char retStr[], 
                          const std::size_t len,
                          const unsigned char *endString = (const unsigned char *)"\0" ) 
{
    retStr[0]='\0';
    if( !retStr || ptrPosition == fileLength ) { return 0; } // bounds check

    // creates unsigned copy of endStr;
    std::size_t count=0; while( endString[count] ) { count++; }
    unsigned char endStr[ count+1 ];
    for(int i=0; endString[i]; i++) { endStr[i] = endString[i]; }
    endStr[ count ] = '\0';

    std::size_t endStrLen = 0;
    while(endStr[endStrLen]) { endStrLen++; }

    retStr[0]='\0';
    
    std::size_t i=0;
    int match =0;
    while( i < len && i+ptrPosition <= fileLength - endStrLen ) {
        while(match < endStrLen ) { 
            if( file[i+ptrPosition+match] == endStr[match] ) { match++; }
            else  { break; }
        }
        if( match!=0 && match == endStrLen ) { ptrPosition+= match; break; }  // breaks if terminating string is found.   
        retStr[i] = file[i+ptrPosition];

        i++;
        match = 0;
    }
    retStr[i]= '\0';

    ptrPosition+= i;

    if( match!=0 && match==endStrLen ) return ptrPosition;
    if( ptrPosition == fileLength ) { return 0; }
    return ptrPosition;        
}

// ret pos after recieved text. 
// if arg3, then returns pos after arg3(if found)
// RetStr[] will not contain *endString;
// ptrPosition modified; == pos After match; if !Match, ptrPosition is unchanged;
std::size_t ParseFileClass::getLineMultDeliminators(unsigned char retStr[], 
                          const std::size_t len,
                          const unsigned char *endString = (const unsigned char *)"\0" ) 
{
    retStr[0]='\0';
    if( !retStr || ptrPosition == fileLength ) { return 0; } // bounds check

    // creates unsigned copy of endStr;
    std::size_t count = 0;while( endString[count] ) { count++; }
    unsigned char endStr[ count+1 ];
    for(int i=0; endString[i]; i++) { endStr[i] = endString[i]; }
    endStr[ count ] = '\0';

    std::size_t endStrLen = 0;
    while(endStr[endStrLen]) { endStrLen++; }
    if( endStrLen == 0 ) { endStrLen++; } // 1 char even when it == '\0';

    retStr[0]='\0';
    
    std::size_t i=0;
    int match =0;
    while( i < len && i+ptrPosition <= fileLength - 1 ) {  // - endStrLen ) {
        while(match < endStrLen ) { 
            if( file[i+ptrPosition ] == endStr[match] ) { break; } // increments every time not hit;
            match++;
        }
        if( match != endStrLen ) {ptrPosition+= 1; break; }  // breaks if terminating string is found.; MATCH MAY == 0; See above; +1 == one past DELIM;   
        retStr[i] = file[i+ptrPosition];

        i++;
        match = 0;
    }
    retStr[i]= '\0';

    ptrPosition+= i;

    if( match != endStrLen ) return ptrPosition;
    if( ptrPosition == fileLength ) { return 0; }
    return ptrPosition;        
}


              
std::size_t ParseFileClass::read(unsigned char retStr[],
                        const std::size_t len ) 
{  //&& len+ptrPosition < fileLength 
    if( !retStr || ptrPosition == fileLength ) { return 0; }
    
    std::size_t i=0;
    for( ; i< len && i+ptrPosition<fileLength; i++ ) {
        retStr[i] = file[i+ptrPosition]; 
    }
    retStr[i] = '\0';
    
    ptrPosition+= i;
    return ptrPosition;
}


// deletes arg1 number of characters from member variable ptrPosition;
int ParseFileClass::deleteCharacters( std::size_t numDelete )
{
    std::size_t end = getFileLength();
    if( ptrPosition+numDelete > end ) { return 0; } 
    
    for(std::size_t i = 0; i <= end - (ptrPosition + numDelete ); i++) { 
        
        file[ ptrPosition+i ] = file[ ptrPosition+i+numDelete ];
    }
    
    setFileLength( end - numDelete );
    
    return 1;
}



 std::size_t ParseFileClass::getFileLengthInConstructor(const char fName[]) 
{
    if( !fName ) { return 0; }
    std::ifstream in;
    in.open(fName, std::ios::binary);
    if( !in ) { return 0; }
    
    in.seekg(0, std::ios::end);
    if( !in.good() ) { in.close(); return 0; }
    std::size_t len = in.tellg();
    in.close();

    return len;
}

unsigned char * ParseFileClass::getFileInConstrutor(const char fName[]) 
{
    if( fileLength ==0 ) { 
        unsigned char *FILE = new unsigned char[1+1];
        FILE[0]='\0';
        
        return FILE; }
    std::ifstream in;
    in.open(fName, std::ios::binary);
    
    unsigned char *FILE = new unsigned char[fileLength+1];
    in.read( (char *)FILE, fileLength );
    in.close();
   
    return FILE;
}
