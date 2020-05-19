#ifndef VACCS_SOURCE_LOCATION_H
#define VACCS_SOURCE_LOCATION_H

#include <string>

using namespace std;

class vaccs_source_location {
    private:
        string file_name;
        int line_num;

    public:
        vaccs_source_location();
        virtual ~vaccs_source_location();

        void set_file_name(string& name) {
            file_name = name;
        }

        void set_line_num(int num) {
            line_num = num;
        }

        string get_file_name() {
            return file_name;
        }

        int get_line_num() {
            return line_num;
        }
};

#endif