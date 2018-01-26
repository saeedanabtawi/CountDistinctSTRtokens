//for strings and streams :
#include <sys/types.h>
#include <dirent.h>
#include <bits/stdc++.h>
#include "stdlib.h"
#include "stdio.h"
#include <algorithm>
//for tokeinzer :
#include "Tokenizer.h"
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
// for time :
#include <chrono>
#include <atomic>
// for set:
#include <unordered_set>
// for hyperloglog:
#include "hyperloglog.hpp"
// for openMP:
#include <omp.h>
// for mmap:
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <boost/algorithm/string/classification.hpp> // Include boost::for is_any_of
#include <boost/algorithm/string/split.hpp> // Include for boost::split

#include <tbb/concurrent_unordered_set.h>

using namespace tbb; 
using namespace std;
using namespace hll;

vector<string> read_directory(const string& name);
void handle_error(const char* msg);
const char* map_file(const char *fname, size_t& length);

// Base class
class Analyzer
{
    public:


        ~Analyzer()
        {

        }
        void print()
        {
            cout << "The Analyzer print function was called" << endl;
        }

        void count_utoken()
        {
             cout << "The Analyzer count_utoken function was called" << endl;
        }
        void Analyze(const string driectory_name)
        {
             cout << "The Analyzer Analyze function was called" << endl;
        }
};
 
// Derived class
class SetAnalyzer : public Analyzer
{
    public:
         
        // definition of a member function already present in Analyzer
        void print()
        {
            cout << "The SetAnalyzer print function was called" << endl;
        }

        // definition of a member function already present in Analyzer
        void count_utoken(const string &str,
                          concurrent_unordered_set<string> &words_set)
        {
            Tokenizer tokenizer;

            tokenizer.set(str);

            string token;

            while((token = tokenizer.next()) != "")
            {
                words_set.insert(token);        
            }
            
            return;  
        }

        // definition of a member function already present in Analyzer
        void Analyze(const string driectory_name)
        {
            
            concurrent_unordered_set<string> words_set;
            vector<string>  files_name  = read_directory(driectory_name);
            vector<string>::const_iterator file_name;
        
            for( file_name = files_name.begin(); file_name < files_name.end(); ++file_name) 
            {
                if(*file_name == "." || *file_name == "..")
                  continue;

                size_t length;
            
                vector<string> lines;
    			vector<string>::const_iterator itr_lines;
    			string s(map_file((driectory_name+'/'+*file_name).c_str(),length));
    			boost::split(lines, s, boost::is_any_of("\n"), boost::token_compress_on);
                
    			#pragma omp parallel default(none) shared(words_set,lines)  
    			{
    				#pragma omp for schedule(static,1) nowait
    				for( itr_lines = lines.begin(); itr_lines < lines.end(); ++itr_lines) 
    				{
    					count_utoken(*itr_lines,words_set);
                	}
                }
           	}      
            printf("\n %d \n",words_set.size()); 
            return;
        }     
};

// Derived class
class HyperloglogAnalyzer : public Analyzer
{
    public:
         
        // definition of a member function already present in Analyzer
        void print()
        {
            cout << "The HyperloglogAnalyzer print function was called" << endl;
        }

        // definition of a member function already present in Analyzer
        void count_utoken(const string &str, 
                          HyperLogLog &hll)
        {             
            Tokenizer tokenizer;
            tokenizer.set(str);
            string token;
            while((token = tokenizer.next()) != "")
            {
                #pragma omp critical
                {
                     hll.add(token.c_str(), token.size());;
                }            
            }  
        }

        // definition of a member function already present in Analyzer
        void Analyze(const string driectory_name)
        {
            HyperLogLog hll(8);
            vector<string>  files_name  = read_directory(driectory_name);
            vector<string>::const_iterator file_name;

            for( file_name = files_name.begin(); file_name < files_name.end(); ++file_name) 
            {
                if(*file_name == "." || *file_name == "..")
                  continue;

                size_t length;

                /* Cache file into memory */
                const char* File_content = map_file((driectory_name+'/'+*file_name).c_str(),
                                                    length);
                vector<string> lines;
                vector<string>::const_iterator itr_lines;
                string s(File_content);
                boost::split(lines, s, boost::is_any_of("\n"), boost::token_compress_on);

                #pragma omp parallel default(none) shared(lines,hll,length)  
                {
                    #pragma omp for schedule(static,1) nowait
                    for( itr_lines = lines.begin(); itr_lines < lines.end(); ++itr_lines) 
                    {
                        count_utoken(*itr_lines,hll);
                    }
                }
            }   
            printf("Cardinality:%lf\n",hll.estimate());  
        }     
};

//main function
int main() 
{
    const string driectory_name = "./Documents";

    auto t1 = std::chrono::high_resolution_clock::now();

    //object of SetAnalyzer class
    /*
    SetAnalyzer setanalyzer = SetAnalyzer();
    setanalyzer.Analyze(driectory_name);
    */

	//object of  HyperloglogAnalyzer class 
    HyperloglogAnalyzer hyperlogloganalyzer = HyperloglogAnalyzer();
    hyperlogloganalyzer.Analyze(driectory_name);
    
    
    auto t2 = std::chrono::high_resolution_clock::now();
    std::cout << "test function took "
              << std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count()
              << " milliseconds\n";
    return 0;
} 
/*
    input  : Directory name 
    output : string vector cntain the name of each file in the driectory
*/
vector<string> read_directory(const string& name)
{
    vector<string> v;

    DIR* dirp = opendir(name.c_str());
    struct dirent * dp;
    while ((dp = readdir(dirp)) != NULL) {
        v.push_back(dp->d_name);
    }
    closedir(dirp);

    return v;
}

void handle_error(const char* msg) {
    perror(msg); 
    exit(255);
}

const char* map_file(const char* fname, size_t& length)
{
    int fd = open(fname, O_RDONLY);
    if (fd == -1)
        handle_error("open");

    // obtain file size
    struct stat sb;
    if (fstat(fd, &sb) == -1)
        handle_error("fstat");

    length = sb.st_size;

    const char* addr = static_cast<const char*>(mmap(NULL, length, PROT_READ, MAP_PRIVATE, fd, 0u));
    if (addr == MAP_FAILED)
        handle_error("mmap");
}
