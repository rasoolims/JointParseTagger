import JointPosTaggerParser.Info;
import JointPosTaggerParser.MultiSentenceParser;
import JointPosTaggerParser.ParseResult;
import  java.io.File;

/**
 * Created by Mohammad Sadegh Rasooli.
 * ML-NLP Lab, Department of Computer Science, Columbia University
 * Date Created: 2/12/15
 * Time: 12:25 AM
 * To report any bugs or problems contact rasooli@cs.columbia.edu
 */

public class Main {
    public static void main(String[] args) throws Exception {
        if (args.length < 2) {
            System.out.println("java -jar JointParseTagger.jar [tag_model_file] [parse_model_file] ");
            System.exit(0);
        }

        String tagModelPath =(new File(args[0])).getAbsolutePath();
        String parseModelPath =(new File(args[1])).getAbsolutePath();
     //   String inputFile = args[2];
      //  String outputFile = args[3];

        int numberOfThreads=8;

        // for Yin-Wen 
        //
        MultiSentenceParser multiSentenceParser=new MultiSentenceParser(numberOfThreads,tagModelPath,parseModelPath);
        
        String[] sentences=new String[500];
        for(int i=0;i<sentences.length;i++){
            sentences[i]= "he is not here he is not here he is not here he is not here he is not here he is not here he is not here he is not here he is not here he is not here he is not here he is not here he is not here he is not here he is not here he is not here he is not here he is not here .";
        }
        
      ParseResult[] results=  multiSentenceParser.parseMergedWordSentences(sentences);
      
        for(int i=0;i<results.length;i++)
            System.out.println(results[i].getConllOutput());
        
        /*
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
            ParseResult parseResult=new ParseResult(words,info,numberOfThreads);
            
            // you should backward index by one (i.e. 0 is root, but index 0 is for word 1) head[0] is head of the first word
            // e.g. head[10] = 13 means head of 11th word is 13
            int[] heads=parseResult.getHeads();
            String[] depLabels=parseResult.getDepLabels();

            writer.write(parseResult.getConllOutput());
        }
        
        System.out.println(count + "...");
        writer.flush();
        writer.close();
         */

        // for Yin-Wen
        multiSentenceParser.shutDownLiveThreads();
    }
}
