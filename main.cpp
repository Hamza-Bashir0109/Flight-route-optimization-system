#include <iostream>
#include <string>
#include <limits>
#include <fstream>
#include <sstream>
#include <algorithm>
using namespace std;

class Airport{
public:
    int id;
    string name;
    string city;
    string country;
    Airport* next;

    Airport(int i, string n, string c, string cr){
        id = i;
        name = n;
        city = c;
        country = cr;
        next = nullptr;
    }
};

class FlightGraph {
private:
    struct AdjNode {
        int airportID;
        double distance;
        double cost;
        double time;
        AdjNode* next;

        AdjNode(int id, double dist, double c, double t):airportID(id), distance(dist), cost(c), time(t), next(nullptr) {}
    };
   struct AdjList {
        AdjNode* head;
        AdjList():head(nullptr){}
    };
    AdjList* adjacencyList;
    int numairport;
    Airport** airports;
public:
    FlightGraph(int maxairpots){
        this->numairport=maxairpots;
        adjacencyList=new AdjList[maxairpots];
        airports=new Airport*[maxairpots];
        for (int i=0;i<maxairpots; ++i) {
            airports[i]=nullptr;
        }
    }   void loadAirportsFromCSV(const string& fileName) {
    ifstream file(fileName);
    if (!file.is_open()) {
        cout << "Error: Could not open file " << fileName << endl;
        return;
    }

    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string idStr, name, city, country;

        getline(ss, idStr, ',');
        getline(ss, name, ',');
        getline(ss, city, ',');
        getline(ss, country, ',');

        if (idStr.empty() || !all_of(idStr.begin(), idStr.end(), ::isdigit)) {
            cout << "Skipping invalid entry: " << line << endl;
            continue;
        }

        int id = stoi(idStr);
        addAirport(id, name, city, country);
    }

    file.close();
    cout << "Airports loaded successfully from " << fileName << endl;
}
    void addAirport(int id, string name, string city, string country) {
        if (id >= numairport || id < 0) {
            cout << "Error: Invalid Airport ID!" << endl;
            return;
        }
        if (airports[id] != nullptr) {
            cout << "Error: Airport ID already exists!" << endl;
            return;
        }
        airports[id] = new Airport(id, name, city, country);
        cout << "Airport added successfully." << endl;
    }

    void addRoute(int sourceID, int destinationID, double distance, double cost, double time) {
        if (sourceID >= numairport || destinationID >= numairport || sourceID < 0 || destinationID < 0) {
            cout << "Error: Invalid Airport IDs for the route!" << endl;
            return;
        }
        if (airports[sourceID] == nullptr || airports[destinationID] == nullptr) {
            cout << "Error: One or both airports do not exist!" << endl;
            return;
        }
        AdjNode* newNode = new AdjNode(destinationID, distance, cost, time);
        newNode->next = adjacencyList[sourceID].head;
        adjacencyList[sourceID].head = newNode;
        cout << "Route added successfully." << endl;
    }

    void dijkstra(int startID, int destID, const string& criterion) {
        const double INF = numeric_limits<double>::infinity();
        double* values = new double[numairport];
        int* previous = new int[numairport];
        bool* visited = new bool[numairport];

        for (int i = 0; i < numairport; ++i) {
            values[i] = INF;
            previous[i] = -1;
            visited[i] = false;
        }
        values[startID] = 0;

        for (int i = 0; i < numairport; ++i) {
            int current = -1;
            double minValue = INF;

            for (int j = 0; j < numairport; ++j) {
                if (!visited[j] && values[j] < minValue) {
                    current = j;
                    minValue = values[j];
                }
            }

            if (current == -1) break;
            visited[current] = true;

            AdjNode* neighbor = adjacencyList[current].head;
            while (neighbor) {
                double newValue = values[current];
                if (criterion == "distance") newValue += neighbor->distance;
                else if (criterion == "cost") newValue += neighbor->cost;
                else if (criterion == "time") newValue += neighbor->time;

                if (newValue < values[neighbor->airportID]) {
                    values[neighbor->airportID] = newValue;
                    previous[neighbor->airportID] = current;
                }
                neighbor = neighbor->next;
            }
        }

        if (values[destID] == INF) {
            cout << "No path exists from Airport " << startID << " to Airport " << destID << "." << endl;
        } else {
            cout << "Optimal path (" << criterion << "): " << values[destID] << endl;
            cout << "Path: ";
            printPath(previous, destID);
            cout << endl;
        }

        delete[] values;
        delete[] previous;
        delete[] visited;
    }

    void printPath(int* previous, int currentID) {
        if (currentID == -1) return;
        printPath(previous, previous[currentID]);
        if (airports[currentID]) {
            cout << airports[currentID]->name << " ";
        } else {
            cout << currentID << " ";
        }
    }

    ~FlightGraph() {
        for (int i = 0; i < numairport; ++i) {
            AdjNode* current = adjacencyList[i].head;
            while (current) {
                AdjNode* temp = current;
                current = current->next;
                delete temp;
            }
            if (airports[i]) {
                delete airports[i];
            }
        }
        delete[] adjacencyList;
        delete[] airports;
    }
};

int main() {
    int maxairpots = 1000;
    FlightGraph graph(maxairpots);
 graph.loadAirportsFromCSV("route.csv");
    int choice;
    do {
        cout << "\nMenu:\n";
        cout << "1. Add Airport\n";
        cout << "2. Add Route\n";
        cout << "3. Find Optimal Path\n";

        cout << "4. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        if (choice == 1) {
            int id;
            string name, city, country;
            cout << "Enter Airport ID: ";
            cin >> id;
            cout << "Enter Airport Name: ";
            cin >> name;
            cout << "Enter City: ";
            cin >> city;
            cout << "Enter Country: ";
            cin >> country;
            graph.addAirport(id, name, city, country);
        } else if (choice == 2) {
            int source, destination;
            double distance, cost, time;
            cout << "Enter Source Airport ID: ";
            cin >> source;
            cout << "Enter Destination Airport ID: ";
            cin >> destination;
            cout << "Enter Distance: ";
            cin >> distance;
            cout << "Enter Cost: ";
            cin >> cost;
            cout << "Enter Time: ";
            cin >> time;
            graph.addRoute(source, destination, distance, cost, time);
        } else if (choice == 3) {
            int source, destination;
            string criterion;
            cout << "Enter Source Airport ID: ";
            cin >> source;
            cout << "Enter Destination Airport ID: ";
            cin >> destination;
            cout << "Enter Criterion (distance/cost/time): ";
            cin >> criterion;
            graph.dijkstra(source, destination, criterion);
        }
        
    } while (choice != 4);

    cout << "Thank you for using the Flight Route Optimization System!" << endl;
    return 0;
}
