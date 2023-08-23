# static-set-cover

Static-set-cover is a C++ compile-time approximator for solving set cover problem.

![Build and Run Tests badge](https://github.com/kiwaygo/static-set-cover/actions/workflows/BuildAndRunTests.yml/badge.svg)

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

Because it would be a super useful tool for developing C++ libraries.

### For example?

For example, as a library developer, you provide an IntList class that contains a list of integers. You also provide a couple of functions that extract certain properties from an IntList object:

```c++
class IntList {
public:
    // Scans through mData and returns the smallest integer
    int getMin() const;

    // Scans through mData and returns the biggest integer.
    int getMax() const;

    // Copies mData to a temp vector v, sorts it, and returns v[v.size()/2].
    int getMedian() const;

private:
    std::vector<int> mData;
};
```

A user of your library may decide to extract any combination of the 3 properties {min, max, median} by calling these functions. Suppose someone wants to extract all 3, so they write:

```c++
void f() {
    IntList list = createList();
    std::cout << list.getMin() << ", " << list.getMax() <<  ", " << list.getMedian();
}
```

At first glance, this looks fine. However, look closer - The program does a lot of unnecessary work! It scans the list twice and sorts the list once. Ideally, merely sorting the list is sufficient to compute min, max, and median!

### Can we do better?

If only the compiler knew how to solve cover set problem. You could rewrite function getMedian so that it returns not only the median, but also its trivial side products, the min and max values. Then, you could inform the compiler that:

- getMin covers target {min}.
- getMax covers target {max}.
- getMedian covers targets {median, min, max}.

Now, if the user attempts to extract all 3 properties from an IntList object, the compiler would figure out that by calling just getMedian, all the requested targets are covered - It would generate nothing more than a call to getMedian, yielding optimal performance.

## How do I start?

Static-set-cover is a header-only library that requires C++17. Tests are built with gcc and exercised by Github action on latest Ubuntu for each commit on the main branch.

To use the library:

- Download the repository.
- Read test cases from "test/" directory to learn the recommended usage. The tests are developed with GoogleTest, and can be run via CTest (CMake's test driver).
- In your project, where needed, include desired headers from the "include/" directory.
