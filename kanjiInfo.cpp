#include <iostream>
#include <fstream>
#include "jmdict_InfoClass.cpp" // furiganize();
// # include ParseFileClass_VERSION[]= "2.0.1";
// # include jmdict_InfoClass_VERSION[]= "0.0.1";
#include "kanjiDict2_InfoClass.cpp"
// # include kanjiDict2_InfoClass_VERSION[]= "0.0.1";
// # include ParseFileClass_VERSION[]= "2.0.1";
#include "Wordnet_DictClass.cpp"



/* 
  * CLI FOR ADDING KANJI INFO TO INPUT FILE;
  * NOTxAPI FOR ACCESSING KANJI INFORMATION.
  *  - ACCESSES KANJICHALLENGECOMPLETE DICTS
  *    + READINGS / ENGLISH MEANING / COMPOUNDS / HELP
  *  - ACCESSES KANJI JMDICT DICTS
  *    + READINGS / ENGLISH MEANING / COMPOUNDS / OTHER? 
  *
*/
/*
 * Programmer:	Pepperdirt
 * github:	github.com/pepperdirt
 *
	-Last Updated:2018/01/07  - Version 0.1.3d
   	                            Fully implemented --input-string swtich
                                 + Speed increase for single STRING lookups. 
	                            Add switch --input-string
                              - Version 0.1.3c
	                            + added --tell-me-delims switch
                              - Version 0.1.3b
	                            + added missing </h1> tag
                              - Version 0.1.3a
                                + seems to work 
                              - Version -.-.- 
	                            + fixed G_GLOSS optional delim add
	                            + syset now gives getExampleSentences() correct start position
	                            + Corrected swapped values
	                            + Removed -L switch, similar term add; 
                                + Corrected adding Furigana to Wordnet
                                + So many CLI switches added... Unfortunately, not properly documenting...
                                + Added Wordnet project to main;
                              - Version 0.1.3
	                            Checks for Extra file [Sentences] (-E ) existance.
	                            Compile error fixed.
                              - Version 0.1.2
	                            Error message for missing files are correct now.
	                          - Version 0.1.1
	                            Added: outputs to STDOUT if no output file given.
                                Fixed: Closed file after opening
                              - Version 0.1.0
	                            Added support for Switch -K S:
                                      Add sentences to Output;
	                            Added support for Switch -K F:
                                      Adds furigana to Sentences added ( -K S );
                                      Two switches are used in conjunction.
                              - Version 0.0.1
   
*/
    enum switch_names { FILE_NAME=0, VERSION_MAJOR=0, VERSION_MINOR=1,VERSION_=3 };
    const char *const versionAlpha = "c";
    enum COMMAND_SWITCHES { 
         I_IN_FILE=1, D_Delim=2, 
         S_SET_FILEDS_TO_ADD_EXTRACT_DATA_FROM_FOR_THEN_INSERTING_INTO_ANSWER_CARDS=3,
         P_PRINT_ALL_KANJI_DATA_TO_FILED_SPECIFIED_BY_P=4, 
         F_FURIGANIZE_BEFORE_ADDING_EXTRA_INFO=5,
         K_HELP_TO_ADD_KANJI_SWITCHES_TO_ACTUALLY_ADD_TO_FILE_F_O_K_S_T_Y=6, 
         M_MARKUP_SPECIFIER_ADD_HTML_MARKUP__DEFAULTS_TO_NO_MARKUP=7,
         O_OUT_FILE=8,
         N_NUMBER_OF_FIELDS=9,
         C_CONVERT_OUTPUT_FILE_DELIMINATOR__THIS_CAN_DIFFER_FROM_ORIGINAL_DELIM=10,
         E_EXTRA_SENTENCES=11,
         J_JMDICT = 12,
         W_WORDNET=13,
         X_KANJIDICT2=14,
         G_GLOSS=15,
         Y_SYNSETS=16,
         R_RELATION_SENTENCES=17,
             TELL_ME_SWITCHES = 18,
             INPUT_STRING_SWITCHES = 19,
             
         H_help=20,
         V_version=21,
         END_TERMINATOR=0
    };
    enum K_HELP_FLAGS { F_FLAG = 0x01, O_FLAG = 0x02, K_FLAG = 0x04, S_FLAG = 0x08, T_FLAG = 0x10, Y_FLAG=0x20 };

std::ostream& operator << (std::ostream& stream, const ustring& str) {
//    if (const auto len = str.size())
int len = str.size();
       stream.write(reinterpret_cast<const char*>(&str[0]), len);
    return stream;
}  



// std::size_t findPos( const unsigned char *const s, const unsigned char *const pin, const std::size_t index );
std::size_t strToNum(const char *const s, const std::size_t index, const int len );
void numToStr(char *const retStr, const std::size_t i);
int strNumlen(const char *s, const std::size_t index );
void help(); 
void getSwitchIndex( unsigned int *const ret, const int argc, const char **const argv );
void getInputFieldNums( std::vector<int>, const char **const argv, const int index );
unsigned int grabFLAG_FROM_KHELP( const int argc, const char **const argv, const int index );
void version(const char **const argv);

std::size_t numLines(ParseFileClass & file);
std::size_t largestDelimField(ParseFileClass & file, const unsigned char *delim);

std::vector<std::size_t> posOfKanji(ParseFileClass &, unsigned char *);
std::vector<std::size_t> findOffsetFromPos(ParseFileClass &FILE,
                                           std::vector<std::size_t> pos,
                                           std::vector<ustring> s,
                                           int searchDirection = 0);
std::size_t largestSize(std::vector<std::size_t> begOff,
                        std::vector<std::size_t> endOff );
void retrieveGlossTerm(unsigned char *glossTerm, const int &size, const unsigned char *const str, const unsigned char *const DELIM);
std::vector<ustring> defineGloss(kanjiDB::Wordnet_DictClass &WN, const std::vector<ustring> synsetIDs, const int numToDefine );
std::vector<ustring> getExampleSentences(kanjiDB::Wordnet_DictClass &Wordnet,
                                         std::vector<ustring> synsetIDs,
                                         const unsigned char * term,
                                         const int &numSentence
                                        );

// Functions for GUESSING command-line switches to use (switch --guessSwitches
std::size_t UNSAFE_isMatch( const unsigned char *needle,
		     const unsigned char *HAY,
		     const std::size_t &PosInHay = 0,
		     const unsigned int needleLen = 0);

int addEndingDelimThatParseFileClassOmits( unsigned char *buff,
                                           const int SIZE = 0 );
void countDeliminatorsPerLine( ParseFileClass &cvsFile,
                               unsigned char **&ppDelimStrs, 
                               unsigned int retINTVAL_COUNTER[],
                               std::size_t LINES_TO_SEARCH  = 0 );
unsigned char ** returnPossibleDelimsInLine(const unsigned char *const buff	);
void removeInvalidatedDeliminators( ParseFileClass &cvsFile,
                                    unsigned char **& ppDelimStrs,
                                    const int NUM_LINES_TO_CHECK = 60 );
    

void guessDeliminators(ParseFileClass &cvsFile  );

int main(const int argc, const char **const argv) {
//for( int i =0 ; i < argc; i++) { std::cout << "["<<i<<"]("<< argv[i] << std::endl;}
    if( argc == 1 ) { help(); return 0; }
//std::vector<ustring> test;
//    const unsigned char **tt = ((const unsigned char **)&test[0]);    
    
    unsigned int switchIndexes[V_version+2];
    for(int i=0; i <= V_version; i++) { switchIndexes[ i ] = '\0'; } 
    switchIndexes[ V_version+1 ] = '\0';
    getSwitchIndex( 
        switchIndexes,
        argc,
        argv
    );

    if( switchIndexes[ H_help ]    ) { help(); return 0; }
    if( switchIndexes[ V_version ] ) { version(argv); return 0; }
    const char *KANJIDICT2 = "kanjidic2.xml";
    const char *JMDICT  = "JMdict.xml";
    const char *WORDNET = "jpn_wn_lmf.xml"; 
//    const char *JMDICT_ALL_LANGS = "JMdict_e_ALL_LANGUAGES.gz";
    

    std::size_t error_afterthoughtVariable = 0;
    // Default value; Can be overridden by UserInput; 
    const char *ENTER = "\x0A";
    const char *outFile = 0; 
    const unsigned char *delim  = (const unsigned char *)"\x0A";
    const unsigned char *outDelim=(const unsigned char *)"\x0A";
    std::vector<int> fieldsToSearch;
    const char *sentences = "\0"; // Random sentences, no format specified
    const char *inputFile = "\0"; // .cvs file
    int printToField = 2; // defaults to 2
    int doFuriganize = 0;
    int doMarkupFile = 0;
    int numberOfFields = 3; // Defaults to 3
    int doGloss        = 0;  
    int doSynsets      = 0;
    int doRelationWordnetSentences = 0;

    
    // Specified w/ -G switch; 
    // Surrounds term to define with this Delim,i.e., asdfsdf GLOSS_DELIM GLOSS-TERM GLOSS_DELIM sdfsdf
    const unsigned char *GLOSS_DELIM = (unsigned char *)"\0"; 
    unsigned int KHelpFLAGS = 0x00; // BIT 0: F, BIT 1: O, BIT 2: K, BUT 3: S;
    const unsigned char HEADER[4]= { 0xEF, 0xBB, 0xBF, 0x00 };
    const unsigned char *YOMI_DELIM = (unsigned char *)"\xE3\x83\xBB";
    unsigned int errorNo = 0;
    int NUMBER_OF_SENTENCES_TO_ADD = 3;
    std::streambuf *coutbuf = std::cout.rdbuf(); // may redirect stdout to file;
//    std::cout.rdbuf(out.rdbuf()); //redirect std::cout to out.txt!
    
//    kanjiDB::jmdict_InfoClass *Jmdict = 0;// ( jmdict );
//    kanjiDB::kanjiDict2_InfoClass *kanjiDict2 = 0; // ( KANJIDICT2 );



                            const unsigned char *FRONT_Q_HEADER = (unsigned char *)"<h1 class=\"front_question\">";
//                            MAIN_KANJI_HEADER = (unsigned char *)"\0";
                            const unsigned char *END_FRONT_MAIN = (unsigned char *)"</h1>";
                            
                            const unsigned char *BACK_Q_HEADER = (unsigned char *)"<p class=\"front_kana\">";
//                            MAIN_KANJI_HEADER = (unsigned char *)"\0";
                            const unsigned char *END_BACK_MAIN = (unsigned char *)"</p>";
                            


                            const unsigned char *MAIN_KANJI_HEADER = (unsigned char *)"<h1 class=\"main_kanji\">";
//                            MAIN_KANJI_HEADER = (unsigned char *)"\0";
                            const unsigned char *END_MAIN_KANJI = (unsigned char *)"</h1>";
//                            END_MAIN_KANJI = (unsigned char *)"<br>"; // Needs Enter key after Kanji; 
                            
                            const unsigned char *MAIN_ONYOMI_HEADER = (unsigned char *)"<label class=\"main_onyomi\">";
//                            MAIN_ONYOMI_HEADER = (unsigned char *)"\0"; 
                            const unsigned char *END_MAIN_ONYOMI = (unsigned char *)"</label>";
//                            END_MAIN_ONYOMI = (unsigned char *)"\0";                 // No newline between yomis;
                            
                            const unsigned char *MAIN_KUNYOMI_HEADER = (unsigned char *)"<label class=\"main_kunyomi\">";
//                            MAIN_KUNYOMI_HEADER = (unsigned char *)"\0"; 
                            const unsigned char *END_MAIN_KUNYOMI = (unsigned char *)"</label><br>";
//                            END_MAIN_KUNYOMI = (unsigned char *)"<br>";                 // Need newline between yomis;
                            
                            const unsigned char *MAIN_COMPOUND_HEADER = (unsigned char *)"<label class=\"main_compounds\">";
//                            MAIN_COMPOUND_HEADER = (unsigned char *)"\0"; 
                            const unsigned char *END_MAIN_COMPOUND = (unsigned char *)"</label><br>";
//                            END_MAIN_COMPOUND = (unsigned char *)"<br>";                 // Need newline between yomis;
                            const unsigned char *MAIN_TRANSLATE_HEADER = (unsigned char *)"<label class=\"main_translation\">";
//                            MAIN_TRANSLATE_HEADER = (unsigned char *)"\0";
                            const unsigned char *END_MAIN_TRANSLATE = (unsigned char *)"</label><br>";
//                            END_MAIN_TRANSLATE = (unsigned char *)"<br>";

                            const unsigned char *GLOSS_HEADER =       (unsigned char *)"<label class=\"gloss\">";
                            const unsigned char *END_GLOSS    =       (unsigned char *)"</label>";
                            const unsigned char *SENTENCES_ADDED_HEADER = (unsigned char *)"<label class=\"sentences\">";
                            const unsigned char *END_SENTENCES_ADDED = (unsigned char *)"</label>";
                            const unsigned char *SYNSET_HEADER =       (unsigned char *)"<label class=\"synsets\">";
                            const unsigned char *END_SYNSET    =       (unsigned char *)"</label>";
                            const unsigned char *SIMILAR_GLOSS_HEADER =       (unsigned char *)"<label class=\"similargloss\">";
                            const unsigned char *END_SIMILAR_GLOSS    =       (unsigned char *)"</label>";


    // USER INPUT
    if(switchIndexes[ TELL_ME_SWITCHES ]  ) {
        inputFile = argv[switchIndexes[ TELL_ME_SWITCHES ]];
        if( inputFile ) { 
            ParseFileClass cvsFile( inputFile );
            if( fieldsToSearch.size() == 0 ) { 
                std::cout << "Error. Input file("<<inputFile<<") not found!\n"; 
                
                return 1<<5;
            }
            guessDeliminators(cvsFile );
            
            return 0;
        }
    }   
   
    if(switchIndexes[ I_IN_FILE ]  ) 
        inputFile = argv[switchIndexes[ I_IN_FILE ]];
    if(switchIndexes[ D_Delim ]  ) 
        delim = (const unsigned char *)argv[switchIndexes[ D_Delim ]];
    const int DELIM_SIZE = strlen( (const char *)delim );

    if(switchIndexes[ C_CONVERT_OUTPUT_FILE_DELIMINATOR__THIS_CAN_DIFFER_FROM_ORIGINAL_DELIM ]  ) 
        outDelim = (const unsigned char *)argv[switchIndexes[ C_CONVERT_OUTPUT_FILE_DELIMINATOR__THIS_CAN_DIFFER_FROM_ORIGINAL_DELIM ]];
    const int OUT_DELIM_SIZE = strlen( (const char *)outDelim );

    if(switchIndexes[ E_EXTRA_SENTENCES ]  ) 
        sentences = argv[switchIndexes[ E_EXTRA_SENTENCES ]];
    if(switchIndexes[ S_SET_FILEDS_TO_ADD_EXTRACT_DATA_FROM_FOR_THEN_INSERTING_INTO_ANSWER_CARDS ]  ) 
        getInputFieldNums(
                          fieldsToSearch, 
                          argv, 
                          switchIndexes[ S_SET_FILEDS_TO_ADD_EXTRACT_DATA_FROM_FOR_THEN_INSERTING_INTO_ANSWER_CARDS ]
        );
    if(switchIndexes[ P_PRINT_ALL_KANJI_DATA_TO_FILED_SPECIFIED_BY_P ]  ) 
        printToField = 
                     strToNum(
                                argv[switchIndexes[ P_PRINT_ALL_KANJI_DATA_TO_FILED_SPECIFIED_BY_P ]],
                                0,
                                strNumlen( 
                                            argv[switchIndexes[ P_PRINT_ALL_KANJI_DATA_TO_FILED_SPECIFIED_BY_P ]],
                                            0
                                         )
                             );
    if(switchIndexes[ N_NUMBER_OF_FIELDS ]  ) 
        numberOfFields = 
                     strToNum(
                                argv[switchIndexes[ N_NUMBER_OF_FIELDS ]],
                                0,
                                strNumlen( 
                                            argv[switchIndexes[ N_NUMBER_OF_FIELDS ]],
                                            0
                                         )
                             );


    if(switchIndexes[ F_FURIGANIZE_BEFORE_ADDING_EXTRA_INFO ]  ) 
        doFuriganize = 
                     strToNum(
                                argv[switchIndexes[ F_FURIGANIZE_BEFORE_ADDING_EXTRA_INFO ]],
                                0,
                                strNumlen( 
                                            argv[switchIndexes[ F_FURIGANIZE_BEFORE_ADDING_EXTRA_INFO ]],
                                            0
                                         )
                             );
    if(switchIndexes[ K_HELP_TO_ADD_KANJI_SWITCHES_TO_ACTUALLY_ADD_TO_FILE_F_O_K_S_T_Y ]  )
        KHelpFLAGS = 
                     grabFLAG_FROM_KHELP( argc,
                                          argv, 
                                          switchIndexes[ K_HELP_TO_ADD_KANJI_SWITCHES_TO_ACTUALLY_ADD_TO_FILE_F_O_K_S_T_Y ]
                                        );
    if(switchIndexes[ G_GLOSS ]  ) 
        doGloss = 1;

    if( doGloss ) 
    {  
        // Test for glossIndx matching another in this set
        // If match, there was no deliminator supplied( would have been +2 );     
        int i =0, glossIndex = switchIndexes[ G_GLOSS ] + 1;    
        for( i =1 ; i < argc; i++) { 
            if( switchIndexes[ i ] == glossIndex ) { 
                break; 
            }    
        }

        // Delim exists, add it; 
        if( i == argc && switchIndexes[ G_GLOSS ] != argc  ) {      
 	    if( switchIndexes[ G_GLOSS ] != argc ) 
     		GLOSS_DELIM = (unsigned char *)argv[switchIndexes[ G_GLOSS ]];
        }        
    }

    if(switchIndexes[ Y_SYNSETS ]  ) 
        doSynsets = 1;
    if(switchIndexes[ R_RELATION_SENTENCES ]  ) 
        doRelationWordnetSentences = 1;
    
    
    // doMarkupFile = 1; would have sufficed; 
    if(switchIndexes[ M_MARKUP_SPECIFIER_ADD_HTML_MARKUP__DEFAULTS_TO_NO_MARKUP ]  ) 
        doMarkupFile = 1;
    if(switchIndexes[ O_OUT_FILE ]  ) 
        outFile = argv[switchIndexes[ O_OUT_FILE ]];
    if(switchIndexes[ J_JMDICT ]  ) 
        JMDICT = argv[switchIndexes[ J_JMDICT ]];
    if(switchIndexes[ W_WORDNET ]  ) 
        WORDNET = argv[switchIndexes[ W_WORDNET ]];
    if(switchIndexes[ X_KANJIDICT2 ]  ) 
        KANJIDICT2 = argv[switchIndexes[ X_KANJIDICT2 ]];


    // Can I have this insid the if statement??
    kanjiDB::OPTIMIZE::OPTIMIZE Jmdict_OptimizeLevel( kanjiDB::OPTIMIZE::OPTIMIZE_SOME() );
    kanjiDB::OPTIMIZE::OPTIMIZE Wordnet_OptimizeLevel( kanjiDB::OPTIMIZE::OPTIMIZE_SOME() );

    const char *const tmpFileName = "tmp_file_input_string"; // Need to find writable directories for win/linux. 
    if(switchIndexes[ INPUT_STRING_SWITCHES ]  ) {
        if( !argv[switchIndexes[ INPUT_STRING_SWITCHES ]] ) { 
            std::cout << "No Input STRING!\n";
            return 1; }
        // --input-string STRING     
        inputFile = tmpFileName;
        std::ofstream out_f; out_f.open( inputFile, std::ios::binary );
        out_f.write( argv[ INPUT_STRING_SWITCHES ], strlen( argv[ INPUT_STRING_SWITCHES ] ) );
        out_f.write( ENTER, 1 );
        out_f.close();
        
        
        if( inputFile ) { 
            std::ifstream ii; ii.open( inputFile ); 
            
            if( !ii ) { 
                std::cout << "Error. Could not write to temp file!\n"; 
                
                return 1<<5;
            }
            ii.close();
        }
            
        numberOfFields = 1; // N_NUMBER_OF_FIELDS
        printToField   = 1; // P_PRINT_ALL_KANJI_DATA_TO_FILED_SPECIFIED_BY_P
        switchIndexes[ I_IN_FILE ]  = 1;
        
        Jmdict_OptimizeLevel.setVal(kanjiDB::OPTIMIZE::NO_OPTIMIZATION ());
        Wordnet_OptimizeLevel.setVal(kanjiDB::OPTIMIZE::NO_OPTIMIZATION ());
    }

    if( !switchIndexes[ I_IN_FILE ]  ) {
        std::cout << "Error. Requries an input file( -I )\n";
        return 1; // Need input file to continue;
    }
    

    kanjiDB::Wordnet_DictClass Wordnet( WORDNET,Wordnet_OptimizeLevel );  
    kanjiDB::jmdict_InfoClass Jmdict  ( JMDICT, Jmdict_OptimizeLevel );
    kanjiDB::kanjiDict2_InfoClass kanjiDict2( KANJIDICT2 );
    ParseFileClass cvsFile( inputFile );
    if( fieldsToSearch.size() == 0 ) { 
        fieldsToSearch.push_back( 1 ); // Default value
    }
    
    // check for files
    if( !cvsFile.getFileLength() )          { std::cout << "Error. Input file("<<inputFile<<") not found!\n"; return 1<<5; }
    if( !Jmdict.fileLen() )           { std::cout << "Error. "<<JMDICT<<" not found!\n"; return 1<<6; }
    if( !kanjiDict2.fileLen() ) { std::cout << "Error. "<<KANJIDICT2<<" not found!\n"; return 1<<7; }
    if( !Wordnet.fileLen()    ) { std::cout << "Error. "<<WORDNET<<" not found!\n"; return 1<<7; }
    
    unsigned char *GZIP_HEADER = (unsigned char *)"\x1F\x8B\x08";
    if( 1==1 ) { 
        unsigned char buff[4];
//        // Test if GZIPPED
        if( Jmdict_OptimizeLevel.getVal() == 0 ) {
            // Manual check of GZIP_HEADER; 
            Jmdict.readStr(buff, 3, 1);
            int i = 1; // cannot read first str w/readStr;
            while( i<3 && buff[ i-1 ] == GZIP_HEADER[ i ]  ){ i++; }
            if( i == 3 ) { 
                std::cout << std::cout << "Error. "<<JMDICT<<" MUST BE unzipped/decompressed!\n"; 
                return 2; 
            }
            
            Jmdict.resetKanjiIndex(); // Not realy necessary;  
        }
        else if( Jmdict.getKeySize() < 5 ) {  std::cout << "Error. "<<JMDICT<<" MUST BE unzipped/decompressed!\n"; return 1<<8; }
        
//        kanjiDict2.readStr(buff, 3, 0);
        
//        i=0;
//        while( i<3 && buff[ i ] == GZIP_HEADER[ i ]  ){ i++; }
        if( kanjiDict2.getKeySize() < 5 ) {  std::cout << "Error. "<<KANJIDICT2<<" MUST BE unzipped/decompressed!\n"; return 1<<9; }




        if( Wordnet_OptimizeLevel.getVal() == 0 ) {
            // Manual check of GZIP_HEADER; 
            Wordnet.readStr(buff, 3, 1);
            int i = 1; // cannot read first str w/readStr;
            while( i<3 && buff[ i-1 ] == GZIP_HEADER[ i ]  ){ i++; }
            if( i == 3 ) { 
                std::cout << std::cout << "Error. "<<JMDICT<<" MUST BE unzipped/decompressed!\n"; 
                return 2; 
            }
            
            Wordnet.resetKanjiIndex(); // Not realy necessary;  
        }
        else if( Wordnet.getKeySize() < 5 )    { std::cout << "Error. "<<WORDNET    <<" MUST BE unzipped/decompressed!\n"; return 1<<10; }
//        
//        kanjiDict2.getKeySize();
    }


    const unsigned char *NULL_CHAR = (unsigned char *)"\0";
    const unsigned char *END_H1    = (unsigned char *)"</h1>";
    const unsigned char *BR        = (unsigned char *)"<br>";
    
    if( !doMarkupFile ) { 

                            BR = (unsigned char *)ENTER; 
                            FRONT_Q_HEADER = BR;
                            END_FRONT_MAIN = NULL_CHAR;
                            
                            BACK_Q_HEADER = BR;
                            END_BACK_MAIN = NULL_CHAR;


                            MAIN_KANJI_HEADER = BR;
                            END_MAIN_KANJI = BR; // Needs Enter key after Kanji; 
                            
                            MAIN_ONYOMI_HEADER = NULL_CHAR; 
                            END_MAIN_ONYOMI = NULL_CHAR;                 // No newline between yomis;
                            
                            MAIN_KUNYOMI_HEADER = NULL_CHAR; 
                            END_MAIN_KUNYOMI = BR;                 // Need newline between yomis;
                            
                            MAIN_COMPOUND_HEADER = NULL_CHAR; 
                            END_MAIN_COMPOUND = BR;                 // Need newline between yomis;
                            MAIN_TRANSLATE_HEADER = NULL_CHAR;
                            END_MAIN_TRANSLATE = BR;
                            
                            SENTENCES_ADDED_HEADER = NULL_CHAR;
                            END_SENTENCES_ADDED = NULL_CHAR; 

                            SENTENCES_ADDED_HEADER = NULL_CHAR;
                            END_SENTENCES_ADDED = BR;

                            SYNSET_HEADER =       NULL_CHAR;
                            END_SYNSET    =       BR;

                            GLOSS_HEADER =       NULL_CHAR;
                            END_GLOSS    =       BR;

                            SIMILAR_GLOSS_HEADER =      NULL_CHAR;
                            END_SIMILAR_GLOSS    =       BR;

    }
    // END USER INPUT    

    // Deliminators for sentences [-E extra sentences ];
    unsigned char delim1[] = "\x0D\x0A";
    unsigned char delim2[] = "\x0A";
    unsigned char delim3[] = "\xE3\x80\x82"; // Period( JPN )
     
    std::vector<ustring> delimVect;
    delimVect.push_back( delim1 );
    delimVect.push_back( delim2 );
    delimVect.push_back( delim3 );
    ParseFileClass parseFile( sentences );

    if( (KHelpFLAGS & S_FLAG) && !parseFile.getFileLength() ) {  
        std::cout << "Error. Input file("<<sentences<<") not found!\n"; return 1<<9;
    }

    // Now the logic for doing the things (2 paths):
    //     -I, -F   ( it's own funciton all together ), output to file furiganizing field specified
    //     -I, -K, -S Searches field(s) S and inserts Kanji information K from input file I;
    
    std::size_t SAVED_POS = cvsFile.getPositionPointer();
    const std::size_t FIELD_LEN_MAX = 1200; //  largestDelimField( cvsFile, delim );
    cvsFile.setGetPointer( SAVED_POS );

    unsigned char buff[FIELD_LEN_MAX+1];

    // No header to skip, Set GetPointer back to 0;
    cvsFile.read( buff, 3 );
    if(
        buff[0] != HEADER[0] ||
        buff[1] != HEADER[1] ||
        buff[2] != HEADER[2] 
      ) 
    {
        cvsFile.setGetPointer( 0 );
    } 
    
    SAVED_POS = cvsFile.getPositionPointer();
    
    if( doFuriganize )
    {
        if( doFuriganize > numberOfFields )
        {
            std::cout << "Error. Field to add Furigana ( -F ) exceeds maximum Field Number( -N ).\n";
            return 4;
        }

    }   

    std::ofstream out; 
    std::size_t pos = 0;
    // Start reading through search fields
    if( 1==1 || KHelpFLAGS && fieldsToSearch.size() ) {
        if( (KHelpFLAGS & S_FLAG) && !sentences ) {
            std::cout << "Error. Need to supply sentences file (-E file) when using switch -K S.\n";
            return 8;
        }
        // Open output file and write UTF-8 HEADER; 
        if( outFile && outFile[0] ) { 
            out.open( outFile, std::ios::binary );
            std::cout.rdbuf(out.rdbuf()); //redirect std::cout to out.txt!
        }
        std::cout << HEADER;
        
    
        
        
        int fieldNumber = 1;
        int fieldsSelected_SIZE = fieldsToSearch.size();

        std::size_t kanjiPos = 0;
        unsigned char kanji[4];
        std::vector<ustring> AddToFieldSelected;
        std::vector<ustring> sentenceToAdd;
        std::vector<ustring> wordnetInfoToAdd;
	std::size_t lastRead;	
        while( (pos = cvsFile.getLine(buff, FIELD_LEN_MAX, delim)) )
        {
	    lastRead = pos; 

            // Increment through file, keeping track of what Delim num
            // Currently on;

            // if sentences switch used, and field is FIRST(ONLY) field in fieldToSearch,
            if( (KHelpFLAGS & S_FLAG) && fieldNumber == fieldsToSearch[0] ) { 
                std::size_t beforeKanji = 0;
                std::vector<std::size_t> kanjipositions = posOfKanji(parseFile, (unsigned char *)buff);
                std::vector<std::size_t> endOff = findOffsetFromPos(parseFile,
                                                                    kanjipositions,
                                                                    delimVect,
                                                                    0); // forwards search;
                
                std::vector<std::size_t> begOff = findOffsetFromPos(parseFile,
                                                                    kanjipositions,
                                                                    delimVect,
                                                                    1); // backwards search;
                
                std::size_t MAX_SIZE_OFFSETS = largestSize( begOff, endOff );
                sentenceToAdd.push_back( SENTENCES_ADDED_HEADER );
                
                unsigned char holdSentence[MAX_SIZE_OFFSETS+1];
                std::size_t numSentences = begOff.size();
                int numAdded = 0;
                std::size_t prevKanji = 0;
                
                // Grab NUMBER_OF_SENTENCES_TO_ADD number of sentences ( if avaliable );
                for(std::size_t asdf = 0; asdf < numSentences 
                                       && numAdded < NUMBER_OF_SENTENCES_TO_ADD; asdf++) {
                    parseFile.setGetPointer( begOff[asdf]  );
                    
                    // Check for duplicate ( same position listed 2x or more )
                    if( beforeKanji !=  begOff[asdf] ) { 
                        parseFile.read( holdSentence, 
                                        ( endOff[asdf] - begOff[asdf] )
                                      );
                        
                        // Add Furigana?
                        if( (KHelpFLAGS & F_FLAG) ) { 
                            unsigned int MAX_LEN_FURIGANAIZED = 1800;
                            unsigned char furiganaizedSentence[ MAX_LEN_FURIGANAIZED + 1 ];
                        
                            Jmdict.addFurigana(holdSentence, 
                                               furiganaizedSentence, 
                                               MAX_LEN_FURIGANAIZED);
                            sentenceToAdd.push_back( furiganaizedSentence );
                        } 
                        else { 
                            sentenceToAdd.push_back( holdSentence );
                        }
                        sentenceToAdd.push_back( delim3 );
                        sentenceToAdd.push_back( BR );
                        sentenceToAdd.push_back( BR );

                        beforeKanji =  begOff[asdf];
                        numAdded++;
                    }
                }
                sentenceToAdd.push_back( END_SENTENCES_ADDED );
                sentenceToAdd.push_back( BR ); 
                sentenceToAdd.push_back( BR );
            }

            if(  fieldNumber == fieldsToSearch[0] ) { 
                if( doGloss ||  doSynsets || doRelationWordnetSentences ) { 
                    
		            int nGloss =0, nSynsets, nRelationWordnetSentences;
                    // Addition of Wordnet Project
                    // These are listed FIRST, meaning they show first in cards;
                    const int GLOSS_TERM_SIZE = 80;
                    unsigned char glossTerm[GLOSS_TERM_SIZE+1];

                    retrieveGlossTerm(glossTerm, GLOSS_TERM_SIZE, buff, GLOSS_DELIM );
                    // Cannot do anything if not found in Wordnet dict.
                    
                    // Success
                    if( glossTerm[0] &&  Wordnet.setKanji( glossTerm ) == 0 ) {
                        unsigned char gloss[320];
                        std::size_t INDEX_OF_TERM = Wordnet.getIndex();  // Term id

            			std::vector<ustring> synsetIDs;
            			const unsigned char **synTmp;
            
            			std::vector<ustring> definitions;
            			if( INDEX_OF_TERM ) {  
                            synsetIDs = Wordnet.synset();
                            if( !synsetIDs.empty() ) {
                			    synTmp = (const unsigned char**)&synsetIDs[0];
                
                			    Wordnet.setSynsetPos( synTmp[ 0 ] );
                			    if( Wordnet.defineSynset( gloss ) == 0 ) { 
                				    definitions.push_back(gloss);
                			        nGloss = 1;
                			    }
                            }
            
            			}
            
            //			if( synsetIDs.size() ) 
            			    // BROKEN: !!
            			    // definitions = defineGloss(Wordnet, synsetIDs, 3 );
                        if( (nGloss = definitions.size()) ) { 
            			    if( !doGloss ) 
            			        nGloss = 0; 

                            // If Gloss can also add switches for -G/L/S


                            unsigned int MAX_LEN_FURIGANAIZED = 2400;
                            unsigned char furiganaizedSentence[ MAX_LEN_FURIGANAIZED + 1 ];
                        
                            std::vector<ustring> exampleSentences;
                            if( doRelationWordnetSentences ) { 
                                // Sets synsets inside function, using sysnetIDs; 
                                // const unsigned char **holdSynsetIDs = (const unsigned char**)&synsetIDs[0];
                                // Make sure is set before moving into Function;
                                Wordnet.setIndex( INDEX_OF_TERM );
                                //std::size_t HOLD_POS = Wordnet.setSynsetPos( holdSynsetIDs[ holdSynsetID_Index ] ); 
                                
                                exampleSentences = getExampleSentences( Wordnet,
                                                                        synsetIDs,
                                                                        glossTerm,
                                                                        3
                                                                      );

                            }
                            nRelationWordnetSentences = exampleSentences.size();                                

                            std::vector<ustring> termsMatchingSynsets;
                            if( doSynsets ) { 
                                const unsigned char **holdSynsetIDs = (const unsigned char**)&synsetIDs[0];
                                
                                termsMatchingSynsets = synsetIdWrittenForm( Wordnet );
                            }
                            nSynsets = termsMatchingSynsets.size();
                            

                            // Add to AddToFileSelected; 
                            if( nGloss ) {
                                wordnetInfoToAdd.push_back( (unsigned char *)"Gloss:<br>");
                                const unsigned char **tt   = ((const unsigned char **)&definitions[0]);                                    
                                for(int i=0; i < nGloss; i++) { 
                                    wordnetInfoToAdd.push_back(GLOSS_HEADER);
                                    
                                    if( (KHelpFLAGS & F_FLAG)  ) {
                                        Jmdict.addFurigana(tt[ i ], 
                                                           furiganaizedSentence, 
                                                           MAX_LEN_FURIGANAIZED);
                                        wordnetInfoToAdd.push_back( furiganaizedSentence );
                                    }
                                    else { 
                                        wordnetInfoToAdd.push_back(definitions[i] );
                                    }
                                    wordnetInfoToAdd.push_back(END_GLOSS);
                                    wordnetInfoToAdd.push_back(BR );
                                }
                                wordnetInfoToAdd.push_back(BR );
                            }

                            if( nRelationWordnetSentences ) { 
                                wordnetInfoToAdd.push_back( (unsigned char *)"Example Sentences:<br>");
                                const unsigned char **tt   = ((const unsigned char **)&exampleSentences[0]);
                                for(int i = 0; i < nRelationWordnetSentences; i++) {
                                    wordnetInfoToAdd.push_back(SENTENCES_ADDED_HEADER);

                                    if(  (KHelpFLAGS & F_FLAG)  ) {
                                        Jmdict.addFurigana(tt[ i ], 
                                                           furiganaizedSentence, 
                                                           MAX_LEN_FURIGANAIZED);
                                        wordnetInfoToAdd.push_back( furiganaizedSentence );
                                    }
                                    else { 
                                        wordnetInfoToAdd.push_back(exampleSentences[i]);
                                    }
                                    wordnetInfoToAdd.push_back(END_SENTENCES_ADDED);
                                    wordnetInfoToAdd.push_back(BR );
                                }
                                wordnetInfoToAdd.push_back(BR );
                            }

                            const unsigned char *const space = (unsigned char *)" ";
                            if( nSynsets ) { 
                                wordnetInfoToAdd.push_back( (unsigned char *)"Synsets: ");
                                const unsigned char **tt   = ((const unsigned char **)&termsMatchingSynsets[0]);
                                for(int i = 0; i < nSynsets; i++) {
                                    wordnetInfoToAdd.push_back(SYNSET_HEADER);                                    
                                    
                                    if(  (KHelpFLAGS & F_FLAG)  ) {
                                        Jmdict.addFurigana(tt[ i ], 
                                                           furiganaizedSentence, 
                                                           MAX_LEN_FURIGANAIZED);
                                        wordnetInfoToAdd.push_back( furiganaizedSentence );
                                    }
                                    else { 
                                        wordnetInfoToAdd.push_back(termsMatchingSynsets[i]);
                                    }
                                    wordnetInfoToAdd.push_back(END_SYNSET);
                                    wordnetInfoToAdd.push_back(space );
                                }
                                wordnetInfoToAdd.push_back(BR );
                            }
                                                            
                        }

                    }                        
                }
            }


            for(int field=0;field<fieldsSelected_SIZE;field++) { 
                
                // Fields to Search through are listed in filedToSearch[]; 
                // When fieldNumber matches, Create Extra Info for Kanji;
                if( fieldsToSearch[field] == fieldNumber ) { // one-BASED;
                    // this is field to gather kanji info on
                    // step through every KHelpFLAGS;    
                    // enum K_HELP_FLAGS { F_FLAG = 0x01, O_FLAG = 0x02, K_FLAG = 0x04, S_FLAG = 0x08,T_FLAG = 0x10, Y_FLAG=0x20 };
                    kanjiPos = nextKanjiPos( Jmdict, buff );
                    while( kanjiPos ) { 
                        kanjiPos--;
                        // Found a Kanji. Add to foundKanji
                        // NOTE** MAY NOT be Kanji. Check with dictionary!
                        kanji[0] = buff[kanjiPos+0];
                        kanji[1] = buff[kanjiPos+1];
                        kanji[2] = buff[kanjiPos+2];
                        kanji[3] = '\0';            

                        std::vector<ustring> onYomi;   
                        std::vector<ustring> kunYomi;  
                        std::vector<ustring> compounds; // holds kanji sentences/definitions(extra)
            
                        char translation[180];
                        unsigned char help[420];
                        
                        translation[0] = '\0'; 
                        help[0] = '\0';
                        if( kanjiDict2.getIndex( kanji, 0 )) 
                        { 
                            // Verified Kanji[];
                            kanjiDict2.setIndex(kanjiDict2.getIndex( kanji, 0 )); // only edit for adding 1.0.0 KanjiInfoClass
                            
                            if( (KHelpFLAGS & O_FLAG) )
                                onYomi  = kanjiDict2.onyomi();
                            if( (KHelpFLAGS & K_FLAG) )
                                kunYomi  = kanjiDict2.kunyomi();
                            translation[0] = '\0';
                            if( (KHelpFLAGS & T_FLAG) )
                                kanjiDict2.translate( translation, 180 );
                            unsigned char yomi[380];
                            
                            // Takes all Kun-yomis and tries to define them;
                            for(int i=0, len; (KHelpFLAGS & Y_FLAG) &&  i < kunYomi.size(); i++) { 

                                len = 0;
                                int PERIOD_FOUND = 0;
                                for(int z=0; kunYomi[i][z]; z++, len++ ) { 
                                    yomi[ len ] = kunYomi[i][z];
                                    
                                    if( yomi[len] == '.' ) { len--; PERIOD_FOUND=1; }
                                }
                                yomi[ len ] = '\0';

                                // Actually an <keb>entry; ( definition for kun-yomi )
                                if( Jmdict.setKanji( yomi ) != 0 ) { 
                                    // Try to find match with Kanji+Okurigana;
                                        len = 0;
                                        yomi[len+0] = kanji[0];yomi[len+1] = kanji[1];yomi[len+2] = kanji[2];
                                        len += 3;

                                        int z=0;
                                        if(PERIOD_FOUND) { while( kunYomi[i][z] != '.' ) { z++; } z++; } // skip '.' period;
                                        while( kunYomi[i][z] ) { yomi[ len ] = kunYomi[i][z]; len++; z++; }
                                        yomi[len] = '\0'; // TRY that (BELOW IF STATEMENT );
                                }

                                if( Jmdict.setKanji( yomi ) == 0 ) { // 0 == success;
                                    int helpLen = 0;
                                        // Place Kanji first, then after period kana;
                                        help[helpLen+0] = kanji[0];help[helpLen+1] = kanji[1];help[helpLen+2] = kanji[2];
                                        helpLen += 3;

                                    if( PERIOD_FOUND ) {
                                        int z=0;
                                        while( kunYomi[i][z] != '.' ) { z++; } // place Period in help[] also; 
                                        while( kunYomi[i][z] ) { help[ helpLen ] = kunYomi[i][z]; helpLen++; z++; }
                                        help[helpLen+0] = ' '; helpLen++;
                                    }                // Repurpose help(), since it isn't used.

                                    
                                    // Repurpose yomi[]; reb;
                                    Jmdict.reb( yomi );
                                    for(int z=0; yomi[z]; z++,helpLen++){ help[helpLen] = yomi[z]; }
                                    help[helpLen] = ' '; helpLen++;
                                    
                                    
                                    // Repurpose yomi[]; 
                                    Jmdict.translate(yomi);
                                    for(int z=0; yomi[z]; z++,helpLen++){ help[helpLen] = yomi[z]; }
                                    help[ helpLen ] =  '\0';
                                    
                                    compounds.push_back( help );
                                }
                            }

                            int l = 0;
                            AddToFieldSelected.push_back( MAIN_KANJI_HEADER );
                            AddToFieldSelected.push_back( kanji );
                            AddToFieldSelected.push_back( END_MAIN_KANJI );
                            
                            l=onYomi.size();
                            if( l ) {
                                AddToFieldSelected.push_back( MAIN_ONYOMI_HEADER );
                                for(int j=0;j<l;j++){AddToFieldSelected.push_back( onYomi[j] ); if(j+1<l){AddToFieldSelected.push_back( YOMI_DELIM );}}
                                AddToFieldSelected.push_back( END_MAIN_ONYOMI );
                            }
                            
                            l=kunYomi.size();
                            if( l ) { 
                                AddToFieldSelected.push_back( MAIN_KUNYOMI_HEADER );
                                for(int j=0;j<l;j++){if(j>0||onYomi.size()){AddToFieldSelected.push_back( YOMI_DELIM );} AddToFieldSelected.push_back( kunYomi[j] );}
                                AddToFieldSelected.push_back( END_MAIN_KUNYOMI );
                            }
                            
                            l=compounds.size();
                            if( l ) { 
                                AddToFieldSelected.push_back( MAIN_COMPOUND_HEADER );
                        
                                // Compound Size == l
                                for(int j=0;j<l;j++){
                                    AddToFieldSelected.push_back( compounds[j] );
                                    AddToFieldSelected.push_back( BR );
                                }
    
                                AddToFieldSelected.push_back( END_MAIN_COMPOUND );
                            }
                            
                            if( translation[0] ) { 
                                AddToFieldSelected.push_back( MAIN_TRANSLATE_HEADER );
                                AddToFieldSelected.push_back( (unsigned char *)translation );
                                AddToFieldSelected.push_back( END_MAIN_TRANSLATE );
                            }
                            //    << translation << END_MAIN_TRANSLATE; // outs trans every time;
                        }
                        
                        kanjiPos+=3; // increment past Kanji; 
                        std::size_t nextKanjiFound = 
                            nextKanjiPos( Jmdict, buff, kanjiPos );
    
                        if( nextKanjiFound ) { 
                            kanjiPos+= nextKanjiFound; 
                            
                            // Extra check. Doesn't seem necessary...
                            if( !buff[kanjiPos-1] || !buff[kanjiPos+0] || !buff[kanjiPos+1] ){ kanjiPos = 0; }                    
                        }
                        else { kanjiPos = 0; } // ZERO NO MORE KANJI; 
                        
                    }
                    
                    // MAIN LOOP COUNTER;
                    
                }
            }
            
            
            // readStr Jumps past Delim; pos+=DELIM_SIZE; // Skip Past Deliminator!
            // Muimi; cvsFile.setGetPointer( pos );
            fieldNumber++;
           
            // Iterated through all fieldNumbers(DelimsPerEntry)
            // Read delims Again, this time printing contents; 
            // Also print out AddToFieldSelected;
            if( fieldNumber > numberOfFields ) {  
                fieldNumber = 1; 
                cvsFile.setGetPointer( SAVED_POS );
                std::size_t err = 0;
                for(int i=0; i < numberOfFields; i++) {
//                    cvsFile.setGetPointer( (err = (cvsFile.getLine(buff, FIELD_LEN_MAX, delim))));
                    err = cvsFile.getLine(buff, FIELD_LEN_MAX, delim);
                    // Before outputing buff, make sure it wasn't to be
                    // Furiganized first!
                    if( i == 0 ) { std::cout << FRONT_Q_HEADER; }
                    if( i == 1 ) { std::cout << BACK_Q_HEADER;  }
                    if( i == 2 ) { std::cout << BR; }
                    if( i+1 != doFuriganize ) {                         
                        std::cout <<  buff; // Output to .cvs file;
                    } 
                    else 
                    {
                        // Add Furigana to buff FIRST;
                        unsigned int MAX_FURIGANA_SIZE = FIELD_LEN_MAX*2;
                        unsigned char furiganaAdded[ MAX_FURIGANA_SIZE +1 ];
                        Jmdict.addFurigana( buff, 
                                            furiganaAdded, 
                                            MAX_FURIGANA_SIZE );
                        std::cout <<  furiganaAdded;
                    }
                    if( i == 0 ) { std::cout << END_FRONT_MAIN; }
                    if( i == 1 ) { std::cout << END_BACK_MAIN;  }
                    if( i == 2 ) { std::cout << BR; }


                    // Print out Extra information for Field i+1;                    
                    if( i+1 == printToField ) {
			   unsigned int sentenceSIZE; 
                        // Start field by printing Sentences!
                        sentenceSIZE = wordnetInfoToAdd.size();
                        if( sentenceSIZE ) {
                            for(unsigned int sentence_counter = 0; sentence_counter < sentenceSIZE; sentence_counter++ ) {
                                std::cout << wordnetInfoToAdd[ sentence_counter ];
                            }
                            wordnetInfoToAdd.clear();
                        }
                        
                        sentenceSIZE = sentenceToAdd.size();
                        if( sentenceSIZE ) { 
                            for(unsigned int sentence_counter = 0; sentence_counter < sentenceSIZE; sentence_counter++ ) 
                            {
                                // Actually not a sentence. Actually pieces of them;
                                std::cout <<  sentenceToAdd[ sentence_counter ];
                            }
                            
                            sentenceToAdd.clear();
                        }
                        
                        
                        sentenceSIZE = AddToFieldSelected.size();
                        if( sentenceSIZE ) { 
                            for(unsigned int j=0; j < sentenceSIZE; j++)
                            {
                                std::cout <<  AddToFieldSelected[j]; 
                            }
                            // Delete contents of Vector; 
                            AddToFieldSelected.clear();                            
                        }
                    }
                    
                    // RE-Insert Deliminator; 
                    std::cout <<  outDelim;
                }
                if( outDelim[0] != 0x0A ) { 
                    std::cout <<  ENTER;
                }
                // Position Pointer should now be at same POS at start of IF() statment; 
                // Save POS pointer; 
                SAVED_POS = cvsFile.getPositionPointer();   
                fieldNumber = 1; 
                
//                if( err == 0 ) { 
//                if( !buff[0] ) { 
//                    cvsFile.setGetPointer(( cvsFile.getFileLength()-1 ));
//                }
            }
       }
        
         
        // Below off by one. fix later; (reading FIELD_LEN_MAX wont return 0)
        if( lastRead + FIELD_LEN_MAX < cvsFile.getFileLength() ) { 
            // stopped prematurely.
	    // Notify user
            error_afterthoughtVariable = lastRead;    
        }
    } 
    out.close();
    std::cout.rdbuf(coutbuf); //reset to standard output again    

    std::remove( tmpFileName ); // deletes temp file. 

    if( error_afterthoughtVariable ) { 
        std::cout << "Error. Stopped prematurely at line position("
		  << error_afterthoughtVariable << ").\n"
		  << "Line exceeded max length("<< FIELD_LEN_MAX <<").\n";
	return 99;
    }
    return 0;
}

void getSwitchIndex(unsigned int *const ret, const int argc, const char **const argv ) 
{
    for(int i=0; i < argc; i++) { ret[i] = 0; }
    
//         <<"kanjiInfo.exe [-I] [-D Delim] [-N ##] [-S ##,##,##,etc] \n
//         << "\t[-P ##] [-F] [-O OutputFile] [-K [ F ] [ O ] [ K ] [ S ]]  \n"
//         << "\t[-M]\n"
// *********************** USER INPUT HANDLER *******************************
    if(argc>0)
    {    /* Exits loop when at the second to last Pointer */
        for(int i=1;i<argc;i++)
        {
            if( *argv[i]=='-')
            {
               switch( toupper(*(argv[i]+1) ) )
               {                       
               case 'I':
                    ret[ I_IN_FILE ] = i+1; 
                    break;
               case 'D':
                    ret[ D_Delim ]   = i+1;
                    break;
                       
               case 'S':
                    ret[ S_SET_FILEDS_TO_ADD_EXTRACT_DATA_FROM_FOR_THEN_INSERTING_INTO_ANSWER_CARDS ] = i+1;
                    break;
               case 'P':
                    ret[ P_PRINT_ALL_KANJI_DATA_TO_FILED_SPECIFIED_BY_P ] = i+1;
                    break;
               case 'F':
                    ret[ F_FURIGANIZE_BEFORE_ADDING_EXTRA_INFO ] = i+1;
                    break;
               case 'K':
                    ret[ K_HELP_TO_ADD_KANJI_SWITCHES_TO_ACTUALLY_ADD_TO_FILE_F_O_K_S_T_Y ] = i+1;
                    break;
               case 'M':
                    ret[ M_MARKUP_SPECIFIER_ADD_HTML_MARKUP__DEFAULTS_TO_NO_MARKUP ] = i+1;
                    break;
               case 'E':
                    ret[ E_EXTRA_SENTENCES ] = i+1;
                    break;
               case 'O': 
                    ret[ O_OUT_FILE ] = i+1; 
                    break;
               case 'N': 
                    ret[ N_NUMBER_OF_FIELDS ] = i+1; 
                    break;
               case 'C': 
                    ret[ C_CONVERT_OUTPUT_FILE_DELIMINATOR__THIS_CAN_DIFFER_FROM_ORIGINAL_DELIM ] = i+1; 
                    break;

               case 'R':
                    ret[ R_RELATION_SENTENCES ] = i+1;
                    break;
               case 'G':
                    ret[ G_GLOSS ] = i+1;
                    break;
               case 'Y':
                    ret[ Y_SYNSETS ] = i+1;
                    break;
               
               case 'J': 
                    ret[ J_JMDICT ] = i+1; 
                    break;
               
               case 'W':                
                    ret[ W_WORDNET ] = i+1; 
                    break;
               case 'X':
                    ret[ X_KANJIDICT2 ] = i+1;
               
               case 'H': 
                    ret[ H_help ] = i+1; 
                    break;
               case 'V': 
                    ret[ V_version ] = i+1; 
                    break;

               case '-':// --?????
                   switch( toupper(*(argv[i]+2) ) ) 
                   {
                      // --tell-me-delims FILE\n"
                       case 'T': // Dont even worry about matching the rest. 
                            ret[ I_IN_FILE ] = i+1;
                            ret[ TELL_ME_SWITCHES ] = i+1; 
                       
                            break; 
                       // --input-string STRING     
                       case 'I':
                            ret[ INPUT_STRING_SWITCHES ] = i+1;
                            ret[ I_IN_FILE ] = i+1; 
                   default:
                   break;                                                         
                   }     

               default:
               break;
               }
            }
        }
    }

// *********************** END USER INPUT HANDLER ***************************

}
void numToStr(char *const retStr, const std::size_t i)
{
    std::size_t len = 0;
    std::size_t mult = 1;
    while( mult ==1 || mult <= i ) { mult *= 10; len++; }
    if( len == 0 ) { retStr[0]=0x30; retStr[1]='\0'; return ; }
    mult /= 10;
    
    std::size_t copyI = i;
    for(unsigned int c=0, num=0; c < len-1; c++, mult /= 10) {
        num = (copyI / mult );
        copyI -=  (num * mult );
        retStr[c] = num + 0x30;
    }
    copyI = i % 10;
    retStr[len-1]= copyI;
    retStr[len-1] += 0x30;
    retStr[len ] = '\0';

    return ;
}
std::size_t strToNum(const char *s, const std::size_t index, const int len ) {
    if( !s || !len || index < 0 ) { return 0; }

    std::size_t ret = 0;
    std::size_t mult = 1; 
    for(int i=len-1; i >= 0; i--, mult*=10) { ret+= (s[ index+i ]-0x30) * mult;  }

    return ret; 
}

int strNumlen(const char *s, const std::size_t index ) { 
    if( !s ) { return 0 ; }
    int i = 0;
    while( s[ index+i ] > 0x2F && s[ index+i ] < 0x3A ) { i++; }

    return i; 
}


// input should be delimiterized list of commas and numbers
// Convert numbers and place into *ret; 
// Arg1: return value, holds ints recovered from user input
// Arg2: CLI user input
// Arg2: Index value of *(argv+?) where string resides;
void getInputFieldNums( std::vector<int> ret, const char **const argv, const int index )
{
    int numIntsAddedToRet = 0;
    for(int i=0; argv[index][i]; i++){ 
        int numLen = strNumlen( argv[index], i );
        if( numLen ) {
            ret.push_back( strToNum( argv[i], i, numLen ) );
            numIntsAddedToRet++;
            i+= numLen-1; // Dont skip past NULL_TERMINATOR;
        } 
    }
    
    return ;
}


unsigned int grabFLAG_FROM_KHELP( const int argc,
                                  const char **const argv, 
                                  const int INDEX )
{
    unsigned int ret = 0;
    
    // Search for flags spanning multiple pointers(argv**)
    // Stops if no flag found, or index < argc; 
    for(int i=0, index=INDEX; 
                 argv[index][i] == ' ' || 
                 argv[index][i] == 'f' || 
                 argv[index][i] == 'F' || 
                 argv[index][i] == 'o' || 
                 argv[index][i] == 'O' || 
                 argv[index][i] == 'K' || 
                 argv[index][i] == 'k' ||
                 
                 argv[index][i] == 'T' || 
                 argv[index][i] == 't' || 
                 argv[index][i] == 'Y' || 
                 argv[index][i] == 'y' || 
                  
                 argv[index][i] == 'S' || 
                 argv[index][i] == 's'; i++) { 
        if( argv[index][i] == 'F' || argv[index][i] == 'f' ) { ret |= 0x01; }
        if( argv[index][i] == 'O' || argv[index][i] == 'o' ) { ret |= 0x02; }
        if( argv[index][i] == 'K' || argv[index][i] == 'k' ) { ret |= 0x04; }
        if( argv[index][i] == 'S' || argv[index][i] == 's' ) { ret |= 0x08; }
        if( argv[index][i] == 'T' || argv[index][i] == 't' ) { ret |= 0x10; }
        if( argv[index][i] == 'Y' || argv[index][i] == 'y' ) { ret |= 0x20; }
        if( argv[index][i+1] == '\0') { 
            if( index+1 < argc ) { 
                index++; 
                i = -1;
            }
        }
    } 
    return ret;
}

void help() { 

std::cout << "Insert Kanji Info from file in batch.\n"
         <<"kanjiInfo.exe [-I inputFile] [-D Delim] [-C Delim] [-S ##,##,##,etc] \n"
         << "\t[-P ##] [-F ##] [-K [ F ] [ O ] [ K ] [ S ] [ T ] [ Y ]]  \n"
         << "\t[-O OutputFile] [-E file] [-N ##] \n"
         << "\t[-J JMdict_Dictionary ] [-W jpn_wn_lmf.xml] [-X kanjidic2.xml] \n"
         << "\t[-M] [-G [Delim]] [-Y] [-R ] [--i STRING] [--t] [-V] [-H]\n"

         << "\nDictionaries -J and -W are Requried. Download these First and\n"
         << "supply them to program. Default values: JMdict.xml / kanjidic2.xml\n"

		<< "\n"
		<< "  --t tell-me-delims FILE\n"
		<< "\t  Searches FILE and tries to tell you what switches to use\n"
		<< "\t  (concerning the switches -D and -N )\n"
		<< "  -I\tInput (.cvs) File to read and add kanji information to.\n"
		<< "  --i input-string STRING\n"
		<< "\t  Use this switch to get information from a single string\n"
        << "\t  if spaces are present, encase the STRING in quotes \"\".\n" 
		<< "  -D\tSet Deliminator to use in read in (.cvs) file ( FLAG: -I )\n"
        << "\t Default deliminator is New line, every line is a new field. \n"
        << "\t If Delim ( -D ) is supplied, there MUST be (-N ##) number\n"
        << "\t of fields per line\n"
        << "  -C\tConvert old Delim( -D ) to new Delim ( -C ) in stdout\n"
		<< "  -N\tNumber of deliminators(fields) per line(entry). If default deliminator.\n"
		<< "\t (newline) is used, -N defaults to 3 newlines(fields) per entry\n"
		<< "  -S\tSets the field(s) to add Kanji info and insert into field (-P )\n"
		<< "\t Default: Add information based on field 1.\n"
		<< "  -P\tInserts all kanji data to specified field. (Default is 2).\n"
		<< "  -F\tAdd Furigana to field number. BEFORE adding extra information\n"
		<< "  -K\tKanji info to add to cards.\n"
		<< "\t F: Add furigana to inserted data (Only adds furigana to sentences [-K S])\n"
		<< "\t O: Add on-yomi for Kanjis\n"
		<< "\t K: Add kun-yomi for Kanjis\n"
		<< "\t T: Add translation of Kanji\n"
		<< "\t Y: Add translation to all Kun-Yomi\n"
		<< "\t S: Add sentences that contain whole field from [-S ]\n"
		<< "\t Sentences requires the use of switch -E to extract sentences from\n"
		<< "  -E\tExtra sentences to extract based on words/Kanji in Field(s) -S\n"
		<< "  -M\tAdd markup(HTML) to format output ( for use in Anki )\n"
		<< "\t Defaults to normal text file\n"
        << "  -G\tAdd Gloss(JPN, from Wordnet) of fields specified by -S switch\n"
        << "\t Defaults to whole field; Optional specify deliminator surrounding\n"
        << "\t term in order to tell program what term to define\n"
        << "  -Y\tAdd Synsets (from Wordnet) from fields specified by -S switch\n"
        << "  -R\tAdd sentences relating to term from Wordnet Project\n"
		<< "  -N\tNumber of fields in cvs file. defaults to 3 if not specified\n"
		<< "  -O\tOutput file name. Defaults to stdout.\n"
        << "  -J\tJMdict needed. Download and supply with -J switch. Default\n"
        << "\t value: JMdict.xml\n"
        << "  -W\tjpn_wn_lmf.xml needed. Download and supply with -W switch. \n"
        << "\t Default value: jpn_wn_lmf.xml\n"
        << "  -X\tKanjidict2.xml needed. Download and supply with -X switch. \n"
        << "\t Default value: kanjidic2.xml\n"
		<< "  -H\tPrints this help.\n"
		<< "  -V\tPrints the version\n"
		<< "\n"
		<< "  Example Usage: \n"
        << "kanjiInfo.exe -J JMdict_e_ALL_LANGUAGES.xml -i list.txt\n"
        << "-K K O T Y -o out.cvs -M -C ;\n"
		<< "\t This is command will use default Delim of \"newLine\" in file list.txt\n"
		<< "\t and output the delim (-C) ';' to out.cvs. It will use default \n"
        << "\t search field (-S), field 1 and output (-K) On/Kun yomi, Translation,\n"
        << "\t and Translation of Kun-Yomis to output out.csv using HTML styling(-M).\n"
		<< "\n";



} 

void version(const char **const argv) { 
    std::cout << *(argv+0) << " Version: "<< VERSION_MAJOR << VERSION_MINOR << VERSION_ <<  versionAlpha << std::endl;
}


std::size_t numLines(ParseFileClass & file)
{
    const unsigned char *delim = (const unsigned char *)"\x0A";
    std::size_t pos = 0;
    std::size_t numLines = 0;
    while( (pos = file.findPos(delim) ) ) {
        numLines++; 
        file.setGetPointer( pos + 1 );
    }
    
    return numLines;
}

std::size_t largestDelimField(ParseFileClass & file, const unsigned char *delim)
{
    std::size_t pos = 0;
    std::size_t lastPos = 0;
    std::size_t MAX_LINE_LENGTH = 0;
    std::size_t lineLen = 0;
    
    while( (pos = file.findPos(delim) ) ) {
        lineLen = pos - lastPos;
        if( lineLen > MAX_LINE_LENGTH ) { MAX_LINE_LENGTH = lineLen; }
        
        file.setGetPointer( pos + 1 );
        lastPos = pos + 1;
    }

    return MAX_LINE_LENGTH;
}


std::vector<std::size_t> posOfKanji(ParseFileClass &FILE,
                                    unsigned char *s)
{
    std::vector<std::size_t> posFound;
    std::size_t isNextMatchFound = 0;
    const unsigned char endDelim[] = "\0";
    
    while( isNextMatchFound = FILE.findPos( s, endDelim )  ) {  
        posFound.push_back( isNextMatchFound );
        FILE.setGetPointer(isNextMatchFound+1);
    }
    
    return posFound; 
}


// Returns vector of positions;
//  Positions are offsets from POS[0] of file(* FILE);
//  Returns offset supplied if no match found ( std::vector pos ); 

// Use Parse File Class == arg1
// arg2: starting position
// arg3: str to look for from pos(arg2);
// arg4 direction of serch( + or - );
// Returns position found; ( 1st[CLOSEST] found in vector s;
std::vector<std::size_t> findOffsetFromPos(ParseFileClass &FILE,
                                           std::vector<std::size_t> pos,
                                           std::vector<ustring> s,
                                           int searchDirection) // 0 == +; 1 == negative search direction
{
    int sign = 1;
    if( searchDirection ) { sign = -1; } 

    std::vector<std::size_t> posFound;
    std::size_t isNextMatchFound = 0;
    std::size_t counter = 0;
    const std::size_t S_SIZE = s.size();
    const std::size_t SIZE = pos.size();
    const unsigned char endDelim[] = "\0";
    std::size_t found = 0;
    std::size_t smallestFound = 0;
    
    std::size_t iter = 0;
    while( counter < SIZE ) { 
        FILE.setGetPointer( pos[counter] );
        smallestFound = 0;

        iter = 0;
        while( iter < S_SIZE ) {
            const unsigned char **hold_ustring = ((const unsigned char **)&s[iter]);
            // ** If s[iter] is !found, will return 0( beg of file );  
            found = FILE.findPos( hold_ustring[0],
                                  endDelim,
                                  0,
                                  searchDirection);

            // period: "\xE3\x80\x82";                                  
            if( found && hold_ustring[0][0] // at least 3 index values avaliable.
                      && hold_ustring[0][1]
                      && hold_ustring[0][0] == 0xE3 
                      && hold_ustring[0][1] == 0x80 
                      && hold_ustring[0][2] == 0x82   ) {
            
                std::size_t pos2 = found;
                if(  searchDirection ) { // (-) direction
                    pos2-= 3; // go back delimLen in attempt to find period again;
                } else {
                    pos2+= 3; // Skip delim(period); in attempt to find periond again;
                }                             
                
                const std::size_t SAVED_POS = FILE.getPositionPointer();
                FILE.setGetPointer(pos2 );
                
                if( FILE.findPos( hold_ustring[0],
                              endDelim,
                              0,
                              searchDirection)
                 == pos2 ) 
                { 
                    // Search until first pos is NOT enter char(JPN);
                    // (first char: curr position; (MULTIPLE period SKIP; Not end delim ));
                    while( 
                    (found = FILE.findPos( hold_ustring[0],
                                  endDelim,
                                  0,
                                  searchDirection)
                    ) == pos2 ) 
                    { 
                        if( !found || found < 3 ) { break; }
                        if(  searchDirection ) { // (-) direction
                            found-= 3; // go back delimLen in attempt to find period again;
                        } else {
                            found+= 3; // Skip delim(period); in attempt to find periond again;
                        }                             
                        
                        FILE.setGetPointer( found );
                        pos2 = found; 
                        
                    }
                } // END IF;
                
                FILE.setGetPointer(SAVED_POS);
            }

            // Skips delim character ( in (NEGATIVE) direcion );
            if( searchDirection  ) {
                if( found ) { 
                    int lengthustring = 0;
                    while( hold_ustring[0][lengthustring] ) { lengthustring++; }
                    found += lengthustring;
                }
            }
            
            if( !smallestFound || 
                (sign * found) < (sign * smallestFound) ) { 
                // Search direction deterimines if smaller or
                // larger value ( value closest to original Point( setGetPointer() )
                if( found ) { 
                    smallestFound = found; 
                } else { 
                    if( searchDirection ) { 
                        // NEGATIVE search direction ( possible 0 );
    
                        // Verify if match found at 0 or no match found
                        FILE.setGetPointer( 0 );
                        unsigned char tmpchar[3+1];
                        
                        // Command will read input (UNTIL delim str(arg3) is found)
                        // If arg3 is found at pos(0), will not fill tmpchar with anyhting; 
                        FILE.getLine(tmpchar,
                                     3,
                                     ((const unsigned char **)&s[iter])[0] );
                        if( tmpchar[0] == '\0' ) {
                            // Was indeed found at position 0.  
                            smallestFound = 0;
                        }
                        FILE.setGetPointer( pos[counter] );
                    }
                }
            }
            iter++; 
        }
        
        
        // Fix Positive search to AT LEAST be equal to starting pos; 
        if( !searchDirection && !smallestFound ) {
            posFound.push_back( pos[counter] );
        } 
        else { // Negative direction form offset;  
            posFound.push_back( smallestFound );
        }
        counter++;
    }
    
    return posFound; 
}

std::size_t largestSize(std::vector<std::size_t> begOff,
                        std::vector<std::size_t> endOff )
{
    std::size_t largestDifference = 0;
    std::size_t difference =0;
    const std::size_t SIZE = begOff.size();
    
    for(std::size_t i=0; i < SIZE; i++) { 
        difference = endOff[i] - begOff[i];

        if( difference > largestDifference ) 
        { 
            largestDifference = difference;  
        }
    }
    
    return largestDifference;
}


// Returns gloss string to define
//  Returns whole contents of *str, OR
//  Returns only what is between TWO *DELIMs ( if present );
// Arg1: Gloss term to return ( retrieved from str )
// Arg2: size of retGloss
// Arg3: string to search for gloss
// Arg4: Deliminator to search for
void retrieveGlossTerm(unsigned char *retGloss, 
                       const int &size, 
                       const unsigned char *const str,
                       const unsigned char *const DELIM)
{
    retGloss[0] = '\0';
    int strSize = 0; 
    if( !str )
        return ; 
    while( str[ strSize     ] ) { strSize++;   }
    
    if( !strSize ) 
        return ; 

    int delimSize=0; 
    if( DELIM ) 
        while( DELIM[ delimSize ] ) { delimSize++; }
    int startRead = 0; // Either after *DELIM OR at pos 0 of *str;
    int endRead = strSize;
    
    for(int i = 0, k; delimSize&& i+delimSize <= strSize; i++) { 
        for(k=0; k < delimSize; k++) { 
            if( str[ i+k ] != DELIM[ k ] )
                break;
        }
        
        if( k == delimSize ) { 
            if( !startRead ) { 
                startRead = i+delimSize; 
            }
            else {  
                endRead   = i;
                
                // Breaks at first delim found; 
                break; 
            }
            i+= delimSize - 1; 
        }
    }
    
    // Return with nothing if buffer overflow possible
    if( size < (endRead - startRead) ) { return; }
    
    // got Variables. Now read in string
    const int NUM_CHARS_TO_READ = endRead - startRead;
    for(int i=0, pos = startRead; i < NUM_CHARS_TO_READ; i++, pos++) 
    {
        retGloss[ i ] = str[ pos ]; 
    }
    retGloss[ NUM_CHARS_TO_READ ] = '\0';

    return ;

}

// Returns definitions
// Probably add as non-member function in class WordnetInfoClass; 
std::vector<ustring> defineGloss( kanjiDB::Wordnet_DictClass &Wordnet, const std::vector<ustring> synsetIDs, const int numToDefine ) 
{ 
    std::vector<ustring> retDef; 
    // Print the first synsetID( snynset ) w/a definition
    const char *const ints = "0123456789";
    if( numToDefine ) { 
        int holdSynsetID_Index = 0;
        int currentIndex = 0;

        const unsigned char ** holdSynsetIDs = (const unsigned char**)&synsetIDs[0];
        const int BUFF_SIZE = 80;
        unsigned char buff[BUFF_SIZE+1]; 
        int numDefined = 0;
        while( numDefined < numToDefine && holdSynsetID_Index < synsetIDs.size()) { 
            int counter = 1;
            
            if(    Wordnet.setSynsetPos( holdSynsetIDs[ holdSynsetID_Index ] )
	        && Wordnet.defineSynset( buff ) == 0 ) { 
/*                int shiftRight = 3;
                if( counter > 10 ) { shiftRight ++; }
                int lenBuff = 0; while( buff[ lenBuff ] ) { lenBuff++; }
                // Moves the Null Character also ( <= );
                for(int i=0; i <= lenBuff; i++) { buff[ lenBuff+shiftRight-i ] = buff[ lenBuff-i ]; }
                if( counter < 10 ) { buff[ 0 ] = ints[ counter ]; }
                if( counter > 9  ) { 
                    buff[ 1 ] = ints[ (counter % 10) ];
                    int tensPos = 0, n=0;
                    while( tensPos < counter ) { tensPos += 10; n++; }
                    buff[ 0 ] = ints[ n ];
                }
                buff[shiftRight-1] = ' '; buff[shiftRight-2] = '.';  
*/                
                retDef.push_back( buff );
                numDefined++; counter++;
                if( numDefined == numToDefine ) { break; }
            }            
            std::vector<ustring> SynsetId = Wordnet.synRealtions();
	    const unsigned char**ccc; 
	    if( SynsetId.size() ) 
                ccc = (const unsigned char**)&SynsetId[0];
            
            for(int a = 0; a < SynsetId.size(); a++) 
            {
                ;
                if(   Wordnet.setSynsetPos( ccc[ a ]  ) 
		   && Wordnet.defineSynset( buff ) == 0 ) { 
/*                    int shiftRight = 3;
                    if( counter > 10 ) { shiftRight ++; }
                    int lenBuff = 0; while( buff[ lenBuff ] ) { lenBuff++; }
                    // Moves the Null Character also ( <= );
                    for(int i=0; i <= lenBuff; i++) { buff[ lenBuff+shiftRight-i ] = buff[ lenBuff-i ]; }
                    if( counter < 10 ) { buff[ 0 ] = ints[ counter ]; }
                    if( counter > 9  ) { 
                        buff[ 1 ] = ints[ (counter % 10) ];
                        int tensPos = 0, n=0;
                        while( tensPos < counter ) { tensPos += 10; n++; }
                        buff[ 0 ] = ints[ n ];
                    }
                    buff[shiftRight-1] = '.'; buff[shiftRight-2] = ' ';  
*/            
                    retDef.push_back( buff );
                    numDefined++; counter++;
                    if( numDefined == numToDefine ) { break; }
                }                            
            }
            
            holdSynsetID_Index++;
        }
    }
    return retDef;
}

// Arg1: Wordnet class
// Arg2: SynsetIds for gathering sentences/gloss
std::vector<ustring> getExampleSentences(kanjiDB::Wordnet_DictClass &Wordnet,
                                         std::vector<ustring> synsetIDs,
                                         const unsigned char * term,
                                         const int &n
                                        )
{
    std::vector<ustring> exmapleSentences;
    if( synsetIDs.size() == 0 ) { return exmapleSentences; }

    const unsigned char **holdSynsetIDs = (const unsigned char**)&synsetIDs[0];
    unsigned char buff[ 320 ];

    
// Print the first example Sentence ( starting from currentIndex
// **Needs changed to (if currentIndex !haveExampleSentence ) 
//   { find next example w/Term included.; }


    // Discard return value. Onlly need synsetPos set; for Wordnet.examples(); 
    std::size_t HOLD_POS = Wordnet.setSynsetPos( holdSynsetIDs[ 0 ] ); // re-lookup for 
    exmapleSentences = Wordnet.examples();
    if( exmapleSentences.size() == 0  ) { 
        // Find next example that contains TERM;
        
        // std::vector<ustring> lexiconIDs = lexiconID( INDEX_OF_TERM );
        // No examples present; iterate through lexiconIDs, finding Position,
 /*       int i =1;
        while( i  < synsetIDs.size() ) { 
            Wordnet.setSynsetPos( holdSynsetIDs[ i ] );
            if( Wordnet.defineSynset( buff ) == 0 ) { 
                exmapleSentences = Wordnet.examples();
                if( exmapleSentences.size() ) { break; }
            }
        
            i++;
        }
 */
        std::vector<ustring> examplesAlreadyAdded;
        exmapleSentences = getExampleSentences(Wordnet, 
                                               term,
                                               n,
                                               examplesAlreadyAdded
                                              ); 
                                             
    }
        
    return exmapleSentences;
}

// If don't supply size, this won't work correctly. Function is compensating
// for ParseFileClass's defficiency( off-by-one if use returnPos as size of buff returned )
// Takes size that is given from ParseFileClass
// Arg1: buff returned from ParseFileClass::getLine(std::size_t ParseFileClass::getLine(unsigned char retStr[], const std::size_t len,const unsigned char *endString = (const unsigned char 
// Arg2: size of buff(size == RETURNED_POS_FROM_getLine - ORIGINAL_POS_BEFORE_getLine )
int addEndingDelimThatParseFileClassOmits( unsigned char *buff,
					    const int SIZE)
{
    int size = SIZE;
    if( size == 0 ) { while( buff[ size ] ) { size++; } }

    if( buff[ size - 1 ] == '\0' ) { 
	// Add ending delim ( 0x0A )
	buff[ size - 1 ] = 0x0A;
	buff[ size ] = '\0';

	return 0;
    }
    
    return 1;
}

void countDeliminatorsPerLine( ParseFileClass &cvsFile,
			  unsigned char **&ppDelimStrs, 
			  unsigned int retINTVAL_COUNTER[],
              std::size_t LINES_TO_SEARCH )    
{
    if( !ppDelimStrs || !ppDelimStrs[0][0] ) { return ; }
    std::size_t SAVED_POS = cvsFile.getPositionPointer();
    const unsigned char *const ENTER = (unsigned char *)"\x0A";

    std::size_t pos = 0;
    unsigned int len = 0; while( ppDelimStrs[ pos ] ) { pos++; }
    const unsigned int ppSIZE = pos; 

    const int LEN_MAX = 1023;
    unsigned char buff[ LEN_MAX+1 ];
    
    // Do first iteration to fill retINTVAL_COUNTER; 
    pos = cvsFile.getLine(buff, LEN_MAX, ENTER);
    std::size_t lastPos = pos - SAVED_POS; 

    addEndingDelimThatParseFileClassOmits( buff, lastPos );  // pos == size(as ParseFileClass returns(not always == to sizeOfBuff :'< 
	for(unsigned int a = 0; a < ppSIZE; a++) { 
        retINTVAL_COUNTER[ a ] = 0;
        // Search for delim strs ( and keep searching until all found;
	    len = 0; 
        while( (len = UNSAFE_isMatch( ppDelimStrs[ a ], buff, len )) ) {
            retINTVAL_COUNTER[ a ]++;
        } // len == posIn *buff;
     }
    
    std::size_t lineNo = 0;
    lastPos = pos;
    while( (pos = cvsFile.getLine(buff, LEN_MAX, ENTER)) )
    {
        len = pos - lastPos;
        lastPos = pos; 
        addEndingDelimThatParseFileClassOmits( buff, len ); // pos == size(as ParseFileClass returns(not always == to sizeOfBuff :'< 
        
    	for(unsigned int a = 0; a < ppSIZE; a++) { 
            len = 0;
            int match = 0;
            
            // If delims present( weren't flagged for removal
            // Verify delim.
            if( retINTVAL_COUNTER[ a ] ) { 
                // Search for delim strs ( and keep searching until all found;
        	    while( (len = UNSAFE_isMatch( ppDelimStrs[ a ], buff, len )) ) {
                    match++;
                } // len == posIn *buff;
        	    
        	    // Flag for removal if delim doesn't == first number of delims. 
        	    if( match != retINTVAL_COUNTER[ a ] ) { 
                    retINTVAL_COUNTER[ a ] = 0;
                }
            }
    	}
    
    	lineNo++;
    	if( lineNo == LINES_TO_SEARCH ) { break; }
    }

   // delete invalidated delims. 
   int newLogicalSize = 0;
    for(int a = 0; a < ppSIZE; a++) { 
    	// Flagged to delete. 
    	if( !retINTVAL_COUNTER[ a ] ) {
    	    delete ppDelimStrs[ a ];
    	}
    	else {
    	    // Move delim to keep to index [ newLogicalSize ];
    	    if( newLogicalSize != a ) { 
    		    ppDelimStrs[ newLogicalSize ] = ppDelimStrs[ a ];
                retINTVAL_COUNTER[ newLogicalSize ] = retINTVAL_COUNTER[ a ];
            }
    
    	    newLogicalSize++;
    	}
    }
    ppDelimStrs[ newLogicalSize ] = 0;
    retINTVAL_COUNTER[ newLogicalSize ] = 0;

   // Arrange deliminators from largest to smallest(occurrances.  
    for(int a = 0, largest, index; a < newLogicalSize; a++) { 
    	// Flagged to delete. 
        largest = 0;
        index = 0;
        
        for(int k = a; k < newLogicalSize; k++) { 
            if( retINTVAL_COUNTER[ k ] > largest ) {
                // Also make sure any Pointers GTR THAN 0x7F are prioritized
                // LOWEST. (At end of list )
                if( ppDelimStrs[ index ][0] < 0x80 ) { 
                    largest = retINTVAL_COUNTER[ k ];
                    index   = k; 
                }
                else {
                    if( (retINTVAL_COUNTER[ k ]>>1) >= largest ) {  // Allows even if == 0. 
                        largest = retINTVAL_COUNTER[ k ]>>1; // divide by 2(maybe this will reduce enough to place at FINAL index(End of List)
                        index   = k; 
                    }
                }
            }
        }
        
        // Swap 
        unsigned char *pSwap = ppDelimStrs[ a ];
        unsigned int iSwap   = retINTVAL_COUNTER[ a ];
        
        ppDelimStrs[ a ]       = ppDelimStrs[ index ];
        retINTVAL_COUNTER[ a ] = retINTVAL_COUNTER[ index ];
        
        ppDelimStrs[ index ]      = pSwap;
        retINTVAL_COUNTER[ index] = iSwap; 
    }
    ppDelimStrs[ newLogicalSize ] = 0;
    retINTVAL_COUNTER[ newLogicalSize ] = 0;


    cvsFile.setGetPointer( SAVED_POS );        
    return ;
}


// Logic to guess deliminators:
//	- search first line. Take EVERY character and 
//	  search next lines for matches ( next... 40 lines? )
//	  Find the deliminator that occurrs MOST (and MUST occurr at every line)
//	  Then, after deliminator is found, find how many per line; 
//	  + If NO suitable deliminator is found, use ENTER_KEY, but ask user
//	    for num lines to delim by...? 
void guessDeliminators(ParseFileClass &cvsFile )
{
    std::size_t pos;
    std::size_t lastPos = 0;
    unsigned int lineNo = 0;
    const int LEN_MAX = 1023;
    unsigned char buff[ LEN_MAX+1 ];
    int len = 0;
    const unsigned char *const ENTER = (unsigned char *)"\x0A";

    // the stored chars(found chars) in a given line. 
    unsigned int INTVAL_STORED[256]; // 0 - 255 value( delim char );
    pos = cvsFile.getLine(buff, LEN_MAX, ENTER);

    // getLine doesn't grab ending deliminator;  Version: 2.0.1
    // Check for EOF at [ pos - (DELIM_SIZE) ];
    // Add for check below; 
    addEndingDelimThatParseFileClassOmits( buff, pos ); // pos == size(as ParseFileClass returns(not always == to sizeOfBuff :'< 

    unsigned char **ppDelimStrs = returnPossibleDelimsInLine( buff );

    len = 0; while( ppDelimStrs[ len ] ) { len++; }
    const int DELIM_SIZE = len; 

    len = 0; while( buff[ len ] ) { len++; }
    if( len == cvsFile.getFileLength() ) { 
        // What do do here...? 
        std::cout << "len == "<<len<<"; ";
    }

    // Read first 60 lines(up to), only keep delims that are in each line
    removeInvalidatedDeliminators( cvsFile, ppDelimStrs, 60 );

    // Count number of deliminators in 30 lines. 
    countDeliminatorsPerLine( cvsFile, ppDelimStrs, INTVAL_STORED, 30 );

    // Return the first two delims. (one for delims in a line, 2nd for end of entry delim )
    // End of entry is PROBABLY /0A (ENTER);
    int logSize = 0;
    for(int a = 0; ppDelimStrs[ a ]; a++) { 
//std::cout << "\n[ "<<a<<" ]\n";
        if( a == 0 ) { 
//            unsigned int numFields = 0xFF&ppDelimStrs[a][ strlen( (char *)ppDelimStrs[a]) - 1 ];
            unsigned int numFields = INTVAL_STORED[ a ];
            std::cout << "File delimiterizes "<< numFields << " field(s) by the character(s): ";
            
        }
        else if( a == 1 ) { 
            std::cout << "End of entry character(s): ";
        
        }
        
        if( a == 0 || a == 1 ) {
            if(      ppDelimStrs[ a ][0] == 0x0A 
                  || ppDelimStrs[ a ][0] == 0x0D ) {
                std::cout << "ENTER_KEY" << std::endl;
            }
            else if( ppDelimStrs[ a ][0] == 0x20  ) { 
                std::cout << "SPACE_KEY" << std::endl;
            }
            else {
                unsigned char printStr[80];
                strcpy( (char *)ppDelimStrs[a], (char *)printStr ); // !! **CHECK strcpy FOR CORRECT USAGE. 
                std::cout << printStr << std::endl;
            }
        }
    }


    if( ppDelimStrs[0]  ) { //&& ppDelimStrs[1] ) {
            std::cout << "\nTry this command ( + all other options you want ):"<< std::endl;
            
            int a = 0;
            if( a == 0 ) { 
                std::cout << "-D ";
                if(      ppDelimStrs[ 0 ][0] == 0x0A 
                      || ppDelimStrs[ 0 ][0] == 0x0D ) {
                    std::cout << "x0A "; // Don't need to do anything, default to enter; 
                }
                else if( ppDelimStrs[ 0 ][0] == 0x20  ) { 
                    std::cout << "x20 ";
                }
                else {
                    std::cout << ppDelimStrs[0];
                }

                std::cout << "-N ";                
                int numFields = INTVAL_STORED[ a ];
                if( numFields > 1 ) {
                    std::cout << numFields; 
                } else { 
                    std::cout << "(NUMBER_OF_FIELDS_PER_ENTRY__NO_PARENTHESES)" ;
                }                             
                
                std::cout << std::endl;
            }
        
    }
/*    else { 
        if( ppDelimStrs[0] ) { 
            std::cout << "Only One deliminator found. Cannot determine how to parse file!\n";
            
            std::cout << "Try this command ( + all other options you want ):"<< std::endl
                      << "-D ";
            if(      ppDelimStrs[ 0 ][0] == 0x0A 
                  || ppDelimStrs[ 0 ][0] == 0x0D ) {
                std::cout << "x0A "; // Don't need to do anything, default to enter; 
            }
            else if( ppDelimStrs[ 0 ][0] == 0x20  ) { 
                std::cout << "x20 ";
            }
            else {
                std::cout << ppDelimStrs[0];
            }
            
            int numFields = INTVAL_STORED[ a ];
            if( numFields > 1 ) {
                std::cout << numFields; 
            } else { 
                std::cout << "(NUMBER_OF_FIELDS_PER_ENTRY__NO_PARENTHESES)" << std::endl;
            }            
        }
    }
*/


    for(int a = 0; ppDelimStrs[ a ]; a++) { 
        delete ppDelimStrs[ a ];
    }
    
    if( ppDelimStrs ) { delete [] ppDelimStrs; }
    
}

// 

// better ?encapsulation(can't think of word), gives ability 
// to change how delims are selected, without changing calling func();
unsigned char ** returnPossibleDelimsInLine(const unsigned char *const buff	)
{
    unsigned char **retPtr;
    // Simply return EVERY character recieved. 
    // May change later to include multicharacter deliminators.
    int len = 0; while( buff[ len ] ) { len++; }
    
    if( len > 255 ) { retPtr = new unsigned char*[ 255+1 ]; }
    else 	    { retPtr = new unsigned char*[ len+1 ]; }
    int PTR_LOGICAL_SIZE = 0;

    // Do check for multi-char ENTER;
    const unsigned char *const WINDOWS_HARD_ENTER = (unsigned char *)"\x0D\x0A";
    
    for(int i = 0, a; i < len; i++) { 

    	// Search through retPtr for buff[i]; 
    	for(a = 0; a < PTR_LOGICAL_SIZE; a++) { 
	        if( retPtr[ a ][ 0 ] == buff[ i ] )
                break ;
    	}
    
    	// buff[i] NOT found. Add here. 
    	if( a == PTR_LOGICAL_SIZE ) { 
    	    if( buff[ i ] == 0x0D ) { 
    		// With this logic, possible to add /0xA as another delim. 
    		if( buff[ i+1 ] == 0x0A ) {
    		    retPtr[ PTR_LOGICAL_SIZE ] = new unsigned char[ 3 ];
    		    retPtr[ PTR_LOGICAL_SIZE ][0] = 0x0D;
    		    retPtr[ PTR_LOGICAL_SIZE ][1] = 0x0A; 
    		    retPtr[ PTR_LOGICAL_SIZE ][2] = '\0'; 
    		    PTR_LOGICAL_SIZE++; // Add extra to LOG_SIZE; 
    		    i++; // Skip the \x0A char. 
    		}
    	    } else {
    	        retPtr[ PTR_LOGICAL_SIZE ] = new unsigned char[ 2 ];
    		retPtr[ PTR_LOGICAL_SIZE ][0] = buff[ i ];
    		retPtr[ PTR_LOGICAL_SIZE ][1] = '\0';
    	    }
    
    	    PTR_LOGICAL_SIZE++; 	    
    	}
    }
    retPtr[ PTR_LOGICAL_SIZE ] = 0;

    return retPtr; 
}



// Returns position AFTER match if match is found
// **Note: PosInHay MUST NOT be greater HAY;
// Returns 0 if no match;
std::size_t UNSAFE_isMatch( const unsigned char *needle,
		     const unsigned char *HAY,
		     const std::size_t &PosInHay,
		     const unsigned int needleLen
		   )
{
    unsigned int NEEDLE_LEN = needleLen; if( !NEEDLE_LEN ) { NEEDLE_LEN = strlen( (char *)needle ); }
    if( !NEEDLE_LEN ) { return 0; }
    for(unsigned int i = 0; i < NEEDLE_LEN; i++) { if( !HAY[ PosInHay+i ] ) { return 0; } }
    
    const unsigned char *const zeroPosition = HAY; 
    HAY += PosInHay + ( NEEDLE_LEN - 1 );
    
    // Start iterating through HAY[] until NULL_TERMINATOR is found('\0'); 
    while( *HAY ) { 
        if( memcmp( needle, (HAY-(NEEDLE_LEN-1)), NEEDLE_LEN ) == 0 )
            return (((reinterpret_cast<const int>(HAY)) - reinterpret_cast<const int>(zeroPosition))+1) ; // ASSUME SIZEOF(1); / sizeof(char ); 
                    
        HAY++;
    }
    
    return 0; 
}
 		     				  


void removeInvalidatedDeliminators( ParseFileClass &cvsFile,
				    unsigned char **& ppDelimStrs,
				    const int NUM_LINES_TO_CHECK )
{
    if( !ppDelimStrs ) { return; }
    if( !ppDelimStrs[0] ) { return; }

    // Store position. Restore at end of funciton.
    std::size_t SAVED_POS = cvsFile.getPositionPointer();


    const int BUFF_MAX = 1023;
    unsigned char buff[ BUFF_MAX+1 ];
    const unsigned char *buffP = buff; 
    const unsigned char *const ENTER = (unsigned char *)"\x0A";

    int PPDELIMSTR_SIZE = 0; while( ppDelimStrs[ PPDELIMSTR_SIZE ] ) { PPDELIMSTR_SIZE++; }

    // Read first 60 lines(up to), only keep delims that are in each line
    // If delim not found in line, remove. 
    unsigned int delimChar;
    std::size_t beg_pos = cvsFile.getPositionPointer();
    std::size_t pos = beg_pos;
    std::size_t len = 0;
    int lineChecked = 0;
    while( (pos = cvsFile.getLine(buff, BUFF_MAX, ENTER)) )
    {
    	len = pos - beg_pos;
    	beg_pos = pos;
    	addEndingDelimThatParseFileClassOmits( buff,
    					       len 
    					     );
    	// Take strings from ppDelimStrs, search through line for match
    	// If a delimStr is not matched in line, REMOVE THIS FROM ppDelimStrs. 
    	for( int a =0; a < PPDELIMSTR_SIZE; a++ ) { 
            // Actually, set to null string. Delete after loop is over.
            if( !UNSAFE_isMatch( ppDelimStrs[ a ], buff) )  // Returns 0 on fail; PosAfterMatch if success. 
                ppDelimStrs[ a ][0] = '\0';
    	}
    
    	// exit if checked NUM_LINES_TO_CHECK of lines. 
    	lineChecked++;
    	if( lineChecked == NUM_LINES_TO_CHECK ) {
    	    break;  
    	}
    }

    int newLogicalSize = 0;
    for(int a = 0; a < PPDELIMSTR_SIZE; a++) { 
	// Flagged to delete. 
	if( !ppDelimStrs[ a ][0] ) {
	    delete ppDelimStrs[ a ];
	}
	else {
	    // Move delim to keep to index [ newLogicalSize ];
	    if( newLogicalSize != a ) 
		ppDelimStrs[ newLogicalSize ] = ppDelimStrs[ a ];

	    newLogicalSize++;
	}
    }
    ppDelimStrs[ newLogicalSize ] = 0;

    // Of course, newLogicalSize is now == to SIZE of ppDelimStrs. 

    // Restore saved position.
    cvsFile.setGetPointer( SAVED_POS );    

    return ; 
}








void printHex(unsigned char *str )
{
    const char* const numStr= { "0123456789ABCDEF" };
    
    int i =0;
    for(  i =0 ; str[ i ]; i++) {
        if( (i+16)  % 16 == 0 ) { std::cout << std::endl; }
        std::cout << "0x" << numStr[ str[ i ]>>4 ] << numStr[ str[ i ]&0x0F ] << " ";
    }
    std::cout << "SIZE: "<<i<<"\n";
}
