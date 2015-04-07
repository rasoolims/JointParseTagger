// Author: Yin-Wen Chang
//
// This main function shows how to call the C interface functions in parse.h.

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

int main(int argc, char* argv[]) {
   Parse* parse = new Parse();
   
   vector<float> scores;

   vector<string> sentences;
   for (int i = 0; i < 10; i++) {
   		sentences.push_back("with any of the propaganda here to achieve nothing !");
   		sentences.push_back("with any of the propaganda here to achieve nothing");
   }

   vector<vector<string>> sents;
   for (int i = 0; i < 10; i++) {
   		sents.push_back(vector<string>{"with","any","of", "the", "propaganda", "here", "to", "achieve", "nothing", "!"});
   		sents.push_back(vector<string>{"with","any","of", "the", "propaganda", "here", "to", "achieve", "nothing"});
   }

   for (int j = 0; j < 2; j++) {
	   struct timeval t1, t2;
       gettimeofday(&t1, NULL);

	   parse->tagAndParseScoreMultiMergedSentence(sentences, scores);
	   gettimeofday(&t2, NULL);
	   int milliSeconds = (t2.tv_sec - t1.tv_sec) * 1000 + (t2.tv_usec - t1.tv_usec)/1000;
	   cout << "1 total time " << milliSeconds << endl;
       for (float s : scores) {
	       cout << s << endl;
	   }
	   parse->tagAndParseScoreMultiSentence(sents, scores);
	   gettimeofday(&t2, NULL);
	   milliSeconds = (t2.tv_sec - t1.tv_sec) * 1000 + (t2.tv_usec - t1.tv_usec)/1000;
	   cout << "2 total time " << milliSeconds << endl;
       for (float s : scores) {
	       cout << s << endl;
	   }
	   vector<vector<string>> tags;
	   vector<vector<int>> dependencies;
	   parse->tagAndParseMultiSentence(sents, scores, tags, dependencies);

	   for (string tag : tags[0]) {
	       cout << tag << " ";
	   }
	   cout << endl;
	   for (int dep : dependencies[0]) {
	       cout << dep << " ";
	   }
	   cout << endl;

	   gettimeofday(&t2, NULL);
	   milliSeconds = (t2.tv_sec - t1.tv_sec) * 1000 + (t2.tv_usec - t1.tv_usec)/1000;
	   cout << "total time " << milliSeconds << endl;
   }
   
   return 0;
}

