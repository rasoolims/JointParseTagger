package JointPosTaggerParser;

import SemiSupervisedPOSTagger.Structures.Pair;
import SemiSupervisedPOSTagger.Tagging.Tagger;
import YaraParser.Structures.IndexMaps;
import YaraParser.Structures.InfStruct;
import YaraParser.Structures.Sentence;
import YaraParser.TransitionBasedSystem.Configuration.Configuration;
import YaraParser.TransitionBasedSystem.Parser.KBeamArcEagerParser;

import java.util.ArrayList;
import java.util.Arrays;

/**
 * Created by Mohammad Sadegh Rasooli.
 * ML-NLP Lab, Department of Computer Science, Columbia University
 * Date Created: 3/2/15
 * Time: 3:40 PM
 * To report any bugs or problems contact rasooli@cs.columbia.edu
 */

public class ParseResult {
    int[] heads;
    String[] depLabels;
    String[] tags;
    final String[] words;
    float parseScore;
    float taggingScore;
    
    
    public ParseResult(final String[] words, Info info) throws Exception {
        this.words=words;
        SemiSupervisedPOSTagger.Structures.Sentence sentence =new SemiSupervisedPOSTagger.Structures.Sentence(words,info.posMaps);
        Pair<int[],Float> origTags = info.tagger.tagWithScore(sentence, false);
        String[] origTagsStr = new String[origTags.first.length];
        for (int i = 0; i < origTags.first.length; i++)
            origTagsStr[i] = info.posMaps.reversedMap[origTags.first[i]];
        Sentence parsingSentence = info.parserMaps.makeSentence(words, origTagsStr, info.infStruct.options.rootFirst, info.infStruct.options.lowercase);
        Configuration bestParse = info.parser.parse(parsingSentence, info.infStruct.options.rootFirst, info.infStruct.options.beamWidth, 8);


        this.tags=origTagsStr;
        this.heads = new int[words.length];
        this.depLabels = new String[words.length];

        for (int i = 0; i < words.length; i++) {
            int head = bestParse.state.getHead(i + 1);
            if (head == words.length + 1)
                head = 0;
            this.heads[i] = head;
            this.depLabels[i] = info.parserMaps.revWords[bestParse.state.getDependency(i + 1)] ;
        }
        
        this.parseScore=bestParse.score/words.length;
        this.taggingScore= origTags.second;
    }

    public String getConllOutput() {
        StringBuilder output = new StringBuilder();
        for (int i = 0; i < words.length; i++) {
            output.append((i + 1) + "\t" + words[i] + "\t_\t" + tags[i] + "\t_\t_\t" + heads[i] + "\t" + depLabels[i] + "\t_\t_\n");
        }
        output.append("\n");
        return output.toString();
    }

    public int[] getHeads() {
        return heads;
    }

    public String[] getDepLabels() {
        return depLabels;
    }

    public String[] getTags() {
        return tags;
    }

    public float getParseScore() {
        return parseScore;
    }

    public float getTaggingScore() {
        return taggingScore;
    }
}
