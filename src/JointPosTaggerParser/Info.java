package JointPosTaggerParser;

import SemiSupervisedPOSTagger.Tagging.Tagger;
import YaraParser.Learning.AveragedPerceptron;
import YaraParser.Structures.IndexMaps;
import YaraParser.Structures.InfStruct;
import YaraParser.TransitionBasedSystem.Parser.KBeamArcEagerParser;

import java.util.ArrayList;

/**
 * Created by Mohammad Sadegh Rasooli.
 * ML-NLP Lab, Department of Computer Science, Columbia University
 * Date Created: 3/2/15
 * Time: 4:07 PM
 * To report any bugs or problems contact rasooli@cs.columbia.edu
 */

public class Info {
   public final KBeamArcEagerParser parser;
    public   final Tagger tagger;
    public  final SemiSupervisedPOSTagger.Structures.IndexMaps posMaps;
    public  final InfStruct infStruct;
    public final IndexMaps parserMaps;

    public Info(KBeamArcEagerParser parser, Tagger tagger, InfStruct infStruct, IndexMaps parserMaps) {
        this.parser = parser;
        this.tagger = tagger;
        this.posMaps = tagger.getMaps();
        this.infStruct = infStruct;
        this.parserMaps = parserMaps;
    }
    
    public Info(String tagModelPath, String parseModelPath, int numberOfThreads) throws Exception {
        tagger = new Tagger(tagModelPath);
        posMaps=tagger.getMaps();
        infStruct = new InfStruct(parseModelPath);

        ArrayList<Integer> dependencyLabels = infStruct.dependencyLabels;
        parserMaps = infStruct.maps;
        AveragedPerceptron averagedPerceptron = new AveragedPerceptron(infStruct);

        int featureSize = averagedPerceptron.featureSize();
        parser = new KBeamArcEagerParser(averagedPerceptron, dependencyLabels, featureSize, parserMaps, numberOfThreads);
    }
    
    public void turnOff(){
        parser.shutDownLiveThreads();
    }
}
