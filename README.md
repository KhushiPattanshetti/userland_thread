
---
```markdown
# User-Level Thread Library with Custom Scheduler

##  Overview
This project implements a **User-Level Thread Library (ULT)** in C, featuring **One-to-One** , **Many-to-One** and **Many-to-Many** threading models. It reduces kernel dependency and provides a **custom Round Robin preemptive scheduler** for efficient context switching.

##  Features
- Thread creation and management using `clone()`.
- CLI support for:
  - Creating threads
  - Killing threads
  - Listing threads
- Supports both **One-to-One** and **Many-to-One** threading models.
- Custom **preemptive Round Robin scheduler**.
- Optimized **context switching** at the user level for better performance.

## 🛠Tech Stack
- **Language**: C
- **OS Concepts**: Threading, Scheduling, Context Switching
- **System Calls**: clone(), signals, timers

