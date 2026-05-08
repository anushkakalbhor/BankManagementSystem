# 🏦 Bank Management System

A C-based Bank Management System developed for the Problem Solving and Programming (PSP) course.

This project simulates core banking operations such as account creation, deposits, withdrawals, balance checking, money transfer, transaction history management, and account administration using file handling in C.

The project also includes a basic web frontend integration using HTML, CSS, JavaScript, and Python server support.

---

# 📌 Project Overview

The Bank Management System is a console-based and API-enabled banking application built primarily using the C programming language.

The system stores account details and transaction records using file handling techniques and supports both:

- Interactive terminal mode
- API mode for web frontend integration

The project demonstrates:
- Structured programming in C
- File handling
- Data persistence
- User authentication
- Banking transaction logic
- Backend integration concepts

---

# 🚀 Features

✅ Create new bank accounts  
✅ Secure login authentication  
✅ Deposit money  
✅ Withdraw money  
✅ Transfer money between accounts  
✅ Check account balance  
✅ Update account details  
✅ Delete accounts  
✅ View transaction history  
✅ Admin panel to view all accounts  
✅ File-based data storage  
✅ Web frontend integration support  

---

# 🛠️ Technologies Used

## Core Language
- C Programming

## Frontend
- HTML
- CSS
- JavaScript

## Backend Support
- Python (server integration)

## Concepts Used
- File Handling
- Structures
- Functions
- Conditional Statements
- Loops
- Authentication
- Data Persistence

---

# 📂 Project Structure

```plaintext
BankManagementSystem-main/
│
├── main.c                # Main terminal-based banking system
├── bank.c                # API-enabled banking backend
├── server.py             # Python server integration
├── index.html            # Frontend interface
├── style.css             # Frontend styling
├── app.js                # Frontend JavaScript logic
│
├── account.dat           # Stores account records
├── transactions.txt      # Stores transaction history
│
├── bank.exe              # Compiled executable
├── a.exe                 # Compiled executable
│
└── README.md
```

---

# ⚙️ Functionalities

## 👤 Account Management

Users can:
- Create accounts
- Update account information
- Delete accounts

Each account stores:
- Account holder name
- Account number
- Password
- Current balance

---

## 💰 Banking Operations

The system supports:

### Deposit Money
Allows users to deposit funds into their account.

### Withdraw Money
Allows secure withdrawal after authentication.

### Transfer Money
Transfers money between accounts securely.

### Balance Inquiry
Displays current account balance.

---

## 🔐 Authentication System

The system includes:
- Password-protected login
- Account verification
- Secure transaction access

---

## 📜 Transaction History

All transactions are logged and stored in:

```plaintext
transactions.txt
```

This includes:
- Deposits
- Withdrawals
- Transfers

---

## 🧾 File Handling

The system stores persistent data using:

| File | Purpose |
|---|---|
| `account.dat` | Stores account information |
| `transactions.txt` | Stores transaction logs |

---

# 🖥️ Program Modes

## 1️⃣ Interactive Terminal Mode

Run the application normally:

```bash
./bank
```

or

```bash
bank.exe
```

This launches the menu-driven banking system.

---

## 2️⃣ API Mode

The application also supports command-line API operations.

Example commands:

### Login

```bash
./bank login <acc_no> <password>
```

### Create Account

```bash
./bank create <name> <acc_no> <password>
```

### Deposit Money

```bash
./bank deposit <acc_no> <password> <amount>
```

### Withdraw Money

```bash
./bank withdraw <acc_no> <password> <amount>
```

### Transfer Money

```bash
./bank transfer <acc_no> <password> <to_acc_no> <amount>
```

### Check Balance

```bash
./bank balance <acc_no> <password>
```

### Transaction History

```bash
./bank history <acc_no> <password>
```

---

# ▶️ How to Run the Project

## Using GCC Compiler

Compile the project:

```bash
gcc main.c -o bank
```

Run the executable:

```bash
./bank
```

---

# 🌐 Web Frontend Integration

The project also contains:
- HTML frontend
- CSS styling
- JavaScript functionality
- Python server integration

Files:
- `index.html`
- `style.css`
- `app.js`
- `server.py`

This enables integration between the C backend and a browser-based interface.

---

# 📋 Main Menu Options

```plaintext
1. Create Account
2. Deposit Money
3. Withdraw Money
4. Check Balance
5. Transfer Money
6. Delete Account
7. Update Account Details
8. View Transaction History
9. Admin - View All Accounts
10. Exit
```

---

# 🧠 Concepts Demonstrated

This project demonstrates practical implementation of:

- Structured Programming
- Banking Logic
- Data Management
- Authentication Systems
- File Handling in C
- Backend Integration
- API-style Command Handling

---

# 📚 Academic Purpose

This project was developed as part of the:

**Problem Solving and Programming (PSP)** coursework

to strengthen understanding of:
- C programming fundamentals
- Real-world application development
- Problem-solving techniques

---

# 🔮 Future Improvements

Possible future enhancements:

- Database integration
- GUI application
- Encryption for passwords
- Online banking features
- User session management
- Improved frontend UI
- Cloud deployment
- Multi-user concurrency support

---

# ⚠️ Disclaimer

This project is developed for educational purposes only.

It is a simulation of banking operations and is not intended for real financial transactions or production banking environments.

---

# 👨‍💻 Contributors

- Project Team Members
- PSP Course Students

---

# 📄 License

This project is intended for academic and learning purposes.

---

# ⭐ Support

If you found this project useful, consider giving the repository a ⭐ on GitHub.
