#include <iostream>
#include <cstring>
#include <fstream>
#include <vector>

using namespace std;

int main(int argc, char* argv[]) {
    if (strcmp(argv[1], "copyfile") != 0)
    {
        cout << "Does not exist command: " << argv[1] << endl;
        return 0;
    }
    else if (argc != 5)
    {
        cout << "Not enough arguments\n";
        return 0;
    }
    else if (strcmp(argv[2], argv[3]) == 0)
    {
        cout << "The destination file name must not be as same as the source file name\n";
    }

    ifstream source(argv[2], ios::binary);
    ofstream destination(argv[3], ios::binary);

    const size_t buffer_size = atoi(argv[4]);
    vector<char> buffer(buffer_size);

    while (source.read(&buffer[0], buffer_size))
    {
        destination.write(&buffer[0], source.gcount());
    }
    
    return 0;
}