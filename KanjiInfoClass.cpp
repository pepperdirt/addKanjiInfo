#include "KanjiInfoClass.h"
/*
         Latest update 2017/11/17 - Version 0.0.1
*/

KanjiInfoClass::KanjiInfoClass(const char fName[]) 
: DB( new ParseFileClass(fName) ),
  DATABASE(getDB()),
//const unsigned char *db = getDB();  
  lastLookup(0)
{

}

KanjiInfoClass::~KanjiInfoClass() 
{
    if( DB ) { 
        delete DB; 
    }
}

// Returns Kanji NUMBER, NOT AN INDEX!
// Returns 0 on fail; 
std::size_t KanjiInfoClass::kanjiNumber(const unsigned char *kanji) const
{

    std::size_t size = getKeySize(); 
    int MAX_KANJI_SIZE = 80;
//    char kanji[MAX_KANJI_SIZE+1];
    std::size_t i=0, kPos = 0;
    
    int kanjiSize = 0, j=0, compare = 0;
    while(  kanjiSize<MAX_KANJI_SIZE && kanji[kanjiSize]  ) { kanjiSize++; }
    
    std::size_t beg = 0;
    if( searchStr( kanji, getKeyPos( lastLookup ) ) != getKeyPos(lastLookup) ) {
        for( i = 0; i < size; i++,j=0, compare=0) { 
            kPos = getKeyPos(i);
            
            // Compare key w/Kanji; 
            while( compare < kanjiSize &&
                   kanji[compare] == DATABASE[kPos+compare] ) { compare++; }
                   

// NEW ADD // (DATABASE compare; )
            if( compare == kanjiSize  ) { 
//std::cout << "i("<<i<<"); kPos("<<kPos<<"); kSize("<<size<<"); ";
//char PKEY[]="0123456789ABCDEF";
//int PRINT_len = 0;
//while(  PRINT_len < compare ) { 
//       unsigned int num = DATABASE[PRINT_len+kPos];
//       std::cout << PKEY[ num >> 4 ] << PKEY[ num & 0x0F ] << " "; 
// 
//       PRINT_len++; }

                beg = i + 1; 
                break; 
            }
            
//            if( searchStr( kanji, getKeyPos(i) ) == getKeyPos(i) ) {
//                beg = i + 1;
//                break;
//            }            
        }
    }
    else { 
        // Kanji was previously searched for ( cached ); 
        beg = lastLookup+1;
//std::cout << "CACHE("<<lastLookup<<"); ";
    }

    return beg;
}


