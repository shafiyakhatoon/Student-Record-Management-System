#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>
#include <limits>

using namespace std;

// ============ OS-specific includes for password masking ============
#ifdef _WIN32
    #include <conio.h>     // Windows: _getch()
#else
    #include <termios.h>   // Linux/macOS: termios for turning off echo
    #include <unistd.h>
#endif

// ================== Helper: split string by delimiter ==================
vector<string> split(const string &s, char delim) {
    vector<string> tokens;
    string item;
    stringstream ss(s);
    while (getline(ss, item, delim)) tokens.push_back(item);
    return tokens;
}

// ================== Cross-platform hidden password input ==================
string getHiddenPassword() {
    string password;
    char ch;

#ifdef _WIN32
    // ---------- Windows version using _getch() ----------
    while (true) {
        ch = _getch();  // read without echo

        // Enter key ends input
        if (ch == '\r') {
            break;
        }

        // Handle Backspace
        if (ch == '\b') {
            if (!password.empty()) {
                password.pop_back();
                cout << "\b \b"; // move back, erase, move back again
            }
        } else {
            // Regular character
            password.push_back(ch);
            cout << "*";
        }
    }
    cout << endl;

#else
    // ---------- Linux / macOS version using termios ----------
    termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);      // get current terminal settings
    newt = oldt;
    newt.c_lflag &= ~(ECHO);            // disable echo
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    while ((ch = getchar()) != '\n' && ch != EOF) {
        // Handle Backspace (127 = DEL, '\b' sometimes used)
        if (ch == 127 || ch == '\b') {
            if (!password.empty()) {
                password.pop_back();
                cout << "\b \b";
            }
        } else {
            password.push_back(ch);
            cout << "*";
        }
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // restore settings
    cout << endl;
#endif

    return password;
}

// ================== Student Class ==================
class Student {
public:
    string roll, name, dept, semester, phone, email, address, password;
    float cgpa;

    Student() : cgpa(0.0f) {}

    void input() {
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Enter Roll No: "; getline(cin, roll);
        cout << "Enter Name: "; getline(cin, name);
        cout << "Enter Department: "; getline(cin, dept);
        cout << "Enter Semester: "; getline(cin, semester);
        cout << "Enter Phone: "; getline(cin, phone);
        cout << "Enter Email: "; getline(cin, email);
        cout << "Enter Address: "; getline(cin, address);
        cout << "Enter CGPA: "; cin >> cgpa;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Set Password for this student: ";
        password = getHiddenPassword();    // use hidden password here
    }

    void display() const {
        cout << "\n-------------------------------";
        cout << "\nRoll No   : " << roll;
        cout << "\nName      : " << name;
        cout << "\nDepartment: " << dept;
        cout << "\nSemester  : " << semester;
        cout << "\nPhone     : " << phone;
        cout << "\nEmail     : " << email;
        cout << "\nAddress   : " << address;
        cout << "\nCGPA      : " << cgpa;
        cout << "\n-------------------------------\n";
    }

    string serialize() const {
        stringstream ss;
        ss << roll << '|' << name << '|' << dept << '|' << semester << '|' 
           << phone << '|' << email << '|' << address << '|' << cgpa << '|' << password;
        return ss.str();
    }

    static Student deserialize(const string &line) {
        Student s;
        vector<string> p = split(line, '|');
        if (p.size() >= 9) {
            s.roll = p[0]; s.name = p[1]; s.dept = p[2]; s.semester = p[3];
            s.phone = p[4]; s.email = p[5]; s.address = p[6];
            s.cgpa = stof(p[7]); s.password = p[8];
        } else s.roll = "";
        return s;
    }
};

// ================== Ticket Class ==================
class Ticket {
public:
    int ticketID;
    string roll, category, description, status;

    Ticket() : ticketID(0) {}

    void input(const string &studentRoll, int nextID) {
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        ticketID = nextID;
        roll = studentRoll;
        cout << "Enter Category (Marks / Details / Other): ";
        getline(cin, category);
        cout << "Enter Description (Don't use '|'): ";
        getline(cin, description);
        status = "Open";
    }

    void display() const {
        cout << "\n-------------------------------";
        cout << "\nTicket ID : " << ticketID;
        cout << "\nStudent   : " << roll;
        cout << "\nCategory  : " << category;
        cout << "\nDescription:\n" << description;
        cout << "\nStatus    : " << status;
        cout << "\n-------------------------------\n";
    }

    string serialize() const {
        stringstream ss;
        ss << ticketID << '|' << roll << '|' << category << '|' << description << '|' << status;
        return ss.str();
    }

    static Ticket deserialize(const string &line) {
        Ticket t;
        vector<string> p = split(line, '|');
        if (p.size() >= 5) {
            t.ticketID = stoi(p[0]); t.roll = p[1]; t.category = p[2];
            t.description = p[3]; t.status = p[4];
        }
        return t;
    }
};

const string STUDENT_FILE = "students.txt";
const string TICKET_FILE = "tickets.txt";

// ================== File helpers ==================
vector<Student> loadAllStudents() {
    vector<Student> v;
    ifstream fin(STUDENT_FILE);
    string line;
    while (getline(fin, line)) {
        if (!line.empty()) {
            Student s = Student::deserialize(line);
            if (!s.roll.empty()) v.push_back(s);
        }
    }
    return v;
}

void saveAllStudents(const vector<Student> &v) {
    ofstream fout(STUDENT_FILE, ios::trunc);
    for (auto &s : v) fout << s.serialize() << "\n";
}

void appendStudent(const Student &s) {
    ofstream fout(STUDENT_FILE, ios::app);
    fout << s.serialize() << "\n";
}

vector<Ticket> loadAllTickets() {
    vector<Ticket> v;
    ifstream fin(TICKET_FILE);
    string line;
    while (getline(fin, line)) {
        if (!line.empty()) v.push_back(Ticket::deserialize(line));
    }
    return v;
}

void saveAllTickets(const vector<Ticket> &v) {
    ofstream fout(TICKET_FILE, ios::trunc);
    for (auto &t : v) fout << t.serialize() << "\n";
}

void appendTicket(const Ticket &t) {
    ofstream fout(TICKET_FILE, ios::app);
    fout << t.serialize() << "\n";
}

int getNextTicketID() {
    vector<Ticket> v = loadAllTickets();
    int m = 0;
    for (auto &t : v) if (t.ticketID > m) m = t.ticketID;
    return m + 1;
}

// ================== Admin side ==================
bool adminLogin() {
    string user, pass;
    cout << "\n--- Admin Login ---\nUsername: ";
    cin >> user;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Password: ";
    pass = getHiddenPassword();
    return (user == "admin" && pass == "admin123");
}

void addStudent() {
    cout << "\n--- Add Student ---\n";
    Student s;
    s.input();
    appendStudent(s);
    cout << "\nStudent added successfully!\n";
}

void viewStudents() {
    cout << "\n--- All Students ---\n";
    vector<Student> v = loadAllStudents();
    if (v.empty()) { cout << "No student records found.\n"; return; }
    for (auto &s : v) s.display();
}

int findStudentIndex(const vector<Student> &v, const string &roll) {
    for (int i = 0; i < (int)v.size(); i++)
        if (v[i].roll == roll) return i;
    return -1;
}

void searchStudent() {
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    string roll;
    cout << "\n--- Search Student ---\nEnter Roll No: ";
    getline(cin, roll);

    vector<Student> v = loadAllStudents();
    int i = findStudentIndex(v, roll);
    if (i == -1) cout << "Student not found.\n";
    else v[i].display();
}

void modifyStudent() {
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    string roll;
    cout << "\n--- Modify Student ---\nEnter Roll No: ";
    getline(cin, roll);

    vector<Student> v = loadAllStudents();
    int i = findStudentIndex(v, roll);
    if (i == -1) { cout << "Student not found.\n"; return; }

    cout << "\nCurrent details:\n";
    v[i].display();

    cout << "\nEnter new details:\n";
    Student s;
    s.input();
    v[i] = s;
    saveAllStudents(v);
    cout << "\nStudent record updated.\n";
}

void deleteStudent() {
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    string roll;
    cout << "\n--- Delete Student ---\nEnter Roll No: ";
    getline(cin, roll);

    vector<Student> v = loadAllStudents();
    int i = findStudentIndex(v, roll);
    if (i == -1) { cout << "Student not found.\n"; return; }

    cout << "\nDeleting record:\n";
    v[i].display();

    char c;
    cout << "Confirm (y/n): ";
    cin >> c;
    if (c == 'y' || c == 'Y') {
        v.erase(v.begin() + i);
        saveAllStudents(v);
        cout << "\nStudent deleted.\n";
    } else cout << "\nCancelled.\n";
}

void viewTickets() {
    vector<Ticket> v = loadAllTickets();
    cout << "\n--- All Tickets ---\n";
    if (v.empty()) { cout << "No tickets found.\n"; return; }
    for (auto &t : v) t.display();
}

void updateTicket() {
    int id;
    cout << "\n--- Update Ticket ---\nEnter Ticket ID: ";
    cin >> id;

    vector<Ticket> v = loadAllTickets();
    bool f = false;
    for (auto &t : v) {
        if (t.ticketID == id) {
            f = true;
            t.display();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Enter new status: ";
            getline(cin, t.status);
            break;
        }
    }
    if (!f) { cout << "Ticket not found.\n"; return; }
    saveAllTickets(v);
    cout << "Status updated.\n";
}

void adminMenu() {
    int c;
    do {
        cout << "\n===== ADMIN MENU =====\n"
             << "1. Add Student\n"
             << "2. View All Students\n"
             << "3. Search Student\n"
             << "4. Modify Student\n"
             << "5. Delete Student\n"
             << "6. View All Tickets\n"
             << "7. Update Ticket\n"
             << "8. Logout\n"
             << "Enter choice: ";
        cin >> c;

        switch (c) {
            case 1: addStudent(); break;
            case 2: viewStudents(); break;
            case 3: searchStudent(); break;
            case 4: modifyStudent(); break;
            case 5: deleteStudent(); break;
            case 6: viewTickets(); break;
            case 7: updateTicket(); break;
            case 8: break;
            default: cout << "Invalid choice.\n";
        }
    } while (c != 8);
}

// ================== Student side ==================
bool studentLogin(string &out) {
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    string roll, pass;
    cout << "\n--- Student Login ---\nEnter Roll No: ";
    getline(cin, roll);
    cout << "Enter Password: ";
    pass = getHiddenPassword();

    vector<Student> v = loadAllStudents();
    for (auto &s : v)
        if (s.roll == roll && s.password == pass) { out = roll; return true; }
    return false;
}

void viewMyRecord(const string &roll) {
    vector<Student> v = loadAllStudents();
    int i = findStudentIndex(v, roll);
    if (i == -1) cout << "Record not found.\n";
    else v[i].display();
}

void raiseTicket(const string &roll) {
    int id = getNextTicketID();
    Ticket t;
    t.input(roll, id);
    appendTicket(t);
    cout << "\nTicket raised successfully! ID: " << id << "\n";
}

void viewMyTickets(const string &roll) {
    vector<Ticket> v = loadAllTickets();
    bool f = false;
    cout << "\n--- My Tickets ---\n";
    for (auto &t : v)
        if (t.roll == roll) { t.display(); f = true; }
    if (!f) cout << "No tickets.\n";
}

void studentMenu(const string &roll) {
    int c;
    do {
        cout << "\n===== STUDENT MENU =====\n"
             << "1. View My Record\n"
             << "2. Raise Ticket\n"
             << "3. View My Tickets\n"
             << "4. Logout\n"
             << "Enter choice: ";
        cin >> c;

        switch (c) {
            case 1: viewMyRecord(roll); break;
            case 2: raiseTicket(roll); break;
            case 3: viewMyTickets(roll); break;
            case 4: break;
            default: cout << "Invalid choice.\n";
        }
    } while (c != 4);
}

// ================== main ==================
int main() {
    int c;
    while (true) {
        cout << "\n===== STUDENT RECORD MANAGEMENT SYSTEM =====\n"
             << "1. Admin Login\n"
             << "2. Student Login\n"
             << "3. Exit\n"
             << "Enter choice: ";
        cin >> c;

        if (c == 1) {
            if (adminLogin()) adminMenu();
            else cout << "Invalid Admin Credentials!\n";

        } else if (c == 2) {
            string roll;
            if (studentLogin(roll)) studentMenu(roll);
            else cout << "Invalid Student Credentials!\n";

        } else if (c == 3) {
            cout << "Exiting...\n";
            break;

        } else {
            cout << "Invalid choice.\n";
        }
    }
    return 0;
}