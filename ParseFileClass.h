#ifndef FILE_POSITION_POINTER
#define FILE_POSITION_POINTER
#include <fstream>

class ParseFileClass {
    private:
            /* const */ std::size_t fileLength;      // used for bounds checking
//            const unsigned char * const file;
            mutable unsigned char *file;
            std::size_t ptrPosition;           // last read position
            std::vector<std::size_t> keyTable; 
/*
            ParseFileClass &ParseFileClass::operator=( const ParseFileClass &other );  // no assignment op
            ParseFileClass::ParseFileClass( const ParseFileClass& other );             // no copy constructor
*/
            ParseFileClass& operator=( const ParseFileClass &other );  // no assignment op
            ParseFileClass( const ParseFileClass& other );             // no copy constructor
            ParseFileClass(); // No default ctor
            
    public:
            explicit ParseFileClass(const char fName[]);
           ~ParseFileClass();
          
	   
           unsigned char *const &getRaw() const { return file; } 
                                                std::size_t findPos(const unsigned char *, 
                                      const unsigned char *endDelim,
                                      const int &paramLen, // 0 == INFINITY
                                      const int &searchDirection ) const; // POSITIVE number == backwards search direction
           
           std::size_t getLine(unsigned char retStr[], 
                                      const std::size_t lengthOfRetStr,
                                      const unsigned char *endDeliminator );
           std::size_t getLineMultDeliminators(unsigned char retStr[], 
                                      const std::size_t lengthOfRetStr,
                                      const unsigned char *endDeliminator );
                          
           std::size_t read(unsigned char retStr[],
                                   const std::size_t length );

           int deleteCharacters( std::size_t i );
                     
           int setGetPointer(std::size_t i) 
                              { if( i < fileLength ) { ptrPosition = i; }
                                else return 1; return 0; }
           
           std::size_t getPositionPointer() { return ptrPosition; }
           std::size_t getFileLength() { return fileLength; }
           void saveFile(char *s) { std::ofstream o; o.open(s,std::ios::binary); o << file; o.close(); }
           
    private:
           void setFileLength(std::size_t i) { fileLength = i; }
           std::size_t getFileLengthInConstructor(const char fName[]);
//           const unsigned char * const getFileInConstrutor(const char fName[]);
           unsigned char * getFileInConstrutor(const char fName[]);
};
#endif
