#include "KanjiInfoClass.h"
/*
 * Programmer:	Pepperdirt
 * github:	github.com/pepperdirt
 *
         Latest update 2018/01/09 - Version 1.0.0a
                                    + virtual incrementIndex() added
                                      - Allows client code to override implementation
                                    + int setIndex(std::size_t lookupIndex)
                                      - CHANGED RETURN VALUE!
                                        * Allows success(0) returned for no lookupTable
                                          condition. 
                                        * Client code will use this value(success/fail);
                                      
                                        (for instance, when client DOES NOT want a lookup table)
                                  - Version 1.0.0
                                    + too much stuff to list; 
                                    + if user of class didn't know implementation details, should be okay, 
                                      however, will break code if using without setting Lookuptable:  getlastLookup(); 
                                    + getKeySize() also returns +1 ( Should not effect client code )
                                      because index will start at one(for client);
                                    + Addd safe use of memebers when no getDB is called; 
                                    + Modified lastLookup so it MUST NOT be 0;
                                      - Returns index values as +1 ( user of class should not know implementation details though )
                                      - when class needs to retrieve the index values, it reduces by one ( via private member: getlastLookup() )
                                    Version 0.0.2
                                    + Fixed Const-ness of return vals and parameters;
                                    + searchStr overloaded to accept starting position
                                  - Version 0.0.1
*/

KanjiInfoClass::KanjiInfoClass(const char fName[]) 
: DB( new ParseFileClass(fName) ),
  DATABASE(getDB()),
//const unsigned char *db = getDB();  
  lastLookup(1) // MUST BE 1 or greater;
{

}

KanjiInfoClass::~KanjiInfoClass() 
{
    if( DB ) { 
        delete DB; 
    }
}

// Returns Kanji NUMBER, NOT AN INDEX! ( shouldn't matter to user. Make class accept as-is and act accordingly ( knowing this is the vaule it SHOULD get back when calling funcitons );
// Returns 0 on fail; 
std::size_t KanjiInfoClass::kanjiNumber(const unsigned char *kanji) const
{

    std::size_t size = getKeySize(); 
    int MAX_KANJI_SIZE = 80;
    std::size_t i=0, kPos = 0;
    
    int kanjiSize = 0, j=0, compare = 0;
    while(  kanjiSize<MAX_KANJI_SIZE && kanji[kanjiSize]  ) { kanjiSize++; }
    
    std::size_t beg = 0;
    if( searchStr( kanji, getKeyPos( lastLookup ) ) != getKeyPos(lastLookup) ) {
        for( i = 1; i < size; i++,j=0, compare=0) { 
            kPos = getKeyPos(i);
            
            // Compare key w/Kanji; 
            while( compare < kanjiSize &&
                   kanji[compare] == DATABASE[kPos+compare] ) { compare++; }
                   

// NEW ADD // (DATABASE compare; )
            if( compare == kanjiSize  ) { 

                beg = i; 
                break; 
            }
            
        }
    }
    else { 
        // Kanji was previously searched for ( cached ); 
        beg = lastLookup;
    }

    return beg;
}


