# 🏦 Bank Management System

A **C-based Bank Management System** with a modern web frontend. The core banking logic is written entirely in C, with a lightweight Python bridge server and an HTML/CSS/JS interface built on top.

---

## 📸 Features

- 🔐 Account Login with password authentication
- 🆕 Create new bank accounts
- 💰 Deposit & Withdraw money
- 🔄 Transfer funds between accounts
- 📋 Transaction history log
- ✏️ Update account name and password
- 🗑️ Delete account
- 🛡️ Admin panel to view all accounts
- ⚠️ Minimum balance of ₹500 enforced

---

## 🛠️ Tech Stack

| Layer | Technology |
|---|---|
| Core Logic | C (file handling, structs, all operations) |
| Web Interface | HTML, CSS, JavaScript |
| Bridge Server | Python 3 (stdlib only, no pip installs) |
| Data Storage | Binary file (`account.dat`) + text file (`transactions.txt`) |

> No external APIs. No database. No internet required. Everything runs locally.

---

## 📁 Project Structure

```
bank-management-system/
├── bank.c            ← Core C program (all banking logic)
├── server.py         ← Python bridge between browser and bank.exe
├── index.html        ← Web UI
├── style.css         ← Styles
├── app.js            ← Frontend logic
└── README.md
```

---

## ⚙️ How It Works

```
Browser  →  server.py  →  bank.exe  →  account.dat
         ←             ←  JSON out  ←  transactions.txt
```

`bank.c` supports two modes:

- **Terminal mode** — run `bank.exe` with no arguments → original interactive menu
- **API mode** — run with arguments e.g. `bank.exe login 1001 mypassword` → outputs JSON for the web frontend

The Python server simply receives requests from the browser, calls `bank.exe` with the right arguments, and returns the JSON output. All actual logic (authentication, file I/O, balance checks) happens inside the C program.

---

## 🚀 Getting Started (Windows)

### Prerequisites

- **GCC** — Install [TDM-GCC](https://jmeubank.github.io/tdm-gcc/) and add to PATH
- **Python 3** — Install from [python.org](https://python.org/downloads), tick "Add to PATH"

### Steps

**1. Clone the repo**
```bash
git clone https://github.com/YOUR_USERNAME/bank-management-system.git
cd bank-management-system
```

**2. Compile the C program**
```bash
gcc bank.c -o bank.exe
```

**3. Start the server**
```bash
python server.py
```

**4. Open in browser**
```
http://localhost:8080
```

### Run as terminal program (original mode)
```bash
bank.exe
```

---

## 🔑 Default Credentials

| Role | Detail |
|---|---|
| Admin Password | `admin123` |
| User Password | Set during account creation |


## 📌 Concepts Used (C)

- Structures (`struct`)
- File handling (`fread`, `fwrite`, `fseek`, `rewind`)
- Binary file I/O (`account.dat`)
- Sequential file search
- Command-line arguments (`argc`, `argv`)
- String operations (`strcmp`, `strcpy`, `strstr`)

Author
Anushka Kalbhor 
B.Tech - Computer Science and Engineering in AIML
Github - anushkakalbhor

