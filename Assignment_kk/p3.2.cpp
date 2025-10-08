#include <bits/stdc++.h>
#include <random>
#include <fstream>
using namespace std;

struct Pos {

    int r, c;
    bool operator==(const Pos& o) const { return r == o.r && c == o.c; }

};

struct Maze {

    vector<vector<char>> grid;
    Pos start{-1,-1}, end{-1,-1};
    bool hasStart = false, hasEnd = false;

};

// random Maze generation
Maze generate(int rows, int cols, double obstacleRatio = 0.3) {
    Maze maze;
    maze.grid.assign(rows, vector<char>(cols, '0'));
    
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<double> dis(0.0, 1.0);
    
    // Generate obstacles
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (dis(gen) < obstacleRatio) {
                maze.grid[i][j] = 'x';
            }
        }
    }
 
    uniform_int_distribution<int> rowDist(0, rows-1);
    uniform_int_distribution<int> colDist(0, cols-1);
    
    // Select start position
    int attempts = 0;
    while (attempts < 1000) {
        int r = rowDist(gen), c = colDist(gen);
        if (maze.grid[r][c] == '0') {
            maze.grid[r][c] = 's';
            maze.start = {r+1, c+1};
            maze.hasStart = true;
            break;
        }
        attempts++;
    }
    
    // Select end position
    attempts = 0;
    while (attempts < 1000) {
        int r = rowDist(gen), c = colDist(gen);
        if (maze.grid[r][c] == '0' && !(r+1 == maze.start.r && c+1 == maze.start.c)) {
            maze.grid[r][c] = 't';
            maze.end = {r+1, c+1};
            maze.hasEnd = true;
            break;
        }
        attempts++;
    }
    
    return maze;
}

// custom Maze generation
Maze generateCustom() {
    int rows, cols;
    double obstacleRatio;
    
    cout << "Enter maze dimensions:" << endl;
    cout << "Rows: "; cin >> rows;
    cout << "Columns: "; cin >> cols;
    cout << "Obstacle ratio (0.0 - 1.0): "; cin >> obstacleRatio;
    
    // Clear input buffer
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    
    if (rows <= 0 || cols <= 0) {
        cout << "Invalid dimensions. Using default 100x100." << endl;
        rows = 100; cols = 100;
    }
    
    if (obstacleRatio < 0.0 || obstacleRatio > 1.0) {
        cout << "Invalid obstacle ratio. Using default 0.25." << endl;
        obstacleRatio = 0.25;
    }
    
    cout << "Generating " << rows << "x" << cols << " maze with " 
         << (obstacleRatio * 100) << "% obstacles..." << endl;
    
    return generate(rows, cols, obstacleRatio);
}

// file output
void saveToFile(const vector<vector<char>>& grid, const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cout << "Cannot create file: " << filename << endl;
        return;
    }
    
    int rows = grid.size(), cols = grid[0].size();
    file << rows << " " << cols << endl;
    
    for (int i = 0; i < rows; ++i) {
        bool hasNonZero = false;
        for (int j = 0; j < cols; ++j) {
            if (grid[i][j] != '0') {
                if (hasNonZero) file << " ";
                file << (j + 1) << " " << grid[i][j];
                hasNonZero = true;
            }
        }
        if (!hasNonZero) {
            file << "0";
        } else {
            file << " 0";
        }
        file << endl;
    }
    file.close();
    cout << "Maze saved to: " << filename << endl;
}

void savePathToFile(const vector<Pos>& path, const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cout << "Cannot create file: " << filename << endl;
        return;
    }
    
    for (const auto& pos : path) {
        file << "(" << pos.r << "," << pos.c << ")";
    }
    file << endl;
    file.close();
    cout << "Path saved to: " << filename << endl;
}

// tool
string trim(string s) {
    size_t start = s.find_first_not_of(" \t"), end = s.find_last_not_of(" \t");
    return start == string::npos ? "" : s.substr(start, end - start + 1);
}

vector<string> split(string s) {
    replace(s.begin(), s.end(), ',', ' ');
    stringstream ss(s);
    vector<string> tokens;
    for (string token; ss >> token;) tokens.push_back(token);
    return tokens;
}

// maze output act
pair<int,int> getSize() {
    while (true) {
        cout << "Enter maze size (rows cols): " << flush;
        string line; getline(cin, line);
        auto tokens = split(trim(line));
        if (tokens.size() != 2) {
            cout << "Please enter two positive integers.\n";
            continue;
        }
        try {
            int r = stoi(tokens[0]), c = stoi(tokens[1]);
            if (r > 0 && c > 0) return {r, c};
        } catch (...) {}
        cout << "Invalid input.\n";
    }
}

// least path
bool isValid(int r, int c, int R, int C) {
    return r >= 1 && r <= R && c >= 1 && c <= C;
}

vector<Pos> findPath(const vector<vector<char>>& grid, Pos start, Pos end) {
    int R = grid.size(), C = grid[0].size();
    int dr[] = {-1, 1, 0, 0}, dc[] = {0, 0, -1, 1};
    
    vector<vector<bool>> visited(R+1, vector<bool>(C+1, false));
    vector<vector<Pos>> parent(R+1, vector<Pos>(C+1, {-1,-1}));
    queue<Pos> q;
    
    q.push(start);
    visited[start.r][start.c] = true;
    
    while (!q.empty()) {
        Pos cur = q.front(); q.pop();
        if (cur == end) break;
        
        for (int i = 0; i < 4; ++i) {
            int nr = cur.r + dr[i], nc = cur.c + dc[i];
            if (isValid(nr, nc, R, C) && grid[nr-1][nc-1] != 'x' && !visited[nr][nc]) {
                visited[nr][nc] = true;
                parent[nr][nc] = cur;
                q.push({nr, nc});
            }
        }
    }
    
    if (!visited[end.r][end.c]) return {};
    
    vector<Pos> path;
    for (Pos at = end; !(at == start); at = parent[at.r][at.c])
        path.push_back(at);
    path.push_back(start);
    reverse(path.begin(), path.end());
    return path;
}

// visualize output
void visualize(const Maze& maze, const vector<Pos>& path = {}) {
    int rows = maze.grid.size(), cols = maze.grid[0].size();
    
    vector<vector<char>> display = maze.grid;
    
    for (size_t i = 1; i + 1 < path.size(); ++i) {
        int r = path[i].r - 1, c = path[i].c - 1;
        if (display[r][c] == '0') {
            display[r][c] = '.';
        }
    }

    cout << "\n=== Maze Statistics ===" << endl;
    cout << "Size: " << rows << " x " << cols << endl;
    cout << "Start: (" << maze.start.r << ", " << maze.start.c << ")" << endl;
    cout << "End: (" << maze.end.r << ", " << maze.end.c << ")" << endl;
    cout << "Path length: " << (path.empty() ? 0 : path.size()) << endl;
    
    // For large mazes, show partial preview
    if (rows > 20 || cols > 20) {
        cout << "\n=== Maze Preview (first 20x20) ===" << endl;
        int previewRows = min(20, rows);
        int previewCols = min(20, cols);
        
        for (int i = 0; i < previewRows; ++i) {
            for (int j = 0; j < previewCols; ++j) {
                cout << display[i][j] << " ";
            }
            cout << endl;
        }
        cout << "... (Full maze saved to file)" << endl;
    } else {
        cout << "\n=== Complete Maze ===" << endl;
        for (const auto& row : display) {
            for (char cell : row) {
                cout << cell << " ";
            }
            cout << endl;
        }
    }
}

// performance testing
void performanceTest() {
    vector<int> sizes = {100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 5000, 10000};
    vector<double> obstacles = {0.1, 0.2, 0.3};
    
    cout << "\n=== Performance Test ===" << endl;
    cout << "Size\tObstacles%\tTime(ms)\tPath Found" << endl;
    cout << "------------------------------------------------" << endl;
    
    for (int size : sizes) {
        for (double obstacleRatio : obstacles) {
            auto startTime = chrono::high_resolution_clock::now();
            
            Maze maze = generate(size, size, obstacleRatio);
            vector<Pos> path;
            if (maze.hasStart && maze.hasEnd) {
                path = findPath(maze.grid, maze.start, maze.end);
            }
            
            auto endTime = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::milliseconds>(endTime - startTime);
            
            cout << size << "x" << size << "\t" << (obstacleRatio * 100) 
                 << "%\t\t" << duration.count() << " ms\t\t" 
                 << (path.empty() ? "No" : "Yes") << endl;
            
            // Stop if it takes too long
            if (duration.count() > 60000) { // 1 minute
                cout << "Stopping test - taking too long..." << endl;
                return;
            }
        }
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(&cout);
    
    cout << "=== Maze Path Finder ===" << endl;
    cout << "Select mode:" << endl;
    cout << "1. Manual maze input" << endl;
    cout << "2. Custom random maze" << endl;
    cout << "3. Performance test" << endl;
    cout << "Choice: " << flush;
    
    string choice;
    getline(cin, choice);
    
    Maze maze;
    
    if (choice == "1") {
    	cout << "not invalid" << endl;
        return 0;
    } else if (choice == "2") {
        maze = generateCustom();
    } else if (choice == "3") {
        performanceTest();
        return 0;
    } else {
        cout << "Invalid choice" << endl;
        return 1;
    }
    
    // Save original maze
    saveToFile(maze.grid, "maze.txt");
    
    // Display maze information
    visualize(maze);
    
    if (!maze.hasStart || !maze.hasEnd) {
        cout << "Error: Could not find valid start or end position" << endl;
        return 1;
    }
    
    cout << "\nFinding shortest path..." << endl;
    auto startTime = chrono::high_resolution_clock::now();
    auto path = findPath(maze.grid, maze.start, maze.end);
    auto endTime = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(endTime - startTime);
    
    if (path.empty()) {
        cout << "No path found from start to end!" << endl;
    } else {
        cout << "Path found! Length: " << path.size() << " steps" << endl;
        cout << "Time taken: " << duration.count() << " ms" << endl;
        
        // Save path
        savePathToFile(path, "path.txt");
        
        // Display maze with path
        visualize(maze, path);
        
        // Output first 10 path steps as example
        cout << "\n=== First 10 Path Steps ===" << endl;
        for (size_t i = 0; i < min((size_t)10, path.size()); ++i) {
            cout << "Step " << i << ": (" << path[i].r << ", " << path[i].c << ")" << endl;
        }
        if (path.size() > 10) {
            cout << "... " << path.size() - 10 << " more steps" << endl;
        }
    }
    
    return 0;
}

