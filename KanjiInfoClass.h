#ifndef KANJI_INFO_CLASS
#define KANJI_INFO_CLASS
#include <fstream>
#include <vector>
#include <string>
#include "ParseFileClass.cpp" // Version 2.0.1; 


  const char KanjiInfoClass_VERSION[]= "1.0.0a";

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
            std::size_t lastLookup;            // MUST NOT == 0 for life of program; 
            
            
    public:
            explicit KanjiInfoClass(const char fName[]);
           virtual ~KanjiInfoClass();
           void setKeyTable( std::vector<std::size_t> table) 
                                                      { 
                                                      keyTable = table;
                                                      // +1 to size so client code can use
                                                      // .size() without modd member getKeySize();
                                                      if( keyTable.size() )
                                                          keyTable.push_back( 0 );  
                                                      }
           const unsigned char *const getDB() const { return DB->getRaw(); }
           
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
                           DB->setGetPointer( KanjiInfoClass::getKeyPos( lastLookup ) );                       
                       }     

                       return DB->findPos(str); 
           }

           std::size_t searchStr(const unsigned char *str, 
                                 const unsigned char *endDelim,
                                 const std::size_t beg = 0) const 
           {
                       if( beg ) { 
                          if( DB->setGetPointer( beg ) ){
                              return 0;
                          }                    
                       }
                       else {
                          DB->setGetPointer( KanjiInfoClass::getKeyPos( lastLookup ) );
                       }

                       return DB->findPos(str, endDelim); 
           }
           
           // NEED FIX DB [ SHOULD BE CONST, but sets endPos ;-; ]
           void readStr(unsigned char *s, 
                        const std::size_t bufferSize,
                        const std::size_t pos = 0  ) const
           {
               if( pos ) { DB->setGetPointer( pos ); }
               else  DB->setGetPointer( KanjiInfoClass::getKeyPos( lastLookup ) );
               DB->read(s, bufferSize);
           }

           std::size_t fileLen() const 
           { 
                       return DB->getFileLength(); 
           }
           
           // return size+1(in constructor);
           // Allows client to iterate through loop by means( i < keyTable.size() ) { i++; }           
           std::size_t getKeySize() const 
           { return keyTable.size(); }

           // Arg1: KanjiIndex from user
           // Return: if keyTable exists, return pos held by keyTable, else return lastLookup; 
           std::size_t getKeyPos( std::size_t i ) const 
           { 
               if( getKeySize() == 0 ) { return lastLookup; }
               return keyTable[ KanjiInfoClass::getlastLookup(i) ]; 
           }
           
           // Return: see getKeyPos(); 
           std::size_t getPos() const { return KanjiInfoClass::getKeyPos( lastLookup ); }
           
           // Return: lastLookup value;
           std::size_t getIndex() const { return lastLookup; } // MUST NOT == 0 

           // NOT AN INDEX! Returns 0 on fail; 
           // Probably should be private; 
           virtual std::size_t kanjiNumber(const unsigned char *k) const;

           // 1-on-fail; 
           int setKanji( const unsigned char *k)
           { 
                // May be overridn, virtual member kanjiNumbere( ); 
                std::size_t kNum = this->kanjiNumber( k ); //0-on-fail; 
                if( !kNum ) { 
                    return 1;
                }

                // BIG! Removing this: kNum--;
                this->setIndex( kNum );  
                return 0;

           }      
           
           // Return 0 on success. 
           virtual int incrementIndex() { return KanjiInfoClass::setIndex( lastLookup+1 ); }
           
           // Documentation purposes, MUST BE 1 or greater at all times; 
           void resetKanjiIndex() { lastLookup = 1; }
           
           // Change how lastLookup( implemented as the INDEX of last lookup );
           // 1-on-fail
           // Dont see any reason to make virtual.
           int setIndex(std::size_t lookupIndex)
           {
               if( !lookupIndex ) {   
                   resetKanjiIndex(); // Documentation purpose(MUST NOT == 0 );
                   return 1; 
               }
               
               std::size_t setKeyTable_IS_RUN = getKeySize();
               if( lookupIndex <= setKeyTable_IS_RUN ) {
                   lastLookup = lookupIndex; 
                   return 0; 
               }
               
               // Sets lastLookup, but still returns Error(1); 
               if( !setKeyTable_IS_RUN ) { 
                   // Use 'lookupIndex' as POSITION; 
                   if( lookupIndex < fileLen() ) { 
                       lastLookup = lookupIndex;
                       return 0;
                   }
               }
               return 1;
           }

           virtual std::vector<ustring> onyomi()  const   = 0; // DB disables const keyword;
           virtual std::vector<ustring> kunyomi() const   = 0;           
           
    private:
           // Only used by non-virtual member functions.
           // Inherited non-virtual Member Functions MUST NOT be modified.  
           std::size_t getlastLookup() const { if( lastLookup ) { return lastLookup - 1; } return 0; }
           std::size_t getlastLookup(const std::size_t i) const { if( i ) { return i - 1; } return 0; }
           // Should only apply to namespace KanjiInfoClass {}; 
           // (BELOW functions);            
           
           // Actual private functions to KanjiInfoClass; 

};
#endif
