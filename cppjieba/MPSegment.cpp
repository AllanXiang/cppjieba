/************************************
 * file enc : AISCII
 * author   : wuyanyi09@gmail.com
************************************/
#include "MPSegment.h"

namespace CppJieba
{
    MPSegment::MPSegment()
    {
    }
    
    MPSegment::~MPSegment()
    {
        if(!dispose())
        {
            LogError("dispose failed.");
        }
    }

    bool MPSegment::init(const char* const filePath)
    {
        if(!_trie.init())
        {
            LogError("_trie.init failed.");
            return false;
        }
        LogInfo("_trie.loadDict(%s) start...", filePath);
        if(!_trie.loadDict(filePath))
        {
            LogError("_trie.loadDict faield.");
            return false;
        }
        LogInfo("_trie.loadDict end.");
        return true;
    }
    
    bool MPSegment::dispose()
    {
        return _trie.dispose();
    }

    bool MPSegment::cut(const string& str, vector<string>& res) const
    {
        vector<TrieNodeInfo> segWordInfos;
        if(!cut(str, segWordInfos))
        {
            return false;
        }
        res.clear();
        string tmp;
        for(uint i = 0; i < segWordInfos.size(); i++)
        {
            if(TransCode::encode(segWordInfos[i].word, tmp))
            {
                res.push_back(tmp);
            }
            else
            {
                LogError("encode failed.");
            }
        }
        return true;
    }

    bool MPSegment::cut(const string& str, vector<TrieNodeInfo>& segWordInfos)const
    {
        if(str.empty())
        {
            return false;
        }
        segWordInfos.clear();
        SegmentContext segContext;
        Unicode sentence;

        if(!TransCode::decode(str, sentence))
        {
            LogError("TransCode::decode failed.");
            return false;
        }

        for(uint i = 0; i < sentence.size(); i++)
        {
            segContext.push_back(SegmentChar(sentence[i]));
        }
        
        //calc DAG
        if(!_calcDAG(segContext))
        {
            LogError("_calcDAG failed.");
            return false;
        }

        if(!_calcDP(segContext))
        {
            LogError("_calcDP failed.");
            return false;
        }

        if(!_cut(segContext, segWordInfos))
        {
            LogError("_cut failed.");
            return false;
        }

        return true;
    }

    bool MPSegment::_calcDAG(SegmentContext& segContext)const
    {
        if(segContext.empty())
        {
            LogError("segContext empty.");
            return false;
        }

        Unicode unicode;
        for(uint i = 0; i < segContext.size(); i++)
        {
            unicode.clear();
            for(uint j = i ; j < segContext.size(); j++)
            {
                unicode.push_back(segContext[j].uniCh);
            }

			vector<pair<uint, const TrieNodeInfo*> > vp;
			if(_trie.find(unicode, vp))
			{
				for(uint j = 0; j < vp.size(); j++)
				{
					uint nextp = vp[j].first + i;
					segContext[i].dag[nextp] = vp[j].second; 
					//cout<<vp[j].first<<endl;
					//LogDebug(vp[j].second->toString());
				}
			}
            if(segContext[i].dag.end() == segContext[i].dag.find(i))
            {
                segContext[i].dag[i] = NULL;
            }
        }
        return true;
    }

    bool MPSegment::_calcDP(SegmentContext& segContext)const
    {
        if(segContext.empty())
        {
            LogError("segContext empty");
            return false;
        }
        
        for(int i = segContext.size() - 1; i >= 0; i--)
        {
            segContext[i].pInfo = NULL;
            segContext[i].weight = MIN_DOUBLE;
            for(DagType::const_iterator it = segContext[i].dag.begin(); it != segContext[i].dag.end(); it++)
            {
                uint nextPos = it->first;
                const TrieNodeInfo* p = it->second;
                double val = 0.0;
                if(nextPos + 1 < segContext.size())
                {
                    val += segContext[nextPos + 1].weight;
                }

                if(p)
                {
                    val += p->logFreq; 
                }
                else
                {
                    val += _trie.getMinLogFreq();
                }
                if(val > segContext[i].weight)
                {
                    segContext[i].pInfo = p;
                    segContext[i].weight = val;
                }
            }
        }
        return true;

    }

    bool MPSegment::_cut(SegmentContext& segContext, vector<TrieNodeInfo>& res)const
    {
        res.clear();

        uint i = 0;
        while(i < segContext.size())
        {
            const TrieNodeInfo* p = segContext[i].pInfo;
            if(p)
            {
                res.push_back(*p);
                i += p->word.size();
            }
            else//single chinese word
            {
                TrieNodeInfo nodeInfo;
                nodeInfo.word.push_back(segContext[i].uniCh);
                nodeInfo.freq = 0;
                nodeInfo.logFreq = _trie.getMinLogFreq();
                res.push_back(nodeInfo);
                i++;
            }
        }
        return true;
    }

}


#ifdef SEGMENT_UT
using namespace CppJieba;

int main()
{
    MPSegment segment;
    segment.init();
    if(!segment._loadSegDict("../dicts/segdict.gbk.v3.0"))
    {
        cerr<<"1"<<endl;
        return 1;
    }
    //segment.init("dicts/jieba.dict.utf8");
    //ifstream ifile("testtitle.gbk");
    ifstream ifile("badcase");
    vector<string> res;
    string line;
    while(getline(ifile, line))
    {
        res.clear();
        segment.cut(line, res);
        PRINT_VECTOR(res);
        getchar();
    }

    segment.dispose();
    return 0;
}

#endif