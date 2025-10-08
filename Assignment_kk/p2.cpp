#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <algorithm>
#include <sstream>
#include <cmath>
#include <ctime>
#include <iomanip>   
#include <utility>   
using namespace std;

// Random
static std::mt19937 rng; 
vector<int> make_random(size_t n){

    uniform_int_distribution<int> d(-1000000, 1000000);
    vector<int> a(n);
    for (auto &x : a) x = d(rng);
    return a;

}

// 1) Insertion Sort
void insertion_sort(vector<int>& a){

    for (size_t i = 1; i < a.size(); ++i){

        int y = a[i];
        int j = static_cast<int>(i) - 1;

        while (j >= 0 && a[j] > y){ 
            a[j + 1] = a[j]; --j; 
        }

        a[j + 1] = y;

    }

}

// 2) Selection Sort（避免 size_t underflow）
void selection_sort(vector<int>& a){

    for (size_t i = 0; i + 1 < a.size(); ++i){

        size_t min_idx = i;

        for (size_t j = i + 1; j < a.size(); ++j)
            if (a[j] < a[min_idx]) min_idx = j;
        if (min_idx != i) swap(a[i], a[min_idx]);

    }

}

// 3) Quick Sort
void quick_sort_helper(vector<int>& a, int left, int right){

    if (left >= right) return;
    int pivot = a[left + (right - left) / 2];
    int i = left, j = right;

    while (i <= j){
        while (a[i] < pivot) ++i;
        while (a[j] > pivot) --j;
        if (i <= j) swap(a[i++], a[j--]);
    }

    if (left < j) quick_sort_helper(a, left, j);
    if (i < right) quick_sort_helper(a, i, right);

}
void quick_sort(vector<int>& a){

    if (!a.empty()) quick_sort_helper(a, 0, (int)a.size() - 1);

}

// 4) Merge Sort
void merge(vector<int>& a, int left, int mid, int right){

    vector<int> temp(right - left + 1);
    int i = left, j = mid + 1, k = 0;

    while (i <= mid && j <= right){
        if (a[i] <= a[j]) temp[k++] = a[i++];
        else              temp[k++] = a[j++];
    }
    while (i <= mid)   temp[k++] = a[i++];
    while (j <= right) temp[k++] = a[j++];

    for (int idx = 0; idx < k; ++idx) a[left + idx] = temp[idx];

}
void merge_sort_t(vector<int>& a, int left, int right){

    if (left >= right) return;
    int mid = left + (right - left) / 2;
    merge_sort_t(a, left, mid);
    merge_sort_t(a, mid + 1, right);
    merge(a, left, mid, right);

}

void merge_sort(vector<int>& a){

    if (!a.empty()) merge_sort_t(a, 0, (int)a.size() - 1);

}

// 5) Heap Sort
void heapify(vector<int>& a, int n, int i){

    int largest = i, left = 2 * i + 1, right = 2 * i + 2;

    if (left  < n && a[left]  > a[largest]) largest = left;
    if (right < n && a[right] > a[largest]) largest = right;
    if (largest != i){ swap(a[i], a[largest]); heapify(a, n, largest); }

}
void heap_sort(vector<int>& a){

    int n = (int)a.size();

    for (int i = n / 2 - 1; i >= 0; --i) heapify(a, n, i);
    for (int i = n - 1; i > 0; --i){ swap(a[0], a[i]); heapify(a, i, 0); }

}

// 安全 log2 by gpt
inline double log2_safe(double x){ return (x <= 1.0) ? 1.0 : std::log2(x); }

// 牆鐘時間
double measure_time(void (*sort_func)(std::vector<int>&), std::vector<int> data){

    auto t0 = std::chrono::high_resolution_clock::now();
    sort_func(data);
    auto t1 = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::milli>(t1 - t0).count();

}

// CPU time
double measure_cpu_ms(void (*f)(std::vector<int>&), std::vector<int> data){

    clock_t c0 = std::clock();
    f(data);
    clock_t c1 = std::clock();
    return 1000.0 * (c1 - c0) / CLOCKS_PER_SEC;

}

// Debug mode
bool is_sorted_nondec(const std::vector<int>& a){

    for (size_t i = 1; i < a.size(); ++i)
        if (a[i-1] > a[i]) return false;
    return true;

}

// 時間複雜度外推
double predict_n2(double n,double n0,double t0){ double k=t0/(n0*n0); return k*n*n; }
double predict_nlogn(double n,double n0,double t0){

    double k=t0/(n0*log2_safe(n0));
    return k*n*log2_safe(n);

}

// 反推在時間 T 內可處理的最大 n by gpt
enum class AlgType { N2, NLOGN };
size_t invert_max_n(double T_ms, AlgType type, double t0_ms, double n0){

    if (t0_ms <= 0) return 0;

    if (type == AlgType::N2){
        double k = t0_ms / (n0*n0);
        return (size_t)std::floor(std::sqrt(T_ms / k));
    }
    
    else{
        double k = t0_ms / (n0 * log2_safe(n0));
        auto time_model = [&](double n){ return k * n * log2_safe(n); };
        double lo = 1.0, hi = std::max(2.0, n0);
        while (time_model(hi) < T_ms) hi *= 2.0;
        for (int it=0; it<60; ++it){
            double mid = 0.5*(lo+hi);
            if (time_model(mid) <= T_ms) lo = mid; else hi = mid;
        }
        return (size_t)std::floor(lo);
    }

}

int main(){

    // 固定 seed
    rng.seed(42);

    // 1) n → time（Volume → Time）
    std::vector<size_t> test_sizes = {10000, 20000, 50000, 100000};

    cout << "Sorting Performance (WALL CLOCK, ms)\n";
    cout << "n\tInsertion\tSelection\tQuick\t\tMerge\t\tHeap\n";
    cout << "----\t---------\t---------\t-----\t\t-----\t\t----\n";

    for (size_t n : test_sizes){
        auto base = make_random(n);
        double w1 = measure_time(insertion_sort, base);
        double w2 = measure_time(selection_sort, base);
        double w3 = measure_time(quick_sort,    base);
        double w4 = measure_time(merge_sort,    base);
        double w5 = measure_time(heap_sort,     base);
        cout.setf(std::ios::fixed); cout << setprecision(2);
        cout << n << "\t" << w1 << "\t\t" << w2 << "\t\t"
             << w3 << "\t\t" << w4 << "\t\t" << w5 << "\n";
    }

    // 2) n → time（Volume → CPU Time）
    cout << "\nSorting Performance (CPU TIME, ms)\n";
    cout << "n\tInsertion\tSelection\tQuick\t\tMerge\t\tHeap\n";
    cout << "----\t---------\t---------\t-----\t\t-----\t\t----\n";

    for (size_t n : test_sizes){
        auto base = make_random(n);
        double c1 = measure_cpu_ms(insertion_sort, base);
        double c2 = measure_cpu_ms(selection_sort, base);
        double c3 = measure_cpu_ms(quick_sort,    base);
        double c4 = measure_cpu_ms(merge_sort,    base);
        double c5 = measure_cpu_ms(heap_sort,     base);
        cout.setf(std::ios::fixed); cout << setprecision(2);
        cout << n << "\t" << c1 << "\t\t" << c2 << "\t\t"
             << c3 << "\t\t" << c4 << "\t\t" << c5 << "\n";
    }

    // Debug mode
    {
        size_t nchk = 50000;
        auto base = make_random(nchk);
        auto d1 = base; insertion_sort(d1);
        auto d2 = base; selection_sort(d2);
        auto d3 = base; quick_sort(d3);
        auto d4 = base; merge_sort(d4);
        auto d5 = base; heap_sort(d5);
        bool ok1 = is_sorted_nondec(d1);
        bool ok2 = is_sorted_nondec(d2);
        bool ok3 = is_sorted_nondec(d3);
        bool ok4 = is_sorted_nondec(d4);
        bool ok5 = is_sorted_nondec(d5);

        if (!(ok1&&ok2&&ok3&&ok4&&ok5)){
            cerr << "[CHECK FAIL] I:" << ok1 << " S:" << ok2
                 << " Q:" << ok3 << " M:" << ok4 << " H:" << ok5 << "\n";
        }
    }
S
    // 3) time → n（Time Budget → Max Volume）
    size_t n0 = 50000;
    auto base0 = make_random(n0);
    double t0_ins = measure_cpu_ms(insertion_sort, base0); // ~ n^2
    double t0_sel = measure_cpu_ms(selection_sort, base0); // ~ n^2
    double t0_quk = measure_cpu_ms(quick_sort,    base0);  // ~ n log n
    double t0_mrg = measure_cpu_ms(merge_sort,    base0);  // ~ n log n
    double t0_hap = measure_cpu_ms(heap_sort,     base0);  // ~ n log n

    vector<pair<string,double>> budgets = {
        {"1 min",   60'000.0},
        {"5 min",  300'000.0},
        {"10 min", 600'000.0},
    };

    cout << "\nMax n within Time Budgets (based on CPU time @ n0="<< n0 <<")\n";
    cout << "Budget\tInsertion\tSelection\tQuick\t\tMerge\t\tHeap\n";
    cout << "------\t---------\t---------\t-----\t\t-----\t\t----\n";

    for (auto &B : budgets){
        const string& label = B.first; double T = B.second;
        size_t n_ins = invert_max_n(T, AlgType::N2,    t0_ins, n0);
        size_t n_sel = invert_max_n(T, AlgType::N2,    t0_sel, n0);
        size_t n_quk = invert_max_n(T, AlgType::NLOGN, t0_quk, n0);
        size_t n_mrg = invert_max_n(T, AlgType::NLOGN, t0_mrg, n0);
        size_t n_hap = invert_max_n(T, AlgType::NLOGN, t0_hap, n0);
        cout << label << "\t" << n_ins << "\t\t" << n_sel << "\t\t"
             << n_quk << "\t\t" << n_mrg << "\t\t" << n_hap << "\n";
    }

    cin.get();

    return 0;

}