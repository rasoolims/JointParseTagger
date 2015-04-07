// Author: Yin-Wen Chang

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <utility>
#include <map>
#include <queue>
#include <set>
#include <vector>
#include <sys/time.h>
#include "parse.h"

using namespace std;
	
Parse::Parse() {
	env = create_vm(&jvm);
	if (env == NULL) {
		cout << "Error: Unable to create JAVA virtual machine in Parse()." << endl;
		exit(1);
	}

	// Obtain classes.
	clsH = env->FindClass("TagAndParse");

	// Obtain the method descriptors.
	if (clsH != NULL) {
		midLoadModels = env->GetStaticMethodID(clsH, "LoadModels", "(Ljava/lang/String;Ljava/lang/String;)V");
		midTagAndParse = env->GetStaticMethodID(clsH, "TagAndParse", "([Ljava/lang/String;)LJointPosTaggerParser/ParseResult;");
		midTagAndParseScore = env->GetStaticMethodID(clsH, "TagAndParseScore", "([Ljava/lang/String;)F");
		midTagAndParseMultiSentence = env->GetStaticMethodID(clsH, "TagAndParseMultiSentence", "([[Ljava/lang/String;)[LJointPosTaggerParser/ParseResult;");
		midTagAndParseScoreMultiSentence = env->GetStaticMethodID(clsH, "TagAndParseScoreMultiSentence", "([[Ljava/lang/String;)[F");
		midTagAndParseScoreMultiMergedSentence = env->GetStaticMethodID(clsH, "TagAndParseScoreMultiMergedSentence", "([Ljava/lang/String;)[F");
	} else {
		printf("\nUnable to find the requested class\n");    	
	}

	// Path to the tagger model.
	jstring taggerModelPath = env->NewStringUTF("/scratch/yinwen/models/pos.en.normalized.model.iter_13");
	
	// Path to the parser model.
	jstring parserModelPath = env->NewStringUTF("/scratch/yinwen/models/en_64beam_with_brown_cluster.universal.model_iter8");

	if (midLoadModels != NULL) {
		cout << "loading tagger model..." << endl;
		env->CallStaticVoidMethod(clsH, midLoadModels, taggerModelPath, parserModelPath);
	} else {
		cerr << "Unable to call LoadModels()" << endl;
	}
}

JNIEnv* Parse::create_vm(JavaVM ** jvm) {

	JNIEnv *env;
	JavaVMInitArgs vm_args;

	JavaVMOption options; 
	
	// Path to the java source code     
	options.optionString = "-Djava.class.path=/afs/csail.mit.edu/u/y/yinwen/sp/yara/JointParseTagger/jar/JointParseTagger.jar:/afs/csail.mit.edu/u/y/yinwen/sp/yara/JointParseTagger/src/:/afs/csail.mit.edu/u/y/yinwen/sp/yara/JointParseTagger/src/JointPosTaggerParser";
	vm_args.version = JNI_VERSION_1_8; // JDK version. This indicates version 1.8
	vm_args.nOptions = 1;
	vm_args.options = &options;
	vm_args.ignoreUnrecognized = 0;

	int ret = JNI_CreateJavaVM(jvm, (void**)&env, &vm_args);
	if(ret < 0)
		printf("\nUnable to Launch JVM\n");       
	return env;
}

double Parse::tagAndParse(vector<string> &words,
		                  vector<string> &tags,
						  vector<int> &dependencies) {
	// Call the methods.
	if(midTagAndParse != NULL) {
		jobjectArray wordArgs = env->NewObjectArray(words.size(), env->FindClass("java/lang/String"), NULL);
		int i = 0;
		for (string w : words) {
			jstring stringArg = env->NewStringUTF(w.c_str());
			env->SetObjectArrayElement(wordArgs, i, stringArg);
			i++;
		}	

		jobjRet = (jobject)env->CallStaticObjectMethod(clsH, midTagAndParse, wordArgs); //Calling the main method.

		// Get the class of object.
		if (jobjRet != NULL) {
			clsR = env->GetObjectClass(jobjRet);
			jfloat score = env->GetFloatField(jobjRet, env->GetFieldID(clsR, "parseScore", "F"));
			jfloat tscore = env->GetFloatField(jobjRet, env->GetFieldID(clsR, "taggingScore", "F"));
		

			jobject jobjHeads = env->GetObjectField(jobjRet, env->GetFieldID(clsR, "heads", "[I"));
			
			// Cast it to a jintarray
			jintArray * arr = reinterpret_cast<jintArray*>(&jobjHeads);
			
			// Get the elements (you probably have to fetch the length of the array as well  
			int * heads = env->GetIntArrayElements(*arr, NULL);
			int length= env->GetArrayLength(*arr);

			// Release it 
			env->ReleaseIntArrayElements(*arr, heads, 0);

			return score;
		} else {
			cerr << "jobjRet = NULL" << endl;
		}
	}
}
	
void Parse::tagAndParseMultiSentence(vector<vector<string> > &sentences,
			                         vector<float> &scores,
						    		 vector<vector<string> > &tags,
								     vector<vector<int> > &dependencies) {
	// Call the method.
	if(midTagAndParseMultiSentence != NULL) {
		jobjectArray sentArgs = env->NewObjectArray(sentences.size(), env->FindClass("[Ljava/lang/Object;"), NULL);
		for (int j = 0; j < sentences.size(); j++) {
			jobjectArray wordArgs = env->NewObjectArray(sentences[j].size(), env->FindClass("java/lang/String"), NULL);
			int i = 0;
			for (string w : sentences[j]) {
				jstring stringArg = env->NewStringUTF(w.c_str());
				env->SetObjectArrayElement(wordArgs, i, stringArg);
				i++;
			}	
			env->SetObjectArrayElement(sentArgs, j, wordArgs);
		}

		jobjRet = (jobject)env->CallStaticObjectMethod(clsH, midTagAndParseMultiSentence, sentArgs); // Calling the main method.
	
		convertParseResultsArray(scores, tags, dependencies);
	} else {
		cerr << "midTagAndParseMultiSentence NULL" << endl;
	}

}

void Parse::tagAndParseScoreMultiSentence(vector<vector<string> > &sentences, vector<float> &scores) {
	// Call the methods.
	if(midTagAndParseScoreMultiSentence != NULL) {
		jobjectArray sentArgs = env->NewObjectArray(sentences.size(), env->FindClass("[Ljava/lang/Object;"), NULL);
		for (int j = 0; j < sentences.size(); j++) {
			jobjectArray wordArgs = env->NewObjectArray(sentences[j].size(), env->FindClass("java/lang/String"), NULL);
			int i = 0;
			for (string w : sentences[j]) {
				jstring stringArg = env->NewStringUTF(w.c_str());
				env->SetObjectArrayElement(wordArgs, i, stringArg);
				i++;
			}	
			env->SetObjectArrayElement(sentArgs, j, wordArgs);
		}

		jobjRet = (jobject)env->CallStaticObjectMethod(clsH, midTagAndParseScoreMultiSentence, sentArgs); // Calling the main method.
		
		if (jobjRet != NULL) {
			// Cast it to a jintarray
			jfloatArray * arr = reinterpret_cast<jfloatArray*>(&jobjRet);
			
			// Get the elements (you probably have to fetch the length of the array as well  
			float * heads = env->GetFloatArrayElements(*arr, NULL);
			int length= env->GetArrayLength(*arr);
			scores.resize(length);
			for (int i = 0; i < length; i++) {
				scores[i] = heads[i];
			}

			// Release it 
			env->ReleaseFloatArrayElements(*arr, heads, 0);
		} else {
			cerr << "jobjRet = NULL" << endl;
		}
	}
}

void Parse::tagAndParseScoreMultiMergedSentence(vector<string> &sentences, vector<float> &scores) {
	// Call the methods.
	if(midTagAndParseScoreMultiMergedSentence != NULL) {
   		clock_t clockStartTime = clock();	
		jobjectArray sentArgs = env->NewObjectArray(sentences.size(), env->FindClass("java/lang/String"), NULL);
		int i = 0;
		for (string w : sentences) {
			jstring stringArg = env->NewStringUTF(w.c_str());
			env->SetObjectArrayElement(sentArgs, i, stringArg);
			i++;
		}	

		jobjRet = (jobject)env->CallStaticObjectMethod(clsH, midTagAndParseScoreMultiMergedSentence, sentArgs); //Calling the main method.
			
		if (jobjRet != NULL) {
			// Cast it to a jintarray
			jfloatArray * arr = reinterpret_cast<jfloatArray*>(&jobjRet);
			
			// Get the elements (you probably have to fetch the length of the array as well  
			float * heads = env->GetFloatArrayElements(*arr, NULL);
			int length= env->GetArrayLength(*arr);
			scores.resize(length);
			for (int i = 0; i < length; i++) {
				scores[i] = heads[i];
			}

			// Release it 
			env->ReleaseFloatArrayElements(*arr, heads, 0);
		} else {
			cerr << "jobjRet = NULL" << endl;
		}
	} else {
		cerr << "mid == NULL" << endl;
	}
}

double Parse::tagAndParseScore(vector<string> words) {
	// Call the methods.
	if(midTagAndParseScore != NULL) {
		jobjectArray wordArgs = env->NewObjectArray(words.size(), env->FindClass("java/lang/String"), NULL);
		int i = 0;
		for (string w : words) {
			jstring stringArg = env->NewStringUTF(w.c_str());
			env->SetObjectArrayElement(wordArgs, i, stringArg);
			i++;
		}	

		return env->CallStaticFloatMethod(clsH, midTagAndParseScore, wordArgs); //Calling the main method.
	}
}

float Parse::convertParseResults(jobject &jobj,
		                         vector<string> &tags,
								 vector<int> &dependencies) {
	clsR = env->GetObjectClass(jobj);
	jfloat score = env->GetFloatField(jobj, env->GetFieldID(clsR, "parseScore", "F"));
	jfloat tscore = env->GetFloatField(jobj, env->GetFieldID(clsR, "taggingScore", "F"));

	// Get the tags.
	jobject jobjTags = env->GetObjectField(jobj, env->GetFieldID(clsR, "tags", "[Ljava/lang/String;"));

	// Cast it to a jintarray
	jobjectArray * objarr = reinterpret_cast<jobjectArray*>(&jobjTags);
	int objlength= env->GetArrayLength(*objarr);
	for (int j = 0; j < objlength; j++) {
		jstring jstr = (jstring)env->GetObjectArrayElement(*objarr, j);
		if (env->ExceptionOccurred()) {
			cerr << "Exception after calling GetObjectArrayElement" << endl;
		}
		const char* cstr = env->GetStringUTFChars(jstr, 0);

		tags.emplace_back(cstr);

		// Release it.
		env->DeleteLocalRef(jstr);
	}


	// Get the heads.
	jobject jobjHeads = env->GetObjectField(jobj, env->GetFieldID(clsR, "heads", "[I"));

	// Cast it to a jintarray
	jintArray * intarr = reinterpret_cast<jintArray*>(&jobjHeads);

	// Get the elements (you probably have to fetch the length of the array as well  
	int * heads = env->GetIntArrayElements(*intarr, NULL);
	int intlength = env->GetArrayLength(*intarr);
	for (int j = 0; j < intlength; j++) {
		dependencies.push_back(heads[j]);
	}

	// Release it 
	env->ReleaseIntArrayElements(*intarr, heads, 0);

	return score;
}

void Parse::convertParseResultsArray(vector<float> &scores,
		vector<vector<string> > &tags,
		vector<vector<int> > &dependencies) {
	if (jobjRet != NULL) {
		// Cast it to a jobjectArray.
		jobjectArray * arr = reinterpret_cast<jobjectArray*>(&jobjRet);

		// Get the elements (you probably have to fetch the length of the array as well  
		int length= env->GetArrayLength(*arr);
		scores.resize(length);
		tags.resize(length);
		dependencies.resize(length);
		for (int i = 0; i < length; i++) {
			jobject jobj = (jobject)env->GetObjectArrayElement(*arr, i);

			if (env->ExceptionOccurred()) {
				cerr << "Exception after calling GetObjectArrayElement" << endl;
			}

			float score = convertParseResults(jobj, tags[i], dependencies[i]);
			scores[i] = score;

			env->DeleteLocalRef(jobj);

		}

		// Release it 
		//env->ReleaseFloatArrayElements(*arr, heads, 0);
	} else {
		cerr << "jobjRet = NULL" << endl;
	}
}

