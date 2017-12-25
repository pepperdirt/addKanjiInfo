#ifndef JMDICT_INFOCLASS
#define JMDICT_INFOCLASS
#include <cstring>
#ifndef KANJI_INFO_CLASS
  #include "KanjiInfoClass.cpp" // pure-poly-base class;  Ver: 1.0.0
#endif

namespace kanjiDB {
          
#ifndef OPTIMIZE_CLASS
#define OPTIMIZE_CLASS
class OPTIMIZE { 
       public: 
      static OPTIMIZE NO_OPTIMIZATION () { return OPTIMIZE(0); }
      static OPTIMIZE OPTIMIZE_SOME()    { return OPTIMIZE(1); }
      static OPTIMIZE OPTIMIZE_MORE()    { return OPTIMIZE(2); }
      const int getVal() const { return this->optimizeLevel; }
      OPTIMIZE(const OPTIMIZE &m):optimizeLevel(m.getVal()) {}; 
       private:
      explicit OPTIMIZE(unsigned int m):optimizeLevel(m) {}; 
      const unsigned int optimizeLevel;

};    
#endif

class jmdict_InfoClass: public KanjiInfoClass {
    private:
            /* const */ std::size_t fileLength;      // used for bounds checking
//            const unsigned char * const file;
            jmdict_InfoClass& operator=( const jmdict_InfoClass &other );  // no assignment op
            jmdict_InfoClass( const jmdict_InfoClass& other );             // no copy constructor
            jmdict_InfoClass(); // No default ctor
            const int OPTIMIZE_LEVEL;
            
    public:
            explicit jmdict_InfoClass(const char fName[], const OPTIMIZE &OptimizeLevel );
           ~jmdict_InfoClass();

           std::vector<ustring> kunyomi () const;
           std::vector<ustring> onyomi  () const;     
           void help(unsigned char *retval);
           void translate(unsigned char *retval);
           std::size_t posOfHiragana(const unsigned char *buff, 
                                     const std::size_t off) const;
           std::size_t posOfKatakana(const unsigned char *buff, 
                                     const std::size_t off) const;

           int inputLenTest(const unsigned char *s, const int i) const;
           // int sizeOfEntry(const unsigned char *s, 
           int setKanji( const unsigned char *k); // virtual
           std::size_t getIndex( const unsigned char *, 
                                 const int startingLen  );
           unsigned int lenOfKanji() {
               return KanjiInfoClass::searchStr( (const unsigned char*)"</" ) - 
                      KanjiInfoClass::getPos();
           
           }
           void reb(unsigned char *retval);
           void addFurigana(const unsigned char *file, 
                             unsigned char *retStr, 
                             const unsigned int lenOfRet);
           unsigned int rubify(unsigned char *ret, unsigned int retLen);

//NEED TO KEEP THIS; IN furiganize.cpp NEED TO CREATE MEMBER METHOD;  private:
           // Values stored are indexVals + 1;
           // 0 == none stored;
           // Anything past 
           enum okuriganaKanjiStartPositions { POTENTIAL_NUM_OF_OKURIGANA = 150 };
           std::size_t indexOfendingKanjiOnly_keyTable[POTENTIAL_NUM_OF_OKURIGANA +2 ]; 
           std::size_t indexOfbeginingKanjiOnly_keyTable[POTENTIAL_NUM_OF_OKURIGANA +2];
           int highestSizeValue;
           
           std::size_t nextIndex_keyTable( unsigned int len ) const
           {
               for(int i = len+1, nextIndexVal; i <= highestSizeValue; i++) { 
                   if((nextIndexVal=indexOfbeginingKanjiOnly_keyTable[ i ])){ return nextIndexVal - 1; }
                   if((nextIndexVal=indexOfendingKanjiOnly_keyTable[ i ]))  { return nextIndexVal - 1; }
               }
               return 0;
           }
           // Should only apply to namespace KanjiInfoClass {}; 
           // (BELOW functions);           
           
           // Actual private functions to jmdict_InfoClass; 
//           void fillKeyTable(); 
};



void func(jmdict_InfoClass &db) {
 std::cout << "do something" << std::endl;
}

} // NAMESPACE

/*
Anything past len: 16 can be grouped as one group;
len: 3; x4235
len: 15; x16488
len: 12; x36451
len: 21; x3905
len: 6; x55179
len: 27; x1262
len: 18; x8403
len: 9; x42916
len: 4; x6
len: 24; x2301
len: 30; x797
len: 42; x104
len: 33; x454
len: 2; x51
len: 5; x11
len: 54; x14
len: 45; x77
len: 36; x300
len: 11; x4
len: 14; x3
len: 39; x156
len: 48; x39
len: 8; x15
len: 57; x6
len: 51; x22
len: 63; x2
len: 60; x7
len: 66; x2
len: 72; x1
len: 69; x1
len: 17; x4
len: 13; x1
*/

#endif

