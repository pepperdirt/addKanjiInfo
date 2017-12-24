Project includes material from the JMdict, KANJIDIC2 dictionary files in accordance with the licence provisions of the Electronic Dictionaries Research Group. 
See http://www.csse.monash.edu.au/~jwb/edict.html and http://www.edrdg.org/

adds Kanji info to a .cvs file ( for importing into Anki? )
# What does it do? See below ( Using list.txt file )

# -- list.txt --
流行
はやり
fashion, fad, vogue, craze
# -- END --


# Command:
kanjiInfo.exe -J JMdict_e_ALL_LANGUAGES.xml -W kanjidic2.xml -i list.txt -K K O T Y -o out.cvs -M -C ;

# Output:
流行;はやり<h1 class="main_kanji">流</h1><label class="main_onyomi">リュウ・ル</label><label class="main_kunyomi">・なが.れる・なが.れ・なが.す・-なが.す</label><br><label class="main_compounds">流.れる ながれる to stream, to flow (liquid, time, etc.), to run (ink)<br>流.れ ながれ stream, current, flow<br>流.す ながす to drain, to pour, to spill, to shed (blood, tears)<br>流.す ながす to drain, to pour, to spill, to shed (blood, tears)<br></label><br><label class="main_translation">current, a sink, flow, forfeit</label><br><h1 class="main_kanji">行</h1><label class="main_onyomi">コウ・ギョウ・アン</label><label class="main_kunyomi">・い.く・ゆ.く・-ゆ.き・-ゆき・-い.き・-いき・おこな.う・おこ.なう</label><br><label class="main_compounds">行.く いく to go, to move (in a direction or towards a specific location), to head (towards), to be transported (towards), to reach<br>行.く いく to go, to move (in a direction or towards a specific location), to head (towards), to be transported (towards), to reach<br>行.き ゆき bound for ...<br>行.き ゆき bound for ...<br>行.う おこなう to perform, to do, to conduct oneself, to carry out<br>行.なう おこなう to perform, to do, to conduct oneself, to carry out<br></label><br><label class="main_translation">going, journey, carry out, conduct, act, line, row, bank</label><br>;fashion, fad, vogue, craze;


