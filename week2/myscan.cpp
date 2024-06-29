// Bùi Anh Quốc 20215634
#include <iostream>
#include <dirent.h>
#include <fstream>
#include <string>

using namespace std;

void write_res(struct dirent** namelist, int n) {
    ofstream output("output.html");

    output << "<body>\n";
    for (int i = 0; i < n; i++)
    {
        if (namelist[i]->d_type == DT_REG)
        {
            output << "<i>" << namelist[i]->d_name << "</i><br>\n";
        } else if (namelist[i]->d_type == DT_DIR)
        {
            output << "<b>" << namelist[i]->d_name << "</b><br>\n";
        } else {
            output << namelist[i]->d_name << "<br>" << endl;
        }
        free(namelist[i]);
    }
    output << "</body>";
    free(namelist);
}

int main(int argc, char *argv[]) {
    string dir = "";

    if (argc < 2) {
        printf("Vui lòng cung cấp thư mục để quét.\n");
        return 1;
    } else {
        dir += argv[1];
        for (int i = 2; i < argc; i++)
        {
            dir += " ";
            dir += argv[i];
        }
    }

    struct dirent **namelist;
    int n;

    n = scandir(dir.c_str(), &namelist, 0, alphasort);
    if (n < 0) {
        cerr << "Không tìm thấy thư mục";
        cout << dir << endl;
    }
    else {
        write_res(namelist, n);
        namelist = NULL;
    }
    
    return 0;
}
