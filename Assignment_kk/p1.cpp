#include<iostream>
using namespace std;

int my_isdigit(char c){ 

return c>='0' && c<='9'; 

}

size_t my_strlen(const char* src){ 

    size_t n=0; 
    while(src[n]) ++n; 
    return n; 

}

void my_strcpy(char* dest, const char* src){

    while((*dest++ = *src++)); 
 
 }

void my_trim(char* s){

    size_t n = my_strlen(s);
    size_t L=0, R=n;
    while(s[L]==' '||s[L]=='\t'||s[L]=='\n'||s[L]=='\r') ++L;
    while(R> L && (s[R-1]==' '||s[R-1]=='\t'||s[R-1]=='\n'||s[R-1]=='\r')) --R;
    size_t k=0; for(size_t i=L;i<R;++i) s[k++]=s[i]; s[k]='\0';

}
//Refer to the following link for the program of the above function and modify it
//https://blog.csdn.net/wsq119/article/details/81431703
//https://blog.csdn.net/2202_75305885/article/details/129527865?ops_request_misc=%257B%2522request%255Fid%2522%253A%25226e04b1203c999a726e37af961397c558%2522%252C%2522scm%2522%253A%252220140713.130102334..%2522%257D&request_id=6e04b1203c999a726e37af961397c558&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~top_positive~default-1-129527865-null-null.142^v102^control&utm_term=strlen&spm=1018.2226.3001.4187
//https://blog.csdn.net/MakerCloud/article/details/88932437
//https://reurl.cc/GNLVeD

int isLeap(int y){ 

    return (y%400==0) || (y%4==0 && y%100!=0);

}
//Determine whether it is a leap year

int daysInMonth(int y,int m){

    int base[13]={0,31,28,31,30,31,30,31,31,30,31,30,31};
    if(m==2) return base[2] + (isLeap(y) ? 1 : 0);
    return (m>=1 && m<=12) ? base[m] : 0;

}
//Determine how many days there are in a month

int parseYMD(const char* src, int& yy, int& mm, int& dd){

    char buf[256];
    size_t len = 0;
    while(src[len] && len<255){ 

        buf[len]=src[len]; 
        ++len; 

    }
    buf[len]='\0'; my_trim(buf);
    const char* p = buf;

    int y=0, m=0, d=0, cnt=0;
    while(my_isdigit(*p) && cnt<4) { 
        
        y = y*10 + (*p-'0'); 
        ++p; 
        ++cnt; 
    
    }

    if(cnt!=4 || *p!='/') 

        return 1;
        ++p;
        cnt=0; 

    while(my_isdigit(*p) && cnt<2){ 

        m = m*10 + (*p-'0'); 
        ++p; 
        ++cnt; 
    
    }

    if(cnt<1 || *p!='/') 

        return 1;
        ++p;
        cnt=0; 
        
    while(my_isdigit(*p) && cnt<2){ 

        d = d*10 + (*p-'0'); 
        ++p; 
        ++cnt; 

    }
    
    if(cnt<1) 

        return 1;

    while(*p==' '||*p=='\t'||*p=='\n'||*p=='\r') 

        ++p;

    if(*p!='\0') 

        return 1;

    // Note: return 1(code == 1) is used as a unified error code for all "format errors"
    // (wrong year digits, wrong separators, missing month/day digits, or extra chars).
    //Improved by gpt (add more format error conditions)

    if(m<1 || m>12) 

        return 2;

    int dim = daysInMonth(y,m);
    if(d<1 || d>dim){

        if(m==2 && d==29 && !isLeap(y)) 

            return 4;

        return 3;

    }
    yy=y; mm=m; dd=d; 
    return 0;
    //Determine other situations
    //code == 0 : The input is correct
    //code == 2 : Wrong month
    //code == 3 : Error day
    //code == 4 : Enter 2/29 but not in a leap year
}

void normalizeYMD(char* out, int y,int m,int d){

    out[0]= char('0'+(y/1000)%10);
    out[1]= char('0'+(y/100)%10);
    out[2]= char('0'+(y/10)%10);
    out[3]= char('0'+(y)%10);
    out[4]= '/';
    out[5]= char('0'+(m/10)%10);
    out[6]= char('0'+(m)%10);
    out[7]= '/';
    out[8]= char('0'+(d/10)%10);
    out[9]= char('0'+(d)%10);
    out[10]='\0';

}
//Three strings that combine the numbers y, m, d into a fixed format: "yyyy/mm/dd"
//Improved by gpt (adding more conditions to resolve spaces)

struct Date { 
    int y, m, d; 
};

long long toSerial(const Date& t) {
	
    long long y = t.y - 1;
    long long s = y*365 + y/4 - y/100 + y/400;

    for (int i = 1; i < t.m; ++i) {
        s += daysInMonth(t.y, i);
    }

    s += t.d;
    return s;
    
}

Date fromSerial(long long s) {
    int lo = 1, hi = 1000000;

    // Binary search the smallest year whose cumulative days >= s
    while (lo < hi) {
    	
        int mid = (lo + hi) / 2;
        long long y = mid - 1;
        long long v = y*365 + y/4 - y/100 + y/400;

        if (v < s) lo = mid + 1;
        else       hi = mid;
        
    }

    int y = lo - 1;
    long long base = (long long)(y - 1)*365 + (y - 1)/4 - (y - 1)/100 + (y - 1)/400;
    long long k = s - base;
    int m = 1;
    
    // Walk months until k fits into the current month
    while (true) {
    	
        int dim = daysInMonth(y, m);
        if (k > dim) { 
            k -= dim; 
            ++m; 
        } else {
            break;
        }
        
    }
    return Date{ y, m, (int)k };
    
}
// Convert a serial day number

int dayOfWeek(const Date& t) {
	
    static int off[] = {0,3,2,5,0,3,5,1,4,6,2,4};
    int y = t.y, m = t.m, d = t.d;

    if (m < 3) y--;

    int w = (y + y/4 - y/100 + y/400 + off[m - 1] + d) % 7;
    return w;
    
}
// Return weekday index for a given date (0=Sun,...,6=Sat) using a month offset method.

long long dateDiffDays(const Date& a, const Date& b) {
	
    long long x = toSerial(a);
    long long y = toSerial(b);
    return x > y ? x - y : y - x;
    
}
// Compute difference in days between two dates.

Date dateAddDays(const Date& a, long long n) {
	
    long long s = toSerial(a) + n;
    if (s < 1) s = 1;
    return fromSerial(s);
    
}
// Add n days to a date

int main() {
	
    printf("This program can show the weekday of a date, count days between two dates,\n");
    printf("and add or subtract days from a given date.\n\n");

    printf("a: Input one date to show its weekday.\n");
    printf("b: Input two dates to show days between them.\n");
    printf("c: Input one date and add or minus x to show the new date.\n\n");

    const char* srcMonths[13] = {
    	
        "", "January","February","March","April","May","June",
        "July","August","September","October","November","December"
        
    };

    const char* WEEKNAME[7] = {
    	
        "Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"
        
    };

    char** MONTH = (char**)malloc(13 * sizeof(char*));
    for (int i = 0; i < 13; ++i) {
    	
        size_t L = my_strlen(srcMonths[i]);
        MONTH[i] = (char*)malloc(L + 1);
        my_strcpy(MONTH[i], srcMonths[i]);
        
    }

    char line[256];

    // Main REPL loop: read mode, then process input
    while (true) {
        printf("mode a/b/c (1/2/3), q quit:\n> ");

        if (!fgets(line, sizeof(line), stdin)) { 
        
            printf("\nend\n"); 
            break; 
            
        }

        my_trim(line);

        if (line[0] == 'q' || line[0] == 'Q') {
		 
            printf("end\n"); 
            break; 
            
        }

        if (line[0] == 'a' || line[0] == 'A' || line[0] == '1') {
        	
            printf("date yyyy/mm/dd:\n> ");

            if (!fgets(line, sizeof(line), stdin)) { 
                printf("\nend\n"); 
                break; 
            }

            int y, m, d;
            int code = parseYMD(line, y, m, d);

            if (code != 0) {
                if      (code == 1) printf("format error\n");
                else if (code == 2) printf("month error\n");
                else if (code == 4) printf("not leap year\n");
                else                printf("day error\n");
                continue;
            }

            Date A{ y, m, d };
            int w = dayOfWeek(A);

            printf("%s %d, %d is %s\n", MONTH[m], A.d, A.y, WEEKNAME[w]);

        } 
		
		else if (line[0] == 'b' || line[0] == 'B' || line[0] == '2') {
			
            printf("yyyy/mm/dd - YYYY/MM/DD:\n> ");

            if (!fgets(line, sizeof(line), stdin)) { 
                printf("\nend\n"); 
                break; 
            }

            char buf[256]; 
            my_strcpy(buf, line); 
            my_trim(buf);

            int dash = -1;
            for (int i = 0; buf[i]; ++i) { 
                if (buf[i] == '-') { dash = i; break; } 
            }

            if (dash == -1) { 
                printf("format error\n"); 
                continue; 
            }

            char Ls[128] = {0}, Rs[128] = {0};

            int i = 0, j = 0;
            for (; i < dash && buf[i]; ++i) Ls[i] = buf[i];
            Ls[i] = '\0'; 
            my_trim(Ls);

            i = dash + 1; 
            while (buf[i] == ' ') ++i;

            for (; buf[i]; ++i) Rs[j++] = buf[i];
            Rs[j] = '\0'; 
            my_trim(Rs);

            int y1, m1, d1, y2, m2, d2;
            int c1 = parseYMD(Ls, y1, m1, d1);
            int c2 = parseYMD(Rs, y2, m2, d2);

            if (c1 || c2) { 
                printf("format error\n"); 
                continue; 
            }

            Date A{ y1, m1, d1 };
            Date B{ y2, m2, d2 };

            long long dif = dateDiffDays(A, B);

            long long sA = toSerial(A);
            long long sB = toSerial(B);

            Date Ld = (sA <= sB) ? A : B;
            Date Rd = (sA <= sB) ? B : A;

            printf("%lld days from %s %d, %d to %s %d, %d\n",
                   dif, MONTH[Ld.m], Ld.d, Ld.y, MONTH[Rd.m], Rd.d, Rd.y);

        } 
		
		else if (line[0] == 'c' || line[0] == 'C' || line[0] == '3') {
            printf("yyyy/mm/dd + x:\n> ");

            if (!fgets(line, sizeof(line), stdin)) { 
                printf("\nend\n"); 
                break; 
            }

            char buf[256]; 
            my_strcpy(buf, line); 
            my_trim(buf);

            int pos = -1;
            for (int i = 0; buf[i]; ++i) { 
                if (buf[i] == '+') { pos = i; break; } 
            }

            if (pos == -1) { 
                printf("format error\n"); 
                continue; 
            }

            char Ls[128] = {0}, Rs[128] = {0};

            int i = 0, j = 0;
            for (; i < pos && buf[i]; ++i) Ls[i] = buf[i];
            Ls[i] = '\0'; 
            my_trim(Ls);

            i = pos + 1; 
            while (buf[i] == ' ') ++i;

            for (; buf[i]; ++i) Rs[j++] = buf[i];
            Rs[j] = '\0'; 
            my_trim(Rs);

            int y, m, d; 
            int code = parseYMD(Ls, y, m, d);

            if (code) { 
                printf("date error\n"); 
                continue; 
            }

            long long x = 0; 
            int sign = 1; 
            int k = 0;

            if      (Rs[k] == '-') { sign = -1; ++k; }
            else if (Rs[k] == '+') {             ++k; }

            if (!my_isdigit(Rs[k])) { 
                printf("x error\n"); 
                continue; 
            }

            while (my_isdigit(Rs[k])) { 
                x = x*10 + (Rs[k] - '0'); 
                ++k; 
            }

            while (Rs[k] == ' ' || Rs[k] == '\t') ++k;

            if (Rs[k] != '\0') { 
                printf("x error\n"); 
                continue; 
            }

            x *= sign;

            Date A{ y, m, d };
            Date B = dateAddDays(A, x);

            printf("%lld days after %s %d, %d is %s %d, %d\n",
                   x, MONTH[A.m], A.d, A.y, MONTH[B.m], B.d, B.y);

        } 
		
		else {
			
            printf("unknown\n");
            
        }

        printf("----------------------------------------\n");
        
    }

    for (int i = 0; i < 13; ++i) {
        free(MONTH[i]);
    }
    free(MONTH);

    return 0;
}