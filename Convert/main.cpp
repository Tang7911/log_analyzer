#include<iostream>
#include<string>
#include<fstream>

void data_getline(std::string fileName)
{
    std::ifstream file(fileName);
    if(file.is_open())
    {
        size_t loc_head = 0;
        size_t loc_hind = 0;
        std::string sub;
        int count = 0;
        std::string line;
        while(getline(file, line))
        {   
            count = 0;
            loc_head = 0;
            loc_hind = 0;
            loc_hind = line.find("|", loc_hind);    
            if(loc_hind != std::string::npos)
            {
                sub = line.substr(0, loc_hind-1);
                std::cout << sub;
            }
            loc_hind++;     
            while(count < 4)
            {
                std::cout << " ";
                loc_head = loc_hind;          
                loc_hind = line.find("|", loc_hind); 
                sub = line.substr(loc_head+1, loc_hind-2-loc_head);
                std::cout << sub;
                loc_hind++;
                count++;
            }
            std::cout<<" ";
            sub = line.substr(++loc_hind); 
            std::cout<< sub <<std::endl;
        }
        file.close();
    }
    else
    {
        std::cout << "无法打开文件" << std::endl;
    }
}

int main()
{
    std::string fileName = "../Data/app.log";
    data_getline(fileName);

    return 0;
}