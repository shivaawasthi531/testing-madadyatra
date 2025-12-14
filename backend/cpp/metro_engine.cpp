#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>
#include <cctype>

using namespace std;

struct Station {
    string name;
    string normalized;
};

struct MetroInfo {
    string start;
    string end;
    string first_train;
    string last_train;
};

// ---------- STRING UTILITIES ----------
string trim(const string &s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end = s.find_last_not_of(" \t\r\n");
    return (start == string::npos) ? "" : s.substr(start, end - start + 1);
}

string collapseSpaces(const string &s) {
    string out;
    bool space = false;

    for (char c : s) {
        if (isspace((unsigned char)c)) {
            if (!space) { out += ' '; space = true; }
        } else { out += c; space = false; }
    }
    return trim(out);
}

string normalizeLocation(string s) {
    transform(s.begin(), s.end(), s.begin(),
        [](unsigned char c) { return tolower(c); });
    s = trim(s);
    s = collapseSpaces(s);
    return s;
}

// ---------- MAIN ----------
int main(int argc, char* argv[]) {

    if(argc < 5) {
        cout << "No metro found for this route!\n";
        return 0;
    }

    string from = normalizeLocation(argv[1]);
    string to   = normalizeLocation(argv[2]);
    string station_csv = argv[3];
    string line_csv    = argv[4];

    map<string, vector<Station>> metro_routes;
    map<string, MetroInfo> metro_info;

    // ---------- READ METRO STATIONS ----------
    ifstream station_file(station_csv);
    if(!station_file.is_open()){
        cout << "Error opening " << station_csv << "\n";
        return 1;
    }

    string line;
    getline(station_file, line); // skip header
    while(getline(station_file, line)){
        stringstream ss(line);
        string metro_line, order, station_name;
        getline(ss, metro_line, ',');
        getline(ss, order, ',');
        getline(ss, station_name, ',');
        metro_line = trim(metro_line);
        station_name = trim(station_name);
        if(metro_line.empty() || station_name.empty()) continue;

        Station st;
        st.name = station_name;
        st.normalized = normalizeLocation(station_name);
        metro_routes[metro_line].push_back(st);
    }
    station_file.close();

    // ---------- READ METRO LINE INFO ----------
    ifstream info_file(line_csv);
    if(!info_file.is_open()){
        cout << "Error opening " << line_csv << "\n";
        return 1;
    }

    getline(info_file, line); // skip header
    while(getline(info_file, line)){
        stringstream ss(line);
        string line_name, start, end, first, last;
        getline(ss, line_name, ',');
        getline(ss, start, ',');
        getline(ss, end, ',');
        getline(ss, first, ',');
        getline(ss, last, ',');
        line_name = trim(line_name);
        if(line_name.empty()) continue;
        metro_info[line_name] = { start, end, first, last };
    }
    info_file.close();

    // ---------- FIND ALL ROUTES ----------
    bool found_any = false;
    for(auto &p : metro_routes){
        string line_name = p.first;
        auto &stations = p.second;

        int from_idx = -1;
        int to_idx = -1;

        for(int i=0; i<stations.size(); i++){
            if(stations[i].normalized.find(from)!=string::npos && from_idx==-1)
                from_idx = i;
            if(stations[i].normalized.find(to)!=string::npos)
                to_idx = i;
        }

        if(from_idx!=-1 && to_idx!=-1 && from_idx<to_idx){
            found_any = true;

            cout << "Metro Line: " << line_name << "\n";

            if(metro_info.count(line_name)){
                cout << "Timings: First Train " << metro_info[line_name].first_train
                     << " | Last Train " << metro_info[line_name].last_train << "\n";
            }

            cout << "Route:\n";
            for(int i=from_idx; i<=to_idx; i++){
                cout << " -> " << stations[i].name << "\n";
            }

            cout << "--------------------------------------\n";
        }
    }

    if(!found_any){
        cout << "No metro found for this route!\n";
    }

    return 0;
}
