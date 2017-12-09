#include "kanjiDict2_InfoClass.h"


namespace kanjiDB { 

  const char kanjiDict2_InfoClass_VERSION[]= "0.0.1";
/*
 * Programmer:	Pepperdirt
 * github:	github.com/pepperdirt
 *
         Latest update 2017/11/17 - Version 0.0.1
*/

kanjiDict2_InfoClass::kanjiDict2_InfoClass(const char fName[]) :indexOfIntval( 0 ), KanjiInfoClass(fName)  
/*: fileLength(getFileLengthInConstructor(fName) ),
  file(getFileInConstrutor(fName) ),  
  ptrPosition(0) */
{  
    
    unsigned char keb[] = "<literal>";
    unsigned char endKeb[] = "</";
    
    std::vector<int> offsetOfReadings;
    std::vector<std::size_t> kebPos_unsorted;
    std::vector<unsigned int>kanjiIntVal;
    unsigned int kebSize = 0;
    
    std::size_t diff = 0;
    std::size_t pos = 1;
    std::size_t len = KanjiInfoClass::fileLen();
//    std::cout << "len: "<< len; 
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
    unsigned char buff[4];
    // FIND FIRST KEB !!
        match = 0; 
        pos = KanjiInfoClass::searchStr( keb, pos );
        pos+= 9;
        kebPos_unsorted.push_back( pos );
        
        // SIGNATURE: <keb>???*</keb>
        // keysIndex.push_back( pos+5 ); // pos - 3 == KANJI [ ? ? ? ];

            KanjiInfoClass::readStr( buff, 3, pos );
            kanjiIntVal.push_back(buff[2]);
            kanjiIntVal[kebSize] |= (buff[1] << 8);
            kanjiIntVal[kebSize] |= (buff[0] << 16);
            SMALLEST_INT_VAL = kanjiIntVal[kebSize];
            LARGEST_INT_VAL = kanjiIntVal[kebSize];
            
        kebSize++;
        
std::vector<std::size_t> fourByteKanji;

    // END FIRST KEB !!
    // First sweep finds kanji with okurigana 
    while( true ) {
        match = 0; 
        pos = KanjiInfoClass::searchStr( keb, pos );
        if( pos == 0 ) { break; } // FAILED TO FIND ANYTHING;         
        pos+= 9;
        len = ( KanjiInfoClass::searchStr( endKeb, pos ) - pos );

        if( len == 3 ) { // Kanji that are less than 3;
        kebPos_unsorted.push_back( pos );
        // SIGNATURE: <literal>???*</literal>
        // keysIndex.push_back( pos+9 ); // pos - 3 == KANJI [ ? ? ? ];
        
            KanjiInfoClass::readStr( buff, 3, pos );
            kanjiIntVal.push_back(buff[2]);
            kanjiIntVal[kebSize] |= (buff[1] << 8);
            kanjiIntVal[kebSize] |= (buff[0] << 16);
            
//            indexOfIntval.push_back(kebSize);
            std::vector<unsigned int> posInIndex;
            if( kanjiIntVal[kebSize] < SMALLEST_INT_VAL ) 
                SMALLEST_INT_VAL = kanjiIntVal[kebSize];
                
            if( kanjiIntVal[kebSize] > LARGEST_INT_VAL ) 
                LARGEST_INT_VAL = kanjiIntVal[kebSize];
                                
//            for(unsigned int i = 0; i < kebSize; i++) { 
//                if( kanjiIntVal[i] ==  kanjiIntVal[kebSize] ) { 
//                    std::cout << "["<< i<<"]: "<<  kanjiIntVal[kebSize] 
//                    << "; POS("<< pos <<"); "<< std::endl;
//                }
//            }


            kebSize++;  
        } 
//        else { 
//               std::size_t end = KanjiInfoClass::searchStr( (const unsigned char *)"</character>", pos );
//               std::size_t posOfReading = KanjiInfoClass::searchStr( (const unsigned char *)"<reading_meaning>", pos ); 
//               
//               if(posOfReading && end > posOfReading ) {
//                   fourByteKanji.push_back( pos );
//                   NUM_FOUR_BYTE_KANJI++;
//               }
//        }
// std::cout << kebSize<< "; ";            
    }

//std::cout << SMALLEST_INT_VAL << " - "<< LARGEST_INT_VAL 
//          << " = " << LARGEST_INT_VAL - SMALLEST_INT_VAL
//          << "("<<(LARGEST_INT_VAL - SMALLEST_INT_VAL) * 4
//          << "; kebs("<< kebSize<<std::endl;

//    for(unsigned int i=0; i < NUM_FOUR_BYTE_5KANJI; i++) { 
//        kebPos_unsorted.push_back( fourByteKanji[i]  );
//    }
    
    unsigned int ALLOCATE_NEEDED = LARGEST_INT_VAL - SMALLEST_INT_VAL;
    unsigned int maxInts = kanjiIntVal.size();
    indexOfIntval = new unsigned int[ ALLOCATE_NEEDED+2 ];
    for(unsigned int i=0;i<ALLOCATE_NEEDED+1; i++){ indexOfIntval[i] = 0; }
    for(unsigned int i=0, index=0, currIntVal; i < maxInts; i++){
        currIntVal = kanjiIntVal[ i ] - SMALLEST_INT_VAL;
        indexOfIntval[ currIntVal ] = i;
    }
    
    KanjiInfoClass::setKeyTable( kebPos_unsorted );
    
}

kanjiDict2_InfoClass::~kanjiDict2_InfoClass() 
{
    if( indexOfIntval ) { delete [] indexOfIntval; }
}

void kanjiDict2_InfoClass::translate(char *retval, int allocatedLen)
{
    int len = 0;
    const unsigned char meaning[]="<rmgroup>";
    const unsigned char endMeaning[]="</rmgroup>";
    const unsigned char yomi[] = "<meaning>";
    const unsigned char genericEnd[] = "</";
    const std::size_t END_POS = searchStr( endMeaning ); 
    std::size_t FILE_offsetYomi = searchStr( yomi,endMeaning );
    
    unsigned char kana[80];
    // Break when no Yomi is found; 
    while( FILE_offsetYomi < END_POS ) {      
        if( !FILE_offsetYomi )
            break;           
        FILE_offsetYomi+= 9;
        
        // Kana present; Find ending pos; (get LENGTH )
        int lenghtOfKana = searchStr( genericEnd, FILE_offsetYomi ) - FILE_offsetYomi;
        
        // +2 is ', ' string; 
        if( allocatedLen 
        && (len+lenghtOfKana+2) > allocatedLen ) { break; }
        
        // Adds the Kana to words; 
        readStr( kana, lenghtOfKana, FILE_offsetYomi );
        
        if( len ) { 
            retval[ len+0 ] = ',';
            retval[ len+1 ] = ' ';
            len+=2; 
        }
        
        for(int l=0; l < lenghtOfKana; l++) { 
            retval[ len+l ] = kana[ l ];
        }
        FILE_offsetYomi += lenghtOfKana;
        len += lenghtOfKana;
        
        FILE_offsetYomi = searchStr( yomi, FILE_offsetYomi ); 
    }
    retval[ len ] = '\0';

    return ;
}

std::vector<ustring> kanjiDict2_InfoClass::genericRead( const unsigned char *begTag, 
                                const char *e  ) const
{
    int endLen = 0;
    while( e[endLen] ) { endLen++; }
    unsigned char end[endLen+1];
    
    endLen = 0;
    while( e[endLen] ) { end[endLen] = e[endLen]; endLen++; }
    end[endLen] = '\0';
    
    std::vector<ustring> words ;//this vector will be returned    
    
    const unsigned char END_KANJI_ENTRY[]="</character>";
    int LEN_BEG_TAG = 0;
    if( begTag ) { 
        while( begTag[ LEN_BEG_TAG ] ) { LEN_BEG_TAG++; }
    }
    
    const std::size_t END_POS = searchStr( END_KANJI_ENTRY ); 
    std::size_t FILE_offsetGeneric = searchStr( begTag,END_KANJI_ENTRY );
    
    unsigned char generic[820];
    // Break when no Yomi is found; 
    while( FILE_offsetGeneric < END_POS ) {      
        if( !FILE_offsetGeneric )
            break;           
        FILE_offsetGeneric += LEN_BEG_TAG;
        
        // Kana present; Find ending pos; (get LENGTH )
        int lenghtOfKana = searchStr( end, FILE_offsetGeneric ) - FILE_offsetGeneric;
        
        // Adds the Kana to words; 
        readStr( generic, lenghtOfKana, FILE_offsetGeneric );

        words.push_back(generic);        
        FILE_offsetGeneric += lenghtOfKana;
        
        FILE_offsetGeneric = searchStr( begTag, FILE_offsetGeneric ); 
    }

    return words;
}

void kanjiDict2_InfoClass::genericReadSingle(char *retVal, int retLen, const unsigned char *begTag,
                         const char *e ) const
{

    int endLen = 0;
    while( e[endLen] ) { endLen++; }
    unsigned char end[endLen+1];
    
    endLen = 0;
    while( e[endLen] ) { end[endLen] = e[endLen]; endLen++; }
    end[endLen] = '\0';
    
    const unsigned char END_KANJI_ENTRY[]="</character>";
    int LEN_BEG_TAG = 0;
    if( begTag ) { 
        while( begTag[ LEN_BEG_TAG ] ) { LEN_BEG_TAG++; }
    }
    
    std::size_t FILE_offsetGeneric = searchStr( begTag,END_KANJI_ENTRY );
    
    int len = 0;
    if( FILE_offsetGeneric ) {      
        FILE_offsetGeneric += LEN_BEG_TAG;
        
        // Kana present; Find ending pos; (get LENGTH )
        int lenghtOfKana = searchStr( end, FILE_offsetGeneric ) - FILE_offsetGeneric;

        unsigned char generic[ lenghtOfKana+1 ];
        // Adds the Kana to words; 
        readStr( generic, lenghtOfKana, FILE_offsetGeneric );
        
        if( lenghtOfKana > retLen ) { lenghtOfKana = retLen; }
        
        while( len < lenghtOfKana  ) { 
            retVal[ len ] = generic[ len ];

            len++;
        }
    }
    retVal[ len ] = '\0';

    return ;
}

// If Hiragana found, returns POS+1
/// SIMPLY pos of Kana(hir / kat );
// IDENTICAL TO nextKatakana; 
std::size_t kanjiDict2_InfoClass::posOfHiragana(const unsigned char *s,
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

std::size_t kanjiDict2_InfoClass::posOfKatakana(const unsigned char *buff, 
                                            const std::size_t offset = 0 ) const
{
//    if( len <= offset ) { return 0; }

    return 0;
}


std::size_t nextKanaPos(kanjiDict2_InfoClass &ths, const unsigned char *buff, int offset = 0) 
{
    std::size_t hir = ths.posOfHiragana( buff, offset ); // (+1)
    std::size_t kat = ths.posOfKatakana( buff, offset ); // 0 on fail; 

    // Find smallest value
    if( kat && hir > kat ) { return kat; }
    else if( hir ) {            return hir; }
    
    return 0;
}



std::vector<ustring> ucsCodepoint(kanjiDict2_InfoClass &ths ) 
{
    return (ths.genericRead( (const unsigned char *)"cp_type=\"ucs\">"  ));
}
std::vector<ustring> ucsjis208(kanjiDict2_InfoClass &ths ) 
{
    return (ths.genericRead( (const unsigned char *)"cp_type=\"jis208\">"  ));
}
std::vector<ustring> grade(kanjiDict2_InfoClass &ths ) 
{
    return (ths.genericRead( (const unsigned char *)"<grade>"  ));
}
std::vector<ustring> strokeCount(kanjiDict2_InfoClass &ths ) 
{
    return (ths.genericRead( (const unsigned char *)"<stroke_count>"  ));
}
std::vector<ustring> frequency(kanjiDict2_InfoClass &ths ) 
{
    return (ths.genericRead( (const unsigned char *)"<freq>"  ));
}
std::vector<ustring> jlpt(kanjiDict2_InfoClass &ths ) 
{
    return (ths.genericRead( (const unsigned char *)"<jlpt>"  ));
}
std::vector<ustring> pinyin(kanjiDict2_InfoClass &ths ) 
{
    return (ths.genericRead( (const unsigned char *)"r_type=\"pinyin\">"  ));
}
std::vector<ustring> koreanRomaji(kanjiDict2_InfoClass &ths ) 
{
    return (ths.genericRead( (const unsigned char *)"r_type=\"korean_r\">"  ));
}
std::vector<ustring> korean(kanjiDict2_InfoClass &ths ) 
{
    return (ths.genericRead( (const unsigned char *)"r_type=\"korean_h\">"  ));
}



} // NAMESPACE; 


