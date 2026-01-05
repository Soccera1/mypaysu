# Contributing to the Revolution

We welcome all comrades who wish to improve the MyPaySU platform. By contributing, you agree to help build a better financial future for the people.

## Coding Standards

*   **Backend:** Use modern C++ (C++11 or later). Keep the ledger thread-safe using `std::mutex`.
*   **Frontend:** Use pure C (C99/C11). Interface with X11 directly; avoid high-level toolkits like GTK or Qt to keep the system lean and efficient.
*   **Aesthetic:** All UI changes must adhere to the Soviet theme. Use `#CC0000` (Red) and `#FFD700` (Gold).
*   **Responsiveness:** Ensure all UI elements are relative or centered; never use hardcoded absolute positions for layout.

## Submission Process

1.  Identify a problem or a way to serve the people better.
2.  Implement your changes.
3.  Ensure your code builds with the existing `Makefile`.
4.  Submit your contribution for review by the Central Committee.

Glory to the revolution!
