#include "jmdict_InfoClass.h"

/*
JMdict=eExtractor.cpp
<keb>
 <reb> ???        
 <sense> 
 <pos> GRAMMAR_ENG </> 
 <xref> CROSS-REFERENCE </>
 <misc> DESCRIPTION( of sence )</>
 <gloss> ENGLISH DEFINITION </>
 </sence>... Sometimes multiple sences!
<keb>

kanji kana extraENGLISH
*/


namespace kanjiDB { 

  const char jmdict_InfoClass_VERSION[]= "1.0.0";
/*
 * Programmer:	Pepperdirt
 * github:	github.com/pepperdirt
 *
         Latest update 2017/12/24 - Version 1.0.0
                                    Optimization Level Added. 
                                  - Version 0.0.1
*/

jmdict_InfoClass::jmdict_InfoClass(const char fName[],
                                   const OPTIMIZE &O) : OPTIMIZE_LEVEL( O.getVal() ),
                                                         KanjiInfoClass(fName)  
/*: fileLength(getFileLengthInConstructor(fName) ),
  file(getFileInConstrutor(fName) ),  
  ptrPosition(0) */
{  
/*
<entry>
<keb> ?? </keb>
 <k_ele>
   <reb> ??? </reb> <!-- furigana / somteimes multiple -->
 </k_ele>
 <sense> 
 <pos> GRAMMAR_ENG </> 
 <xref> CROSS-REFERENCE </>
 <misc> DESCRIPTION( of sence )</>
 <gloss> ENGLISH DEFINITION </>
 <gloss xml:lang="dut"> </> <!-- dut/ger/hun/swe ??? -->
 </sence>... Sometimes multiple sences!
</entry>
*/

    // FILL: indexOfendingKanjiOnly_keyTable[POTENTIAL_NUM_OF_OKURIGANA];
    for(int i=0; i <= POTENTIAL_NUM_OF_OKURIGANA; i++) { 
        indexOfendingKanjiOnly_keyTable[ i ] = 0; 
    }
    for(int i=0; i <= POTENTIAL_NUM_OF_OKURIGANA; i++) { 
        indexOfbeginingKanjiOnly_keyTable[ i ] = 0; 
    }
    
    std::size_t pos = 3; // FIRST KANJI; 
    unsigned char keb[] = "<keb>";
    unsigned char endKeb[] = "</";
    
    std::vector<int> kebLens;
    std::vector<unsigned int> kebMults;
    std::vector<std::size_t> kebPos_unsorted;
    
    int kebSize = 0;
    
    std::size_t diff = 0;
    
    std::vector<std::size_t> keysIndex;
    std::size_t len = KanjiInfoClass::fileLen();
    // Split table in two sections for quicker lookups
    // 1st: kanji+kanji
    // 1st: kanji+kana+kanji
    // 2nd: kanji+kana
    // 3rd: 
    // Nah. We'll be looking for matches based on Length;
    // 1st: len==3
    // 2nd: len==6
    // 3rd: len==9
    int maxLengthOfKeb = 0;
    int lengthOfKeb = 0;
    std::size_t p = 0;
    int match = 0;

    // FIND FIRST KEB !!
        match = 0; 
        pos = KanjiInfoClass::searchStr( keb, pos );
        pos+= 5;
        
        // SIGNATURE: <keb>???*</keb>
        // keysIndex.push_back( pos+5 ); // pos - 3 == KANJI [ ? ? ? ];

        // skips past keb delim; ( moves position pointer past '<keb>' )        
        lengthOfKeb = KanjiInfoClass::searchStr( endKeb, pos ) - pos;
        kebLens.push_back( lengthOfKeb );
        kebMults.push_back( 1 );
        kebSize++;
    // END FIRST KEB !!

    // First sweep finds kanji with okurigana 
    while( true ) {
        match = 0; 
        pos = KanjiInfoClass::searchStr( keb, pos );
        if( pos == 0 ) { break; } // FAILED TO FIND ANYTHING;         
        pos+= 5;
        kebPos_unsorted.push_back( pos );
        
        // SIGNATURE: <keb>???*</keb>
        // keysIndex.push_back( pos+5 ); // pos - 3 == KANJI [ ? ? ? ];

        // skips past keb delim; ( moves position pointer past '<keb>' )        
        lengthOfKeb = KanjiInfoClass::searchStr( endKeb, pos ) - pos;
        while( match < kebSize ) { 
            if( kebLens[match] == lengthOfKeb ) { 
                break; 
            } 
            
            match++; 
        }
        
        if( match == kebSize ) { 
// std::cout << "len("<< lengthOfKeb << "): "<< KanjiInfoClass::searchStr( endKeb, pos )-pos << std::endl;            
            // Not a duplicate, add it now!
            kebLens.push_back(lengthOfKeb);
            kebSize++;  
            
            if( lengthOfKeb > maxLengthOfKeb ) { 
                maxLengthOfKeb = lengthOfKeb; 
            }
            
            kebMults.push_back( 1 );            
        } else { 
            kebMults[match]+= 1; 
        }
        
        pos+= lengthOfKeb;
    }
        
    // Have data needed to split lookup table into sections
    // Based on length of entry;
    // - numOfKanjiOnly_IndexLookup[]
    //     Index values "[ ??? ]" are lengths of entries within <keb>??</keb>
    //     tags. 
    //     Value extracted from [] is the number of KANJI ONLY <keb></keb>;
    //     The following( after [] ) are kanji w/okurigana, or the like.
//    std::cout << "maxLength: "<< maxLengthOfKeb 
//              << "pos: "<< p 
//              << "kebSize: " << kebSize << std::endl;
    // sort based on kebLength
    for(int i=0; i < kebSize; i++) { 
        int smallest = kebLens[i];
        int indx = i;
        
        for(int k = i; k < kebSize; k++) { 
            if( kebLens[ k ] < smallest ) { 
                smallest = kebLens[ k ];
                indx = k;
            }
        }
        
        if( i != indx ) { 
            smallest = kebLens[ i ];
            kebLens[ i ] = kebLens[ indx ];
            kebLens[ indx ] = smallest; 
        }
    }
  
  

    int FINAL_LEN_COUNT = 0;
    highestSizeValue = maxLengthOfKeb;
    // Temporarily use this as keytable; 
    KanjiInfoClass::setKeyTable( kebPos_unsorted );
    
    const unsigned char *db = KanjiInfoClass::getDB();
    unsigned char buff[maxLengthOfKeb+1];
    //std::vector<int> kebLens;
    //std::vector<std::size_t> kebMults;
    std::vector<std::size_t> kebPositions_sorted;
    
    std::size_t NUM_ENTRIES = kebPos_unsorted.size();
    // Make 2 passes.
    //  One for pushing KANJI ONLY onto kebPositions
    //  Two for pushing KANJI+KANA / KANA+KANJI onto kebPositions;
    //  At Two's start, place Two's index into 
    //  numOfKanjiOnly_IndexLookup[ LENGTH_OF_KEB_ENTRY ] = Two'sIndex; 
    for(int i=0; i < kebSize; i++) { // kebSize == num Lens; 
std::size_t multiples = 0;
        int currentLen = kebLens[i];
        int numFound = 0;
        // First Pass; (KANJI ONLY) accepted. 
        for(std::size_t j=0; j < NUM_ENTRIES; j++) { 
            if( kebPos_unsorted[j] ) {
                pos = KanjiInfoClass::searchStr(endKeb,  kebPos_unsorted[j] );
                lengthOfKeb = pos - kebPos_unsorted[j];
                pos -= lengthOfKeb;
                
                for(int k=0; k < lengthOfKeb; k++) { buff[k] = db[pos+k]; }
                buff[lengthOfKeb] = '\0';
                
                if( lengthOfKeb == currentLen &&
                    !posOfHiragana(buff, 0) &&
                    !posOfKatakana(buff, 0) 
                  ) 
                { 
                    if( !indexOfbeginingKanjiOnly_keyTable[ lengthOfKeb  ] ) {
                        indexOfbeginingKanjiOnly_keyTable[ lengthOfKeb ] 
                        = kebPositions_sorted.size() + 1; 
                    }

                    // if no kana present, add to kebPositions_sorted;
                    // Next pass ( next loop, can add kana also;
                    kebPositions_sorted.push_back( pos );
multiples++;
                    kebPos_unsorted[j] = 0; // Dont re-use;
                    
                    if( i+1 == kebSize ) { 
                        // Final kebs in array; Count how many
                        // Place in ending delim's place;
                        FINAL_LEN_COUNT+= 1;
                    } 
                }
            }
        }

        // Place startingPosition of Leftover( okurigana ) in 
        // numOfKanjiOnly_IndexLookup[ j ]
        
multiples = 0;        
        int holdFINAL = FINAL_LEN_COUNT;
        FINAL_LEN_COUNT = 0;
        // SECOND PASS; Accept regardless ( non-zero'd kebPos_unsorted[j];
        for(std::size_t j=0; j < NUM_ENTRIES; j++) { 
            if( kebPos_unsorted[j] ) {
                pos = KanjiInfoClass::searchStr(endKeb,  kebPos_unsorted[j] );
                lengthOfKeb = pos - kebPos_unsorted[j];
                pos -= lengthOfKeb;
                                
                if( lengthOfKeb == currentLen ) 
                { 
                    if( !indexOfendingKanjiOnly_keyTable[ lengthOfKeb  ] ) {
                        indexOfendingKanjiOnly_keyTable[ lengthOfKeb  ] 
                        = kebPositions_sorted.size() + 1; 
                    }

                    // if no kana present, add to kebPositions_sorted;
                    // Next pass ( next loop, can add kana also;
                    kebPositions_sorted.push_back( pos );
multiples++;
                    kebPos_unsorted[j] = 0; // Dont re-use; 

                    if( i+1 == kebSize ) { 
                        // Final kebs in array; Count how many
                        // Place in ending delim's place;
                        FINAL_LEN_COUNT+= 1;
                    } 
                    
                }
            }            
        }
        
        if( !FINAL_LEN_COUNT ) { FINAL_LEN_COUNT = holdFINAL; }
        if( FINAL_LEN_COUNT ) { 
            if( holdFINAL ) { // indexBeg holds final ;
                indexOfendingKanjiOnly_keyTable[highestSizeValue+1] = 
                indexOfbeginingKanjiOnly_keyTable[highestSizeValue] + FINAL_LEN_COUNT;
            }
            else { // indexEnd holds final;
                indexOfbeginingKanjiOnly_keyTable[highestSizeValue+1] = 
                indexOfendingKanjiOnly_keyTable[highestSizeValue] + FINAL_LEN_COUNT;
            }
            
        }
    } 
    KanjiInfoClass::setKeyTable( kebPositions_sorted );
    
    
}

jmdict_InfoClass::~jmdict_InfoClass() 
{
//    if( file ) { delete [] file; } 
//    file = 0;
}

// Call setKanji(unsigned char *kanji) before calling kunyomi;
std::vector<ustring> jmdict_InfoClass::kunyomi() const
{
    std::vector<ustring> words ;//this vector will be returned    
    return words; 
    
    const unsigned char * tmp = getDB();
    const std::size_t KANJI_POS = getPos();
    const unsigned char Compounds[]="Compounds:";
    const unsigned char HARD_ENTER[]="\x0D\x0A";
    tmp+= KANJI_POS; // NEXT LINE; 
    std::size_t farPtrKana = 0 ; //posOfHiragana( tmp ); // offset from tmp; 
    if( farPtrKana ) { farPtrKana--; }    

    // Test for Hiragana
    std::size_t FILE_offsetCompounds = searchStr( Compounds );

    if( FILE_offsetCompounds < (getPos() + farPtrKana) ) { 
        return words; 
    }

    tmp+= farPtrKana;
    
    // Hiragana present; Find ending pos; 
    FILE_offsetCompounds = searchStr( HARD_ENTER, KANJI_POS+farPtrKana );
    
    // length; Or,  IndexOf( 0x0D ); 
    int lenghtOfKana = FILE_offsetCompounds - (KANJI_POS + farPtrKana);
    
    // Adds the Kana to words; 
    unsigned char kana[80];
    for(int i=0; i < lenghtOfKana; i++) { 
        int len = 0;
        while( i+len < lenghtOfKana&&
               tmp[i+len] != ','  &&
               tmp[i+len] != 0x0D &&
               tmp[i+len] != 0x0A  )
        { 
            kana[len] =  tmp[i+len];   
            len++;
        }
        kana[len] = '\0';
        
        words.push_back(kana);

        // Finds next Kana; 
        len++; // skips over delim ( , ); 
        while( tmp[i+len] == ' ' ) { len++; }
        i+= len - 1;
    }   
    tmp-= ( KANJI_POS + farPtrKana );
   
    return words;
}


// Call setKanji(unsigned char *kanji) before calling onyomi;
std::vector<ustring> jmdict_InfoClass::onyomi() const
{                     
    std::vector<ustring> words ;//this vector will be returned    
    return words;
    
    const unsigned char Compounds[]="Compounds:"; 

    const unsigned char * tmp = getDB();
    tmp+= getPos();
    std::size_t farPtrKana = 0; // posOfKatakana( tmp ); // offset from tmp; 
    if( farPtrKana ) { farPtrKana--; }
    std::size_t FILE_offsetCompounds = searchStr( Compounds ); // offset from beg file; 

//std::cout << "getPos(): "<< getPos() << std::endl
  //        << "farPtrKana: "<< farPtrKana
    //      << "; FILE_offsetCompounds: "<< FILE_offsetCompounds 
      //    << std::endl;
    
    if( FILE_offsetCompounds < (getPos() + farPtrKana) ) { 
//std::cout << "returned ;-;\n";
//std::cout << "    if( FILE_offsetCompounds < (getPos() + farPtrKana) ) { " 
//          << std::endl
  //        << FILE_offsetCompounds << "< " << std::endl
    //      << (getPos() + farPtrKana) << std::endl;
        // No Katakana present; 
        // Cannot proceed without finding "Compounds" also ( Buffer too small) ??
        return words; 
    }
    
    // -2 == 0x0D 0A before Compounds; 
    int lenghtOfKana = FILE_offsetCompounds - (getPos() + farPtrKana) - 2;
    tmp-= getPos();
    tmp+= getPos() + farPtrKana;

    // Adds the Kana to words; 
    unsigned char kana[80];
    for(int i=0; i < lenghtOfKana; i++) { 
        int len = 0;
        while( i+len < lenghtOfKana&&
               tmp[i+len] != ','  &&
               tmp[i+len] != 0x0D &&
               tmp[i+len] != 0x0A  )
        { 
            kana[len] =  tmp[i+len];   
            len++;
        }
        kana[len] = '\0';
        
        words.push_back(kana);

        // Finds next Kana; 
        len++; // skips over delim ( , ); 
        while( tmp[i+len] == ' ' ) { len++; }
        i+= len - 1;
    }
    
    tmp-= (farPtrKana + getPos());
    return words; 
}


void jmdict_InfoClass::help(
                                         unsigned char *retval 
                                       ) 
{
    retval[0]='\0';
    return;
                                           
    const unsigned char * tmp = getDB();
    const unsigned char Compounds[]="Compounds:";
    const unsigned char helpENTER[] = "(help)\x0D\x0A";
    const unsigned char HARD_ENTER[] = "\x0D\x0A";
    
    std::size_t KANJI_POS = getPos();
    tmp+= KANJI_POS; // NEXT LINE; 
    std::size_t begPos = searchStr( Compounds ) + 12; 
    
    tmp-= KANJI_POS;
    tmp+= begPos; 
    
    // Find next Index ( +1 );
    std::size_t indexKey = KanjiInfoClass::getIndex() + 1;
    if( indexKey >= getKeySize() ) { return; }
    
    
    // Test for "(help)" Remove this if present; 
    std::size_t endPos = getKeyPos( indexKey );
    if( endPos >= (6+ searchStr( helpENTER, begPos )) ) { 
        endPos = searchStr( helpENTER, begPos ); // NOT END POS; 
    } else { 
        return ; // No help Avaliable; 
    }

    std::size_t pos = begPos;
    while( true ) { 
        std::size_t p = searchStr( HARD_ENTER, pos+1 );
        if( p&& p < endPos ) { pos = p+2; } 
        else { break; }
    }
    tmp-= begPos;
    if( pos ) { begPos = pos; }
    tmp+= begPos;

    // length; Or,  IndexOf( 0x0D ); 
    int lenghtOfKana = endPos - begPos;    
     
    // Adds the Kana to words; 
    int len = 0;
    while( len < lenghtOfKana&&
           tmp[len] != 0x0D &&
           tmp[len] != 0x0A  )
    { 
        retval[len] =  tmp[len];   
        len++;
    }
    retval[len] = '\0';

    tmp-= begPos;
    return; 
}

void jmdict_InfoClass::translate(unsigned char *retval)
{
    retval[0]='\0';
    int retLen = 0;
    unsigned char tmp[380+1];

    const unsigned char gloss[]="<gloss>";
    const int GLOSS_DELIM_LEN = strlen( (char *)gloss );
    const unsigned char endGlossPart1[] = "</sense>";
    const unsigned char genericEnd[] = "</";
    std::size_t KANJI_POS = getPos();
    std::size_t endPos;
    std::size_t ENTRY_END = searchStr( endGlossPart1, KANJI_POS );

    
while( (KANJI_POS = searchStr( gloss, KANJI_POS )) ) { 

    if( KANJI_POS > ENTRY_END ) { break; }
    KANJI_POS += GLOSS_DELIM_LEN;

    endPos = searchStr( genericEnd, KANJI_POS ); 
    int lenghtOfKana = endPos - KANJI_POS;
    if( lenghtOfKana > 180 ) { lenghtOfKana = 380; }


    // Adds the Kana to words; 
    readStr( tmp, lenghtOfKana, KANJI_POS );
    
    if( retLen  ) {     
        retval[retLen] = ',';
        retval[retLen+1] = ' ';
        retLen+=2;
    }

    int len = 0;
    while( len < lenghtOfKana  )
    { 
        retval[retLen+len] =  tmp[len];   
        len++;
    }
    retLen+= len; 

    KANJI_POS+= lenghtOfKana;
}
    retval[retLen] = '\0';

    return ;        
}

// If Hiragana found, returns POS+1
/// SIMPLY pos of Kana(hir / kat );
// IDENTICAL TO nextKatakana; 
std::size_t jmdict_InfoClass::posOfHiragana(const unsigned char *s,
                                            const std::size_t offset = 0) const
{
    std::size_t len = 0;
    if( len == 0 ) { 
        while(s[len]){ len++; }
    }
    
    if( len <= offset ) { return 0; }

    for(std::size_t i = offset; i <= len-3; i++) { 
        if( s[i+0] == 0xE3 ) {         // SAME
            if( s[i+1] == 0x81 ) 
            {
                if( s[i+2] > 0x80
                &&  s[i+2] < 0xC0 ) { return i+1; }
            }
            else if( s[i+1]== 0x82 ) { 
                if( s[i+2] > 0x7F
                &&  s[i+2] < 0xC0 ) { return i+1; }
            }
            else if( s[i+1]== 0x83 ) { 
                if( s[i+2] > 0x7F
                &&  s[i+2] < 0xB3 ) { return i+1; }
            }
        }
    }    
    return 0;

}

std::size_t jmdict_InfoClass::posOfKatakana(const unsigned char *buff, 
                                            const std::size_t offset = 0 ) const
{
//    if( len <= offset ) { return 0; }

    return 0;
}

// Arg1: a string to search for lengths to test against database
// Arg2: highest numChars want to test
// Returns the next posible increment of chars that can be tested;
int jmdict_InfoClass::inputLenTest(const unsigned char *s, const int i = 0) const
{
    
    int testLenOfStr = i;
    if( testLenOfStr >  highestSizeValue ) { 
        testLenOfStr = highestSizeValue; 
    }

    if( i == 0 ) { 
        // check length of string;
        while( testLenOfStr <= highestSizeValue &&
               s[ testLenOfStr ] )
        {
            testLenOfStr++;
        }
     }
     
     if( testLenOfStr == 0  ) { return 0; }


    while( testLenOfStr > 0 ) { 
        if( indexOfendingKanjiOnly_keyTable[   testLenOfStr ] ||  
            indexOfbeginingKanjiOnly_keyTable[ testLenOfStr ] ) { 
            break; 
        }
        testLenOfStr--;
    }
    
    return testLenOfStr; 
}


int jmdict_InfoClass::setKanji( const unsigned char *s)
{
    int sLen = 0;
    while( s[sLen] ) { sLen++; }
    
    int kanaPos = posOfHiragana( s );//  - 1;
     if( kanaPos >= sLen ) { kanaPos = 0; }
    int kat     = posOfKatakana( s );// - 1;
     if( kat >= sLen ) { kat = 0; }
    if( kat&& kat < kanaPos ) { kanaPos = kat; }
    if( kanaPos == 0 ) { kanaPos = kat; }
    
    unsigned char endDelim[] = "</";

    std::size_t nextIndexVal;
    int numIndexesLeft;
    std::size_t index_i = 0;
    if( kanaPos 
    &&  (index_i = indexOfendingKanjiOnly_keyTable[ sLen ]) ) 
    {
        index_i--;
        // Find how many are avaliable
        nextIndexVal = nextIndex_keyTable( sLen );
        numIndexesLeft = nextIndexVal - index_i;
    }
    else if( !kanaPos 
    &&    (index_i = indexOfbeginingKanjiOnly_keyTable[ sLen ]) )
    {
        index_i--;
        // Find how many are avaliable
        nextIndexVal = (indexOfendingKanjiOnly_keyTable[ sLen ]);
        
        if( !nextIndexVal ) {
           nextIndexVal = nextIndex_keyTable( sLen );
        } else { 
           nextIndexVal--; 
        }
           
        numIndexesLeft = nextIndexVal - index_i;

    }
    else { return 1;  }// FAIL; 

    
    const std::size_t SAVED_INDEX = KanjiInfoClass::getIndex();
    for(int k = 0; k < numIndexesLeft; k++) { 
       KanjiInfoClass::setIndex( index_i );
       
       if( searchStr( s, endDelim ) ) { return 0; }
       // s NOT FOUND between <keb>???</keb>
       
       index_i++;
    }

    // Before exiting on FAIL, return original SAVED_POSITION
    KanjiInfoClass::setIndex( SAVED_INDEX );
    return 2; // err 2;       
}

// starts at startingLen, and tries to find definition indexPos
// If not found, return 0; ( MEANING: LOGIC ERROR: cannot give first <keb>[0] ; )
std::size_t jmdict_InfoClass::getIndex( const unsigned char *str, 
                                        const int startingLen  )
{
    unsigned char s[startingLen];
    for(int i=0; i < startingLen; i++) { s[i] = str[i]; }
    s[startingLen]= '\0';
              
    int sLen = startingLen; // Can be (-); 
    int kanaPos = posOfHiragana( s );//  - 1;
     if( kanaPos >= startingLen ) { kanaPos = 0; }
    int kat     = posOfKatakana( s );// - 1;
     if( kat >= startingLen ) { kat = 0; }
    if( kat&& kat < kanaPos ) { kanaPos = kat; }
    if( kanaPos == 0 ) { kanaPos = kat; }
    
    unsigned char endDelim[] = "</";

    std::size_t retIndex = 0;
    std::size_t index_i = 0;
    std::size_t pos = 0;
    if( kanaPos > 1 ) { 
        kanaPos--; 
        
        while( kanaPos < sLen ) { 
            if( (index_i = indexOfendingKanjiOnly_keyTable[ sLen ]) ) { 
               index_i--;
               // SomeKanas can be compared w/s(KEY)
               // Find how many are avaliable
               std::size_t nextIndexVal = nextIndex_keyTable( sLen );
               int numIndexesLeft = nextIndexVal - index_i;

               for(int k = 0; k < numIndexesLeft; k++) { 
//               if( sLen == highestSizeValue ) { break; }
                   
                   KanjiInfoClass::setIndex( index_i );
                   
                   if( searchStr( s, endDelim ) ) { return index_i; }
                   // s NOT FOUND between <keb>???</keb>
                   
                   index_i++;
               }
            }
            sLen--;    
            s[sLen] = '\0';
        }
        
        // sLen is before a kana;
        // NO KANA, search through KANJI only
        while( sLen > 0 ) { 
            if( (index_i = indexOfbeginingKanjiOnly_keyTable[ sLen ]) ) { 
               index_i--;
               // SomeKanas can be compared w/s(KEY)
               // Find how many are avaliable
               std::size_t nextIndexVal = (indexOfendingKanjiOnly_keyTable[ sLen ]);
               
               int numIndexesLeft; 
               if( !nextIndexVal ) {
                   nextIndexVal = nextIndex_keyTable( sLen );
               } else { 
                   nextIndexVal--; 
               }
                   
               numIndexesLeft = nextIndexVal - index_i;

               for(int k = 0; k < numIndexesLeft; k++) { 
                   
                   KanjiInfoClass::setIndex( index_i );
                   
                   if( searchStr( s, endDelim ) ) { return index_i; }
                   // s NOT FOUND between <keb>???</keb>
                   
                   index_i++;
               }
            }
            sLen--;    
            s[sLen] = '\0';            
        }

    }
    else { 
        // sLen is before a kana;
        // NO KANA, search through KANJI only
        while( sLen > 0 ) { 

            if( (index_i = indexOfbeginingKanjiOnly_keyTable[ sLen ]) ) { 
               index_i--;
               // SomeKanas can be compared w/s(KEY)
               // Find how many are avaliable
               std::size_t nextIndexVal = (indexOfendingKanjiOnly_keyTable[ sLen ]);

               int numIndexesLeft; 
               if( !nextIndexVal  ) {
                   nextIndexVal = nextIndex_keyTable( sLen );

               } else { 
                   nextIndexVal--; 
               }
                   
               numIndexesLeft = nextIndexVal - index_i;
               for(int k = 0; k < numIndexesLeft; k++) { 
                   
                   KanjiInfoClass::setIndex( index_i );
                   
                   if( searchStr( s, endDelim ) ) { return index_i; }
                   // s NOT FOUND between <keb>???</keb>
                   
                   index_i++;
               }
            }
            sLen--;    
            s[sLen] = '\0';            
        }
    }


    return 0;
}

unsigned int jmdict_InfoClass::rubify(// jmdict_InfoClass &ths, 
                                      unsigned char *ret,
                                      unsigned int retLENGTH = 0)
{
    const unsigned char endTag[] = "</";
    const unsigned char rebTag[] = "<reb>";
    
    // should have been run: Jmdict.setIndex( indexVal );
    unsigned int lenOfKeb = lenOfKanji();
    std::size_t lenOfRet = 0;
    
    unsigned char keb[320];
    std::size_t end = searchStr(endTag);
    std::size_t beg = KanjiInfoClass::getPos();
    unsigned int kebSIZE = end-beg;
    readStr(keb, end-beg ); // holds Kanji ( <keb>???</keb>
    
    unsigned char reb[320];
    beg = searchStr(rebTag) + 5;
    end = searchStr(endTag, beg);
    unsigned int rebSIZE = end-beg;
    readStr(reb, end-beg, beg); // holds kana of <keb>; 
        
    // find first kana in keb ( if present )
    // If present, split keb into many parts and
    // furigana will be placed on multiple parts
    unsigned int kebKanaPos=0;
    unsigned int holdPos = 0;
    int rebPos=0;
    
    // nextKanaPos(this, keb, 0) #PASTE
    int atLeastOneKanaPresent = 0;
        std::size_t hir = posOfHiragana( keb, 0 ); // (+1)
        std::size_t kat = posOfKatakana( keb, 0 ); // 0 on fail; 
        // Find smallest value
        if( kat && hir > kat ) { atLeastOneKanaPresent = kat; }
        else if( hir ) {         atLeastOneKanaPresent = hir; }

    
    
    while( true ) {    
            // nextKanaPos(this, keb, kebKanaPos) #PASTE
            holdPos = 0;
                hir = posOfHiragana( keb, kebKanaPos ); // (+1)
                kat = posOfKatakana( keb, kebKanaPos ); // 0 on fail; 
                // Find smallest value
                if( kat && hir > kat ) { holdPos = kat; }
                else if( hir ) {         holdPos = hir; }
     
//        // Only break after we add one furigana!
        if( holdPos == 0 
        &&  atLeastOneKanaPresent ) { break; } // ONLY BREAKPOINT, create another;
        
        // SPECIAL CASE for kanjiONLY  <keb>;if( holdPos == 0 && lenOfRet == 0 ) { holdPos = 1; }
        // Special case for (holdPos == 0 && lenOfRet == 0 ); 
        if( holdPos != kebKanaPos+1           // should be kanji first; 
        ||  !atLeastOneKanaPresent ) { 
            // canidate for furigana;
            kebKanaPos = holdPos - 1; 
            
            // Start of Ruby tag;
            ret[lenOfRet+0]='<'; ret[lenOfRet+1]='r'; 
            ret[lenOfRet+2]='u'; ret[lenOfRet+3]='b';
            ret[lenOfRet+4]='y'; ret[lenOfRet+5]='>';
            lenOfRet+=6;
            
            int lenOfKebSpecial = 0;
            if( !atLeastOneKanaPresent ) {  // SPECIAL!
                kebKanaPos = 0;
                while( keb[lenOfKebSpecial] ) { 
                    ret[lenOfRet] = keb[lenOfKebSpecial]; 
                    lenOfKebSpecial++;
                    lenOfRet++;
                }
            }

            // Print chars before kana(keb's) ( to be given furigana from <reb></> )
            for(int i=0;i<kebKanaPos;i++, lenOfRet++) { ret[lenOfRet] = keb[i]; }

            // "<rp>(</rp><rt>" 
            ret[lenOfRet+0]='<'; ret[lenOfRet+1]='r'; 
            ret[lenOfRet+2]='p'; ret[lenOfRet+3]='>';
            ret[lenOfRet+4]='('; ret[lenOfRet+5]='<';
            ret[lenOfRet+6]='/'; ret[lenOfRet+7]='r';
            ret[lenOfRet+8]='p'; ret[lenOfRet+9]='>';
            ret[lenOfRet+10]='<'; ret[lenOfRet+11]='r';
            ret[lenOfRet+12]='t'; ret[lenOfRet+13]='>';
            lenOfRet+= 14;
            
            // Before entering loop below, FIRST kana of <reb>
            // MUST NOT match kana at kebKanaPos;
            ret[lenOfRet] = reb[ rebPos ];
            lenOfRet++;
            rebPos++;
            
            
            // SKIP ON SPECIAL; Instead add all <reb>;
            // SPECIAL CASE for kanjiONLY  <keb>;
            if( atLeastOneKanaPresent ) {
                
                // Now need the furigana for the above Kanji;<reb></reb>
                // Really a guess to extract furigana
                // Could make sure with readings, but won't do that unless needed...
                while(  reb[ rebPos   ] !=  keb[ kebKanaPos   ] 
                    ||  reb[ rebPos+1 ] !=  keb[ kebKanaPos+1 ]
                    ||  reb[ rebPos+2 ] !=  keb[ kebKanaPos+2 ] 
                ){
    
                    ret[lenOfRet] = reb[ rebPos ];
                    lenOfRet++;
                    rebPos++;
                }
                // Skip over okurigana of <keb>'s kanji;
                rebPos+=3; // MUST BE 3-BYTE Kana.
                
            } else { // SPECIAL!
                // Add all reb;
                lenOfRet--;
                for(int i=0;reb[i];i++, lenOfRet++) { ret[lenOfRet] = reb[i]; }
            }
            
            // First Furigana placed, add footer;
            ret[lenOfRet+0]='<'; ret[lenOfRet+1]='/'; // </rt><rp>)</rp></ruby>
            ret[lenOfRet+2]='r'; ret[lenOfRet+3]='t';
            ret[lenOfRet+4]='>'; ret[lenOfRet+5]='<';
            ret[lenOfRet+6]='r'; ret[lenOfRet+7]='p';
            ret[lenOfRet+8]='>'; ret[lenOfRet+9]=')';
            ret[lenOfRet+10]='<'; ret[lenOfRet+11]='/';
            ret[lenOfRet+12]='r'; ret[lenOfRet+13]='p';
            ret[lenOfRet+14]='>'; ret[lenOfRet+15]='<';
            ret[lenOfRet+16]='/'; ret[lenOfRet+17]='r';
            ret[lenOfRet+18]='u'; ret[lenOfRet+19]='b';
            ret[lenOfRet+20]='y'; ret[lenOfRet+21]='>';
            lenOfRet+= 22;

            // SKIP ON SPECIAL; Instead add all <reb>;
            // SPECIAL CASE for kanjiONLY  <keb>;
            if( atLeastOneKanaPresent ) {
            
                // Add okurigana from <keb> ( the kana we found ) to ret[];
                ret[lenOfRet+0] = keb[ kebKanaPos   ]; 
                ret[lenOfRet+1] = keb[ kebKanaPos+1 ]; 
                ret[lenOfRet+2] = keb[ kebKanaPos+2 ]; 
                lenOfRet+= 3;
                
                // done. Main loop will find next NON kanji to print;
                // Increment past currentKana; 
                kebKanaPos+=3;  // increment past the kana; 
            } else { // SPECIAL!
                // EXIT LOOP on keb's ending index;
                kebKanaPos = lenOfKebSpecial;
                            
                break; // Only possible way to break in SPECIAL;
            }
        }
        else { 
            // Keb's Kana was in first position. 
            // Write this kana out and try again
            // for a Kanji;
            ret[lenOfRet+0] = keb[ kebKanaPos   ]; 
            ret[lenOfRet+1] = keb[ kebKanaPos+1 ]; 
            ret[lenOfRet+2] = keb[ kebKanaPos+2 ]; 
            lenOfRet+= 3;
            kebKanaPos+= 3;
        } 
    }
    // no more kana in <keb>
    // Write out to ret[] and exit loop!
    ret[ lenOfRet ] = '\0';

    return lenOfRet;
}


void jmdict_InfoClass::reb(unsigned char *retval)
{
    static unsigned char rebStr[] = "<reb>";
    static unsigned char endReb[] = "</";
    std::size_t pos = searchStr( rebStr ) + 5;
    std::size_t end = searchStr( endReb, pos );
    
    
    readStr( retval, (end-pos), pos );
    return ;
}

void jmdict_InfoClass::addFurigana(const unsigned char *file, 
                 unsigned char *retStr, 
                 const unsigned int lenOfRet)
{
    int retLen = 0;

    int SIZE_FILE = 0;
    while( file[ SIZE_FILE ] ) { SIZE_FILE++; }
    
    int buffLen = SIZE_FILE; 
    unsigned char buff[buffLen+1];
    int testLength = 0;
    int skip = 0;
    int beg=0;
    
    // Boolean val for when KANJI+kana was not matched in <keb> tags
    // Keep testing this to check accuracy of method
    // Method: search for first match to KANJI+kana*</keb> 
    //         Searches only strings of length 9; 
    int fudgeFuriganize = 0; 
         std::size_t nextKanaPos = 0;

    while( beg < SIZE_FILE ) { 
        std::size_t indexVal = 0;
        skip = 0;

        int const TRY_LEN = 12;
        testLength = 0;        
        while( beg+testLength < SIZE_FILE-skip 
               &&file[beg+testLength] ) { 
            buff[testLength] = file[beg+testLength]; 
            testLength++; 
        }
        buff[ testLength ] = '\0';
        buff[ TRY_LEN ] = '\0';
        
        while( beg+skip < SIZE_FILE ) { 
            int newLength = jmdict_InfoClass::inputLenTest(buff);
            skip += testLength - newLength;
            testLength = newLength;
            
            if( testLength != 0 ) { buff[testLength] = '\0'; }
           indexVal = getIndex( buff, testLength );

           // Try a loose Kanji compare 
           // if is_kana && lenToSearch == 2; then search( KANJI_KANA????</keb>; )
                // nextKanaPos(this, keb, kebKanaPos) #PASTE
                nextKanaPos = 0;
                    std::size_t hir = posOfHiragana( buff, 0 ); // (+1)
                    std::size_t kat = posOfKatakana( buff, 0 ); // 0 on fail; 
                    // Find smallest value
                    if( kat && hir > kat ) { nextKanaPos = kat; }
                    else if( hir ) {         nextKanaPos = hir; }
     
         
         if( indexVal
           &&  lenOfKanji() == 3
           &&  nextKanaPos == 4  ) {
               const std::size_t SAVE_INDEX_VAL = getIndex( buff, testLength ); 
                testLength  = 0;
                while( beg+testLength < SIZE_FILE-skip 
                       &&file[beg+testLength] ) { 
                    buff[testLength] = file[beg+testLength]; 
                    testLength++; 
                }
                buff[ testLength ] = '\0';

               // Failed to find Kanji+kana, try again;
               // GTR_THN 1 == 1Kanji, 1 Hiragana;
               // NEEDS TO BECOME A MEMBER FUNCTION
               const unsigned char endDelim[] = "</";

               std::size_t index_i // START OF KANJI && KANA MIX <keb> 
                = indexOfendingKanjiOnly_keyTable[ 9 ];
               index_i--; 
std::size_t sLen = 9;

               // Find how many are avaliable
               std::size_t nextIndexVal = nextIndex_keyTable( sLen );
               int numIndexesLeft = nextIndexVal - index_i;
               const unsigned char c = buff[6];
               buff[6] = '\0';
               unsigned int k = 0;
               for( k = 0; k < numIndexesLeft; k++) { 
                   setIndex( index_i );
                   
                   // SEARCH LOOSE for kanji ( NO DELIM </keb> );
                   if( searchStr( buff, endDelim ) ) { break; }
                   // s NOT FOUND between <keb>???</keb>
                   
                   index_i++;
               }
               buff[6] = c;

               // I believe if == there was NO MATCH for Kanji;
//               if( index_i != numIndexesLeft ) { 
               if( k != numIndexesLeft ) { 
                   indexVal = KanjiInfoClass::getIndex();
                   fudgeFuriganize = 1;
               }
               else { 
                   // NEED TO SET BACK TO ORIGINAL VALUES
                   // Or at least that seems to be why the bug exist; 
                   testLength = newLength;
                   indexVal = getIndex( buff, newLength );
//                   SAVE_INDEX_VAL
               }

           }

           if( indexVal ) { break; }
           skip+= 1;
           
           buff[ SIZE_FILE- ( skip + beg ) ] = '\0';
break;
       }
       if( !indexVal ) { 
        testLength = 0;        
        while( beg+testLength < SIZE_FILE-skip 
               &&file[beg+testLength] ) { 
            buff[testLength] = file[beg+testLength]; 
            testLength++; 
        }
        buff[ testLength ] = '\0';

           // Try the remaining numbers ( GTR THAN tryLength )
            while( beg+skip < SIZE_FILE ) {
                
                int newLength = inputLenTest(buff);
                skip += testLength - newLength;
                testLength = newLength;
                
                if( testLength != 0 ) { buff[testLength] = '\0'; }
               indexVal = getIndex( buff, testLength );
               if( indexVal ) { break; }
               skip+= 1;

               if( (SIZE_FILE- ( skip + beg )) <= TRY_LEN ) { break; }
               buff[ SIZE_FILE - ( skip + beg ) ] = '\0';
break;               
           }
           
       }
       
       
       if( indexVal ) { 
           // FOUND in database;     
           setIndex( indexVal );
           unsigned int lengthOfKanj = lenOfKanji();
           
unsigned char rubyStr[519];
int lenOfRuby = rubify( rubyStr);

if( fudgeFuriganize ) { 
    rubyStr[ lenOfRuby - 3 ] = buff[6];
    rubyStr[ lenOfRuby - 2 ] = buff[7];
    rubyStr[ lenOfRuby - 1 ] = buff[8];
}

if( lenOfRuby + retLen <= lenOfRet ) { 
for(int i=0; i < lenOfRuby; i++) { 
    retStr[ retLen + i ] = rubyStr[ i ];
}
retLen+= lenOfRuby; 
}

fudgeFuriganize = 0;
           beg+= lengthOfKanj - 1;  
       } else {  
           if( retLen + 1 <= lenOfRet ) { 
               retStr[ retLen ] = file[beg];
               retLen++; 
           } else { break; }
       }      
       
       beg++;
    } 
    retStr[ retLen ] = '\0';
    
    if( retLen == lenOfRet ) { 
        while(true ) { 
            if( retLen >= 3  ) {
                if( retStr[retLen-3] >= 0xE3
                ||  retStr[retLen-3] <= 0x80 ) { 
                    break; 
                }
            } else { break; }
            
            
            // nulls last 3byte char; 
            retStr[retLen-3] = '\0';
            retLen-=3;
        }
    }

    return ;

}

std::size_t nextKanaPos(jmdict_InfoClass &ths, const unsigned char *buff, int offset = 0) 
{
    std::size_t hir = ths.posOfHiragana( buff, offset ); // (+1)
    std::size_t kat = ths.posOfKatakana( buff, offset ); // 0 on fail; 

    // Find smallest value
    if( kat && hir > kat ) { return kat; }
    else if( hir ) {            return hir; }
    
    return 0;
}



std::size_t nextKanjiPos(jmdict_InfoClass &ths, const unsigned char *buff, std::size_t offset = 0) 
{

    std::size_t plusOff=0;
    unsigned int buffLen = 0;
    while(buff[buffLen]){ buffLen++; }
    
while( offset+plusOff < buffLen ) {     
    while( buff[offset+plusOff] && buff[offset+plusOff] < 0x80 ) { //< 0xE0 ) {
            plusOff++; } // skip non-Japaanese
    if( offset+plusOff >= buffLen ) { break; }
    int hir = ths.posOfHiragana( buff, offset+plusOff ) - 1; //can be (-1)
    int kat = ths.posOfKatakana( buff, offset+plusOff ) - 1;
    if( hir != offset+plusOff &&
	    kat != offset+plusOff ) 
    {
	    return plusOff+1; // Must be kanji, not hir or kat;
    }

    // Find smallest value
    if( hir >= 0 && kat >= 0 ) {
	if( hir < kat ) { plusOff = hir - offset; }
    	else 	    { plusOff = kat - offset; }	
    }
    else if( hir >= 0 ) { plusOff = hir - offset; }
    else                { plusOff = kat - offset; }
    plusOff+=3;
    }

    return 0;
}

} // NAMESPACE; 


