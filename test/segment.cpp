#include <iostream>
#include <fstream>
#include "../src/MPSegment.hpp"
#include "../src/HMMSegment.hpp"
#include "../src/MixSegment.hpp"

using namespace CppJieba;

void cut(const ISegment * seg, const char * const filePath)
{
    ifstream ifile(filePath);
    vector<string> res;
    string line;
    while(getline(ifile, line))
    {
        if(!line.empty())
        {
            res.clear();
            seg->cut(line, res);
            cout<<join(res.begin(), res.end(),"/")<<endl;
        }
    }
}

const char * const TEST_FILE = "../test/testdata/testlines.utf8";
const char * const JIEBA_DICT_FILE = "../dict/jieba.dict.utf8";
const char * const HMM_DICT_FILE = "../dict/hmm_model.utf8";

int main(int argc, char ** argv)
{
    //demo
    //{
    //    HMMSegment seg(HMM_DICT_FILE);
    //    if(!seg)
    //    {
    //        cout<<"seg init failed."<<endl;
    //        return EXIT_FAILURE;
    //    }
    //    cut(&seg, TEST_FILE);
    //}
    //{
    //    MixSegment seg(JIEBA_DICT_FILE, HMM_DICT_FILE);
    //    if(!seg)
    //    {
    //        cout<<"seg init failed."<<endl;
    //        return EXIT_FAILURE;
    //    }
    //    cut(&seg, TEST_FILE);
    //}
    {
        MPSegment seg(JIEBA_DICT_FILE);
        if(!seg)
        {
            cout<<"seg init failed."<<endl;
            return false;
        }
        cut(&seg, TEST_FILE);
    }
    return EXIT_SUCCESS;
}
