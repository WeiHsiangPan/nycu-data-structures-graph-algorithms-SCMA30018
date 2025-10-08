#include <bits/stdc++.h>
using namespace std;

// def of struc
struct Pos {
	
    int r, c;
    bool operator==(const Pos& o) const { return r == o.r && c == o.c; }
    
};

struct Maze {
	
    vector<vector<char>> grid;
    Pos start{-1,-1}, end{-1,-1};
    bool hasStart = false, hasEnd = false;
    
};

//tool
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

vector<int> getColumns(string input, int maxCol) {
	
    auto tokens = split(trim(input));
    vector<int> cols;
    
    for (auto& t : tokens) {
        try {
            int col = stoi(t);
            if (col >= 1 && col <= maxCol) cols.push_back(col);
        } catch (...) {}
    }
    
    sort(cols.begin(), cols.end());
    cols.erase(unique(cols.begin(), cols.end()), cols.end());
    return cols;
    
}

int getColumn(string prompt, int maxCol) {
	
    while (true) {
        cout << prompt << flush;
        string input; getline(cin, input);
        input = trim(input);
        
        if (input.empty() || input == "0") return 0;
        
        try {
            int col = stoi(input);
            if (col >= 1 && col <= maxCol) return col;
        } catch (...) {}
        
        cout << "Please enter 1.." << maxCol << " or 0.\n";
    }
    
}

// maze construct
Maze build() {
	
    auto [rows, cols] = getSize();
    Maze maze;
    maze.grid.assign(rows, vector<char>(cols, '0'));

    cout << "\n=== Instructions ===\n"
         << "For each row:\n"
         << " (A) Obstacle columns (space/comma separated, 0 = none)\n"
         << " (B) Start column (if not placed)\n"
         << " (C) End column (if not placed)\n\n";

    for (int r = 1; r <= rows; ++r) {
        cout << "\n--- Row " << r << " ---\n";
        
        cout << "Obstacle columns: " << flush;
        string line; getline(cin, line);
        for (int c : getColumns(line, cols))
            maze.grid[r-1][c-1] = 'x';

        if (!maze.hasStart) {
            int col = getColumn("Start column: ", cols);
            if (col && maze.grid[r-1][col-1] != 'x') {
                maze.grid[r-1][col-1] = 's';
                maze.start = {r, col};
                maze.hasStart = true;
            }
        }

        if (!maze.hasEnd) {
            int col = getColumn("End column: ", cols);
            if (col) {
                if (maze.grid[r-1][col-1] == 'x') {
                    cout << "Cell occupied.\n";
                } else if (maze.hasStart && maze.start.r == r && maze.start.c == col) {
                    cout << "Cell has start.\n";
                } else {
                    maze.grid[r-1][col-1] = 't';
                    maze.end = {r, col};
                    maze.hasEnd = true;
                }
            }
        }
        
    }
    cout << "\n=== Final Maze ===\n";
    
    for (auto& row : maze.grid) {
        for (size_t j = 0; j < row.size(); ++j)
            cout << row[j] << (j+1 == row.size() ? '\n' : ' ');
    }
    return maze;
}

// find least path
bool valid(int r, int c, int R, int C) {
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
            if (valid(nr, nc, R, C) && grid[nr-1][nc-1] != 'x' && !visited[nr][nc]) {
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
string showPath(const vector<Pos>& path) {
    string result;
    for (auto& p : path) 
        result += "(" + to_string(p.r) + "," + to_string(p.c) + ")";
    return result;
}

vector<vector<string>> markPath(const vector<vector<char>>& grid, const vector<Pos>& path) {
    int R = grid.size(), C = grid[0].size();
    vector<vector<string>> result(R, vector<string>(C));
    
    for (int i = 0; i < R; ++i)
        for (int j = 0; j < C; ++j)
            result[i][j] = string(1, grid[i][j]);
    
    int step = 1;
    for (size_t i = 1; i + 1 < path.size(); ++i) {
        int r = path[i].r - 1, c = path[i].c - 1;
        result[r][c] = to_string(step++);
    }
    
    return result;
}

void show(const vector<vector<string>>& grid) {
    for (auto& row : grid) {
        for (size_t j = 0; j < row.size(); ++j)
            cout << row[j] << (j+1 == row.size() ? '\n' : ' ');
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(&cout);

    

        Maze maze = build();
        if (!maze.hasStart || !maze.hasEnd) {
            cout << "Need both start and end.\n";
            return 0;
        }
        
        auto path = findPath(maze.grid, maze.start, maze.end);
        if (path.empty()) {
            cout << "No path found.\n";
            return 0;
        }
        
        cout << "\nPath: " << showPath(path) << "\n";
        cout << "\nMaze with path:\n";
        show(markPath(maze.grid, path));
        
        return 0;
        
    
    
}