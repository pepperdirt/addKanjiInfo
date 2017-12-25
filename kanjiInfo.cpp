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
	-Last Updated:2017/12/10  - Version 0.1.3
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
    enum switch_names { FILE_NAME=0, VERSION_MAJOR=0, VERSION_MINOR=0,VERSION_=1 };
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
         L_GLOSSSIMILAR=17,

         H_help=18,
         V_version=19,
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

int main(const int argc, const char **const argv) {

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
    int doSynsets      = 0;
    int doGloss        = 0;  
    int doMoreGloss    = 0;

    unsigned int KHelpFLAGS = 0x00; // BIT 0: F, BIT 1: O, BIT 2: K, BUT 3: S;
    const char HEADER[4]= { 0xEF, 0xBB, 0xBF, 0x00 };
    const unsigned char *YOMI_DELIM = (unsigned char *)"\xE3\x83\xBB";
    unsigned int errorNo = 0;
    int NUMBER_OF_SENTENCES_TO_ADD = 3;
    std::streambuf *coutbuf = std::cout.rdbuf(); // may redirect stdout to file;
//    std::cout.rdbuf(out.rdbuf()); //redirect std::cout to out.txt!
    
//    kanjiDB::jmdict_InfoClass *Jmdict = 0;// ( jmdict );
//    kanjiDB::kanjiDict2_InfoClass *kanjiDict2 = 0; // ( KANJIDICT2 );

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

                            const unsigned char *SENTENCES_ADDED_HEADER = (unsigned char *)"<label class=\"sentences\">";
                            const unsigned char *END_SENTENCES_ADDED = (unsigned char *)"</label>";
    

    // USER INPUT
    if(switchIndexes[ I_IN_FILE ]  ) 
        inputFile = argv[switchIndexes[ I_IN_FILE ]];
    else
    { 
        std::cout << "Error. Requries an input file( -I )\n";
        return 1; // Need input file to continue;
    }
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
        doGloss = 
                     strToNum(
                                argv[switchIndexes[ G_GLOSS ]],
                                0,
                                strNumlen( 
                                            argv[switchIndexes[ G_GLOSS ]],
                                            0
                                         )
                             );

    if(switchIndexes[ Y_SYNSETS ]  ) 
        doSynsets = 1;
    if(switchIndexes[ Y_SYNSETS ]  ) 
        doMoreGloss = 1;

    
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
    

    kanjiDB::Wordnet_DictClass Wordnet( WORDNET,kanjiDB::OPTIMIZE::OPTIMIZE_SOME() );  
    kanjiDB::jmdict_InfoClass Jmdict  ( JMDICT, kanjiDB::OPTIMIZE::OPTIMIZE_SOME() );
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
//        unsigned char buff[4];
//        // Test if GZIPPED
//        Jmdict.readStr(buff, 3, 0);
//        int i = 0;
//        while( i<3 && buff[ i ] == GZIP_HEADER[ i ]  ){ i++; }
        if( Jmdict.getKeySize() < 5 ) {  std::cout << "Error. "<<JMDICT<<" MUST BE unzipped/decompressed!\n"; return 1<<8; }
        
//        kanjiDict2.readStr(buff, 3, 0);
        
//        i=0;
//        while( i<3 && buff[ i ] == GZIP_HEADER[ i ]  ){ i++; }
        if( kanjiDict2.getKeySize() < 5 ) {  std::cout << "Error. "<<KANJIDICT2<<" MUST BE unzipped/decompressed!\n"; return 1<<9; }

        if( Wordnet.getKeySize() < 5 )    { std::cout << "Error. "<<WORDNET    <<" MUST BE unzipped/decompressed!\n"; return 1<<10; }
//        
//        kanjiDict2.getKeySize();
    }


    const unsigned char *NULL_CHAR = (unsigned char *)"\0";
    const unsigned char *END_H1    = (unsigned char *)"</h1>";
    const unsigned char *BR        = (unsigned char *)"<br>";
    
    if( !doMarkupFile ) { 
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
    const std::size_t FIELD_LEN_MAX = largestDelimField( cvsFile, delim );
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
    if( KHelpFLAGS && fieldsToSearch.size() ) {
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
        
        while( (pos = cvsFile.getLine(buff, FIELD_LEN_MAX, delim)) )
        {

            // Increment through file, keeping track of what Delim num
            // Currently on;
            
            // if sentences switch used, and field is FIRST(ONLY) field in fieldToSearch,
            if( (KHelpFLAGS & S_FLAG) &&  printToField == fieldsToSearch[0] ) { 
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
                            
                            AddToFieldSelected.push_back( MAIN_ONYOMI_HEADER );
                            l=onYomi.size();for(int j=0;j<l;j++){AddToFieldSelected.push_back( onYomi[j] ); if(j+1<l){AddToFieldSelected.push_back( YOMI_DELIM );}}
                            AddToFieldSelected.push_back( END_MAIN_ONYOMI );
                            AddToFieldSelected.push_back( MAIN_KUNYOMI_HEADER );
                            l=kunYomi.size();for(int j=0;j<l;j++){if(j>0||onYomi.size()){AddToFieldSelected.push_back( YOMI_DELIM );} AddToFieldSelected.push_back( kunYomi[j] );}
                            AddToFieldSelected.push_back( END_MAIN_KUNYOMI );
                            AddToFieldSelected.push_back( MAIN_COMPOUND_HEADER );
                            l=compounds.size();
                    
                            // Compound Size == l
                            for(int j=0;j<l;j++){
                                AddToFieldSelected.push_back( compounds[j] );
                                AddToFieldSelected.push_back( (unsigned char *)"<br>" );
                            }

                            AddToFieldSelected.push_back( END_MAIN_COMPOUND );
                            AddToFieldSelected.push_back( MAIN_TRANSLATE_HEADER );
                            AddToFieldSelected.push_back( (unsigned char *)translation );
                            AddToFieldSelected.push_back( END_MAIN_TRANSLATE );
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
                
                for(int i=0; i < numberOfFields; i++) { 
                    cvsFile.setGetPointer((cvsFile.getLine(buff, FIELD_LEN_MAX, delim)));

                    // Before outputing buff, make sure it wasn't to be
                    // Furiganized first!
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

                    // Print out Extra information for Field i+1;                    
                    if( i+1 == printToField ) { 
                        // Start field by printing Sentences!
                        unsigned int sentenceSIZE = sentenceToAdd.size();
                        if( sentenceSIZE ) { 
                            for(unsigned int sentence_counter = 0; sentence_counter < sentenceSIZE; sentence_counter++ ) 
                            {
                                // Actually not a sentence. Actually pieces of them;
                                std::cout <<  sentenceToAdd[ sentence_counter ];
                            }
                            
                            sentenceToAdd.clear();
                        }
                        
                        
                        int vectorSize = AddToFieldSelected.size();

                        for(int j=0; j < vectorSize; j++)
                        {
                            std::cout <<  AddToFieldSelected[j]; 
                        }
                        
                        // Delete contents of Vector; 
                        AddToFieldSelected.clear();
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
            }
       }
        
         
        
    } 
    out.close();
    std::cout.rdbuf(coutbuf); //reset to standard output again    

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
         << "\t[-M] [-G [Delim]] [-Y] [-L] [-V] [-H]\n"

         << "\nDictionaries -J and -W are Requried. Download these First and\n"
         << "supply them to program. Default values: JMdict.xml / kanjidic2.xml\n"

		<< "\n"
		<< "  -I\tInput (.cvs) File to read and add kanji information to.\n"
		<< "  -D\tSet Deliminator to use in read in (.cvs) file ( FLAG: -I )\n"
        << "\t Default deliminator is New line, every line is a new field. \n"
        << "  -C\tConvert old Delim( -D ) to new Delim ( -C ) in stdout\n"
        << "\t If Delim ( -D ) is supplied, there MUST be (-N ##) number\n"
        << "\tof fields per line\n"
		<< "  -N\tNumber of deliminators(fields) per line. If default deliminator.\n"
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
        << "  -L\tAdd Similar meaning Glosses.\n"
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
    std::cout << *(argv+0) << " Version: "<< VERSION_MAJOR << VERSION_MINOR << VERSION_ << std::endl;
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
