package JointPosTaggerParser;

import SemiSupervisedPOSTagger.Structures.Pair;
import YaraParser.Structures.Sentence;
import YaraParser.TransitionBasedSystem.Configuration.Configuration;

import java.util.concurrent.Callable;

/**
 * Created by Mohammad Sadegh Rasooli.
 * ML-NLP Lab, Department of Computer Science, Columbia University
 * Date Created: 3/31/15
 * Time: 4:48 PM
 * To report any bugs or problems contact rasooli@cs.columbia.edu
 */

public class ParserThread implements Callable<Pair<ParseResult,Integer>>{
    Info info;
    final String[] words;
    int senID;

    public ParserThread(int senID,String[] words, Info info) {
        this.senID=senID;
        this.words = words;
        this.info = info;
    }

    @Override
    public Pair<ParseResult,Integer> call() throws Exception {
       return  new Pair<ParseResult, Integer>(new ParseResult(words,info,1),senID);
    }
}
