#ifndef KANJIDICT2_INFOCLASS
#define KANJIDICT2_INFOCLASS
#include <string>
#ifndef KANJI_INFO_CLASS
  #include "KanjiInfoClass.cpp" // pure-poly-base class; 
#endif

namespace kanjiDB { 
class kanjiDict2_InfoClass: public KanjiInfoClass {
    private:
            /* const */ std::size_t fileLength;      // used for bounds checking
//            const unsigned char * const file;
            kanjiDict2_InfoClass& operator=( const kanjiDict2_InfoClass &other );  // no assignment op
            kanjiDict2_InfoClass( const kanjiDict2_InfoClass& other );             // no copy constructor
            kanjiDict2_InfoClass(); // No default ctor
            unsigned int NUM_FOUR_BYTE_KANJI;
            unsigned int SMALLEST_INT_VAL;
            unsigned int LARGEST_INT_VAL;
            //std::vector<unsigned int> indexOfIntval;
            unsigned int *indexOfIntval;
            
    public:
            explicit kanjiDict2_InfoClass(const char fName[]);
           ~kanjiDict2_InfoClass();

           std::vector<ustring> genericRead( const unsigned char *begTag, 
                                             const char *end= "</" ) const;
           void genericReadSingle( char *ret, int retLEN, const unsigned char *begTag,
                                     const char *end="</" ) const;

           std::vector<ustring> kunyomi  () {return (genericRead( (const unsigned char *)"=\"ja_kun\">"  )); }               
           std::vector<ustring> onyomi   () {return (genericRead( (const unsigned char *)"=\"ja_on\">"  )); }     
           void translate(char *retval, int len=0); // 0 is INFINITY len;
                            
           std::size_t posOfHiragana(const unsigned char *buff, 
                                     const std::size_t off) const;
           std::size_t posOfKatakana(const unsigned char *buff, 
                                     const std::size_t off) const;

           // ret 1+ on success; 0 on fail;
           unsigned int getIndex( const unsigned char *s, 
                                 const int startingLen  ) const 
           {
               unsigned int v = s[startingLen+2];
               v |= (s[startingLen+1] << 8);
               v |= (s[startingLen+0] << 16);
               return getIndex( v ); // 0 on Fail; +1 on success;
           }
                                       
           unsigned int lenOfKanji() {
               return KanjiInfoClass::searchStr( (const unsigned char*)"</" ) - 
                      KanjiInfoClass::getPos();
           
           }


           // Anything past 
           // Should only apply to namespace KanjiInfoClass {}; 
           // (BELOW functions);           
           
           private:
           // Actual private functions to kanjiDict2_InfoClass; 
//           void fillKeyTable(); 
           unsigned int getIndex( unsigned int kanjiVal ) const {  
               if( kanjiVal <= LARGEST_INT_VAL 
               &&  kanjiVal >= SMALLEST_INT_VAL )  
                   return indexOfIntval[ kanjiVal - SMALLEST_INT_VAL ]+1;
               else
                   return 0;
           }                                 
             
};



void func(kanjiDict2_InfoClass &db) {
 std::cout << "do something" << std::endl;
}

} // NAMESPACE


#endif

