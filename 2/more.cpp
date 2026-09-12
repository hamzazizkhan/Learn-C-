#include <iostream>

int main(){
    int x = 9;
    int y=10;
    int final = x+y;
    std::cout << final;
    
    std::cout << "moving on to something else \n";

    char tmp[12] = "i am ";
    char const ham[6]= "hamza";

    for(int i=0; i<6; i++){
        tmp[strlen(tmp)] += ham[i];
    }
    std::cout << tmp;

    return 0;
}