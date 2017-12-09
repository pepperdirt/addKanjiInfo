#ifndef KANJI_INFO_CLASS
#define KANJI_INFO_CLASS
#include <fstream>
#include <vector>
#include <string>
#include "ParseFileClass.cpp" // Version 1.0.2; 


  const char KanjiInfoClass_VERSION[]= "0.0.1";

namespace unsigned_types { 
    typedef std::basic_string<unsigned char> ustring;
}

using unsigned_types::ustring; 

class KanjiInfoClass {
    private:
            /* const */ std::size_t fileLength;      // used for bounds checking
//            const unsigned char * const file;
            KanjiInfoClass& operator=( const KanjiInfoClass &other );  // no assignment op
            KanjiInfoClass( const KanjiInfoClass& other );             // no copy constructor
            KanjiInfoClass(); // No default ctor
             
            class ParseFileClass *DB; // Database API;
	    const unsigned char *DATABASE;
            std::vector<std::size_t> keyTable; // lookup Table for Keys (kanji)
            std::size_t lastLookup;            // Cached lookup ( needed for researching key );
            
            
    public:
            explicit KanjiInfoClass(const char fName[]);
           virtual ~KanjiInfoClass();
           void setKeyTable( std::vector<std::size_t> table) 
                                                      { 
                                                      keyTable = table; 
                                                      }
           const unsigned char *getDB() { return DB->getRaw(); }
           
           std::size_t searchStr(const unsigned char *str
                                 , const std::size_t beg = 0
                                 ) const 
           {
                       if( beg ) { 
                          if( DB->setGetPointer( beg ) ){
                              return 0;
                          }                    
                       }
                       else {
                           DB->setGetPointer( keyTable[ lastLookup ] );                       
                       }     

                       return DB->findPos(str); 
           }

           std::size_t searchStr(const unsigned char *str, 
                                 const unsigned char *endDelim) const 
           {
               DB->setGetPointer( keyTable[ lastLookup ] );
               return DB->findPos(str, endDelim); 
           }
           
           // NEED FIX DB [ SHOULD BE CONST, but sets endPos ;-; ]
           void readStr(unsigned char *s, 
                        const std::size_t bufferSize,
                        const std::size_t pos = 0  ) const
           {
               if( pos ) { DB->setGetPointer( pos ); }
               else  DB->setGetPointer( keyTable[ lastLookup ] );
               DB->read(s, bufferSize);
           }

           std::size_t fileLen() const 
           { 
                       return DB->getFileLength(); 
           }
           
           
           std::size_t getKeySize() const 
           { return keyTable.size(); }

           std::size_t getKeyPos( std::size_t i ) const 
           { return keyTable[i]; }
           
           
           std::size_t getPos() const { return keyTable[lastLookup ]; }
           std::size_t getIndex() const { return lastLookup; }
//           std::size_t getPos

           // NOT AN INDEX! Returns 0 on fail; 
           // Probably should be private; 
           std::size_t kanjiNumber(const unsigned char *k) const;
// DOUBTFUL THIS SHOULD BE ADDED;           std::size_t kanjiNumber_Loose(const unsigned char *k) const;

// COULD BRAKE STUFF!!!; Reversing order( FAIL IS 1 );
           virtual int setKanji( const unsigned char *k)
           { 

                std::size_t kNum = kanjiNumber( k ); //0-on-fail; 
//std::cout << "kNum("<<kNum<<"); ";
                if( !kNum ) { 
                    return 1;
                }

                kNum--;
                lastLookup = kNum;  
                return 0;

           }      
           
           int setIndex(std::size_t lookupIndex)
           {
               if(lookupIndex < keyTable.size() ) {
                   lastLookup = lookupIndex; 
                   return 0; 
               }
               return 1;
           }

           virtual std::vector<ustring> onyomi()= 0; // DB disables const keyword;
           virtual std::vector<ustring> kunyomi()= 0;           
           
           
    private:
           // Should only apply to namespace KanjiInfoClass {}; 
           // (BELOW functions);            
           
           // Actual private functions to KanjiInfoClass; 

};
#endif
