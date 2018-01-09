#include "Wordnet_DictClass.h"


/*



// MAIN TAGS (HEAD)
<Synset id='jpn-1.1-15121625-n' baseConcept='3'> -- term;
<Synset id='jpn-1.1-15121880-n' baseConcept='3'>
<SenseAxis id='sa_jpn-1.1-22247' relType='eq_synonym'>

// MAY optimize dictionary:
       // Simply replace all <Synset id='jpn-1.1-????????-?' 
       // Replace the ID( past jpn-1.1-') with the RAW number( POSITION of that 
                                                               particular ID )
       // THIS ALONE, should speed up lookups considerably. 
       // Leave the <Synset id='jpn-1.1-01211489-n' baseConcept='3'> tag
       //  as-is. No need to modify. In fact, it will give the POSITION a value;
       //  AND if need-be, can use position of that particular tag to track-back to Originating ID

A lexicon entry ( entry for term );
   <LexicalEntry id ='w211719'>
      <Lemma writtenForm='ŒÛ•‘Œƒ—ã' partOfSpeech='n'/>
      <Sense id='w211719_01211019-n' synset='jpn-1.1-01211019-n'/>
      <Sense id='w211719_06691442-n' synset='jpn-1.1-06691442-n'/>
   </LexicalEntry>



Definitions of term / Example sentences of term;
( the number: 01211489 matches the LexiconEntry id='w(NUMBER)'>
  <Synset id='jpn-1.1-01211489-n' baseConcept='3'>
	<Definition gloss="Žu‹C‚ð‚‚ß‚é‚Ì‚É–ð—§‚Â‚±‚ÆB">
   	<Statement example="–ˆ’©A‰Ô‚ðŒ©‚é‚±‚Æ‚ÅŽ„‚ÌŽm‹C‚Í‚‚Ü‚Á‚½"/>
	</Definition>
      <SynsetRelations>
         <SynsetRelation targets='jpn-1.1-01211019-n' relType='hype'/>
      </SynsetRelations>
  </Synset>
*/

/*
</Definition>
      <SynsetRelations>
         <SynsetRelation targets='jpn-1.1-00802318-v' relType='hype'/>
         <SynsetRelation targets='jpn-1.1-00737884-v' relType='hypo'/>
         <SynsetRelation targets='jpn-1.1-00669366-v' relType='hypo'/>
         <SynsetRelation targets='jpn-1.1-00669243-v' relType='hypo'/>
         <SynsetRelation targets='jpn-1.1-00668805-v' relType='hypo'/>
         <SynsetRelation targets='jpn-1.1-00669099-v' relType='hypo'/>
         <SynsetRelation targets='jpn-1.1-00669630-v' relType='hypo'/>
         <SynsetRelation targets='jpn-1.1-00670179-v' relType='hypo'/>
      </SynsetRelations>
   </Synset>

  <SenseAxis id='sa_jpn-1.1-83825' relType='eq_synonym'>
      <Target ID='jpn-1.1-00668805-v'/>
      <Target ID='eng-30-00668805-v'/>
   </SenseAxis>


        // No meaning;
        termNumBuff[0] = db[ keysIndex[ i ] - posFromTermPos + 0 ];
        termNumBuff[1] = db[ keysIndex[ i ] - posFromTermPos + 1 ];
        termNumBuff[2] = db[ keysIndex[ i ] - posFromTermPos + 2 ];
        termNumBuff[3] = db[ keysIndex[ i ] - posFromTermPos + 3 ];
        termNumBuff[4] = db[ keysIndex[ i ] - posFromTermPos + 4 ];
        termNumBuff[5] = db[ keysIndex[ i ] - posFromTermPos + 5 ];
        termNumBuff[6] = '\0';
*/        


//namespace wordnetDB { 
namespace kanjiDB { 

  const char Wordnet_DictClass_VERSION[]= "1.0.1";
/*
 * Programmer:	Pepperdirt
 * github:	github.com/pepperdirt
 *
         Latest update 2017/12/27 - Version 1.0.1
                                  - Version 1.0.1_b
                                    + std::size_t  Wordnet_DictClass::setSynsetPos(const unsigned char *const synsetID)
                                      fixed logic err. Matches OPTIMIzATION functionality; 
                                    + More functionality.
                                    + Cleaned up some non-member functions. 
                                    + Code updated to comply w/Cpp Standards.  
                                    + getExampleSentences(?) Feature added:  
                                      - Pass in already added sentences as argument
                                        check for matches, dont add matches
                                    + kanjiNumber(const unsigned char *term)
                                      - fixed to return proper w/Optimization level > 0;
                                  - Version 1.0.0
                                    + added helper functions;
                                    + kanjiNumber() logic err, flipped
                                    + added Optimization Options(and implemented)
                                    + encapsualted more
                                      - Modified members to not accept arguments
                                      - Need to set position via setSynsetPos, kanjiNumber, 
                                    + virtual member redefined (kanjiNumber ) to find term;
                                    TODO: look over logic again. Is design solid? -- as per THIS(1.0.0) update
                                    TODO2: actually document/version proper. Should not update LARGE chunks
                                           Maybe learn how to git from local storage ( is it possible? ).
                                    Version 0.0.1
*/

Wordnet_DictClass::Wordnet_DictClass(const char fName[],
                                     const OPTIMIZE &O)  : KanjiInfoClass(fName),

WORDNET_OPTIMIZE_LEVEL( O.getVal() ), 
                                                                                                      keytable_DefinitionPos(0),
                                                           savedSynset(1),
                                                           SYNSET_STR((unsigned char *)"synset='jpn-1.1-"), 
                                                           END_LEXICON_ENTRY_STR((unsigned char *)"</LexicalEntry>"),
                                                           SYNSET_TAG((unsigned char *)"<Synset"),
                                                           GLOSS_STR((unsigned char *)"<Definition gloss=\""),
                                                           END_GLOSS_STR((unsigned char *)"\""),
                                                           EXAMPLE_STR   ((unsigned char *)"<Statement example=\""),
                                                           END_EXAMPLE_STR   ((unsigned char *)"\"/>"),                                                           
                                                           END_SYNSET_TAG   ((unsigned char *)"</Synset>"),                                                           
                                                           SYNSET_RELATION   ((unsigned char *)"<SynsetRelation targets='"),                           
                                                           SYNSET_RELATION_TYPE   ((unsigned char *)"relType='"),                           
                                                           WRITTEN_FORM   ((unsigned char *)"writtenForm='"),                           
                                                           SENSE_ID_WRITTEN   ((unsigned char *)"<Sense id='"),                           
                                                           END_SENSE_ID_WRITTEN   ((unsigned char *)"_")                           
                                                           

 /*: fileLength(getFileLengthInConstructor(fName) ),
  file(getFileInConstrutor(fName) ),  
  ptrPosition(0) */
{  
 if( WORDNET_OPTIMIZE_LEVEL == 0 ) { return ; }
/*
   *
   *     KanjiInfoClass::setKeyTable( kebPositions_sorted ) will be filled 
   *     w/ Term positions. will have a separate array for positions of 
   *     definition / example sentences. 
*/
    std::size_t pos = 3; // FIRST KANJI;
    const int KEB_TAG_SIZE =  9;
    unsigned char keb[KEB_TAG_SIZE+1] = "tenForm='"; // Term is between the '' marks. 
    const unsigned char *const db = KanjiInfoClass::getDB();
    
    std::vector<std::size_t> keysIndex; // SHOULD BE NAMED POSITIONS!!
    std::vector<unsigned int> keySum;           // First 3 bytes summed up. 
                                                // Index values match that of keysIndex[]; 
    unsigned int numericKeySum = 0;

    // FIND FIRST KEB !!
        pos = KanjiInfoClass::searchStr( keb, pos );
        pos+= KEB_TAG_SIZE;
        keysIndex.push_back( pos );
        
        
        // SIGNATURE: <Lemma writtenForm='????' partOfSpeech='n'/>
        // keysIndex.push_back( pos+5 ); // pos - 3 == KANJI [ ? ? ? ];

        // skips past keb delim; ( moves position pointer past '<keb>' )        
        numericKeySum = db[pos+2]; numericKeySum |= (db[pos+1]<<8); numericKeySum |= (db[pos+0]<<16);  
        keySum.push_back( numericKeySum );

    // END FIRST KEB !!

    // First sweep finds kanji with okurigana 
    while( true ) {
        pos = KanjiInfoClass::searchStr( keb, pos );
        if( pos == 0 ) { break; } // FAILED TO FIND ANYTHING;         
        pos+= KEB_TAG_SIZE;
        keysIndex.push_back( pos );
        
        // SIGNATURE: <keb>???*</keb>
        // keysIndex.push_back( pos+5 ); // pos - 3 == KANJI [ ? ? ? ];

        // skips past keb delim; ( moves position pointer past '<keb>' )        
        numericKeySum = db[pos+2]; numericKeySum |= (db[pos+1]<<8); numericKeySum |= (db[pos+0]<<16);  
        keySum.push_back( numericKeySum );
    }
      

    // Now have numeric values. Sort keys from Smallest to greatest values; 
    // -1: MUST NOT move the last index(end of file), Would need to code for this exception otherwise
    unsigned int kebSize = keysIndex.size() - 1; 
    for(unsigned int i=0; i < kebSize; i++) { 
        unsigned int smallest = keySum[i];
        unsigned int indx = i;
        
        // Find smallest kebValues ( term );
        for(unsigned int k = i; k < kebSize; k++) { 
            if( keySum[ k ] < smallest ) { 
                smallest = keySum[ k ];
                indx = k;
            }
        }
        
        // if original position was not smaller, swap values around
        if( i != indx ) { 
            // Swap sum totals; 
            smallest = keySum[ i ];
            keySum[ i ] = keySum[ indx ];
            keySum[ indx ] = smallest; 
            
            // Swap index values
            smallest = keysIndex[ i ];
            keysIndex[ i ] = keysIndex[ indx ];
            keysIndex[ indx ] = smallest; 
        }
    }
    KanjiInfoClass::setKeyTable( keysIndex );
    kebSize++; 
    
 if( WORDNET_OPTIMIZE_LEVEL == 1 ) { return ; }
 return ;
     // Too memory intensive to create 2nd lookup table; 
    // Need to rethink methodology; 
    
    // Now, find definitions for the keysIndex table. 
    // Search for tag: 
    keytable_DefinitionPos = new std::size_t [ kebSize + 1 ];

    const int lenOfSenseSyn = 8;
    const unsigned char * termDefineTag = (unsigned char *)"<Synset id='"; // FULL STRING(20): <Synset id='jpn-1.1-????????-?' baseConcept='?'>
    const unsigned char * senseSynTag   = (unsigned char *)"<Sense id='"; // +19 to synRefNum
    const int lenTillSenseRefNum = 19;
    // const unsigned char * synIdString = (unsigned char *)"jpn-1.1-"; // (-20) == start pos of ( termDefineTag ) (if match to Snyset tag)
    const int lenSynIdString = 8;
    const std::size_t START_OF_DEFINITIONS  = KanjiInfoClass::searchStr( termDefineTag, 1 ) - 2;

    std::vector<std::size_t> definitionIndexKeyPos; // SHOULD BE NAMED POSITIONS!!
    pos = START_OF_DEFINITIONS;
    while( (pos = KanjiInfoClass::searchStr( termDefineTag, pos )) ) { 
        definitionIndexKeyPos.push_back( pos );
        pos++;
    }


    // Holds only the id of senceSyn;
    // This will be the search string ( to find definition )
    // Find can be one of 3 possibilities
    //    - 1: the definition as per key ( termDefineTag ) 
    //    - 2: a synsetRelation
    //    - 3: a '<SenseAxis ' holding eng tag id string;
    //    Find possibility 1 by matching to FULL STRING ( not just id Number );
    unsigned char synNumberONLY[ lenOfSenseSyn + 1 ];      
    for(unsigned int i=0, k=0; i < kebSize; i++) { 
        // <Synset id='jpn-1.1-01211489-n' baseConcept='3'>

        // Get the sence number ( matches to a definnition and example sentence; )x
        KanjiInfoClass::setIndex( i+1 );

        // get searchStrings for below loop. 
        // Finding definition/example positions for each SynSence;
        pos = KanjiInfoClass::searchStr( senseSynTag );
        
        k=0;
        while( k < lenOfSenseSyn ) { 
            synNumberONLY[ k ] = db[ pos+ lenTillSenseRefNum + k ];
            k++;
        }
        synNumberONLY[ k ] = '\0';


        // Start an array of synSets
        int synSetPosSize = 0;

        // Find Definition/Example position; Place in synSetPos[]; 
//        pos = START_OF_DEFINITIONS;
//        while( (     pos = KanjiInfoClass::searchStr( synNumberONLY, pos )) )
        std::size_t defKeySize = definitionIndexKeyPos.size();
        for(std::size_t z = 0; z < defKeySize; z++) 
        { 
            pos = definitionIndexKeyPos[z] + 20; // to synNumberONLY;
            int match = 0;
            
            while( synNumberONLY[ match ] ==  db[ pos + match ] ) { 
                match++; 
            } 

            // Now can search for '<Synset id='jpn-1.1-TERM_NUM_BUFF'
            // First find will be definition / example sentence pair; 

            // Search backwards to find if match to *termDefineTag
            // While loop shouldn't buffer overflow, since '\0' MUST NOT
            // be matched in db[]; 
            
            // Matched SynId exactly, this contains GLOSS and Example sentnece. 
              if( match == lenSynIdString ) { 
                // Don't store multiple synset matches; 
                // synSetPos[ synSetPosSize ] = pos;                     
                // synSetPosSize++; 
                
                synSetPosSize++;
                keytable_DefinitionPos[ i ] = pos; 
                break; 
            }
            pos+=1;
            
        }

    }
    
    
     if( WORDNET_OPTIMIZE_LEVEL == 2 ) { return ; } 
}

Wordnet_DictClass::~Wordnet_DictClass() 
{
                                        
    if( keytable_DefinitionPos ) { delete keytable_DefinitionPos; }
}


unsigned int Wordnet_DictClass::termValue(const std::size_t pos) const
{
    const unsigned char *const  db = getDB();
    
    return (
            db[ pos +2 ] | ( 
                             ( db[ pos +1 ] << 8 ) | (db[ pos + 0] <<16) 
                           )
           );
}

unsigned int Wordnet_DictClass::termValue(const unsigned char *const term) const
{
    int len =0;
    while(len < 3 && term[len] ) { len++; }
    
    if( len != 3 ) { return 0; }

    return (
            term[ 2 ] | ( 
                             ( term[ 1 ] << 8 ) | (term[ 0] <<16) 
                           )
           );
    
}

unsigned int Wordnet_DictClass::ballparkIndexVal( const unsigned char *const term) const
{
    // Try optimization. Terms sorted by first 3 bytes(numeric addition of)
    std::size_t SIZE =  KanjiInfoClass::getKeySize();
    if( !SIZE ) { return 0; };
    
    unsigned int quarterSize = SIZE / 4; 
    unsigned int indexVal = 0;
    
    // Iterates by a multiple. Stops one shy of max ( buffer over-run )
    // returns quarter of match;
    const unsigned int termVal = termValue( term );
    for(int i =1; i < 4; i++) { 
        
        // if == to, possibility of duplicates ( MUST BE smaller );
        if( termVal < termValue( KanjiInfoClass::getKeyPos( indexVal+quarterSize )) ) 
        {
            break;
        }
        
        indexVal += quarterSize;
    }

    return indexVal;
}

// Gets a set of synsets for a given index;
// if SIZE() == 0, Changes behavior. index becomes POSITION in file;
// Synsets are IDs of similar term;
std::vector<ustring> Wordnet_DictClass::synset() const
{
    std::vector<ustring> retVal;
    const int lenSYNSET_STR = strlen( (char *)SYNSET_STR );    
    std::size_t  pos = KanjiInfoClass::getPos();            // Safe to use regardless of state. 
    if( pos >= fileLen() ) { return retVal; } // buffoverrun of fileLen considered
    
    const int lenSynsetID_NUMBER = 8;
    unsigned char synsetBuffer[ lenSynsetID_NUMBER+1 ];
    while( (pos = KanjiInfoClass::searchStr( SYNSET_STR, END_LEXICON_ENTRY_STR, pos )) ) 
    {
        pos+= lenSYNSET_STR;
        KanjiInfoClass::readStr( synsetBuffer, lenSynsetID_NUMBER, pos );
    
        retVal.push_back( synsetBuffer );
    }
    
    return retVal;
}
/*
// Gets a set of synsets for a given term;
std::vector<ustring> Wordnet_DictClass::synset(const unsigned char *const term) const
{

    unsigned int SIZE = KanjiInfoClass::getKeySize();
    const unsigned char *const endWrittenForm = (unsigned char *)"'"; // term-end;
    
    std::size_t pos = 1;
    if( SIZE && (pos = KanjiInfoClass::searchStr( SYNSET_STR, 
                                                  endWrittenForm, 
                                                  KanjiInfoClass::getPos() )) ) 
    {
        return synset( KanjiInfoClass::getIndex() );
    }
    const unsigned int indexVal = ballparkIndexVal( term );

    for(unsigned int i=indexVal; i < SIZE; i++) { 
        pos = KanjiInfoClass::getKeyPos( i );
        
        if( (pos = KanjiInfoClass::searchStr( term, endWrittenForm, pos )) ) 
        {
           // i == term; Found synsets; break;
            return synset( i );
        }
        
    }
    
    if( !SIZE ) { 
        std::size_t FOUND_POS;
        pos = 1;
        int len_WRITTEN_FORM = strlen((char *)WRITTEN_FORM);
        while( (pos=KanjiInfoClass::searchStr( WRITTEN_FORM, pos )) )
        {
            pos += len_WRITTEN_FORM; 
            if( (FOUND_POS = KanjiInfoClass::searchStr( term, endWrittenForm, pos )) ) 
            {
               // !SIZE, synset accepts POSITION of term
               // inside <LexiconEntryid='w????; Found synsets; break;
                return synset( FOUND_POS );
            }
        } 
    }
    std::vector<ustring> v;
    return v;
    
}
*/

// Gets a set of synsets for a given index;
// if SIZE() == 0, Changes behavior. index becomes POSITION in file; 
std::vector<ustring> Wordnet_DictClass::lexiconID() const
{
    std::vector<ustring> retVal;

    const int lenSYNSET_STR = strlen( (char *)SENSE_ID_WRITTEN );    
    std::size_t  pos = KanjiInfoClass::getPos();            // Safe to use regardless of state. 
    if( pos >= fileLen() ) { return retVal; } // buffoverrun of fileLen considered

    
    const int lenSynsetID_NUMBER = 7;
    unsigned char synsetBuffer[ lenSynsetID_NUMBER+1 ];    

    while( (pos = KanjiInfoClass::searchStr( SENSE_ID_WRITTEN, END_LEXICON_ENTRY_STR, pos )) ) 
    {
        pos+= lenSYNSET_STR;
        KanjiInfoClass::readStr( synsetBuffer, lenSynsetID_NUMBER, pos );
    
        retVal.push_back( synsetBuffer );
    }

    return retVal;
}

/*
// returns Position 
std::vector<ustring> Wordnet_DictClass::lexiconID(const unsigned char *const term) const
{
    unsigned int SIZE = KanjiInfoClass::getKeySize();
    
    std::size_t pos = 1; 
    if( SIZE ) {
        pos = KanjiInfoClass::getPos();
        if( (pos = KanjiInfoClass::searchStr( SENSE_ID_WRITTEN, END_SENSE_ID_WRITTEN, pos )) ) { 
            return synset( KanjiInfoClass::getIndex() );
        }
    }

    const unsigned int indexVal = ballparkIndexVal( term );

    for(unsigned int i=indexVal; i < SIZE; i++) { 
        pos = KanjiInfoClass::getKeyPos( i );
        
        if( (pos = KanjiInfoClass::searchStr( term, END_SENSE_ID_WRITTEN, pos )) ) 
        {
           // i == term; Found synsets; break;
            return lexiconID( i );
        }
        
    }

    if( !SIZE ) { 

        std::size_t FOUND_POS;
        pos = 1;
        int len_WRITTEN_FORM = strlen((char *)SENSE_ID_WRITTEN);
        while( (pos=KanjiInfoClass::searchStr( SENSE_ID_WRITTEN, pos )) )
        {

            pos += len_WRITTEN_FORM; 
            if( (FOUND_POS = KanjiInfoClass::searchStr( term, END_SENSE_ID_WRITTEN, pos )) ) 
            {
               // i == term; Found synsets; break;
                return lexiconID( FOUND_POS );
            }
        } 
    }
    std::vector<ustring> v;
    return v;
        
}
*/

// Need to search through DB for tag( SYNSET_TAG );
std::size_t  Wordnet_DictClass::setSynsetPos(const unsigned char *const synsetID)
{

    std::size_t SIZE = KanjiInfoClass::getKeySize();
    if( SIZE ) { 
        SIZE -= 1;  // Used in below initialazation; 
        SIZE = KanjiInfoClass::getKeyPos( SIZE ); 
    }

    const std::size_t STARTING_POS = SIZE;
    const unsigned char *const singleQuote = (unsigned char *)"'"; // END_EXAMPLE_STR
    
    std::size_t pos = STARTING_POS;
    if(!pos) { pos = 1; }
    std::size_t FOUND_POS = 1;

    // SYNSET_TAG is definition tag for Synset; 
    while( (pos = KanjiInfoClass::searchStr( SYNSET_TAG, pos )) ) 
    {
        // <Synset id='jpn-1.1-XXXXXXXX-a' baseConcept='3'>    
        pos+= 20; // Match OPTIMIZED Wordnet, position to Start of ID(NUM ONLY); 
//        if( (FOUND_POS = KanjiInfoClass::searchStr( synsetID, END_LEXICON_ENTRY_STR, pos )) ) 
        if( (FOUND_POS = KanjiInfoClass::searchStr( synsetID, singleQuote, pos )) ) 
        {
            // Pos of synsetID ( NUMBER_ONLY )
            savedSynset =  pos;
            return pos;
        }
    }

    return 0;
}


// offset+9 of savedSynset;
//   <Synset id='jpn-1.1-01211489-n' baseConcept='3'>
void Wordnet_DictClass::synsetGrammarNote(unsigned char *retVal) const
{
    retVal[0] = '\0';
    

    const unsigned char *const END_quote = (unsigned char *)"'";
    
    std::size_t pos = savedSynset+9;
    KanjiInfoClass::readStr( retVal, 
                             KanjiInfoClass::searchStr(END_quote, pos) - pos, // SIZE
                             pos );
    return ;    
}
// Then find if a match to synsetID; 
int Wordnet_DictClass::defineSynset(unsigned char *retVal) const
{
    retVal[0] = '\0';
    const int len_GLOSS_STR =  strlen( (char *)GLOSS_STR );
    unsigned char synsetID[ 8 + 1 ];
    const unsigned char *const END_quote = (unsigned char *)"\"";
    // ID NUMBER
    KanjiInfoClass::readStr( synsetID, 8, savedSynset+20 );
    
    std::size_t FOUND_POS = KanjiInfoClass::searchStr( GLOSS_STR, END_SYNSET_TAG, savedSynset ) + len_GLOSS_STR;
    KanjiInfoClass::readStr( retVal, 
                             KanjiInfoClass::searchStr( END_quote, FOUND_POS ) - FOUND_POS, // SIZE STRING;
                             FOUND_POS   // pos;
                           );
    return 0;
}

// Grab all examples for synset;
std::vector<ustring> Wordnet_DictClass::examples() const
{
    std::vector<ustring> retExamples;
    unsigned char buff[320];
    const int len_EXAMPLE_STR = strlen( (char *)EXAMPLE_STR);

    std::size_t pos = savedSynset;
    
    while( (pos = KanjiInfoClass::searchStr( EXAMPLE_STR, END_SYNSET_TAG, pos )) )
    {
        pos += len_EXAMPLE_STR;
        KanjiInfoClass::readStr( buff, 
                                 KanjiInfoClass::searchStr(END_EXAMPLE_STR, pos) - pos, // SIZE
                                 pos );

        retExamples.push_back( buff );
    }
    
    return retExamples;
}

// Grab all synRelations;
std::vector<ustring> Wordnet_DictClass::synRealtions() const
{
    std::vector<ustring> retRelationIDs;
    unsigned char buff[320];
    const int lenOfSenseSyn = 8;
    const int len_EXAMPLE_STR = strlen( (char *)SYNSET_RELATION);
    const int offsetOfId = 8; // to offset of id without('jpn-1.1-'); 
    std::size_t pos = savedSynset;
    
    while( (pos = KanjiInfoClass::searchStr( SYNSET_RELATION, END_SYNSET_TAG, pos )) )
    {
        pos += len_EXAMPLE_STR + offsetOfId;
        KanjiInfoClass::readStr( buff, 
                                 lenOfSenseSyn, // SIZE
                                 pos );

        retRelationIDs.push_back( buff );
    }
    
    return retRelationIDs;
}

// Grab all synRelations;
std::vector<ustring> Wordnet_DictClass::synRealtionTypes() const
{
    std::vector<ustring> retRelationTypes;
    unsigned char buff[20];
    const int len_EXAMPLE_STR = strlen( (char *)SYNSET_RELATION_TYPE);
    std::size_t pos = savedSynset;
    const unsigned char *const singleQuote = (unsigned char *)"'";
    
    while( (pos = KanjiInfoClass::searchStr( SYNSET_RELATION_TYPE, END_SYNSET_TAG, pos )) )
    {
        pos += len_EXAMPLE_STR;
        KanjiInfoClass::readStr( buff, 
                                 KanjiInfoClass::searchStr( singleQuote, pos ) - pos, // SIZE
                                 pos );

        retRelationTypes.push_back( buff );
    }
    
    return retRelationTypes;
}


//           std::vector<ustring> synRelations(const unsigned char *const ) const;



// Virtual member, repurpose for Wordnet_DictClass; 
// Arg1: term to define;
// Return: 0-on-fail; Return IndexValue of Term on success; 
// Return: 0-on-fail: setKeyTable_IS_NOT_RUN, return POSITION on success;( not -1 );
// Special implementation: if no setKeyTable() was called(set), 
// Defaults to POSITION instead of Index of Kanjil; 
std::size_t Wordnet_DictClass::kanjiNumber(const unsigned char *term) const
{
    const int term_MAX_SIZE = 24;
    if( !term ) { return 0; }
    
    const int STR_SIZE = strlen((char *)term);
    if( STR_SIZE > term_MAX_SIZE ) { return 0; }

    
    std::size_t beg = 0;
    // This should be safe (getKeyPos won't throw run-time error ); 
    // Safe to run below searchStr(); - Arg2 MUST NOT be 0 when no keyTable set ( Wordnet.getKeySize()==0 );
    // ^ safe; built into (base) class; 
    if( searchStr( term, getKeyPos( getIndex() ) ) == getKeyPos( getIndex() ) ) 
    {        
        // Should not be allowed here if                   
        beg = getKeyPos( getIndex() );
        return beg; // Fail on 0; 
    }

    // Find strPos;
    int STR_LEN = STR_SIZE;
    if( STR_LEN > term_MAX_SIZE ) { STR_LEN = term_MAX_SIZE; }
    unsigned char tmp[ term_MAX_SIZE + 2 + 1 + 1 ]; // Prepend ='(2), Suffix '(1) + NULL_TERMINATOR(1)
    int i=0; 
    tmp[ i ] = '\'';
    i++;
    for(int j=0; j < STR_LEN; j++, i++) { 
     tmp[ i ] = term[ j ];
    }
    tmp[ i ] = '\'';
    tmp[ i +1 ] = '\0';
    i += 1;
    std::size_t POSITION;
    
    if( WORDNET_OPTIMIZE_LEVEL > 0 ) {
        // Sets str to search to char AFTER quote char; (including ending quote delim );
        for(i=0; i < STR_LEN; i++){ 
            tmp[ i ] = tmp[i+1];
        } 
        tmp[ i ] = '\'';
        tmp[ i+1 ] = '\0';
        
        std::size_t indx = 1;
        const unsigned char *const ending_char_singlequote = (unsigned char *)"'";
        
        std::size_t SIZE = getKeySize();
        // Strict search, only returns term matching whole word in dict. 
        for(indx = 1; indx < SIZE; indx++) { 
            POSITION = getKeyPos(indx);
            
            if( searchStr( tmp, ending_char_singlequote, POSITION ) == POSITION ) { 
                return indx; 
            }
        }


        // Loose search ( returns positive for match INSIDE whole term;
        for(indx = 1; indx < SIZE; indx++) { 
            POSITION = getKeyPos(indx);
            if( searchStr( term, ending_char_singlequote, POSITION ) ) { 
                return indx; 
            }
        }
    } else { 
        POSITION = searchStr( tmp, 1 );
        if( POSITION == 0  ) { 
         // Test out; Try find first occurrence LOOSELY ( without ending quote );
         tmp[i-1] = '\0';
         POSITION = searchStr( tmp, 1 );
        }
        if( POSITION ) { // ACTUALLY CODED SO POSSIBLE 0; FIX THIS?
         POSITION++; // skip past first char; 
         return POSITION;
        }
    }
    
    return 0; // Fail on 0;
}


// Call setKanji(unsigned char *kanji) before calling kunyomi;
std::vector<ustring> Wordnet_DictClass::kunyomi() const
{
    std::vector<ustring> words ;//this vector will be returned    
    return words; 
}


// Call setKanji(unsigned char *kanji) before calling onyomi;
std::vector<ustring> Wordnet_DictClass::onyomi() const
{                     
    std::vector<ustring> words ;//this vector will be returned    
    return words;
}


// setSynsetPos has been run; 
// Helper function, returns all WrittenForms associatiated w/sysnsetIds;
std::vector<ustring> synsetIdWrittenForm(Wordnet_DictClass &WN)
{
    std::vector<ustring> retWrittenForm;
    unsigned char buff[320];
    std::size_t SAVED_SYNSET_INDEX = WN.getIndex(); // term lookup; 
    std::size_t SYNSET_INDEX = 1;
    WN.resetKanjiIndex(); // CANNOT == 0; 
    
    std::vector<ustring> synsetRelTypes = WN.synRealtionTypes();
    
    std::vector<ustring> synsetRelations = WN.synRealtions();

    const unsigned char ** sr  = (const unsigned char**)&synsetRelations[0];
    const int synSIZE = synsetRelations.size();
    
    const unsigned char *const writtenFormTag =    (unsigned char *)"<Lemma writtenForm='";
    const int writtenFormTagSize = 20;
    const unsigned char *const EndWrittenForm = (unsigned char *)"'"; //  iconEntry>";
    const unsigned char *const EndLexiconEntry = (unsigned char *)"</Lex"; //  iconEntry>";
    //    <Sense id='w230510_00668805-v' synset='jpn-1.1-00668805-v'/>
    
    const unsigned char ** retForm;
    std::size_t pos = 1; // WN.getPos(); // MUST NOT == 0 when searching; 
    std::size_t pos2 = 1;

    
    // Find EVERY writtenFormTag, then search for synsetId; 
    while( (pos= WN.searchStr( writtenFormTag, pos )) ) {
        pos+= writtenFormTagSize; 
        
        // Iterate through synsetRelations();
        // Search for matches in current LexiconEntry; 
	std::size_t sizeOf; // comp with strlen()
	for(int i=0; i < synSIZE; i++) {
            pos2 = WN.searchStr( sr[i], EndLexiconEntry, pos );
            
            // Match found, read writtenForm and add to retWrittenForm( if not duplicate )
            if( pos2 ) { 
                WN.readStr( buff, 
                            WN.searchStr( EndWrittenForm, pos ) - pos, // strSize;  
                            pos 
                          );
                sizeOf = strlen( (char *) buff );
                int SIZE = retWrittenForm.size();
                int notMatch = 1;
                
                // Only add to retWrittenForm if not a match
                for(int c = 0; c < SIZE; c++) { 
                    if( strlen( (char *)retForm[c] ) == sizeOf  ) { 
                        //match += memcmp ( buff, retWrittenForm[c], sizeOf );
                        if( 0 == (notMatch = memcmp ( buff, retForm[c], sizeOf )) ) { break; } // breaks if match is found ( match == 0 )
                    }
                }
                
                // didin't find a match
                if( notMatch ) { 
                    // Add to writtenForms found; 
                    
                    // Add relation type;
                    buff[sizeOf] = '('; buff[sizeOf+1] = ' ';
                    sizeOf += 2;
                    int z=0;
                    for(; synsetRelTypes[i][z]; z++) { 
                        buff[ sizeOf+z ] = synsetRelTypes[i][z];
                    }
                    buff[sizeOf+z] = ' '; buff[sizeOf+z+1] = ')';
                    buff[sizeOf+z+2] = '\0';  
                    
                    
                    retWrittenForm.push_back( buff );
                    retForm  = (const unsigned char**)&retWrittenForm[0];
                }
            }
        }
        
        SYNSET_INDEX++; 


        // Speeds up lookups by placing pos at next LexiconEntry; 
        // if no LookupTable, sets to Arg1; 
        // Safely handles out-of-bounds; 
        if( WN.setIndex( SYNSET_INDEX ) == 0 ) { // Success 
            pos = WN.getPos(); 
        }
    }

    WN.setIndex(SAVED_SYNSET_INDEX); // restore state;         
    return retWrittenForm;    
}



// Helper function, returns sentences containing term;
// Arg1: retValue - positions of ID(NUM_ONLY) and Sentence pairs.
// Arg2: this
// Arg3: term to use when searching for sentences
// Arg4: tag to search for within SYNSET tag "<SYNSET"
// Arg5: ending delim string for Arg4.
// Arg6: number of sentences to grab ( if avalaible )
// Arg7: example sentences already added. 
void getExampleSentences_getPositions(   std::size_t *retPos,
                                         const Wordnet_DictClass &WN,
                                         const unsigned char *const term,
                                         const unsigned char *const tagInsideSynset,
                                         const unsigned char *const tagInsideSynsetEnd,
                                         const int &nsize,
                                         std::vector<ustring> examplesAlreadyAdded )
{
    retPos[0] = 0; retPos[1] = 0; retPos[2] = 0; // ending delim;
    int retPosIndex = 0;
    
    // retPos max will be third of MAX_SIZE; ( holds synID / sentence Position pairs / sentencePositionIndex_WITHIN_synID );
    const int examplesAlreadyAdded_MAX_SIZE = 254;
    const int RETPOS_SIZE_ALLOCATED = examplesAlreadyAdded_MAX_SIZE / 3; 

    if( !term || nsize <= 0 ) { return ; }

    const unsigned char **alreadyAdded = 0;
    const int NUM_EXAMPLES_ALREADY_ADDED = examplesAlreadyAdded.size();
    int NUM_EXAMPLES_ALREADY_ADDED_LEN = NUM_EXAMPLES_ALREADY_ADDED;
    if( NUM_EXAMPLES_ALREADY_ADDED_LEN > examplesAlreadyAdded_MAX_SIZE )
        NUM_EXAMPLES_ALREADY_ADDED_LEN = examplesAlreadyAdded_MAX_SIZE; 
    int LENS_ALREADY_ADDED[ +1 ];
    if( NUM_EXAMPLES_ALREADY_ADDED_LEN  ) 
    {
       alreadyAdded = (const unsigned char**)&examplesAlreadyAdded[0];
       int len = 0;
       for(int i =0; i < NUM_EXAMPLES_ALREADY_ADDED_LEN; i++) { 
           len = 0;
           while( alreadyAdded[i][len] ) { len++; }
           
           LENS_ALREADY_ADDED[ i ] = len;
       }
    }

    int numSentenceCollected = 0;

    const int NEAR_PTR_SYNID_NUM_ONLY = 20;
    const unsigned char *const SYNSET_TAG = (unsigned char *)"<Synset id";    
    const int lenTagInsideSynset = strlen( (char *)tagInsideSynset ); // 
    const unsigned char *const END_SYNSET_TAG = (unsigned char *)"</Synset>";
    
    unsigned char buff[320];
    std::size_t pos = WN.searchStr( SYNSET_TAG, 1 ); // Starting sysnet def/example; 
    std::size_t synid_pos = pos; // stores synid pos; 


    while( (synid_pos = WN.searchStr( SYNSET_TAG, synid_pos ) )) { 
        synid_pos += NEAR_PTR_SYNID_NUM_ONLY; 
        pos = synid_pos;
        
        // May be mutiple Examples; Test for all of them to add. 
        int indexWithinSynsetTag = 0;
        while( (pos = WN.searchStr( tagInsideSynset, END_SYNSET_TAG, pos ) )) { 
            pos += lenTagInsideSynset;
            
            // If term found in example str, Add to retSentences;
            if( WN.searchStr( term, tagInsideSynsetEnd, pos ) ) {
                WN.readStr( buff,
                            WN.searchStr( tagInsideSynsetEnd, pos ) - pos, // length;
                            pos
                          );
                
                int numExamplesNotMatching = 0;
                for( int i =0, LENGTH, len; numExamplesNotMatching < NUM_EXAMPLES_ALREADY_ADDED_LEN; numExamplesNotMatching++ ) {
                    
                    LENGTH = 0;
                    while( buff[ LENGTH ] ) { LENGTH++; }
                    
                    if( LENGTH ==  LENS_ALREADY_ADDED[ i ] ) {
    
                        // Possible match, test; 
                        for( len=0; len < LENGTH; len++) { 
                            if( buff[ len ] != alreadyAdded[i][len] ) { 
                                break; 
                            }
                        }
                        
                        // Break if match found ( DO NOT ADD buff[] )
                        if( len == LENGTH ) { break; }
                    }  
                }
                
                if( numExamplesNotMatching == NUM_EXAMPLES_ALREADY_ADDED_LEN ) { 
                    
                    // No match found, add here; 
                    retPos[ retPosIndex   ] = synid_pos;
                    retPos[ retPosIndex+1 ] = pos; 
                    retPos[ retPosIndex+2 ] = indexWithinSynsetTag;
                    retPosIndex += 3; 
                    
                    numSentenceCollected++; 
                }
                
                if( numSentenceCollected == nsize || 
                    numSentenceCollected == RETPOS_SIZE_ALLOCATED ) { break; }
                    
                indexWithinSynsetTag++; 
            }
        }

        if( numSentenceCollected == nsize || 
            numSentenceCollected == RETPOS_SIZE_ALLOCATED ) { break; }

    }
    retPos[ retPosIndex   ] = 0;
    retPos[ retPosIndex+1 ] = 0;    
    retPos[ retPosIndex+2 ] = 0;    
    
    return ;
}


/*
   Example Setences containing Term:
               const unsigned char *const EXAMPLE_STR = (unsigned char *)"<Statement example=\"";    
               const unsigned char *const exampleEnd = (unsigned char *)"\"";
   Gloss Sentences containing Term:
               const unsigned char *const exampleSentenceStr = (unsigned char *)"<Definition gloss=\"";
               const unsigned char *const exampleEnd = (unsigned char *)"\"";

   can also grab these w/caareful 'term'(arg2) manipulation.
              <SynsetRelations>
                 <SynsetRelation targets='jpn-1.1-01785180-a' relType='sim'/>
              </SynsetRelations>
   
*/

// Helper function, returns sentences containing term;
// Arg1: this
// Arg2: term to use when searching for sentences
// Arg3: tag to find match ( skips past this string )
// Arg4: ending string (after Arg3);
// Arg5: number of sentences to grab ( if avalaible )
// Arg6: example sentences already added. 
std::vector<ustring> getStringsMatchingTermInsideTag(const Wordnet_DictClass &WN,
                                         const unsigned char *const term,
                                         const unsigned char *const begTagStr,
                                         const unsigned char *const endBegTagStr,
                                         const int &n,
                                         std::vector<ustring> examplesAlreadyAdded )
{
    const int examplesAlreadyAdded_MAX_SIZE = 254;
    std::vector<ustring> retSentences;
    if( !term || n <= 0 ) { return retSentences; }
    
    const int synID_Example_Position_Pairs_SIZE = examplesAlreadyAdded_MAX_SIZE;
    std::size_t synID_Example_Position_Pairs[ examplesAlreadyAdded_MAX_SIZE + 1 ];

    getExampleSentences_getPositions( synID_Example_Position_Pairs,
                                      WN,
                                      term,
                                      begTagStr,
                                      endBegTagStr,
                                      n,
                                      examplesAlreadyAdded
                                    );  

    unsigned char buff[255]; // Possible buffer overrun. Sentence MUST NOT exceed 255; 
    std::size_t pos = 1;
    int i =0;

    while( synID_Example_Position_Pairs[ i ] ) 
    { 
        WN.readStr( buff,
                    WN.searchStr( endBegTagStr, synID_Example_Position_Pairs[ i+1 ] ) 
                       - synID_Example_Position_Pairs[ i+1 ], // length;
                    synID_Example_Position_Pairs[ i+1 ]
                  );
        retSentences.push_back( buff );
        
        i+=3;
    }
    
    return retSentences;
}



// Helper function, returns sentences containing term;
// Arg1: this
// Arg2: term to use when searching for sentences
// Arg3: number of sentences to grab ( if avalaible )
// Arg4: example sentences already added. 
// Arg5: 3 data sets possible to grab 
//       - Synid(NUM ONLY)
//       - gloss setnences
//       - ?
std::vector<ustring> getExampleSentences(const Wordnet_DictClass &WN,
                                         const unsigned char *const term,
                                         const int &n,
                                         std::vector<ustring> examplesAlreadyAdded,
                                         const int GRAB_DATASET_NUMBER = 1)
{
    const int examplesAlreadyAdded_MAX_SIZE = 254;
    std::vector<ustring> retSentences;
    if( !term || n <= 0 ) { return retSentences; }
    
    const int synID_Example_Position_Pairs_SIZE = examplesAlreadyAdded_MAX_SIZE;
    std::size_t synID_Example_Position_Pairs[ examplesAlreadyAdded_MAX_SIZE + 1 ];
    const unsigned char *const EXAMPLE_STR = (unsigned char *)"<Statement example=\"";    
    const unsigned char *const exampleEnd = (unsigned char *)"\"";

    getExampleSentences_getPositions( synID_Example_Position_Pairs,
                                      WN,
                                      term,
                                      EXAMPLE_STR,
                                      exampleEnd,
                                      n,
                                      examplesAlreadyAdded
                                    );  

    unsigned char buff[255]; // Possible buffer overrun. Sentence MUST NOT exceed 255; 
    std::size_t pos = 1;
    int i =0, k;
    while( synID_Example_Position_Pairs[ i ] ) 
    { 
        if( GRAB_DATASET_NUMBER == 1 ) { // i+1  
            WN.readStr( buff,
                        WN.searchStr( exampleEnd, synID_Example_Position_Pairs[ i+GRAB_DATASET_NUMBER ] ) 
                           - synID_Example_Position_Pairs[ i+GRAB_DATASET_NUMBER ], // length;
                        synID_Example_Position_Pairs[ i+GRAB_DATASET_NUMBER ]
                      );
        } 
        else if( GRAB_DATASET_NUMBER == 0 ) 
            WN.readStr( buff, 
                        8,
                        synID_Example_Position_Pairs[ i+GRAB_DATASET_NUMBER ]
                      );  
        else if( GRAB_DATASET_NUMBER == 2 ) 
        {
           std::size_t number = synID_Example_Position_Pairs[ i+GRAB_DATASET_NUMBER ];
           // CPY + PASTE; 
                
                std::size_t len = 0;
                std::size_t mult = 1;
                while( mult ==1 || mult <= number ) { mult *= 10; len++; }
                if( len == 0 ) { 
                    buff[0]=0x30; buff[1]='\0';  
                }
                else 
                {
                    mult /= 10;
                    
                    std::size_t copyI = synID_Example_Position_Pairs[ i+GRAB_DATASET_NUMBER ];
                    for(unsigned int c=0, num=0; c < len-1; c++, mult /= 10) {
                        num = (copyI / mult );
                        copyI -=  (num * mult );
                        buff[c] = num + 0x30;
                    }
                    buff[len-1] = (0x30+(number%10));
                    buff[len ] = '\0';
                }                         
        }
        retSentences.push_back( buff );
        
        i+=3;
    }
    
    return retSentences;
}


// Helper function, returns sentences containing term;
// Arg1: this
// Arg2: term to use when searching for sentences
// Arg3: number of sentences to grab ( if avalaible )
// Arg4: example sentences already added. 
// Arg5: 3 data sets possible to grab 
//       - Synid(NUM ONLY)
//       - gloss setnences
//       - ?
std::vector<ustring> getGlossSentences(const Wordnet_DictClass &WN,
                                         const unsigned char *const term,
                                         const int &n,
                                         std::vector<ustring> examplesAlreadyAdded,
                                         const int GRAB_DATASET_NUMBER = 1)
{
    const int examplesAlreadyAdded_MAX_SIZE = 254;
    std::vector<ustring> retSentences;
    if( !term || n <= 0 ) { return retSentences; }
    
    const int synID_Example_Position_Pairs_SIZE = examplesAlreadyAdded_MAX_SIZE;
    std::size_t synID_Example_Position_Pairs[ examplesAlreadyAdded_MAX_SIZE + 1 ];
    const unsigned char *const definitionStr = (unsigned char *)"<Definition gloss=\"";
    const unsigned char *const definitionEnd = (unsigned char *)"\"";

    getExampleSentences_getPositions( synID_Example_Position_Pairs,
                                      WN,
                                      term,
                                      definitionStr,
                                      definitionEnd,
                                      n,
                                      examplesAlreadyAdded
                                    );  

    unsigned char buff[255]; // Possible buffer overrun. Sentence MUST NOT exceed 255; 
    std::size_t pos = 1;
    int i =0, k=0;
    while( synID_Example_Position_Pairs[ i ] ) 
    { 
        if( GRAB_DATASET_NUMBER == 1 ) { // i+1  
            WN.readStr( buff,
                        WN.searchStr( definitionEnd, synID_Example_Position_Pairs[ i+1 ] ) 
                           - synID_Example_Position_Pairs[ i+1 ], // length;
                        synID_Example_Position_Pairs[ i+1 ]
                      );
        } 
        else if( GRAB_DATASET_NUMBER == 0 ) 
            WN.readStr( buff, 
                        8,
                        synID_Example_Position_Pairs[ i+GRAB_DATASET_NUMBER ]
                      );  
        else if( GRAB_DATASET_NUMBER == 2 ) 
        {
           std::size_t number = synID_Example_Position_Pairs[ i+GRAB_DATASET_NUMBER ];
           // CPY + PASTE; 
                
                std::size_t len = 0;
                std::size_t mult = 1;
                while( mult ==1 || mult <= number ) { mult *= 10; len++; }
                if( len == 0 ) { 
                    buff[0]=0x30; buff[1]='\0';  
                }
                else 
                {
                    mult /= 10;
                    
                    std::size_t copyI = synID_Example_Position_Pairs[ i+GRAB_DATASET_NUMBER ];
                    for(unsigned int c=0, num=0; c < len-1; c++, mult /= 10) {
                        num = (copyI / mult );
                        copyI -=  (num * mult );
                        buff[c] = num + 0x30;
                    }
                    buff[len-1] = (0x30+(number%10));
                    buff[len ] = '\0';
                }                         
        }
        retSentences.push_back( buff );
        
        i+=3;
    }
    
    return retSentences;
}


} // NAMESPACE; 


