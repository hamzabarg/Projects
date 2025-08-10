#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <map>
using namespace std;

#define MAXCITY 30
#define MAXFLIGHT 200
#define ROUNDTRIP 0
#define ONEWAY 1
#define MINLAYOVER 30

string startCity, endCity;
void FindPaths(int start, int end, const vector<vector<int> >& next, vector<int> currentPath, vector<vector<int> >& allPaths);

struct FlightType {
    int FlightNo;
    string startCity;
    int timeDepart;
    string endCity;
    int timeArrival;
    int noOfPassengers;
    FlightType* nextDeparture;
    FlightType* nextArrival;
};

struct CityListType {
    string cityName;
    FlightType* nextDeparture;
    FlightType* nextArrival;
};

struct RouteType {
    int Day;
    int nHops;
    int FlightNo1;
    int FlightNo2;
};

struct ReservationType {
    string firstName;
    string lastName;
    int tripType;
    RouteType route1;
    RouteType route2;
    ReservationType* nextReserve;
};

CityListType cityList[MAXCITY];
FlightType* flightList[MAXFLIGHT];
ReservationType* reserveHead = nullptr;
ReservationType* reserveTail = nullptr;

int cityCount = 0;
map<string, int> cityIndex;

FlightType* MakeFlightNode(int FlightNo, const string& startCity, int timeDepart, const string& endCity, int timeArrival) {
    FlightType* newFlight = new FlightType;
    newFlight->FlightNo = FlightNo;
    newFlight->startCity = startCity;
    newFlight->timeDepart = timeDepart;
    newFlight->endCity = endCity;
    newFlight->timeArrival = timeArrival;
    newFlight->noOfPassengers = 0;
    newFlight->nextDeparture = nullptr;
    newFlight->nextArrival = nullptr;
    return newFlight;
}

void AddCity(const string& cityName) {
    if (cityIndex.find(cityName) == cityIndex.end() && cityCount < MAXCITY) {
        cityList[cityCount].cityName = cityName;
        cityList[cityCount].nextDeparture = nullptr;
        cityList[cityCount].nextArrival = nullptr;
        cityIndex[cityName] = cityCount++;
    }
}

void AddFlight(FlightType* flight) {
    int startIdx = cityIndex[flight->startCity];
    int endIdx = cityIndex[flight->endCity];

    flight->nextDeparture = cityList[startIdx].nextDeparture;
    cityList[startIdx].nextDeparture = flight;

    flight->nextArrival = cityList[endIdx].nextArrival;
    cityList[endIdx].nextArrival = flight;
}

void DisplayAllCities() {
    cout << "\nCities Serviced by the Airline:\n";
    cout << left << setw(10) << "City" << endl;
    cout << string(10, '-') << endl;
    for (int i = 0; i < cityCount; ++i) {
        cout << left << setw(10) << cityList[i].cityName << endl;
    }
    cout << endl;
}

void DisplayDepartures(const string& cityName) {
    int idx = cityIndex[cityName];
    FlightType* flight = cityList[idx].nextDeparture;
    cout << "\nDepartures from " << cityName << ":\n";
    cout << left << setw(10) << "FlightNo" << setw(15) << "Start City" << setw(10) << "Depart" << setw(15) << "End City" << setw(10) << "Arrival" << endl;
    cout << string(60, '-') << endl;
    while (flight) {
        cout << left << setw(10) << flight->FlightNo << setw(15) << flight->startCity << setw(10) << flight->timeDepart
            << setw(15) << flight->endCity << setw(10) << flight->timeArrival << endl;
        flight = flight->nextDeparture;
    }
    cout << endl;
}

void DisplayArrivals(const string& cityName) {
    int idx = cityIndex[cityName];
    FlightType* flight = cityList[idx].nextArrival;
    cout << "\nArrivals to " << cityName << ":\n";
    cout << left << setw(10) << "FlightNo" << setw(15) << "Start City" << setw(10) << "Depart" << setw(15) << "End City" << setw(10) << "Arrival" << endl;
    cout << string(60, '-') << endl;
    while (flight) {
        cout << left << setw(10) << flight->FlightNo << setw(15) << flight->startCity << setw(10) << flight->timeDepart
            << setw(15) << flight->endCity << setw(10) << flight->timeArrival << endl;
        flight = flight->nextArrival;
    }
    cout << endl;
}

int FindRoute(const string& startCity, const string& endCity, RouteType& route) {
    int startIdx = cityIndex[startCity];
    int endIdx = cityIndex[endCity];

    FlightType* flight = cityList[startIdx].nextDeparture;
    while (flight) {
        if (flight->endCity == endCity) {
            route.nHops = 1;
            route.FlightNo1 = flight->FlightNo;
            return 0;
        }
        flight = flight->nextDeparture;
    }

    flight = cityList[startIdx].nextDeparture;
    while (flight) {
        FlightType* connecting = cityList[cityIndex[flight->endCity]].nextDeparture;
        while (connecting) {
            if (connecting->endCity == endCity && connecting->timeDepart >= flight->timeArrival + MINLAYOVER) {
                route.nHops = 2;
                route.FlightNo1 = flight->FlightNo;
                route.FlightNo2 = connecting->FlightNo;
                return 0;
            }
            connecting = connecting->nextDeparture;
        }
        flight = flight->nextDeparture;
    }

    return -1;
}

ReservationType* MakeReserveNode(const string& firstName, const string& lastName, int tripType, const RouteType& route1, const RouteType& route2) {
    ReservationType* newReserve = new ReservationType;
    newReserve->firstName = firstName;
    newReserve->lastName = lastName;
    newReserve->tripType = tripType;
    newReserve->route1 = route1;
    newReserve->route2 = route2;
    newReserve->nextReserve = nullptr;
    return newReserve;
}

void AddReservation(ReservationType* reservation) {
    if (!reserveHead) {
        reserveHead = reserveTail = reservation;
    }
    else {
        reserveTail->nextReserve = reservation;
        reserveTail = reservation;
    }
}

void AddSamplePassengers() {
    RouteType route1, route2;

    route1.nHops = 1;
    route1.FlightNo1 = 111;

    ReservationType* passenger1 = MakeReserveNode("Alice", "Brown", ONEWAY, route1, route2);
    AddReservation(passenger1);

    ReservationType* passenger2 = MakeReserveNode("Charlie", "Davis", ONEWAY, route1, route2);
    AddReservation(passenger2);

    ReservationType* passenger3 = MakeReserveNode("Bob", "Smith", ONEWAY, route1, route2);
    AddReservation(passenger3);

    route1.FlightNo1 = 222;

    ReservationType* passenger4 = MakeReserveNode("Diana", "Evans", ONEWAY, route1, route2);
    AddReservation(passenger4);

    cout << "Sample passengers added successfully.\n";
}


void DisplayReservations() {
    ReservationType* current = reserveHead;
    cout << "\nReservations:\n";
    cout << left << setw(15) << "First Name" << setw(15) << "Last Name" << setw(10) << "FlightNo1" << setw(10) << "FlightNo2" << endl;
    cout << string(50, '-') << endl;
    while (current) {
        cout << left << setw(15) << current->firstName << setw(15) << current->lastName << setw(10) << current->route1.FlightNo1;
        if (current->route1.nHops == 2) {
            cout << setw(10) << current->route1.FlightNo2;
        }
        else {
            cout << setw(10) << "-";
        }
        cout << endl;
        current = current->nextReserve;
    }
    cout << endl;
}

void MakeReservation() {
    string firstName, lastName, startCity, endCity;
    int tripType;
    RouteType route1, route2;

    cout << "Enter first name: ";
    cin >> firstName;
    cout << "Enter last name: ";
    cin >> lastName;
    cout << "Enter start city: ";
    cin >> startCity;
    cout << "Enter end city: ";
    cin >> endCity;

    if (FindRoute(startCity, endCity, route1) == 0) {
        ReservationType* newReserve = MakeReserveNode(firstName, lastName, ONEWAY, route1, route2);
        AddReservation(newReserve);
        cout << "\nReservation made successfully.\n";
    }
    else {
        cout << "\nNo route found between the cities.\n";
    }
}

void FindShortestPaths(const string& startCity, const string& endCity) {
    int startIdx = cityIndex[startCity];
    int endIdx = cityIndex[endCity];

    vector<vector<int> > dist(cityCount, vector<int>(cityCount, INT_MAX));
    vector<vector<int> > next(cityCount, vector<int>(cityCount, -1));

    FlightType* flight = cityList[startIdx].nextDeparture;
    while (flight) {
        int endIdx = cityIndex[flight->endCity];
        dist[startIdx][endIdx] = 1;
        next[startIdx][endIdx] = flight->FlightNo;
        flight = flight->nextDeparture;
    }

    for (int k = 0; k < cityCount; ++k) {
        for (int i = 0; i < cityCount; ++i) {
            for (int j = 0; j < cityCount; ++j) {
                if (dist[i][k] != INT_MAX && dist[k][j] != INT_MAX &&
                    dist[i][j] > dist[i][k] + dist[k][j]) {
                    dist[i][j] = dist[i][k] + dist[k][j];
                    next[i][j] = next[k][j];
                }
            }
        }
    }

    if (dist[startIdx][endIdx] == INT_MAX) {
        cout << "No path found between " << startCity << " and " << endCity << endl;
    }
    else {
        cout << "Shortest paths between " << startCity << " and " << endCity << ":\n";

        vector<vector<int> > allPaths;
        FindPaths(startIdx, endIdx, next, vector<int>(), allPaths);

        for (const auto& path : allPaths) {
            cout << "Path: ";
            for (int flightNo : path) {
                cout << flightNo << " ";
            }
            cout << endl;
        }
    }
}

void FindPaths(int start, int end, const vector<vector<int> >& next, vector<int> currentPath, vector<vector<int> >& allPaths) {
    if (start == end) {
        allPaths.push_back(currentPath);
        return;
    }

    for (int intermediate = 0; intermediate < cityCount; ++intermediate) {
        if (next[start][intermediate] != -1) {
            currentPath.push_back(next[start][intermediate]);
            FindPaths(intermediate, end, next, currentPath, allPaths);
            currentPath.pop_back();
        }
    }
}

void DeleteReservation(const string& firstName, const string& lastName) {
    if (!reserveHead) {
        cout << "No reservations available to delete.\n";
        return;
    }

    ReservationType* current = reserveHead;
    ReservationType* prev = nullptr;

    while (current) {
        if (current->firstName == firstName && current->lastName == lastName) {
            if (prev) {
                prev->nextReserve = current->nextReserve;
            }
            else {
                reserveHead = current->nextReserve;
            }

            if (current == reserveTail) {
                reserveTail = prev;
            }

            delete current;
            cout << "Reservation for " << firstName << " " << lastName << " deleted successfully.\n";
            return;
        }

        prev = current;
        current = current->nextReserve;
    }

    cout << "Reservation for " << firstName << " " << lastName << " not found.\n";
}

void DisplayPassengers(int flightNo) {
    vector<ReservationType*> passengers;

    ReservationType* current = reserveHead;
    while (current) {
        if (current->route1.FlightNo1 == flightNo || current->route1.FlightNo2 == flightNo) {
            passengers.push_back(current);
        }
        current = current->nextReserve;
    }

    sort(passengers.begin(), passengers.end(), [](ReservationType* a, ReservationType* b) {
        return a->lastName < b->lastName;
        });

    cout << "\nPassengers on Flight " << flightNo << ":\n";
    cout << left << setw(15) << "First Name" << setw(15) << "Last Name" << endl;
    cout << string(30, '-') << endl;

    for (const auto& passenger : passengers) {
        cout << left << setw(15) << passenger->firstName << setw(15) << passenger->lastName << endl;
    }

    if (passengers.empty()) {
        cout << "No passengers found for this flight.\n";
    }

    cout << endl;
}

void FindAllPossibleRoutesHelper(int startIdx, int endIdx, vector<int>& path, vector<vector<int> >& allPaths) {
    if (startIdx == endIdx) {
        allPaths.push_back(path);
        return;
    }

    FlightType* flight = cityList[startIdx].nextDeparture;
    while (flight) {
        int nextIdx = cityIndex[flight->endCity];
        if (find(path.begin(), path.end(), flight->FlightNo) == path.end()) { // Avoid revisiting flights
            path.push_back(flight->FlightNo);
            FindAllPossibleRoutesHelper(nextIdx, endIdx, path, allPaths);
            path.pop_back();
        }
        flight = flight->nextDeparture;
    }
}

void FindAllPossibleRoutes(const string& startCity, const string& endCity) {
    if (cityIndex.find(startCity) == cityIndex.end() || cityIndex.find(endCity) == cityIndex.end()) {
        cout << "One or both cities are not serviced by the airline.\n";
        return;
    }

    int startIdx = cityIndex[startCity];
    int endIdx = cityIndex[endCity];
    vector<vector<int> > allPaths;
    vector<int> path;

    FindAllPossibleRoutesHelper(startIdx, endIdx, path, allPaths);

    if (allPaths.empty()) {
        cout << "No routes found between " << startCity << " and " << endCity << ".\n";
    }
    else {
        cout << "Possible routes between " << startCity << " and " << endCity << ":\n";
        for (const auto& p : allPaths) {
            cout << "Route: ";
            for (int flightNo : p) {
                cout << flightNo << " ";
            }
            cout << endl;
        }
    }
}


int main() {
    AddCity("Lahore");
    AddCity("Karachi");
    AddCity("Peshawar");
    AddCity("Quetta");

    FlightType* flight1 = MakeFlightNode(111, "Lahore", 700, "Karachi", 830);
    FlightType* flight2 = MakeFlightNode(222, "Lahore", 800, "Peshawar", 900);
    FlightType* flight3 = MakeFlightNode(333, "Peshawar", 930, "Quetta", 1100);
    FlightType* flight4 = MakeFlightNode(444, "Karachi", 600, "Lahore", 700);

    AddFlight(flight1);
    AddFlight(flight2);
    AddFlight(flight3);
    AddFlight(flight4);

    AddSamplePassengers();

    while (true) {
        cout << "\nAirline Reservation System\n";
        cout << string(30, '=') << endl;
        cout << "1. Display all cities\n";
        cout << "2. Display departures from a city\n";
        cout << "3. Display arrivals to a city\n";
        cout << "4. Make a reservation\n";
        cout << "5. Display reservations\n";
        cout << "6. Delete a reservation\n";
        cout << "7. Shortest Path between two cities\n";
        cout << "8. Display passengers of a flight\n";
        cout << "9. Find all possible routes between two cities\n";
        cout << "10. Exit\n";


        cout << string(30, '=') << endl;

        int choice;
        cout << "Enter your choice: ";
        cin >> choice;
        switch (choice) {
        case 1:
            DisplayAllCities();
            break;
        case 2: {
            string city;
            cout << "Enter city: ";
            cin >> city;
            DisplayDepartures(city);
            break;
        }
        case 3: {
            string city;
            cout << "Enter city: ";
            cin >> city;
            DisplayArrivals(city);
            break;
        }
        case 4:
            MakeReservation();
            break;
        case 5:
            DisplayReservations();
            break;
        case 6: {
            string firstName, lastName;
            cout << "Enter first name: ";
            cin >> firstName;
            cout << "Enter last name: ";
            cin >> lastName;
            DeleteReservation(firstName, lastName);
            break;
        }
        case 7: {
            cout << "Enter start city: ";
            cin >> startCity;
            cout << "Enter end city: ";
            cin >> endCity;
            FindShortestPaths(startCity, endCity);
            break;
        }
        case 8: {
            int flightNo;
            cout << "Enter flight number: ";
            cin >> flightNo;
            DisplayPassengers(flightNo);
            break;
        }
        case 9: {
            cout << "Enter start city: ";
            cin >> startCity;
            cout << "Enter end city: ";
            cin >> endCity;
            FindAllPossibleRoutes(startCity, endCity);
            break;
        }
        case 10:
            cout << "Exiting...\n";
            return 0;

        default:
            cout << "Invalid choice.\n";
        }
    }

    return 0;
}