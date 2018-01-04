Project includes material from the JMdict, KANJIDIC2 dictionary files in accordance with the licence provisions of the Electronic Dictionaries Research Group. 
See http://www.csse.monash.edu.au/~jwb/edict.html and http://www.edrdg.org/

Project includes material from Japanese Wordnet
Japanese Wordnet (vXX) © 2009-2011 NICT, 2012-2015 Francis Bond and 2016-2017 Francis Bond, Takayuki Kuribayashi http://compling.hss.ntu.edu.sg/wnja/index.en.html

Define words in Japanese via terminal. ( Requires Japanese Wordnet's DB ) FILE: Japanese Wordnet as WordNet-LMF (xml)
adds Kanji info to a .cvs file ( for importing into Anki? )
# What does it do? See below ( Using list.txt file )

# -- list.txt --
流行
はやり
fashion, fad, vogue, craze
# -- END --


# Command:
kanjiInfo.exe -i list.txt -K F K O T Y -o out.cvs -M -C \; -R -G

# Output:
<h1 class="front_question">流行</h1>;<p class="front_kana">はやり</p>Gloss:<br><label class="gloss">ある<ruby>時代<rp>(</rp><rt>じだい</rt><rp>)</rp></ruby>に<ruby>人気<rp>(</rp><rt>にんき</rt><rp>)</rp></ruby>のある<ruby>趣味<rp>(</rp><rt>しゅみ</rt><rp>)</rp></ruby>。</label><br><br>Example Sentences:<br><label class="sentences"><ruby>皮<rp>(</rp><rt>かわ</rt><rp>)</rp></ruby>が<ruby>最新<rp>(</rp><rt>さいしん</rt><rp>)</rp></ruby>の<ruby>流行<rp>(</rp><rt>はやり</rt><rp>)</rp></ruby>だ</label><br><label class="sentences"><ruby>彼<rp>(</rp><rt>あれ</rt><rp>)</rp></ruby>は<ruby>時流<rp>(</rp><rt>じりゅう</rt><rp>)</rp></ruby>を追いかけた</label><br><label class="sentences">1920<ruby>年代<rp>(</rp><rt>ねんだい</rt><rp>)</rp></ruby>は<ruby>独自<rp>(</rp><rt>どくじ</rt><rp>)</rp></ruby>の<ruby>流行<rp>(</rp><rt>はやり</rt><rp>)</rp></ruby>があった</label><br><br><h1 class="main_kanji">流</h1><label class="main_onyomi">リュウ・ル</label><label class="main_kunyomi">・なが.れる・なが.れ・なが.す・-なが.す</label><br><label class="main_compounds">流.れる ながれる to stream, to flow (liquid, time, etc.), to run (ink)<br>流.れ ながれ stream, current, flow<br>流.す ながす to drain, to pour, to spill, to shed (blood, tears)<br>流.す ながす to drain, to pour, to spill, to shed (blood, tears)<br></label><br><label class="main_translation">current, a sink, flow, forfeit</label><br><h1 class="main_kanji">行</h1><label class="main_onyomi">コウ・ギョウ・アン</label><label class="main_kunyomi">・い.く・ゆ.く・-ゆ.き・-ゆき・-い.き・-いき・おこな.う・おこ.なう</label><br><label class="main_compounds">行.く いく to go, to move (in a direction or towards a specific location), to head (towards), to be transported (towards), to reach<br>行.く いく to go, to move (in a direction or towards a specific location), to head (towards), to be transported (towards), to reach<br>行.き ゆき bound for ...<br>行.き ゆき bound for ...<br>行.う おこなう to perform, to do, to conduct oneself, to carry out<br>行.なう おこなう to perform, to do, to conduct oneself, to carry out<br></label><br><label class="main_translation">going, journey, carry out, conduct, act, line, row, bank</label><br>;<br>fashion, fad, vogue, craze<br>;
