# Student Record Management System with Ticketing

This is a *console-based Student Record Management System (SRMS)* written in C++.  
It supports:

- Admin and Student login
- Managing student records (CRUD)
- Storing data in a | (pipe) separated text file
- A simple *ticket system* for students to raise issues
- *Hidden password input* (with * masking)
- Works on *Windows, **Linux, and **macOS*

---

## 1. Features

### 👨‍💼 Admin Features

- Login with secure password input
- Add new students
- View all student records
- Search student by Roll Number
- Modify existing student details
- Delete student records
- View all tickets raised by students
- Update ticket status

*Default Admin Credentials:*

- *Username:* admin  
- *Password:* admin123

---

### 🎓 Student Features

- Login using:
  - *Roll Number* (e.g., 22BCS101)
  - Password set by admin at the time of student creation
- View *own profile* (record)
- Raise tickets (issues like marks/details/other)
- View all tickets raised by themselves

---

## 2. File Structure

The program uses 2 data files in the *same folder* as the executable:

1. students.txt
2. tickets.txt

Both are *plain text* and use **| (pipe)** as the field separator.

---

### 2.1 students.txt Format

Each line represents one student in this format:

```text
roll|name|dept|semester|phone|email|address|cgpa|password
