#pragma once



#include <iostream>
#include <boost/tokenizer.hpp> //For parsing data from file
#include <string>
#include <fstream>      // std::ifstream


//
//class gf2
//{
//public:
//    virtual ~gf2();
//    gf2();


typedef boost::tokenizer<boost::escaped_list_separator<char> > Tokenizer;

class CSVBitMatrix
{

public:
    CSVBitMatrix(){};
    ~CSVBitMatrix(){};

    std::vector<char> loadCSVFile(std::string filename) const
    {
        std::ifstream ifs(filename, std::ios::binary| std::ios::ate);
        std::ifstream::pos_type bytecount = ifs.tellg();

        assert(ifs && "file does not exist");
        assert(bytecount > 0 && "file is empty");

        std::vector<char> result(bytecount);
        ifs.seekg(0, std::ios::beg);
        ifs.read(&result[0], bytecount);
        return result;
    }

    void parseCSV(const std::vector<char>& input, std::vector<std::vector<bool> >& out) const
    {
        std::vector<char>::const_iterator eol = input.begin();  // end of previous line


        size_t count = 0;
        size_t linecount = 0;
        size_t cols = 0;
        for(auto it = input.begin(); it != input.end(); ++it)
        {
            if(*it == '\n')
            {
                std::string line;
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

                out.push_back(std::vector<bool>());

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
                    //we expect the number of columns to stay consistent across rows
                    assert((uint32_t) out[linecount].size() == cols);
                }

                linecount++;
                eol = it;
            }
//            cout << *it;
            count ++;
        }
    }

//    template<int R, int C> void parseString(uint8_t H[R][C], const  std::vector<char>& input) const
//    {
////        std::cout << input << std::endl;
//    }

};
