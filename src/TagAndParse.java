import JointPosTaggerParser.Info;
import JointPosTaggerParser.MultiSentenceParser;
import JointPosTaggerParser.ParseResult;
import SemiSupervisedPOSTagger.Structures.Pair;
import YaraParser.Learning.AveragedPerceptron;
import SemiSupervisedPOSTagger.Tagging.Tagger;
import YaraParser.Structures.IndexMaps;
import YaraParser.Structures.InfStruct;
import YaraParser.Structures.Sentence;
import YaraParser.TransitionBasedSystem.Configuration.BeamElement;
import YaraParser.TransitionBasedSystem.Configuration.Configuration;
import YaraParser.TransitionBasedSystem.Parser.KBeamArcEagerParser;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileReader;
import java.io.FileWriter;
import java.util.ArrayList;
import java.util.concurrent.CompletionService;
import java.util.concurrent.ExecutorCompletionService;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class TagAndParse {
	// Models.
	public static Info info;
	public static MultiSentenceParser multiSentenceParser;

	// Number of threads.
	static int numOfThreads = 10;

	public static void LoadModels(String taggerModelPath, String parserModelPath) throws Exception {
       info=new Info(taggerModelPath, parserModelPath, 1);
       multiSentenceParser = new MultiSentenceParser(numOfThreads, info);
	   System.out.println("Done loading both models.");
	}

	public static float[] TagAndParseScoreMultiSentence(String[][] sentences) throws Exception {
      	ParseResult[] results = multiSentenceParser.parseSentences(sentences);
		float[] scores = new float[results.length];
		for (int i = 0; i < results.length; i++) {
			scores[i] = results[i].getParseScore();
		}
		return scores;
	}

	public static float[] TagAndParseScoreMultiMergedSentence(String[] sentences) throws Exception {
        ParseResult[] results = multiSentenceParser.parseMergedWordSentences(sentences);
		float[] scores = new float[results.length];
		for (int i = 0; i < results.length; i++) {
			scores[i] = results[i].getParseScore();
		}
		
		return scores;
	}

	public static ParseResult[] TagAndParseMultiSentence(String[][] sentences) throws Exception {
		ParseResult[] results = multiSentenceParser.parseSentences(sentences);

		return results;
	}

	public static ParseResult TagAndParse(String[] words) throws Exception {
        ParseResult parseResult = new ParseResult(words,info, numOfThreads);
		
		return parseResult;
	}

	public static float TagAndParseScore(String[] words) throws Exception {
    	ParseResult parseResult = TagAndParse(words);
		
		// Returns the score normalized by the length.
		return parseResult.getParseScore();
	}

	public static void TurnOff() {
        //info.turnOff();
        multiSentenceParser.shutDownLiveThreads();
	}

    public static void main(String[] args) throws Exception {
        if (args.length < 4) {
            System.out.println("java -jar JointParseTagger.jar [tag_model_file] [parse_model_file] [input_raw_file] [output_parse_file]");
            System.exit(0);
        }

        String tagModelPath = args[0];
        String parseModelPath = args[1];
        String inputFile = args[2];
        String outputFile = args[3];
        
        //int numOfThreads=8;

        // for Yin-Wen
        //Info info=new Info(tagModelPath,parseModelPath,numberOfThreads);
		LoadModels(tagModelPath, parseModelPath);
        
		BufferedReader reader = new BufferedReader(new FileReader(inputFile));
        BufferedWriter writer = new BufferedWriter(new FileWriter(outputFile));


        String line;
        int count = 0;
        while ((line = reader.readLine()) != null) {
            if (line.trim().length() == 0)
                continue;
            count++;
            if (count % 100 == 0)
                System.out.print(count + "...");
            // for Yin-Wen
            String[] words = line.trim().split(" ");

            // for Yin-Wen
            //ParseResult parseResult=new ParseResult(words,info);
            ParseResult parseResult = TagAndParse(words);

            // you should backward index by one (i.e. 0 is root, but index 0 is for word 1) head[0] is head of the first word
            // e.g. head[10] = 13 means head of 11th word is 13
            int[] heads=parseResult.getHeads();
            String[] depLabels=parseResult.getDepLabels();

            writer.write(parseResult.getConllOutput());
			System.out.println(parseResult.getParseScore());
			System.out.println(TagAndParseScore(words));
        }
        
        System.out.println(count + "...");
        writer.flush();
        writer.close();

        // for Yin-Wen
        info.turnOff();
    }
}
