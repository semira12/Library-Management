#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <limits>
#include <vector> 

using namespace std;

struct Book {
    int id;
    string title;
    string author;
    int edition;
    int year;
    string status;      
    string borrower;
    int dueDay, dueMonth, dueYear; 
    queue<string> waitlist; 
    Book* next;
};

Book* head = nullptr;
const string FILE_NAME = "library_data_final.txt";

bool isAlphaOnly(const string& s) {
    if (s.empty()) return false;
    for (char c : s) {
        if (!isalpha(c) && c != ' ') return false;
    }
    return true;
}

int getValidInt(string prompt) {
    int value;
    while (true) {
        cout << prompt;
        cin >> value;
        if (cin.fail()) {
            cin.clear(); 
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
            cout << "Invalid input! Please enter a number.\n";
        } else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
    }
}

string getValidAlphaString(string prompt) {
    string value;
    while (true) {
        cout << prompt;
        getline(cin, value);
        if (isAlphaOnly(value)) {
            return value;
        } else {
            cout << "Invalid input! Text only (no numbers allowed).\n";
        }
    }
}

string getValidText(string prompt) {
    string value;
    while (true) {
        cout << prompt;
        getline(cin, value);
        if (value.empty() || value == " ") {
            cout << "Input cannot be empty.\n";
        } else {
            return value;
        }
    }
}

bool isIdUnique(int id) {
    Book* temp = head;
    while (temp) {
        if (temp->id == id) return false;
        temp = temp->next;
    }
    return true;
}

bool hasBorrowedBook(string studentName) {
    Book* temp = head;
    while (temp) {
        if (temp->status == "Issued" && temp->borrower == studentName) {
            return true;
        }
        temp = temp->next;
    }
    return false;
}

long toTotalDays(int d, int m, int y) {
    return (y * 365) + (m * 30) + d;
}

int getDaysInMonth(int m, int y) {
    if (m == 2) return (y % 4 == 0) ? 29 : 28;
    if (m == 4 || m == 6 || m == 9 || m == 11) return 30;
    return 31;
}

void saveToFile() {
    ofstream file(FILE_NAME);
    if (!file) return;
    
    Book* temp = head;
    while (temp) {
        file << temp->id << endl;
        file << temp->title << endl;
        file << temp->author << endl;
        file << temp->edition << endl;
        file << temp->year << endl;
        file << temp->status << endl;
        file << temp->borrower << endl;
        file << temp->dueDay << " " << temp->dueMonth << " " << temp->dueYear << endl;
        
        queue<string> qCopy = temp->waitlist;
        file << qCopy.size() << endl;
        while(!qCopy.empty()){
            file << qCopy.front() << endl;
            qCopy.pop();
        }

        temp = temp->next;
    }
    file.close();
}

void loadFromFile() {
    ifstream file(FILE_NAME);
    if (!file) return;

    while (file.peek() != EOF) {
        Book* newBook = new Book();
        file >> newBook->id;
        file.ignore(); 
        
        getline(file, newBook->title);
        if (newBook->title.empty()) { delete newBook; break; }

        getline(file, newBook->author);
        file >> newBook->edition;
        file >> newBook->year;
        file.ignore();
        
        getline(file, newBook->status);
        getline(file, newBook->borrower);
        file >> newBook->dueDay >> newBook->dueMonth >> newBook->dueYear;
        file.ignore();

        int qSize;
        file >> qSize;
        file.ignore();
        for(int i=0; i<qSize; i++) {
            string waiter;
            getline(file, waiter);
            newBook->waitlist.push(waiter);
        }

        newBook->next = head;
        head = newBook;
    }
    file.close();
}

void addBook() {
    Book* newBook = new Book();
    cout << "\n--- Add New Book ---\n";

    while (true) {
        newBook->id = getValidInt("Enter Book ID (4 digits): ");
        if (newBook->id < 1000 || newBook->id > 9999) {
            cout << " ID must be 4 digits.\n";
        } else if (!isIdUnique(newBook->id)) {
            cout << " ID already exists.\n";
        } else {
            break;
        }
    }

    while(true) {
        newBook->title = getValidText("Enter Title: ");
        newBook->author = getValidAlphaString("Enter Author (Text only): "); 
        
        while(true) {
            newBook->edition = getValidInt("Enter Edition: ");
            if (newBook->edition < 1) {
                cout << " Edition must be 1 or higher.\n";
            } else {
                break;
            }
        }

        bool distinct = true;
        Book* temp = head;
        while(temp) {
            if(temp->title == newBook->title && temp->author == newBook->author && temp->edition == newBook->edition) {
                cout << " Error: This exact book (Title+Author+Edition) already exists.\n";
                distinct = false;
                break;
            }
            temp = temp->next;
        }
        if(distinct) break;
    }

    while(true) {
        newBook->year = getValidInt("Enter Year: ");
        if(newBook->year < 1800 || newBook->year > 2025) cout << " Invalid Year.\n";
        else break;
    }

    newBook->status = "Available";
    newBook->borrower = "None";
    newBook->dueDay = 0; newBook->dueMonth = 0; newBook->dueYear = 0;

    newBook->next = head;
    head = newBook;

    saveToFile();
    cout << " Book added successfully!\n";
}

void searchAndBorrow() {
    if (!head) { cout << "Library is empty.\n"; return; }

    string criteria = getValidText("Enter Title or Author to search: ");
    
    vector<Book*> matches;
    Book* temp = head;
    while (temp) {
        if (temp->title.find(criteria) != string::npos || temp->author.find(criteria) != string::npos) {
            matches.push_back(temp);
        }
        temp = temp->next;
    }

    if (matches.empty()) {
        cout << " No books found matching that criteria.\n";
        return;
    }

    cout << "\n Found " << matches.size() << " match(es):\n";
    for (Book* b : matches) {
        cout << "ID: " << b->id << " | Title: " << b->title 
             << " | Ed: " << b->edition << " | Status: " << b->status << endl;
    }

    int targetID = getValidInt("\nEnter ID of the book you want to borrow (0 to cancel): ");
    if (targetID == 0) return;

    Book* selectedBook = nullptr;
    for(Book* b : matches) {
        if(b->id == targetID) {
            selectedBook = b;
            break;
        }
    }

    if (!selectedBook) {
        cout << " ID not in the search results.\n";
        return;
    }

    if (selectedBook->status == "Issued") {
        cout << " Book is already issued.\n";
        cout << "Do you want to join the waitlist for THIS book? (1=Yes, 0=No): ";
        int choice = getValidInt("");
        if (choice == 1) {
            string sName = getValidAlphaString("Enter your Name (Text only): ");
            selectedBook->waitlist.push(sName);
            saveToFile();
            cout << " Added to waitlist. You are position " << selectedBook->waitlist.size() << ".\n";
        }
        return;
    }

    string studentName = getValidAlphaString("Enter Student Name (Text only): ");
    if (hasBorrowedBook(studentName)) {
        cout << "ALERT: " << studentName << " already has a borrowed book. Please return it first.\n";
        return;
    }

    cout << "--- Enter Borrow Date (Today's Date) ---\n";
    int bD = getValidInt("Day: ");
    int bM = getValidInt("Month: ");
    int bY = getValidInt("Year: ");

    int dD = bD + 7;
    int dM = bM;
    int dY = bY;
    
    int maxDays = getDaysInMonth(bM, bY);
    
    if (dD > maxDays) {
        dD = dD - maxDays;
        dM++;
        if (dM > 12) {
            dM = 1;
            dY++;
        }
    }

    selectedBook->status = "Issued";
    selectedBook->borrower = studentName;
    selectedBook->dueDay = dD;
    selectedBook->dueMonth = dM;
    selectedBook->dueYear = dY;
    
    saveToFile();
    cout << "Book Issued to " << studentName << " successfully!\n";
    cout << "DUE DATE calculated: " << dD << "/" << dM << "/" << dY << " (7 days from borrow date)\n";
}

void returnBook() {
    if (!head) { cout << "Library is empty.\n"; return; }

    int id = getValidInt("Enter Book ID to return: ");
    
    Book* temp = head;
    while (temp) {
        if (temp->id == id) {
            if (temp->status == "Available") {
                cout << "This book is not issued.\n";
                return;
            }

            cout << "Process Return for: " << temp->title << " (Borrowed by " << temp->borrower << ")\n";
            cout << "Original Due Date: " << temp->dueDay << "/" << temp->dueMonth << "/" << temp->dueYear << endl;
            
            cout << "--- Enter Actual Return Date ---\n";
            int rD = getValidInt("Day: ");
            int rM = getValidInt("Month: ");
            int rY = getValidInt("Year: ");

            long dueTotal = toTotalDays(temp->dueDay, temp->dueMonth, temp->dueYear);
            long returnTotal = toTotalDays(rD, rM, rY);

            if (returnTotal > dueTotal) {
                long overdueDays = returnTotal - dueTotal;
                int fine = overdueDays * 5; 
                cout << "BOOK IS OVERDUE by " << overdueDays << " days!\n";
                cout << "Please collect Fine: $" << fine << endl;
            } else {
                cout << "Returned on time. No fine.\n";
            }

            temp->status = "Available";
            temp->borrower = "None";
            temp->dueDay = 0; temp->dueMonth = 0; temp->dueYear = 0;

            if (!temp->waitlist.empty()) {
                string nextPerson = temp->waitlist.front();
                temp->waitlist.pop();
                
                cout << "NOTICE: This book is reserved. Auto-assigning to waitlist member.\n";
                temp->status = "Issued";
                temp->borrower = nextPerson;
                
                int nD = rD + 7;
                int nM = rM;
                int nY = rY;
                int maxDays = getDaysInMonth(rM, rY);
                if (nD > maxDays) {
                    nD -= maxDays;
                    nM++;
                    if(nM > 12) { nM = 1; nY++; }
                }

                temp->dueDay = nD;
                temp->dueMonth = nM;
                temp->dueYear = nY;
                cout << "Book automatically issued to " << nextPerson << "\n";
                cout << "New Due Date: " << nD << "/" << nM << "/" << nY << endl;
            }

            saveToFile();
            return;
        }
        temp = temp->next;
    }
    cout << "Book ID not found.\n";
}

void displayBooks() {
    if (!head) { cout << "Library is empty.\n"; return; }
    Book* temp = head;
    cout << "\n================= LIBRARY INVENTORY =================\n";
    while (temp) {
        cout << "ID: " << temp->id << " | " << temp->title << " by " << temp->author << endl;
        cout << "   Edition: " << temp->edition << " | Year: " << temp->year << endl;
        cout << "   Status: " << temp->status;
        if (temp->status == "Issued") {
            cout << " | Borrower: " << temp->borrower;
            cout << " | Due: " << temp->dueDay << "/" << temp->dueMonth << "/" << temp->dueYear;
        }
        if (!temp->waitlist.empty()) {
            cout << " | Waitlist: " << temp->waitlist.size() << " person(s)";
        }
        cout << "\n-----------------------------------------------------\n";
        temp = temp->next;
    }
}

void removeBook() {
    if (!head) { cout << "Library is empty.\n"; return; }
    int id = getValidInt("Enter Book ID to remove: ");

    Book* temp = head;
    Book* prev = nullptr;

    while (temp) {
        if (temp->id == id) {
            if (temp->status == "Issued") {
                cout << " Cannot remove book. It is currently issued to " << temp->borrower << ".\n";
                return;
            }
            if (!prev) head = temp->next;
            else prev->next = temp->next;
            
            delete temp;
            saveToFile();
            cout << "Book removed successfully.\n";
            return;
        }
        prev = temp;
        temp = temp->next;
    }
    cout << "Book ID not found.\n";
}

void showAllWaitlists() {
    if (!head) return;
    bool foundAny = false;
    Book* temp = head;
    cout << "\n====== ACTIVE WAITLISTS (By Book) ======\n";
    while(temp) {
        if(!temp->waitlist.empty()) {
            foundAny = true;
            cout << "Book: " << temp->title << " (ID: " << temp->id << ")\n";
            queue<string> copyQ = temp->waitlist;
            int pos = 1;
            while(!copyQ.empty()) {
                cout << "   " << pos++ << ". " << copyQ.front() << endl;
                copyQ.pop();
            }
        }
        temp = temp->next;
    }
    if(!foundAny) cout << "No active reservations.\n";
}

void sortBooks() {
    if (!head || !head->next) { cout << "Not enough books to sort.\n"; return; }
    for (Book* i = head; i->next != nullptr; i = i->next) {
        for (Book* j = i->next; j != nullptr; j = j->next) {
            if (i->title > j->title) {
                swap(i->id, j->id);
                swap(i->title, j->title);
                swap(i->author, j->author);
                swap(i->edition, j->edition);
                swap(i->year, j->year);
                swap(i->status, j->status);
                swap(i->borrower, j->borrower);
                swap(i->dueDay, j->dueDay);
                swap(i->dueMonth, j->dueMonth);
                swap(i->dueYear, j->dueYear);
                swap(i->waitlist, j->waitlist); 
            }
        }
    }
    saveToFile();
    cout << "Books sorted alphabetically.\n";
}

int main() {
    loadFromFile();
    
    int choice;
    do {
        cout << "\nADVANCED LIBRARY SYSTEM\n";
        cout << "1. Add Book\n";
        cout << "2. Display Inventory\n";
        cout << "3. Search & Borrow Book\n"; 
        cout << "4. Return Book (Calculate Fines)\n";
        cout << "5. Remove Book (By ID)\n";
        cout << "6. Sort Books\n";
        cout << "7. Show Waitlists\n";
        cout << "0. Exit\n";
        
        cout << "Choice: ";
        if (!(cin >> choice)) {
            cout << "Invalid input. Please enter a number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); 

        switch(choice) {
            case 1: addBook(); break;
            case 2: displayBooks(); break;
            case 3: searchAndBorrow(); break;
            case 4: returnBook(); break;
            case 5: removeBook(); break;
            case 6: sortBooks(); break;
            case 7: showAllWaitlists(); break;
            case 0: cout << "Exiting...\n"; break;
            default: cout << "Invalid Option.\n";
        }
    } while (choice != 0);

    return 0;
}