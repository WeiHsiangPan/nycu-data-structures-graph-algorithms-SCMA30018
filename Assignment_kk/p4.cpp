#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <stdexcept>
#include <string>
#include <sstream>
#include <random>
#include <chrono>
#include <limits>

using namespace std;

/* ====================== SparseMatrix: each row is a singly linked list ====================== */
class SparseMatrix {
private:
    struct Node {
        int col;           // 1-based column index
        long long val;
        Node* next;
        Node(int c, long long v, Node* n=nullptr) : col(c), val(v), next(n) {}
    };

    int m_, n_;                 // dimensions m x n
    vector<Node*> rows_;        // rows_[i] = head of row-(i+1) list

    static void freeList(Node* p){
        while(p){ Node* q=p->next; delete p; p=q; }
    }
    static Node* cloneList(Node* p){
        if(!p) return nullptr;
        Node dummy(0,0);
        Node* tail=&dummy;
        while(p){ tail->next=new Node(p->col,p->val); tail=tail->next; p=p->next; }
        return dummy.next;
    }

    void insertOrAccumulateSorted(int r, int col, long long val){
        if(val==0) return;
        Node*& head = rows_[r-1];
        Node dummy(0,0,head);
        Node* prev=&dummy; Node* cur=head;
        while(cur && cur->col < col){ prev=cur; cur=cur->next; }
        if(cur && cur->col==col){
            cur->val += val;
            if(cur->val==0){ prev->next=cur->next; delete cur; }
        }else{
            prev->next = new Node(col, val, cur);
        }
        head = dummy.next;
    }

    static Node* addTwoSortedLists(Node* a, Node* b){
        Node dummy(0,0); Node* tail=&dummy;
        while(a || b){
            if(b==nullptr || (a && a->col < b->col)){
                tail->next=new Node(a->col,a->val); a=a->next;
            }else if(a==nullptr || (b && b->col < a->col)){
                tail->next=new Node(b->col,b->val); b=b->next;
            }else{
                long long s=a->val+b->val;
                if(s!=0){ tail->next=new Node(a->col,s); tail=tail->next; }
                a=a->next; b=b->next;
                continue;
            }
            tail=tail->next;
        }
        return dummy.next;
    }

    static long long sparseDot(Node* rA, Node* rB) {
        long long acc=0;
        while(rA && rB){
            if(rA->col < rB->col) rA=rA->next;
            else if(rB->col < rA->col) rB=rB->next;
            else { acc += rA->val * rB->val; rA=rA->next; rB=rB->next; }
        }
        return acc;
    }

public:
    SparseMatrix(): m_(0), n_(0) {}
    SparseMatrix(int m,int n): m_(m), n_(n), rows_(m,nullptr) {}
    ~SparseMatrix(){ clear(); }

    SparseMatrix(const SparseMatrix& o): m_(o.m_), n_(o.n_), rows_(o.m_,nullptr){
        for(int i=0;i<m_;++i) rows_[i]=cloneList(o.rows_[i]);
    }
    SparseMatrix& operator=(const SparseMatrix& o){
        if(this==&o) return *this;
        clear(); m_=o.m_; n_=o.n_; rows_.assign(m_,nullptr);
        for(int i=0;i<m_;++i) rows_[i]=cloneList(o.rows_[i]);
        return *this;
    }

    void clear(){
        for(auto &h: rows_) { freeList(h); h=nullptr; }
        rows_.clear(); m_=n_=0;
    }

    int rows() const { return m_; }
    int cols() const { return n_; }

    /* ---------- Input / Output ---------- */
    void readFromStream(istream& in){
        clear();
        if(!(in>>m_>>n_)) throw runtime_error("Invalid header.");
        rows_.assign(m_,nullptr);
        for(int r=1;r<=m_;++r){
            while(true){
                int c; in>>c; if(!in) throw runtime_error("Invalid row.");
                if(c==0) break;
                long long v; in>>v; if(!in) throw runtime_error("Invalid pair.");
                insertOrAccumulateSorted(r,c,v);
            }
        }
    }

    void readInteractive(int m, int n, istream& in, ostream& out){
        clear(); m_=m; n_=n; rows_.assign(m_, nullptr);
        string line;
        for(int r=1; r<=m_; ++r){
            out << "Row " << r << " -> enter: (col value ... 0): " << flush;
            getline(in, line);
            if(line.empty()){ --r; continue; }
            for(char& ch: line) if(ch==',') ch=' ';
            stringstream ss(line);
            int c; long long v;
            while(ss >> c){
                if(c==0) break;
                if(!(ss >> v)){ out << "Invalid format, re-enter this row.\n"; --r; break; }
                if(c < 1 || c > n_){ out << "Column index must be 1.." << n_ << ", re-enter this row.\n"; --r; break; }
                insertOrAccumulateSorted(r, c, v);
            }
        }
    }

    void printDense(ostream& out) const {
        for(int r=0;r<m_;++r){
            vector<long long> line(n_,0);
            for(Node* p=rows_[r]; p; p=p->next) line[p->col-1]=p->val;
            for(int c=0;c<n_;++c){ if(c) out<<' '; out<<line[c]; }
            out<<'\n';
        }
    }

    void printListAndMemory(ostream& out) const {
        long long nodes=0;
        for(int r=0;r<m_;++r){
            for(Node* p=rows_[r]; p; p=p->next){ out<<p->col<<' '<<p->val<<' '; ++nodes; }
            out<<0<<'\n';
        }
        long long units = nodes*3LL + m_ + 2;
        out << "#memory_units " << units << '\n';
    }

    SparseMatrix transpose() const {
        SparseMatrix T(n_, m_);
        for(int r=1;r<=m_;++r)
            for(Node* p=rows_[r-1]; p; p=p->next)
                T.insertOrAccumulateSorted(p->col, r, p->val);
        return T;
    }

    SparseMatrix add(const SparseMatrix& B) const {
        if(m_!=B.m_ || n_!=B.n_) throw invalid_argument("add: dimension mismatch");
        SparseMatrix C(m_, n_);
        for(int r=0;r<m_;++r) C.rows_[r]=addTwoSortedLists(rows_[r], B.rows_[r]);
        return C;
    }

    SparseMatrix multiply(const SparseMatrix& B) const {
        if(n_ != B.m_) throw invalid_argument("mul: dimension mismatch");
        SparseMatrix C(m_, B.n_);
        SparseMatrix BT = B.transpose();

        for(int i=1; i<=m_; ++i){
            Node* rowA = rows_[i-1];
            if(!rowA) continue;
            vector<int> touched; touched.reserve(64);
            vector<char> mark(B.n_+1, 0);
            for(Node* a = rowA; a; a=a->next){
                for(Node* b = B.rows_[a->col-1]; b; b=b->next){
                    int j = b->col;
                    if(!mark[j]){ mark[j]=1; touched.push_back(j); }
                }
            }
            sort(touched.begin(), touched.end());
            for(int j : touched){
                long long val = sparseDot(rowA, BT.rows_[j-1]);
                if(val!=0) C.insertOrAccumulateSorted(i, j, val);
            }
        }
        return C;
    }
};

/* ====================== Utility: parse (rows,cols) ====================== */
static pair<int,int> parseDimsLine(const string& raw){
    string s; s.reserve(raw.size());
    for(char ch: raw){
        if(ch=='(' || ch==')' || ch==',' ) s.push_back(' ');
        else s.push_back(ch);
    }
    stringstream ss(s);
    int r,c;
    if(!(ss>>r>>c)) throw runtime_error("Invalid dimension format. Example: 4 5 or (4,5)");
    if(r<=0 || c<=0) throw runtime_error("Dimensions must be positive integers.");
    return {r,c};
}

/* ====================== (a) Manual Mode ====================== */
void manualMode(){
    cout << "\n=== (a) Manual Input Mode ===\n";
    cout << "This program can perform matrix addition and multiplication.\n";
    cout << "You will now enter two matrices: A and B.\n";

    cout << "\nEnter dimension of matrix A (e.g. 4 5 or (4,5)): " << flush;
    string line; getline(cin, line);
    auto [ar, ac] = parseDimsLine(line);

    SparseMatrix A;
    cout << "Enter each row of A as (col value ... 0):\n";
    A.readInteractive(ar, ac, cin, cout);

    cout << "\nEnter dimension of matrix B (e.g. 5 4 or (5,4)): " << flush;
    getline(cin, line);
    auto [br, bc] = parseDimsLine(line);

    SparseMatrix B;
    cout << "Enter each row of B as (col value ... 0):\n";
    B.readInteractive(br, bc, cin, cout);

    cout << "\n--- Matrix A (dense) ---\n"; A.printDense(cout);
    cout << "--- Matrix A (list + memory) ---\n"; A.printListAndMemory(cout);

    cout << "\n--- Matrix B (dense) ---\n"; B.printDense(cout);
    cout << "--- Matrix B (list + memory) ---\n"; B.printListAndMemory(cout);

    bool canAdd = (A.rows()==B.rows() && A.cols()==B.cols());
    bool canMul = (A.cols()==B.rows());

    cout << "\nDimension check:\n";
    cout << "Addition:  " << (canAdd ? "Possible" : "Not possible") << "\n";
    cout << "Multiplication:  " << (canMul ? "Possible" : "Not possible") << "\n";

    if(canAdd){
        cout << "\nA + B (dense):\n";
        SparseMatrix C = A.add(B);
        C.printDense(cout);
        cout << "A + B (list + memory):\n";
        C.printListAndMemory(cout);
    }
    if(canMul){
        cout << "\nA * B (dense):\n";
        SparseMatrix D = A.multiply(B);
        D.printDense(cout);
        cout << "A * B (list + memory):\n";
        D.printListAndMemory(cout);
    }
    if(!canAdd && !canMul){
        cout << "\nNeither addition nor multiplication can be performed (dimension mismatch).\n";
    }
}

/* ====================== Random Generation ====================== */
static SparseMatrix genRandomSparse(int m, int n, double zeroProb, int minVal=1, int maxVal=9){
    stringstream ssTotal;
    ssTotal << m << " " << n << "\n";
    for(int r=1; r<=m; ++r){
        for(int c=1; c<=n; ++c){
            double z = (double)rand() / RAND_MAX;
            if(z >= zeroProb){
                int val = (rand() % (maxVal - minVal + 1)) + minVal;
                ssTotal << c << " " << val << " ";
            }
        }
        ssTotal << 0 << "\n";
    }
    SparseMatrix M;
    M.readFromStream(ssTotal);
    return M;
}

static SparseMatrix genRandomDense(int m, int n, int minVal=1, int maxVal=9){
    return genRandomSparse(m, n, 0.0, minVal, maxVal);
}

/* ====================== (b) Random Mode ====================== */
void randomMode(){
    cout << "\n=== (b) Random Generation Mode ===\n";
    cout << "Enter matrix dimension (rows cols), e.g. 200 200: " << flush;
    string line; getline(cin, line);
    auto [m,n] = parseDimsLine(line);

    double zeroProb = 0.95;

    cout << "\nGenerating two sparse matrices (zero density = 0.95)...\n";
    auto t0 = chrono::high_resolution_clock::now();
    SparseMatrix SA = genRandomSparse(m, n, zeroProb);
    SparseMatrix SB = genRandomSparse(m, n, zeroProb);
    auto t1 = chrono::high_resolution_clock::now();

    cout << "Generating two dense matrices (almost all nonzero)...\n";
    auto t2 = chrono::high_resolution_clock::now();
    SparseMatrix DA = genRandomDense(m, n);
    SparseMatrix DB = genRandomDense(m, n);
    auto t3 = chrono::high_resolution_clock::now();

    auto genSparseMs = chrono::duration_cast<chrono::milliseconds>(t1 - t0).count();
    auto genDenseMs  = chrono::duration_cast<chrono::milliseconds>(t3 - t2).count();

    cout << "\n--- Generation Time ---\n";
    cout << "Sparse matrix generation: " << genSparseMs << " ms\n";
    cout << "Dense  matrix generation: " << genDenseMs  << " ms\n";

    bool addOK = (SA.rows()==SB.rows() && SA.cols()==SB.cols());
    bool mulOK = (SA.cols()==SB.rows());

    cout << "\n=== Sparse Matrix Operations (SA, SB) ===\n";
    if(addOK){
        auto sA0 = chrono::high_resolution_clock::now();
        SparseMatrix SAdd = SA.add(SB);
        auto sA1 = chrono::high_resolution_clock::now();
        cout << "SA + SB time: " 
             << chrono::duration_cast<chrono::milliseconds>(sA1 - sA0).count() 
             << " ms\n";
    }else{
        cout << "SA + SB: dimension mismatch\n";
    }
    if(mulOK){
        auto sM0 = chrono::high_resolution_clock::now();
        SparseMatrix SMul = SA.multiply(SB);
        auto sM1 = chrono::high_resolution_clock::now();
        cout << "SA * SB time: " 
             << chrono::duration_cast<chrono::milliseconds>(sM1 - sM0).count() 
             << " ms\n";
    }else{
        cout << "SA * SB: dimension mismatch\n";
    }

    cout << "\n=== Dense Matrix Operations (DA, DB) ===\n";
    if(DA.rows()==DB.rows() && DA.cols()==DB.cols()){
        auto dA0 = chrono::high_resolution_clock::now();
        SparseMatrix DAdd = DA.add(DB);
        auto dA1 = chrono::high_resolution_clock::now();
        cout << "DA + DB time: " 
             << chrono::duration_cast<chrono::milliseconds>(dA1 - dA0).count() 
             << " ms\n";
    }else{
        cout << "DA + DB: dimension mismatch\n";
    }
    if(DA.cols()==DB.rows()){
        auto dM0 = chrono::high_resolution_clock::now();
        SparseMatrix DMul = DA.multiply(DB);
        auto dM1 = chrono::high_resolution_clock::now();
        cout << "DA * DB time: " 
             << chrono::duration_cast<chrono::milliseconds>(dM1 - dM0).count() 
             << " ms\n";
    }else{
        cout << "DA * DB: dimension mismatch\n";
    }

    cout << "\n(Note: to print matrices, call printDense() or printListAndMemory() here.)\n";
}

/* ====================== Main Menu ====================== */
int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cout << "=== Matrix Calculator ===\n";
    cout << "This program can perform matrix addition and multiplication.\n";
    cout << "(a) Manual input mode: enter two matrices and compute results.\n";
    cout << "(b) Random mode: generate two m-by-n sparse matrices (zero density 0.95) "
            "and two dense matrices, then perform addition and multiplication with CPU timing.\n";
    cout << "Enter your choice (a/b): " << flush;

    string choice; getline(cin, choice);
    if(choice=="a" || choice=="A"){
        manualMode();
    }else if(choice=="b" || choice=="B"){
        randomMode();
    }else{
        cout << "Invalid choice.\n";
        return 0;
    }
    return 0;
}
