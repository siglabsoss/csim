#pragma once



#include <iostream>
#include <boost/tokenizer.hpp> //For parsing data from file
#include <string>


//
//class gf2
//{
//public:
//    virtual ~gf2();
//    gf2();


typedef boost::tokenizer<boost::escaped_list_separator<char> > Tokenizer;

class CSVMatrix
{

public:
    CSVMatrix(){};
    ~CSVMatrix(){};

    std::vector<char> loadCSVFile(string filename) const
    {
        ifstream ifs(filename, ios::binary|ios::ate);
        ifstream::pos_type bytecount = ifs.tellg();

        std::vector<char> result(bytecount);
        ifs.seekg(0, ios::beg);
        ifs.read(&result[0], bytecount);
        return result;
    }

    void parseCSV(uint32_t& rows, uint32_t& cols, const std::vector<char>& input, vector<vector<uint8_t> >& out) const
    {
        std::vector<char>::const_iterator eol = input.begin();  // end of previous line


        size_t count = 0;
        size_t linecount = 0;
        for(auto it = input.begin(); it != input.end(); ++it)
        {
            if(*it == '\n')
            {
                string line;
                line.assign(eol, it);
//                cout << "line was " << line << endl;
                std::vector<std::string> vec;

                Tokenizer tok(line);
                vec.assign(tok.begin(), tok.end());

                if(vec.size() == 1)
                {
                    continue;
                }

//                cout << "vec size " << vec.size() << endl;
//                cout << "vec contents:" << endl;

                out.push_back(vector<uint8_t>());

                for(auto it2 = vec.begin(); it2 != vec.end(); ++it2)
                {
//                    cout << "itr size " << it2->size() << endl;
                    if( it2->size() > 0 )
                    {
                        out[linecount].push_back(atoi(it2->c_str()));
                    }
//                    cout << atoi(it2->c_str());
//                    cout << *it2;
                }
//                cout << endl;

                if(linecount == 0)
                {
                    cols = (uint32_t) out[linecount].size();
                }
                else
                {
                    assert((uint32_t) out[linecount].size() == cols);
                }

                linecount++;
                eol = it;
            }
//            cout << *it;
            count ++;
        }

        rows = out.size();
    }

//    template<int R, int C> void parseString(uint8_t H[R][C], const  std::vector<char>& input) const
//    {
////        std::cout << input << std::endl;
//    }

};
