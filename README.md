# **Advanced Library Management System**

A C++ console-based application designed to manage a library inventory. It allows administrators to track books, handle student borrowing/returns, manage waitlists for popular titles, and automatically calculate overdue fines.

## **🚀 Features**

* **Persistent Storage:** Automatically saves and loads library data from library\_data\_final.txt.  
* **Inventory Management:** Add, remove, sort, and display books with unique IDs.  
* **Smart Borrowing:** \* Search by title or author.  
  * Prevents a single student from borrowing more than one book at a time.  
  * Automatic due date calculation (set to 7 days).  
* **Waitlist System:** If a book is issued, users can join a queue. The book is automatically assigned to the next person in line upon return.  
* **Fine Calculation:** Calculates a fine of **$5 per day** for overdue books based on a comparison of due dates and return dates.  
* **Input Validation:** Robust checks for data types, date ranges, and duplicate entries.

---

## **🛠 Data Structures Used**

| Structure | Purpose |
| :---- | :---- |
| **Singly Linked List** | Used to store the primary collection of Book objects for dynamic memory management. |
| **Queue (std::queue)** | Used within each book object to manage a "First-In, First-Out" waitlist for borrowers. |
| **Struct** | Defines the Book object containing ID, title, author, status, and date information. |
| **Vector** | Used temporarily during searches to store and display matching results. |

---

## **📖 How to Use**

### **1\. Installation**

Ensure you have a C++ compiler (like GCC or Clang) installed.

Bash  
g++ library\_system.cpp \-o library\_system  
./library\_system

### **2\. Main Menu Options**

* **Add Book:** Enter a 4-digit ID, title, author, and edition. The system prevents duplicate book entries.  
* **Search & Borrow:** Search for a book. If available, enter the current date to calculate the due date. If unavailable, you can join the waitlist.  
* **Return Book:** Enter the return date. If late, the system will display the total fine. If a waitlist exists, the book is instantly re-issued to the next student.  
* **Sort Books:** Reorganizes the entire linked list alphabetically by book title.  
* **Show Waitlists:** Displays all books currently requested by other students.

---

## **⚠️ Important Notes**

* **Date Format:** Dates are entered as integers (Day, Month, Year).  
* **Data File:** Do not manually edit library\_data\_final.txt unless you follow the specific line-by-line format used in saveToFile(), as this may corrupt the loading process.  
* **Fine Logic:** The system uses a simplified day-counting logic ($TotalDays \= (Y \\times 365\) \+ (M \\times 30\) \+ D$).

