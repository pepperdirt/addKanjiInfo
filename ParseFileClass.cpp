#include "ParseFileClass.h"

  const char ParseFileClass_VERSION[]= "2.0.1a";
/*
 * Programmer:	Pepperdirt
 * github:	github.com/pepperdirt
 *
         Latest update 2017/12/07 - Version 2.0.1a
                                    + Uses memcmp in comparisons(speedy)
                                  - Version 2.0.1_
                    + Updated to comply w/cpp standards
				    + Cleaning up code;
	 			    Version 2.0.1
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


/* // Vrsion 0
// ret pos of agr1 in file; 
// if arg3, then returns pos after arg3(if found), but does not add to retString
std::size_t ParseFileClass::findPos(const unsigned char paramSearchString[],
                                           const unsigned char endDelimString[]=(const unsigned char *)"\0",
                                           const int &paramLen =0,
                                           const int &searchDirection =0 ) const // 0 == positive direction; 1 == negative; 
{
    if( !paramSearchString || ptrPosition == fileLength) { return 0; }
    int iterator= 1;
    if( searchDirection ) { iterator = -1; }

    int strLen = paramLen;
    if( strLen == 0 ) { 
        while( paramSearchString[strLen] ) { strLen++; }
    }
    if( strLen == 0 ) { return 0; }

//    if( paramLen ) {  // checks for out of bounds arg3;
//        int i = 0;
//        while( paramSearchString[i] ) { i++; }
//        if( i < paramLen ) { return 0; } // ParamLen exceeds buffer!
//    }

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
*/


// Version 2; 
//   - Not much faster. A little less ledgible. Probably scrap for orignial... Maybe. 
// ret pos of agr1 in file; 
// if arg3, then returns pos after arg3(if found), but does not add to retString
std::size_t ParseFileClass::findPos(const unsigned char *paramSearchString,
                                           const unsigned char *endDelimString=(const unsigned char *)"\0",
                                           const int &paramLen =0,
                                           const int &searchDirection =0 ) const // 0 == positive direction; 1 == negative; 
{
    if( !paramSearchString || ptrPosition == fileLength) { return 0; }
    int iterator= 1;
    if( searchDirection ) { iterator = -1; }

    std::size_t strLen = paramLen;
    if( strLen == 0 ) { 
        while( paramSearchString[strLen] ) { strLen++; }
    }
    if( strLen == 0 ) { return 0; }

//    if( paramLen ) {  // checks for out of bounds arg3;
//        int i = 0;
//        while( paramSearchString[i] ) { i++; }
//        if( i < paramLen ) { return 0; } // ParamLen exceeds buffer!
//    }

    // creates unsigned copy of endDelim;
    int delimLen = 0; while( endDelimString[ delimLen ] ) { delimLen++; }

    const unsigned char *const zeroPosition = file;
    
    if( ptrPosition <= fileLength-strLen ) { // even if -iterator, numWrap will exceed fileLen;

        std::size_t endPointerPos = (fileLength - (strLen - 1));
        if( iterator < 0 ) {
              endPointerPos = 0;
        }
        file += ptrPosition; 
        while( file != ( zeroPosition+endPointerPos ) ) { 
        
            if( memcmp( paramSearchString, file, strLen ) == 0 ) { // std::cout << "MATCH!; ";           
                // REPURPOSE strLen. == posOfMatch; 
                strLen = (file) - zeroPosition;
                file -= strLen;
                return  strLen;
            }
            
            if( delimLen && memcmp( endDelimString, file, delimLen ) == 0 ) { // std::cout << "MATCH!; ";           
                strLen = 0;
                break;         
            }
                          
            file+= iterator; 
        }

        file -= (file - zeroPosition);
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
    int END_STR_LENGHT= strlen((char *)endString );
    const int MAX_ENDSTR = 254;
    unsigned char endStr[ MAX_ENDSTR+1 ];
    int l=0;
    for( l=0; l < MAX_ENDSTR && l < END_STR_LENGHT; l++) { endStr[l] = endString[l]; }
    endStr[ l ] = '\0';
    END_STR_LENGHT  = l;

    retStr[0]='\0';
   
    std::size_t i = 0; 
    int match =0;
    while( i < len && i+ptrPosition <= fileLength - END_STR_LENGHT ) {
        while(match < END_STR_LENGHT ) { 
            if( file[i+ptrPosition+match] == endStr[match] ) { match++; }
            else  { break; }
        }
        if( match!=0 && match == END_STR_LENGHT ) { ptrPosition+= match; break; }  // breaks if terminating string is found.   
        retStr[i] = file[i+ptrPosition];

        i++;
        match = 0;
    }
    retStr[i]= '\0';

    ptrPosition+= i;

    if( match!=0 && match==END_STR_LENGHT ) { return ptrPosition; }
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
    int count = 0;while( endString[count] ) { count++; }
    const int ENDSTR_MAX = 254;
    unsigned char endStr[ ENDSTR_MAX+1 ];
    count = 0;
    for( ; count < ENDSTR_MAX && endString[count]; count++) { endStr[count] = endString[count]; }
    endStr[ count ] = '\0';

    std::size_t endStrLen = count;

    retStr[0]='\0';
    
    std::size_t i=0;
    unsigned int match =0;
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
