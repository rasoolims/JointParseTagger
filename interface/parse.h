// Author: Yin-Wen Chang
//
// The Parse class is an interface to joint tagging and parsing.
// The taggerModelPath and parserModelPath need to be updated to point to the
// location of the models. The JVM option "options.optionString" also needs to
// be updated so that it points to the JointParserTagger.jar and the direction
// ../src that contains TagAndParse.java. vm_args.version has to be set to the
// correct JDK version.

#ifndef parse_h
#define parse_h

#include "jni.h"

using namespace std;

// This class is the C++ interface for the Java class "TagAndParse" in
// "../src/TagAndParse.java".
class Parse {
  public:
	// Constructor. It creates the jvm (Java virtual machine) and gets the
	// handle of the Java classes and methods. It called the Java methods that
	// loads the tagger model and the parser model.
	Parse();

	// Destructor.
	~Parse() {
		// Call the functon TurnOff().
		jmethodID midTurnOff = env->GetStaticMethodID(clsH, "TurnOff", "()V");
		if (midTurnOff != NULL) {
			env->CallStaticVoidMethod(clsH, midTurnOff);
		} else {
			cerr << "Unable to call TurnOff()" << endl;
		}

		// Destroy Java VM.
		jvm->DestroyJavaVM();
	}

	// Tags and parses the sentence and returns the score, the dependency tree
	// will be stored at "tags" and "dependencies".
	double tagAndParse(vector<string> &words,
			           vector<string> &tags,
			           vector<int> &dependencies);
	
	// Tags and parses multiple sentences at a time and returns the score, the
	// dependency tree will be stored at "tags" and "dependencies".
	void tagAndParseMultiSentence(vector<vector<string> > &sentences,
			                      vector<float> &scores,
								  vector<vector<string> > &tags,
								  vector<vector<int> > &dependencies);

	// Tags and parses multiple sentences at a time and stores the score in scores.
	void tagAndParseScoreMultiSentence(vector<vector<string> > &sentences,
			                           vector<float> &scores);
	
	// Tags and parses multiple sentences at a time and stores the score in scores.
	// Each sentence is a string of space seperated words.
	void tagAndParseScoreMultiMergedSentence(vector<string> &sentences,
			                                 vector<float> &scores);

	// Tags and parses the sentence and returns the score.
	double tagAndParseScore(vector<string> words);

  private:
    JNIEnv *env;
	JavaVM *jvm;
	
	// For the Java class TagAndParse.
	jclass clsH = NULL;

	// The return Java class ParseResult.
	jclass clsR = NULL;

	// Method ID.
	jmethodID midLoadModels = NULL;
	jmethodID midTagAndParse = NULL;
	jmethodID midTagAndParseScore = NULL;
	jmethodID midTagAndParseMultiSentence = NULL;
	jmethodID midTagAndParseScoreMultiSentence = NULL;
	jmethodID midTagAndParseScoreMultiMergedSentence = NULL;

	// jobject for hoding the returned object of a JAVA class.
	jobject jobjRet = NULL;

	// Creates the JVM.
	JNIEnv* create_vm(JavaVM ** jvm);

	// Functions.
	
	// Converts a jobject "jobj" that is a Java object ParaseResults to C
	// vectors "tags" and "dependencies". Returns the score.
	float convertParseResults(jobject &jobj,
		                      vector<string> &tags,
						      vector<int> &dependencies);

	// Converts a jobject that is an array of the Java class ParseResults to C
	// vectors "scores", "tags", and "dependencies". The jobject is stored in the
	// global variable jobjRet.
	void convertParseResultsArray(vector<float> &scores,
			                      vector<vector<string> > &tags,
								  vector<vector<int> > &dependencies);
};

#endif  // #define parse_h
