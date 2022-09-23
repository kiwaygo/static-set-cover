# static-set-cover

Static-set-cover is a C++ compile-time approximator for solving set cover problem.

## What is set cover problem?

Say, there is a vault in a bank. The vault must be always guarded by a security staff. The bank manager considers hiring from the below 4 candidates:

- Candidate A can work on Monday, Tuesday, Wednesday and Thursday.
- Candidate B can work on Thursday and Friday.
- Candidate C can work on Saturday and Sunday.
- Candidate D can work on Tuesday, Friday, Saturday and Sunday.

What is the least amount of candidates to hire to secure the vault? Which ones?

Aforementioned is a set cover problem. It gives a list of targets (the days in a week), a list of candidates, and which targets each candidate covers. The goal is to select the least candidates to get all the targets covered.

Set cover problem is NP-hard, but there are known polynomial-time algorithms out there that approximate its solution to reasonable errors.

## Why solving at compile-time?

Because it would be a super useful tool for developing C++ libraries. Say, as a library developer, you provide an IntList class that contains a list of integers. You also provide a couple of functions that extract certain properties from an IntList object:

- Function getMin scans the list and returns the smallest integer.
- Function getMax scans the list and returns the largest integer.
- Function getMedian copies the list to a temporary vector, sorts the elements, and returns their median. Note that with the sorted temporary vector, getting min and max values becomes as easy as accessing the first and the last element of the vector.

A user of your library may decide to extract any combination of the 3 properties {min, max, median} by calling these functions. Suppose someone wants to extract all 3, so they first call getMin, then call getMax, then call getMedian. Oops! The resulting program ends up being far from optimal. It scans the list twice then sort the list once, and yet, ideally, sorting the list once already allows efficient extraction of min, max, and median!

If only the compiler knew how to solve cover set problem. You could rewrite function getMedian so that it returns not only the median, but also the side products, the min and max values. Then, you could inform the compiler that:

- Function getMin covers target {min}.
- Function getMax covers target {max}.
- Function getMedian covers targets {median, min, max}.

Thus, if the user attempts to extract all 3 properties from a list object, the compiler would figure out that by calling just getMedian, all the requested targets are covered - It generates nothing more than a call to getMedian, yielding optimal performance.

## How do I start?

Static-set-cover is a header-only library. It requires C++17 and has been tested on gcc 8.1. It may work with other compilers, but there is currently no guarantee.

To use the library:

- Download the repository.
- Read test cases from "test/" directory to learn the recommended usage. The tests are built with GoogleTest, and can be run with ctest (CMake's test driver).
- In your project, where needed, include desired headers from the "include/" directory.

