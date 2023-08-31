# Static-set-cover

**Static-set-cover is a C++ compile-time approximator for solving set cover problem.**

Check out the [Github repository](https://github.com/kiwaygo/static-set-cover). Issues and pull requests welcomed.

![Build and Run Tests badge](https://github.com/kiwaygo/static-set-cover/actions/workflows/BuildAndRunTests.yml/badge.svg)

## What is set cover problem?

Say, there is a vault in a bank. The vault must be always guarded by a security staff. The bank manager considers hiring from the below 4 candidates:

- Candidate A can work on Monday, Tuesday, Wednesday and Thursday.
- Candidate B can work on Thursday and Friday.
- Candidate C can work on Saturday and Sunday.
- Candidate D can work on Tuesday, Friday, Saturday and Sunday.

What is the least amount of candidates to hire to secure the vault the entire week? Which ones?

Aforementioned is a set cover problem. It gives a list of targets (the days in a week), a list of candidates, and which targets each candidate covers. The goal is to select the least candidates to get all the targets covered.

Set cover problem is NP-hard, but there are known polynomial-time algorithms out there that approximate its solution to reasonable errors.

## Why solving at compile-time?

Because it would be a super useful tool for developing C++ libraries.

### For example?

For example, as a library developer, you provide a class that owns a list of integers, and a few member functions to extract properties from such list:

```c++
class IntList {
    vector<int> mData;
public:
    int getMin() const { /* Scan through mData. Return the smallest integer.*/ }
    int getMax() const { /* Scan through mData. Return the biggest integer. */ }
    int getMedian() const { /* Copy mData to temp vector v. Sort v. Return v[v.size()/2]. */ }
};
```

A user may try to get any combination of {min, max, median}. If they want to extract all three, they write:

```c++
void f(const IntList& list) {
    cout << list.getMin() << ", " << list.getMax() <<  ", " << list.getMedian();
}
```

At first glance, this looks fine. However, look closer - The program does a lot of unnecessary work! It scans the list twice and sorts the list once. Ideally, just sorting the list is enough to compute min, max, and median!

Another user may try to get min and max, so they write:

```c++
void g(const IntList& list) {
    cout << list.getMin() << ", " << list.getMax();
}
```

Uh oh. This yields another program that does unnecessary work, but it is due to a different reason - It scans the list twice, but we can do just fine scanning the list once to find both min and max.

### Can we do better?

Let's see... As the library developer, perhaps you could provide a few specialized member functions.

```c++
class IntList {
    // ...
    tuple<int, int, int> getMinMaxMedian() const;
    tuple<int, int> getMinMax() const;
};
```

It is now up to the user to call the most efficient function. But there are two problems with this approach:

- This is error-prone. If a user is not aware of these special getters, they could call the old functions and lose performance.
- This scales poorly for library developer. For $3$ properties, you could end up writing $2^3 = 8$ member functions to take care of all specialized implementations. For $n$ properties, you might need $2^n$, which is a lot.

Probably acceptable, but not great.

### If only...

If only the compiler knew how to solve set cover problem. Then, as the library developer, you could write along the lines of:

```c++
struct Cover1 {
    using Targets = tuple<Min>;
    static tuple<int> eval(const IntList&);
};

struct Cover2 {
    using Targets = tuple<Max>;
    static tuple<int> eval(const IntList&);
};

struct Cover3 {
    using Targets = tuple<Min, Max, Median>;
    static tuple<int, int, int> eval(const IntList&);
};

struct Cover4 {
    using Targets = tuple<Min, Max>;
    tuple<int, int> eval(const IntList&);
};

// Tada!
using IntListProperties = SetCoverProblem<Cover1, Cover2, Cover3, Cover4>;
```

When the user wants to get all three properties from an IntList object, they would write:
```c++
void f(const IntList& list) {
    tuple<int, int, int> result = IntListProperties::eval<Min, Max, Median>(list); // Calls Cover3::eval
    cout << get<0>(result) << ", " << get<1>(result) <<  ", " << get<2>(result);
}
```

Or, if they want to get just min and max, they would write:
```c++
void g(const IntList& list) {
    tuple<int, int> result = IntListProperties::eval<Min, Max>(list); // Calls Cover4::eval
    cout << get<0>(result) << ", " << get<1>(result);
}
```

For you, it is really easy to add a new IntList property:
```c++
struct Cover5 {
    using Targets = tuple<Avg>;
    tuple<float> eval(const IntList&);
};

// Old: using IntListProperties = SetCoverProblem<Cover1, Cover2, Cover3, Cover4>;
using IntListProperties = SetCoverProblem<Cover1, Cover2, Cover3, Cover4, Cover5>;
```

The API would even work out-of-the-box for trickier cases:
```c++
void i(const IntList& list) {
    tuple<int, int> result1 = IntListProperties::eval<Min, Median>(list); // Calls Cover3::eval.
    tuple<int, int> result2 = IntListProperties::eval<Median, Min>(list); // Calls Cover3::eval.
    tuple<int, float> result3 = IntListProperties::eval<Min, Avg>(list); // Calls both Cover1::eval and Cover5::eval.
}
```

### Guess what?

The said class template that performs all the magic:
```
template <typename...> class SetCoverProblem;
```
is exactly what this project, static-set-cover, aims to provide!

## How do I start?

Static-set-cover is a header-only library that requires C++17. Tests are built with gcc and exercised by Github action on latest Ubuntu for each commit on the main branch.

To use the library:

- Download the [repository](https://github.com/kiwaygo/static-set-cover).
- Read test cases from "test/" directory to learn the recommended usage. The tests are developed with GoogleTest, and can be run via CTest (CMake's test driver).
- In your project, where needed, include desired headers from the "include/" directory.
