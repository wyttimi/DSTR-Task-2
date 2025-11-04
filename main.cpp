// CT077-3-2-DSTR Lab Evaluation Work #2
// Hospital Patient Care Management System Using Core Data Structures
// Build: g++ -std=c++17 -O2 -Wall -Wextra hospital.cpp -o hospital
// Run:   ./hospital

#include <iostream>
#include <iomanip>
#include <cstring>
#include <limits>
using namespace std;

// ============================== CONFIG ===============================
const int MAX_PATIENTS   = 100;   // for Role 1 (Queue)
const int MAX_SUPPLIES   = 100;   // for Role 2 (Stack)
const int MAX_EMERG      = 100;   // for Role 3 (Priority Queue - Max Heap)
const int MAX_AMBULANCES = 20;    // for Role 4 (Circular Queue)

// ============================ UTILITIES ==============================
static inline void line(char ch='-', int n=60){ for(int i=0;i<n;++i) cout<<ch; cout<<"\n"; }

// Robust, cross-platform safe getline for char buffers
static inline void safe_getline(char* buf, int cap){
    buf[0] = '\0';
    cin.getline(buf, cap);
    if(cin.fail()){
        // If the stream failed due to format issues or left-over newline, clean it up
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        buf[0] = '\0';
    }
    int len = strlen(buf);
    while(len>0 && (buf[len-1]=='\n' || buf[len-1]=='\r')){
        buf[len-1]='\0'; --len;
    }
}

// ============================ ROLE 1 =================================
// Patient Admission Clerk — FIFO Queue (array-based, head/tail indices)
struct Patient{
    char id[16];        // CHANGED: support alphanumeric IDs like "P01"
    char name[50];
    char condition[30];
};

struct PatientQueue{
    Patient data[MAX_PATIENTS];
    int head=0, tail=0, count=0; // circular queue

    bool isFull()  const { return count==MAX_PATIENTS; }
    bool isEmpty() const { return count==0; }

    bool enqueue(const Patient& p){
        if(isFull()) return false;
        // simple validation: require non-empty id & name
        if(p.id[0]=='\0' || p.name[0]=='\0') return false;
        data[tail]=p; tail = (tail+1)%MAX_PATIENTS; ++count; return true;
    }
    bool dequeue(Patient& out){
        if(isEmpty()) return false;
        out = data[head]; head = (head+1)%MAX_PATIENTS; --count; return true;
    }
    void print() const{
        if(isEmpty()){ cout<<"No patients waiting.\n"; return; }
        cout<<left<<setw(12)<<"ID"<<setw(22)<<"Name"<<"Condition"<<"\n"; line();
        for(int i=0;i<count;++i){
            int idx = (head+i)%MAX_PATIENTS;
            cout<<left<<setw(12)<<data[idx].id<<setw(22)<<data[idx].name<<data[idx].condition<<"\n";
        }
        cout<<"Total waiting: "<<count<<"\n";
    }
} gPatients;

void ui_admit_patient(){
    if(gPatients.isFull()){ cout<<"Patient queue is full.\n"; return; }
    Patient p{};
    cout<<"Enter Patient ID (e.g., P01): "; safe_getline(p.id, 16);
    cout<<"Enter Patient Name: ";          safe_getline(p.name, 50);
    cout<<"Enter Condition Type (e.g., Flu/Checkup): "; safe_getline(p.condition, 30);
    if(gPatients.enqueue(p)) cout<<"Admitted to queue.\n";
    else cout<<"Failed to admit (ensure ID/Name not empty and queue not full).\n";
}
void ui_discharge_patient(){
    Patient p{}; if(gPatients.dequeue(p)){
        cout<<"Discharged earliest admitted patient: ["<<p.id<<"] "<<p.name<<" ("<<p.condition<<")\n";
    } else {
        cout<<"No patients to discharge.\n";
    }
}

// ============================ ROLE 2 =================================
// Medical Supply Manager — LIFO Stack (array-based top index)
struct Supply{
    char type[30];
    int  quantity;
    char batch[20];
};

struct SupplyStack{
    Supply data[MAX_SUPPLIES];
    int top=-1; // -1 means empty

    bool isFull()  const { return top==MAX_SUPPLIES-1; }
    bool isEmpty() const { return top==-1; }

    bool push(const Supply& s){ if(isFull()) return false; data[++top]=s; return true; }
    bool pop(Supply& out){ if(isEmpty()) return false; out=data[top--]; return true; }
    void print() const{
        if(isEmpty()){ cout<<"No supplies available.\n"; return; }
        cout<<left<<setw(16)<<"Type"<<setw(10)<<"Qty"<<"Batch"<<"\n"; line();
        for(int i=top;i>=0;--i){
            cout<<left<<setw(16)<<data[i].type<<setw(10)<<data[i].quantity<<data[i].batch<<"\n";
        }
        cout<<"Total batches: "<<(top+1)<<"\n";
    }
} gSupplies;

void ui_add_supply(){
    if(gSupplies.isFull()){ cout<<"Supply store is full.\n"; return; }
    Supply s{}; cout<<"Enter Supply Type: "; safe_getline(s.type, 30);
    cout<<"Enter Quantity: ";
    while(!(cin>>s.quantity)){ cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); cout<<"Enter Quantity (number): "; }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout<<"Enter Batch: "; safe_getline(s.batch, 20);
    if(gSupplies.push(s)) cout<<"Recorded (stack top).\n"; else cout<<"Failed to add supply.\n";
}
void ui_use_last_supply(){
    Supply s{}; if(gSupplies.pop(s)){
        cout<<"Using LAST ADDED supply: "<<s.type<<" x"<<s.quantity<<" (Batch: "<<s.batch<<")\n";
    } else {
        cout<<"No supplies to use.\n";
    }
}

// ============================ ROLE 3 =================================
// Emergency Department Officer — Priority Queue by severity (max-heap)
struct EmergencyCase{
    char patient[50];
    char type[40];
    int priority; // higher = more critical
};

struct EmergencyMaxHeap{
    EmergencyCase data[MAX_EMERG+1]; // 1-based indexing for simpler heap math
    int sz=0;

    bool isFull()  const { return sz==MAX_EMERG; }
    bool isEmpty() const { return sz==0; }

    static void swapEC(EmergencyCase &a, EmergencyCase &b){ EmergencyCase t=a; a=b; b=t; }

    void push(const EmergencyCase& e){
        if(isFull()){ cout<<"Emergency queue is full.\n"; return; }
        data[++sz]=e; // sift up
        int i=sz;
        while(i>1 && data[i].priority>data[i/2].priority){ swapEC(data[i], data[i/2]); i/=2; }
    }
    EmergencyCase top(){ return data[1]; }
    void pop(){
        if(isEmpty()) return; data[1]=data[sz--]; int i=1;
        while(true){
            int l=2*i, r=2*i+1, largest=i;
            if(l<=sz && data[l].priority>data[largest].priority) largest=l;
            if(r<=sz && data[r].priority>data[largest].priority) largest=r;
            if(largest==i) break; swapEC(data[i], data[largest]); i=largest;
        }
    }
    void print() const{
        if(isEmpty()){ cout<<"No emergency cases pending.\n"; return; }
        cout<<left<<setw(22)<<"Patient"<<setw(18)<<"Emergency"<<"Priority"<<"\n"; line();
        // show in heap array order (levels)
        for(int i=1;i<=sz;++i){
            cout<<left<<setw(22)<<data[i].patient<<setw(18)<<data[i].type<<data[i].priority<<"\n";
        }
        cout<<"(Processing always serves highest priority first.)\n";
    }
} gEmerg;

void ui_log_emergency(){
    if(gEmerg.isFull()){ cout<<"Emergency list full.\n"; return; }
    EmergencyCase e{}; cout<<"Patient Name: "; safe_getline(e.patient, 50);
    cout<<"Type of Emergency: "; safe_getline(e.type, 40);
    cout<<"Priority Level (1-10, higher is more critical): ";
    while(!(cin>>e.priority)){ cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); cout<<"Enter priority (number): "; }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    if(e.priority<0) e.priority=0; if(e.priority>1000) e.priority=1000;
    gEmerg.push(e); cout<<"Emergency logged.\n";
}
void ui_process_most_critical(){
    if(gEmerg.isEmpty()){ cout<<"No emergencies in queue.\n"; return; }
    EmergencyCase t=gEmerg.top(); gEmerg.pop();
    cout<<"ATTEND MOST CRITICAL => "<<t.patient<<" ("<<t.type<<") with priority "<<t.priority<<"\n";
}

// ============================ ROLE 4 =================================
// Ambulance Dispatcher — Circular Queue (rotation scheduling)
struct Ambulance{ char plate[16]; };

struct AmbulanceCQueue{
    Ambulance data[MAX_AMBULANCES];
    int head=0, tail=0, count=0; // circular queue

    bool isFull()  const { return count==MAX_AMBULANCES; }
    bool isEmpty() const { return count==0; }

    bool enqueue(const Ambulance& a){ if(isFull()) return false; data[tail]=a; tail=(tail+1)%MAX_AMBULANCES; ++count; return true; }
    bool dequeue(Ambulance& out){ if(isEmpty()) return false; out=data[head]; head=(head+1)%MAX_AMBULANCES; --count; return true; }
    void rotateOnce(){ if(count<=1) return; Ambulance first{}; dequeue(first); enqueue(first); }
    void print() const{
        if(isEmpty()){ cout<<"No ambulances registered.\n"; return; }
        cout<<"Rotation Order (head -> tail):\n"; line();
        for(int i=0;i<count;++i){ int idx=(head+i)%MAX_AMBULANCES; cout<<i+1<<". "<<data[idx].plate<<"\n"; }
        cout<<"Total ambulances: "<<count<<"\n";
    }
} gAmb;

void ui_register_ambulance(){
    if(gAmb.isFull()){ cout<<"Ambulance roster full.\n"; return; }
    Ambulance a{}; cout<<"Enter Ambulance Plate/ID: "; safe_getline(a.plate, 16);
    if(gAmb.enqueue(a)) cout<<"Ambulance added to active-duty list.\n"; else cout<<"Failed to register.\n";
}
void ui_rotate_shift(){
    if(gAmb.isEmpty()){ cout<<"No ambulances to rotate.\n"; return; }
    gAmb.rotateOnce(); cout<<"Shift rotated. Next up is now at head.\n";
}

// ============================ MENUS ==================================
void menu_patients(){
    while(true){
        line('='); cout<<"PATIENT ADMISSION CLERK (Queue)\n"; line('=');
        cout<<"1) Admit Patient\n2) Discharge Patient (earliest)\n3) View Patient Queue\n0) Back\n> ";
        int ch; if(!(cin>>ch)){ cin.clear(); cin.ignore(numeric_limits<streamsize>::max(),'\n'); continue; } cin.ignore(numeric_limits<streamsize>::max(),'\n');
        if(ch==0) return; else if(ch==1) ui_admit_patient();
        else if(ch==2) ui_discharge_patient();
        else if(ch==3) gPatients.print();
        else cout<<"Invalid.\n";
    }
}

void menu_supplies(){
    while(true){
        line('='); cout<<"MEDICAL SUPPLY MANAGER (Stack)\n"; line('=');
        cout<<"1) Add Supply Stock (push)\n2) Use 'Last Added' Supply (pop)\n3) View Current Supplies\n0) Back\n> ";
        int ch; if(!(cin>>ch)){ cin.clear(); cin.ignore(numeric_limits<streamsize>::max(),'\n'); continue; } cin.ignore(numeric_limits<streamsize>::max(),'\n');
        if(ch==0) return; else if(ch==1) ui_add_supply();
        else if(ch==2) ui_use_last_supply();
        else if(ch==3) gSupplies.print();
        else cout<<"Invalid.\n";
    }
}

void menu_emergency(){
    while(true){
        line('='); cout<<"EMERGENCY DEPT OFFICER (Priority Queue - Max Heap)\n"; line('=');
        cout<<"1) Log Emergency Case (push)\n2) Process Most Critical Case (pop-max)\n3) View Pending Emergency Cases\n0) Back\n> ";
        int ch; if(!(cin>>ch)){ cin.clear(); cin.ignore(numeric_limits<streamsize>::max(),'\n'); continue; } cin.ignore(numeric_limits<streamsize>::max(),'\n');
        if(ch==0) return; else if(ch==1) ui_log_emergency();
        else if(ch==2) ui_process_most_critical();
        else if(ch==3) gEmerg.print();
        else cout<<"Invalid.\n";
    }
}

void menu_ambulance(){
    while(true){
        line('='); cout<<"AMBULANCE DISPATCHER (Circular Queue)\n"; line('=');
        cout<<"1) Register Ambulance (enqueue)\n2) Rotate Ambulance Shift\n3) Display Ambulance Schedule\n0) Back\n> ";
        int ch; if(!(cin>>ch)){ cin.clear(); cin.ignore(numeric_limits<streamsize>::max(),'\n'); continue; } cin.ignore(numeric_limits<streamsize>::max(),'\n');
        if(ch==0) return; else if(ch==1) ui_register_ambulance();
        else if(ch==2) ui_rotate_shift();
        else if(ch==3) gAmb.print();
        else cout<<"Invalid.\n";
    }
}

int main(){
    ios::sync_with_stdio(false); cin.tie(nullptr);

    while(true){
        line('='); cout<<"HOSPITAL PATIENT CARE MANAGEMENT SYSTEM\n"; line('=');
        cout<<"1) Patient Admission Clerk (Queue)\n";
        cout<<"2) Medical Supply Manager (Stack)\n";
        cout<<"3) Emergency Dept Officer (Priority Queue)\n";
        cout<<"4) Ambulance Dispatcher (Circular Queue)\n";
        cout<<"0) Exit\n> ";
        int ch; if(!(cin>>ch)){ cin.clear(); cin.ignore(numeric_limits<streamsize>::max(),'\n'); continue; } cin.ignore(numeric_limits<streamsize>::max(),'\n');
        if(ch==0){ cout<<"Goodbye!\n"; break; }
        switch(ch){
            case 1: menu_patients(); break;
            case 2: menu_supplies(); break;
            case 3: menu_emergency(); break;
            case 4: menu_ambulance(); break;
            default: cout<<"Invalid choice.\n"; break;
        }
    }
    return 0;
}
