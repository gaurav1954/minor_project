#include <iostream>
#include <fstream>
#include <vector>
#include <stdexcept>
#include <array>
#include <climits>
#include <cmath>
#include <queue>

using namespace std;

class ParkingSpot
{
public:
    char size;    // Size of the spot
    int occupied; // Indicates if the spot is occupied (1 for occupied, 0 for unoccupied)
    int priority;

    ParkingSpot(char s, int o)
    {
        this->size = s;
        this->occupied = o;
        if (s == 'S')
        {
            priority = 1;
        }
        else if (s == 'M')
        {
            priority = 2;
        }
        else if (s == 'L')
        {
            priority = 3;
        }
    }
};

class ParkingLot
{
public:
    ParkingSpot ***lot; // Private pointer to a 2D array of ParkingSpot
    int rows;
    int columns;

    // Default constructor to read occupancy status from a file
    ParkingLot()
    {
        // Open a file for reading occupancy status
        ifstream file("occupancy.txt");
        if (!file)
        {
            cerr << "Error opening file for reading." << endl;
            return;
        }

        // Read occupancy status and initialize the parking lot
        rows = 15;    // You can adjust this based on your file structure
        columns = 15; // You can adjust this based on your file structure

        lot = new ParkingSpot **[rows];
        for (int i = 0; i < rows; i++)
        {
            lot[i] = new ParkingSpot *[columns];
            for (int j = 0; j < columns; j++)
            {
                int occupancy_status;
                file >> occupancy_status;

                char size;
                if (j >= 0 && j <= 5)
                {
                    size = 'S';
                }
                else if (j >= 6 && j <= 11)
                {
                    size = 'M';
                }
                else
                {
                    size = 'L';
                }
                lot[i][j] = new ParkingSpot(size, occupancy_status); // Initialize with ParkingSpot objects
            }
        }

        file.close(); // Close the file
    }

    void printSpotSizes()
    {
        cout << "\n\nThis is the view of parking lot with entry point as 1st row and 1st column" << endl;
        cout << "\n All the 1's represent filled spots and all the 0's represent vacant spots" << endl;
        for (int i = 0; i < rows; i++)
        {
            cout << "           ";
            for (int j = 0; j < columns; j++)
            {
                // Assuming lot[i][j]->occupied contains the spot size
                cout << "+---";
            }
            cout << "+\n           ";

            for (int j = 0; j < columns; j++)
            {
                cout << "| " << lot[i][j]->occupied << " ";
            }
            cout << "|\n";
        }
        cout << "           ";
        // Print the bottom border of the grid
        for (int j = 0; j < columns; j++)
        {
            cout << "+---";
        }
        cout << "+\n";
    }
    void printSpotSizesAfterAllocation(array<int, 2> &nearestSpot)
    {
        cout << "\n\n This is the parking lot with green box as the nearest spot" << endl;
        for (int i = 0; i < rows; i++)
        {
            cout << "           ";
            for (int j = 0; j < columns; j++)
            {
                // Print the top border of each cell
                cout << "+---";
            }
            cout << "+\n           ";

            for (int j = 0; j < columns; j++)
            {
                if (i == nearestSpot[0] && j == nearestSpot[1])
                {
                    // Set text color to red for the nearest spot
                    cout << "| \x1b[32m" << lot[i][j]->occupied << "\x1b[0m ";
                }
                else
                {
                    // Set text color to green for other spots
                    cout << "| \x1b[31m" << lot[i][j]->occupied << "\x1b[0m ";
                }
            }
            // Print the right border of the last cell and end the line
            cout << "|\n";
        }

        // Print the bottom border of the grid after all rows
        cout << "           ";
        for (int j = 0; j < columns; j++)
        {
            cout << "+---";
        }
        cout << "+\n";
    }

    // to mark to spot as occupied
    void occupySpot(array<int, 2> &nearestSpot)
    {
        int row = nearestSpot[0];
        int col = nearestSpot[1];
        if (row >= 0 && row < rows && col >= 0 && col < columns)
        {
            lot[row][col]->occupied = 1;
        }
        else
        {
            cerr << "Invalid spot coordinates." << endl;
        }
    }

    // linear searchs
    vector<array<int, 2>> filterEmptySpotsUsingLinearSeach(char vehicleSize)
    {
        vector<array<int, 2>> emptySpots;

        for (int i = 0; i < rows; i++)
        {
            for (int j = 0; j < columns; j++)
            {
                if (lot[i][j]->occupied == 0 && lot[i][j]->size == vehicleSize)
                {
                    array<int, 2> coords = {i, j};
                    emptySpots.push_back(coords);
                }
            }
        }
        return emptySpots;
    }
    // save to file
    void saveOccupancyStatusToFile()
    {
        ofstream file("occupancy.txt"); // Open a file for writing
        if (!file)
        {
            cerr << "Error opening file for writing." << endl;
            return;
        }

        for (int i = 0; i < rows; i++)
        {
            for (int j = 0; j < columns; j++)
            {
                file << lot[i][j]->occupied << ' ';
            }
            file << endl;
        }

        file.close(); // Close the file
        cout << "Occupancy status written to file." << endl;
    }

    // binary serach
    vector<array<int, 2>> filterEmptySpotsBinarySearch(char vehicleSize)
    {
        vector<array<int, 2>> emptySpots;

        int vehiclePriority;
        if (vehicleSize == 'S')
        {
            vehiclePriority = 1;
        }
        else if (vehicleSize == 'M')
        {
            vehiclePriority = 2;
        }
        else if (vehicleSize == 'L')
        {
            vehiclePriority = 3;
        }
        else
        {
            // Handle invalid size
            return emptySpots;
        }

        for (int i = 0; i < rows; i++)
        {
            int left = 0;
            int right = columns - 1;

            while (left <= right)
            {
                int mid = left + (right - left) / 2; // Calculate the middle column index

                if (lot[i][mid]->occupied == 0 && lot[i][mid]->size == vehicleSize)
                {
                    array<int, 2> coords = {i, mid};
                    emptySpots.push_back(coords);

                    int tleft = mid - 1;
                    while (tleft >= 0 && lot[i][tleft]->size == vehicleSize && lot[i][tleft]->occupied == 0)
                    {
                        array<int, 2> coords = {i, tleft};
                        emptySpots.push_back(coords);
                        tleft--;
                    }
                    int tright = mid + 1;
                    while (tright < 15 && lot[i][tright]->size == vehicleSize && lot[i][tright]->occupied == 0)
                    {
                        array<int, 2> coords = {i, tright};
                        emptySpots.push_back(coords);
                        tright++;
                    }
                    break;
                }
                else if (lot[i][mid]->priority <= vehiclePriority)
                {
                    left = mid + 1; // Search the right half
                }
                else
                {
                    right = mid - 1; // Search the left half
                }
            }
        }

        return emptySpots;
    }

    // Destructor to write occupancy status to a file
    ~ParkingLot()
    {
        // Deallocate memory
        for (int i = 0; i < rows; i++)
        {
            for (int j = 0; j < columns; j++)
            {
                delete lot[i][j];
            }
            delete[] lot[i];
        }
        delete[] lot;
    }
};

int manhattanDistance(const array<int, 2> &spot1, const array<int, 2> &spot2)
{
    return abs(spot1[0] - spot2[0]) + abs(spot1[1] - spot2[1]);
}

// Function to find the nearest spot using Dijkstra's algorithm
array<int, 2> findNearestSpotUsingDijkastras(const vector<array<int, 2>> &emptySpots, const array<int, 2> &entrance)
{
    // Create a priority queue for Dijkstra's algorithm
    priority_queue<pair<int, array<int, 2>>> pq;
    vector<vector<int>> distance(15, vector<int>(15, INT_MAX));

    // Initialize the distance for the entrance
    distance[entrance[0]][entrance[1]] = 0;
    pq.push({0, entrance});

    // Dijkstra's algorithm to find the nearest spot
    while (!pq.empty())
    {
        array<int, 2> current = pq.top().second;
        pq.pop();

        for (const auto &spot : emptySpots)
        {
            int newDistance = distance[current[0]][current[1]] + manhattanDistance(current, spot);

            if (newDistance < distance[spot[0]][spot[1]])
            {
                distance[spot[0]][spot[1]] = newDistance;
                pq.push({-newDistance, spot});
            }
        }
    }

    // Find the nearest spot
    int minDistance = INT_MAX;
    array<int, 2> nearestSpot;

    for (const auto &spot : emptySpots)
    {
        if (distance[spot[0]][spot[1]] < minDistance)
        {
            minDistance = distance[spot[0]][spot[1]];
            nearestSpot = spot;
        }
    }

    return nearestSpot;
}

array<int, 2> findNearestSpotBellmanFord(const vector<array<int, 2>> &emptySpots, const array<int, 2> &entrance)
{
    vector<vector<int>> distance(15, vector<int>(15, INT_MAX));

    // Initialize the distance for the entrance
    distance[entrance[0]][entrance[1]] = 0;

    // Apply Bellman-Ford to find the minimum distances
    for (int k = 0; k < emptySpots.size(); k++)
    {
        for (const auto &spot : emptySpots)
        {
            int i = spot[0];
            int j = spot[1];
            int w = manhattanDistance(spot, entrance);

            if (distance[i][j] > distance[entrance[0]][entrance[1]] + w)
            {
                distance[i][j] = distance[entrance[0]][entrance[1]] + w;
            }
        }
    }

    // Find the nearest spot
    int minDistance = INT_MAX;
    array<int, 2> nearestSpot;

    for (const auto &spot : emptySpots)
    {
        int i = spot[0];
        int j = spot[1];

        if (distance[i][j] < minDistance)
        {
            minDistance = distance[i][j];
            nearestSpot = spot;
        }
    }

    return nearestSpot;
}

class ParkingLotManager
{
public:
    ParkingLot parkingLot;
    // Member function to encapsulate your main logic
    void showAnalyticalView(const char &vehicleSize)
    {
        try
        {
            array<int, 2> entrance = {0, 0};

            // Define table headers
            cout << "\n\n"
                 << left << setw(15) << "Algorithm" << setw(15) << "Approach" << setw(23) << "Time (microseconds)"
                 << "Nearest Spot" << endl;
            cout << setfill('-') << setw(68) << "" << setfill(' ') << endl;

            vector<string> algorithms = {"Dijkstra's", "Dijkstra's", "Bellman-Ford", "Bellman-Ford"};
            vector<string> approaches = {"Binary Search", "Linear Search", "Binary Search", "Linear Search"};

            array<int, 2> nearestSpot;
            for (int i = 0; i < 4; i++)
            {

                vector<array<int, 2>> emptySpots;

                auto start = chrono::high_resolution_clock::now();

                if (i < 2)
                {
                    emptySpots = (i == 0) ? parkingLot.filterEmptySpotsBinarySearch(vehicleSize) : parkingLot.filterEmptySpotsUsingLinearSeach(vehicleSize);
                    nearestSpot = findNearestSpotUsingDijkastras(emptySpots, entrance);
                }
                else
                {
                    emptySpots = (i == 2) ? parkingLot.filterEmptySpotsBinarySearch(vehicleSize) : parkingLot.filterEmptySpotsUsingLinearSeach(vehicleSize);
                    nearestSpot = findNearestSpotBellmanFord(emptySpots, entrance);
                }

                auto end = chrono::high_resolution_clock::now();
                auto duration = chrono::duration_cast<chrono::microseconds>(end - start);

                cout << setw(15) << algorithms[i] << setw(15) << approaches[i] << setw(23) << duration.count();
                cout << "Row: " << nearestSpot[0] + 1 << ", Column: " << nearestSpot[1] + 1 << endl;
            }
            cout << "\n\n"
                 << endl;
        }
        catch (const exception &e)
        {
            cerr << "Error: " << e.what() << endl;
        }
    }
    void findClosestSpot(const char &vehicleSize)
    {
        int option = 0;
        cout << "Choose a combination to find the closest point" << endl;
        cout << "1. Dikjkastras with Linear Search" << endl;
        cout << "2. Dikjkastras with Binary Search" << endl;
        cout << "3. Bellman Ford with Linear Search" << endl;
        cout << "4. Bellman Ford with Binary Search" << endl;
        cout << "Enter your option:";
        cin >> option;
        array<int, 2> entrance = {0, 0};
        vector<array<int, 2>> emptySpots;
        array<int, 2> nearestSpot;
        switch (option)
        {
        case 1:
            emptySpots = parkingLot.filterEmptySpotsUsingLinearSeach(vehicleSize);
            nearestSpot = findNearestSpotUsingDijkastras(emptySpots, entrance);
            break;
        case 2:
            emptySpots = parkingLot.filterEmptySpotsBinarySearch(vehicleSize);
            nearestSpot = findNearestSpotUsingDijkastras(emptySpots, entrance);
            break;
        case 3:
            emptySpots = parkingLot.filterEmptySpotsUsingLinearSeach(vehicleSize);
            nearestSpot = findNearestSpotBellmanFord(emptySpots, entrance);
            break;
        case 4:
            emptySpots = parkingLot.filterEmptySpotsBinarySearch(vehicleSize);
            nearestSpot = findNearestSpotBellmanFord(emptySpots, entrance);
            break;

        default:
            cout << "Invalid option. Please select a valid combination." << endl;
        }
        cout << "The neares spot is at row number "
             << "\x1b[32m" << nearestSpot[0] + 1 << "\x1b[0m"
             << " and column number "
             << "\x1b[32m" << nearestSpot[1] + 1 << "\x1b[0m "
             << endl;
        parkingLot.printSpotSizes();
        parkingLot.printSpotSizesAfterAllocation(nearestSpot);
        parkingLot.occupySpot(nearestSpot);
        parkingLot.saveOccupancyStatusToFile();
    }
};

int main()
{
    // Instantiate ParkingLotManager and run the main logic
    ParkingLotManager manager;
    char vehicleSize;
    cout << "Enter your vehicle size (S for small, M for medium, L for large): ";
    cin >> vehicleSize;
    manager.findClosestSpot(vehicleSize);
    manager.showAnalyticalView(vehicleSize);

    return 0;
}
