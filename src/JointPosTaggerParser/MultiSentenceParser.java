package JointPosTaggerParser;

import SemiSupervisedPOSTagger.Structures.Pair;

import java.util.concurrent.CompletionService;
import java.util.concurrent.ExecutorCompletionService;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

/**
 * Created by Mohammad Sadegh Rasooli.
 * ML-NLP Lab, Department of Computer Science, Columbia University
 * Date Created: 3/31/15
 * Time: 4:52 PM
 * To report any bugs or problems contact rasooli@cs.columbia.edu
 */

public class MultiSentenceParser {
    ExecutorService executor;
    CompletionService<Pair<ParseResult,Integer>> pool;
    final Info info;

    public MultiSentenceParser(int numofThreads, Info info) {
        executor = Executors.newFixedThreadPool(numofThreads);
        pool = new ExecutorCompletionService<Pair<ParseResult,Integer>>(executor);
        this.info=info;
        System.out.println("loading all models done!");
    }

    public MultiSentenceParser(int numofThreads, String tagModelPath, String parseModelPath) throws Exception {
        executor = Executors.newFixedThreadPool(numofThreads);
        pool = new ExecutorCompletionService<Pair<ParseResult,Integer>>(executor);
        this.info=new Info(tagModelPath,parseModelPath,numofThreads);
        System.out.println("loading all models done!");
    }


    public ParseResult[] parseSentences(String[][] sentences) throws Exception {
        ParseResult[] results = new ParseResult[sentences.length];

        for(int i=0;i<sentences.length;i++)
            pool.submit(new ParserThread(i,sentences[i],info));
        
        for(int i=0;i<sentences.length;i++){
            Pair<ParseResult,Integer> pair=pool.take().get();
            results[pair.second]=pair.first;
        }

        return results;

    }

    public ParseResult[] parseMergedWordSentences(String[] sentences)  {
        try {
            ParseResult[] results = new ParseResult[sentences.length];


            for (int i = 0; i < sentences.length; i++)
                pool.submit(new ParserThread(i, sentences[i].split(" "), info));

            for (int i = 0; i < sentences.length; i++) {
                Pair<ParseResult, Integer> pair = pool.take().get();
                results[pair.second] = pair.first;
            }

            return results;
        }catch (Exception ex){
            System.err.println(ex.getMessage()+"\n\n");
            ex.printStackTrace();
            return  null;
        }
    }

    public void shutDownLiveThreads() {
        info.turnOff();
        executor.shutdown();
    }

}
