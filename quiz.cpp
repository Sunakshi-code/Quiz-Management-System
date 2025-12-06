#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <sstream>
#include <cctype>
#include <limits>
#include <chrono>
#include <thread>
#include <conio.h>  // Windows only
#include <random>
using namespace std;

// ===================== STRUCTS =====================
struct User {
    string id;
    string username;
    string password;
    int score = 0;
};

struct Question {
    int id;
    string text;
    string optA, optB, optC, optD;
    char correct;
};

// ===================== UTILITY =====================
vector<User> loadUsers() {
    vector<User> users;
    ifstream in("users.txt");
    if (!in) return users;

    User u;
    while (in >> u.id >> u.username >> u.password >> u.score) {
        users.push_back(u);
    }
    return users;
}

bool saveUsers(const vector<User>& users) {
    ofstream out("users.txt", ios::trunc);
    if (!out) return false;

    for (auto &u : users) {
        out << u.id << " " << u.username << " " << u.password << " " << u.score << "\n";
    }
    return true;
}

vector<Question> loadQuestions() {
    vector<Question> q;
    ifstream file("questions.txt");
    if (!file) return q;

    Question temp;
    while (file >> temp.id) {
        file.ignore(numeric_limits<streamsize>::max(), '\n');
        getline(file, temp.text);
        getline(file, temp.optA);
        getline(file, temp.optB);
        getline(file, temp.optC);
        getline(file, temp.optD);
        file >> temp.correct;
        file.ignore(numeric_limits<streamsize>::max(), '\n');
        q.push_back(temp);
    }
    return q;
}

bool saveQuestions(const vector<Question>& q) {
    ofstream file("questions.txt", ios::trunc);
    if (!file) return false;

    for (auto &x : q) {
        file << x.id << "\n" << x.text << "\n" << x.optA << "\n"
             << x.optB << "\n" << x.optC << "\n" << x.optD << "\n"
             << x.correct << "\n";
    }
    return true;
}

vector<string> wrapText(const string& text, int width) {
    vector<string> lines;
    string word, line;
    stringstream ss(text);

    while (ss >> word) {
        if (line.empty()) line = word;
        else if ((int)line.length() + 1 + (int)word.length() > width) {
            lines.push_back(line);
            line = word;
        } else {
            line += " " + word;
        }
    }

    if (!line.empty()) lines.push_back(line);
    return lines;
}

// ===================== ADMIN MODULE =====================
void editQuestion();

void viewQuestions() {
    while (true) {
        vector<Question> q = loadQuestions();
        const int W_ID = 5, W_Q = 35, W_OPT = 22;

        cout << "\n===== QUESTION LIST =====\n";
        cout << "\n" << left << setw(W_ID) << "ID"
             << setw(W_Q) << "Question"
             << setw(W_OPT) << "Option A"
             << setw(W_OPT) << "Option B"
             << setw(W_OPT) << "Option C"
             << setw(W_OPT) << "Option D"
             << "Correct\n";

        cout << string(W_ID + W_Q + W_OPT*4 + 7, '=') << "\n\n";

        for (auto &x : q) {
            auto qw = wrapText(x.text, W_Q);
            auto aw = wrapText(x.optA, W_OPT);
            auto bw = wrapText(x.optB, W_OPT);
            auto cw = wrapText(x.optC, W_OPT);
            auto dw = wrapText(x.optD, W_OPT);

            int rows = max({(int)qw.size(), (int)aw.size(), (int)bw.size(),
                            (int)cw.size(), (int)dw.size()});

            for (int i = 0; i < rows; i++) {
                if (i == 0) cout << setw(W_ID) << x.id;
                else cout << setw(W_ID) << "";

                cout << setw(W_Q) << (i < qw.size() ? qw[i] : "")
                     << setw(W_OPT) << (i < aw.size() ? aw[i] : "")
                     << setw(W_OPT) << (i < bw.size() ? bw[i] : "")
                     << setw(W_OPT) << (i < cw.size() ? cw[i] : "")
                     << setw(W_OPT) << (i < dw.size() ? dw[i] : "");

                if (i == 0) cout << x.correct;
                cout << "\n";
            }
            cout << "\n";
        }

        cout << "\n===== ACTIONS =====\n";
        cout << "\n1. Edit Question\n2. Delete Question\n3. Back\nEnter choice: ";
        int ch;
        cin >> ch;

        if (ch == 1) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            editQuestion();
        }
        else if (ch == 2) {
            int id;
            cout << "Enter ID to delete: ";
            cin >> id;

            vector<Question> newList;
            for (auto &x : q)
                if (x.id != id) newList.push_back(x);

            saveQuestions(newList);
            cout << "Deleted Successfully!\n";
        }
        else return;
    }
}

void editQuestion() {
    vector<Question> q = loadQuestions();
    if (q.empty()) { cout << "No questions to edit.\n"; return; }

    int id;
    cout << "\nEnter Question ID to edit: ";
    cin >> id;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    for (auto &x : q) {
        if (x.id == id) {
            string input;

            cout << "Current Question: " << x.text << "\nNew Question (Press enter to leave same): ";
            getline(cin, input);
            if (!input.empty()) x.text = input;

            cout << "Option A (" << x.optA << "): ";
            getline(cin, input);
            if (!input.empty()) x.optA = input;

            cout << "Option B (" << x.optB << "): ";
            getline(cin, input);
            if (!input.empty()) x.optB = input;

            cout << "Option C (" << x.optC << "): ";
            getline(cin, input);
            if (!input.empty()) x.optC = input;

            cout << "Option D (" << x.optD << "): ";
            getline(cin, input);
            if (!input.empty()) x.optD = input;

            cout << "Correct Answer (" << x.correct << "): ";
            getline(cin, input);
            if (!input.empty()) {
                char c = toupper(input[0]);
                if (c=='A'||c=='B'||c=='C'||c=='D') x.correct = c;
            }

            saveQuestions(q);
            cout << "✔ Question Updated Successfully!\n";
            return;
        }
    }

    cout << "❌ ID not found.\n";
}

void addQuestion() {
    Question q;
    cout << "\n===== ADD NEW QUESTION =====\n";
    cout << "Enter Question ID: ";
    cin >> q.id;
    cin.ignore();

    cout << "Enter Question: ";
    getline(cin, q.text);

    cout << "Option A: "; getline(cin, q.optA);
    cout << "Option B: "; getline(cin, q.optB);
    cout << "Option C: "; getline(cin, q.optC);
    cout << "Option D: "; getline(cin, q.optD);

    cout << "Correct Answer (A-D): ";
    cin >> q.correct;
    q.correct = toupper(q.correct);

    ofstream f("questions.txt", ios::app);
    f << q.id << "\n" << q.text << "\n" << q.optA << "\n"
      << q.optB << "\n" << q.optC << "\n" << q.optD << "\n"
      << q.correct << "\n";

    cout << "✔ Question Added!\n";
}

void viewUsers() {
    auto users = loadUsers();

    cout << "\n===== REGISTERED USERS =====\n";
    cout << left << setw(10) << "ID"
         << setw(20) << "Username"
         << setw(20) << "Password"
         << "Score\n";

    cout << string(10 + 20 + 20 + 5, '=') << "\n";

    for (auto &u : users) {
        cout << setw(10) << u.id
             << setw(20) << u.username
             << setw(20) << u.password
             << u.score << "\n";
    }
}

void deleteUserAdmin() {
    auto users = loadUsers();
    if (users.empty()) {
        cout << "No users available to delete.\n";
        return;
    }

    string id;
    cout << "Enter User ID to delete: ";
    cin >> id;

    bool found = false;
    vector<User> newList;

    for (auto &u : users) {
        if (u.id == id) {
            found = true;
            continue;
        }
        newList.push_back(u);
    }

    if (!found) {
        cout << "❌ User ID not found.\n";
        return;
    }

    saveUsers(newList);
    cout << "✔ User Deleted Successfully!\n";
}

bool adminLogin() {
    const string AUSER="admin", APASS="admin123";

    string u,p;
    cout << "\n===== ADMIN LOGIN =====\n";
    cout << "\nAdmin Username: ";
    cin >> u;
    cout << "Admin Password: ";
    cin >> p;

    if (u==AUSER && p==APASS) {
        cout << "✔ Admin Login Successful!\n";
        return true;
    }
    cout << "❌ Invalid Credentials.\n";
    return false;
}

void adminDashboard() {
    while (true) {
        cout << "\n===== ADMIN DASHBOARD =====\n";
        cout << "\n1. View Questions\n";
        cout << "2. Add Question\n";
        cout << "3. View Users\n";
        cout << "4. Delete User\n";
        cout << "5. Logout\n";
        cout << "Enter choice: ";

        int c;
        cin >> c;

        if (c==1) viewQuestions();
        else if (c==2) { cin.ignore(); addQuestion(); }
        else if (c==3) viewUsers();
        else if (c==4) deleteUserAdmin();
        else if (c==5) return;
        else cout << "Invalid.\n";
    }
}

// ===================== USER MODULE =====================
void registerUser() {
    string id, username, password;

    auto users = loadUsers();  // Load existing registered users

    cout << "\n===== USER REGISTRATION =====\n";
    cout << "\nEnter User ID: ";
    cin >> id;

    cout << "Enter Username: ";
    cin >> username;

    cout << "Enter Password: ";
    cin >> password;

    // ===== CHECK WITH EXISTING USERS =====
    for (auto &u : users) {

        if (u.id == id) {
            cout << "❌ Registration Failed! User ID already exists.\n";
            return;
        }

        if (u.username == username) {
            cout << "❌ Registration Failed! Username already exists.\n";
            return;
        }

        if (u.password == password) {
            cout << "❌ Registration Failed! This password is already used by another user.\n";
            return;
        }
    }

    // ===== SAVE NEW USER =====
    ofstream file("users.txt", ios::app);
    file << id << " " << username << " " << password << " " << 0 << "\n";

    cout << "\n✔ User registered successfully!\n";
}

bool loginUser(User &user) {
    string id,name,pass;

    cout << "\n===== USER LOGIN =====\n";
    cout << "\nEnter ID: ";
    cin >> id;
    cout << "Enter Username: ";
    cin >> name;
    cout << "Enter Password: ";
    cin >> pass;

    auto users = loadUsers();
    for (auto &u : users) {
        if (u.id==id && u.username==name && u.password==pass) {
            user = u;
            cout << "✔ Login Successful!\n";
            return true;
        }
    }
    cout << "❌ Invalid Credentials.\n";
    return false;
}

// ===================== QUIZ WITH TIMER =====================
void takeQuiz(User &user) {
    auto questions = loadQuestions();
    if (questions.empty()) {
        cout << "No questions available!\n";
        return;
    }

    // 🔀 Shuffle only for quiz (admin order stays same)
    shuffle(questions.begin(), questions.end(), std::mt19937(std::random_device{}()));

    int score = 0;
    const int TIME_LIMIT = 20;

    cout << "\n===== QUIZ START =====\n";
    cout << "You have " << TIME_LIMIT << " seconds per question.\n";
    cout << "Press 'S' to skip a question.\n";

    // Temporary struct for shuffled options
    struct Option {
        char label;       // 🔧 FIX ADDED
        string text;
        bool isCorrect;
    };

    for (auto &q : questions) {

        // === BUILD OPTION LIST ===
       // === BUILD OPTION LIST ===
// === BUILD OPTION LIST ===
      vector<Option> opts = {
       {'A', q.optA, q.correct == 'A'},
       {'B', q.optB, q.correct == 'B'},
       {'C', q.optC, q.correct == 'C'},
       {'D', q.optD, q.correct == 'D'}
   };

// === SHUFFLE OPTIONS ===
     shuffle(opts.begin(), opts.end(), std::mt19937(std::random_device{}()));

// === PRINT QUESTION ===
      static int qNo = 1;
       cout << "\nQ" << qNo++ << ": " << q.text << "\n";

// === NEW LABELS AFTER SHUFFLE ===
       char newLabels[] = {'A', 'B', 'C', 'D'};
       char newCorrect = 'A';

// === PRINT SHUFFLED OPTIONS WITH NEW LABELS ===
      for (int i = 0; i < 4; i++) {
       if (opts[i].isCorrect)
        newCorrect = newLabels[i];
        cout << newLabels[i] << ". " << opts[i].text << "\n";
}

        char ans = ' ';
        bool answered = false;
        bool skipped = false;

        auto start = chrono::steady_clock::now();

        while (!answered && !skipped) {
            auto now = chrono::steady_clock::now();
            int elapsed = chrono::duration_cast<chrono::seconds>(now - start).count();
            int remaining = TIME_LIMIT - elapsed;

            if (remaining <= 0) break;

            if (remaining <= 5) cout << "\033[31m";  // Red color
            else cout << "\033[0m";

            cout << "\rTime Left: " << remaining << "s " << flush;

            if (_kbhit()) {
                ans = _getch();
                ans = toupper(ans);

                if (ans == 'S') {
                    skipped = true;
                    break;
                }

                if (ans >= 'A' && ans <= 'D') {
                    answered = true;
                }
            }

            this_thread::sleep_for(chrono::milliseconds(100));
        }

        cout << "\033[0m\r";

        if (skipped) {
            cout << "⏭ Question skipped.\n";
            continue;
        }

        if (!answered) {
            cout << "⏰ Time's up! Correct Answer: " << newCorrect << "\n";
            continue;
        }

        cout << "Your Answer: " << ans << "\n";

        if (ans == newCorrect) {
            cout << "✔ Correct!\n";
            score++;
        } else {
            cout << "❌ Wrong! Correct Answer: " << newCorrect << "\n";
        }
    }

    cout << "\n===== QUIZ END =====\n";
    cout << "You scored " << score << " out of " << questions.size() << "\n";

    // ===== RESULT FEEDBACK =====
    double percent = (double)score / questions.size() * 100;

    cout << "\n===== RESULT FEEDBACK =====\n";
    if (percent >= 90) cout << "🎉 Excellent Performance! You're a quiz master!\n";
    else if (percent >= 70) cout << "👍 Great job! Keep up the good work!\n";
    else if (percent >= 50) cout << "🙂 Good effort! You can improve even more.\n";
    else if (percent >= 30) cout << "😐 Not bad. Practice will make you better.\n";
    else cout << "🙁 Don't give up! Try again.\n";

    // ===== UPDATE USER SCORE =====
    auto users = loadUsers();
    for (auto &u : users) {
        if (u.id == user.id) {
            u.score = score;
            user.score = score;
            break;
        }
    }

    saveUsers(users);
}


void userPanel(User &user) {
    while (true) {
        cout << "\n===== USER DASHBOARD =====\n";
        cout << "\n1. View Profile\n";
        cout << "2. Take Quiz\n";
        cout << "3. Logout\n";
        cout << "Enter choice: ";
        int ch;
        cin >> ch;

        if (ch == 1) {
            cout << "\nID: " << user.id
                 << "\nUsername: " << user.username
                 << "\nScore: " << user.score << "\n";
        } else if (ch == 2) {
            takeQuiz(user);
        } else if (ch == 3) return;
        else cout << "Invalid.\n";
    }
}

void forgotPassword() {
    string id,name;

    cout << "\n===== FORGOT PASSWORD =====\n";
    cout << "\nEnter ID: ";
    cin >> id;
    cout << "Enter Username: ";
    cin >> name;

    auto users = loadUsers();
    for (auto &u : users) {
        if (u.id==id && u.username==name) {
            cout << "Your Password: " << u.password << "\n";
            char c;
            cout << "Change? (y/n): ";
            cin >> c;
            if (c=='y'||c=='Y') {
                cout << "New Password: ";
                cin >> u.password;
                saveUsers(users);
                cout << "✔ Updated.\n";
            }
            return;
        }
    }
    cout << "❌ User not found.\n";
}

// ===================== MAIN =====================
int main() {
    while (true) {
        cout << "\n===== ONLINE QUIZ SYSTEM =====\n";
        cout << "\n1. Register User\n";
        cout << "2. User Login\n";
        cout << "3. Forgot Password\n";
        cout << "4. Admin Login\n";
        cout << "5. Exit\n";
        cout << "Enter choice: ";

        int c;
        cin >> c;

        switch (c) {
            case 1:
                registerUser();
                break;

            case 2: {
                User currentUser;
                if (loginUser(currentUser))
                    userPanel(currentUser);
                break;
            }

            case 3:
                forgotPassword();
                break;

            case 4:
                if (adminLogin())
                    adminDashboard();
                break;

            case 5:
                cout << "\nExiting program...\n";
                return 0;

            default:
                cout << "Invalid choice.\n";
        }
    }

    return 0;
}
