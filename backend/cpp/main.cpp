#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>
#include <cctype>

using namespace std;

struct Stop {
    string name;
    int fare;
};

struct BusInfo {
    string start;
    string end;
    string first_time;
    string last_time;
};

// ---------- UTIL FUNCTIONS ----------

string trim(const string &s) {
    size_t start = s.find_first_not_of(" \t");
    size_t end = s.find_last_not_of(" \t");
    return (start == string::npos) ? "" : s.substr(start, end - start + 1);
}

string normalizeLocation(string s) {
    transform(s.begin(), s.end(), s.begin(),
              [](unsigned char c){ return tolower(c); });

    string out;
    for (int i = 0; i < s.size(); i++) {
        if (s[i] == ' ') {
            if (i > 0 && s[i-1] == '-') continue;
            if (i + 1 < s.size() && s[i+1] == '-') continue;
        }
        out += s[i];
    }
    return trim(out);
}

// ---------- MAIN ----------

int main(int argc, char* argv[]) {

    // 🔴 ONLY UPDATE: INPUT FROM COMMAND LINE
    if (argc < 3) {
        cout << "No bus found for this route!\n";
        return 0;
    }

    string from = argv[1];
    string to   = argv[2];

    from = normalizeLocation(from);
    to   = normalizeLocation(to);

    map<string, vector<Stop>> bus_routes;
    map<string, BusInfo> bus_info;

    // ---------- READ BUS STOPS ----------
    ifstream stops_file("dtc_bus_stops.csv");
    if (!stops_file.is_open()) {
        cout << "Error opening dtc_bus_stops.csv\n";
        return 1;
    }

    string line;
    getline(stops_file, line);

    while (getline(stops_file, line)) {
        stringstream ss(line);
        string bus, order, stop_name, fare_str;

        getline(ss, bus, ',');
        getline(ss, order, ',');
        getline(ss, stop_name, ',');
        getline(ss, fare_str, ',');

        bus = trim(bus);
        stop_name = trim(stop_name);
        fare_str = trim(fare_str);

        if (bus.empty() || stop_name.empty() || fare_str.empty()) continue;
        if (!all_of(fare_str.begin(), fare_str.end(), ::isdigit)) continue;

        bus_routes[bus].push_back({stop_name, stoi(fare_str)});
    }
    stops_file.close();

    // ---------- READ BUS INFO ----------
    ifstream bus_file("dtc_buses.csv");
    if (!bus_file.is_open()) {
        cout << "Error opening dtc_buses.csv\n";
        return 1;
    }

    getline(bus_file, line);
    while (getline(bus_file, line)) {
        stringstream ss(line);
        string bus, start, end, first, last, fare;

        getline(ss, bus, ',');
        getline(ss, start, ',');
        getline(ss, end, ',');
        getline(ss, first, ',');
        getline(ss, last, ',');
        getline(ss, fare, ',');

        bus = trim(bus);
        start = trim(start);
        end = trim(end);
        first = trim(first);
        last = trim(last);

        if (bus.empty()) continue;

        bus_info[bus] = {start, end, first, last};
    }
    bus_file.close();

    // ---------- FIND ROUTE ----------
    bool found = false;

    for (auto &p : bus_routes) {
        string bus = p.first;
        auto &stops = p.second;

        int from_idx = -1, to_idx = -1;

        for (int i = 0; i < stops.size(); i++) {
            string s = normalizeLocation(stops[i].name);
            if (s == from) from_idx = i;
            if (s == to)   to_idx = i;
        }

        if (from_idx != -1 && to_idx != -1 && from_idx < to_idx) {
            found = true;

            cout << "Bus Number: " << bus;

            if (bus_info.count(bus)) {
                cout << " (" << bus_info[bus].start
                     << " -> " << bus_info[bus].end << ")\n";
                cout << "Timings: First Bus " << bus_info[bus].first_time
                     << " | Last Bus " << bus_info[bus].last_time << "\n";
            }

            cout << "Route:\n";
            for (int i = from_idx; i <= to_idx; i++) {
                cout << " -> " << stops[i].name
                     << " (Fare: Rs " << stops[i].fare << ")\n";
            }
            
        }
        
    }

    if (!found)
        cout << "No bus found for this route!\n";

    return 0;
}
