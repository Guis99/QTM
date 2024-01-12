#include "..\include\QTM.hpp"

using namespace QTM;

// Don't wrap 'this' pointer inside of smart pointer in a class method if SP gets ref-counted after function call ends 

int main() {
    std::unordered_map<bool, std::string> score;
    score[true] = "pass"; score[false] = "fail";

    std::shared_ptr<Cell> initptr(nullptr);

    QuadTreeMesh mesh(3, 3, 2, 3, 2);
    
    

    return 0;
}