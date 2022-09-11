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

Because that is super useful for C++ library developers. Say, a C++ library provides a class that contains a list of floating point numbers. It also provides functions that extract certain properties from an object of such class:

- Function getMin scans through the list and returns the smallest float.
- Function getMax scans through the list and returns the largest float.
- Function getMedian copies the list to a temporary vector, sorts the elements, and returns their median. Note that with the sorted temporary vector, getting min and max becomes as easy as accessing the first and the last element of the vector.

A user of the library may decide to extract any combination of the 3 properties (min, max, and median) by calling these functions on an object of the class. Suppose a user wants to extract all 3 properties, so they write client code that calls all 3 functions - getMin, getMax and getMedian. Oops! The resulting program ends up far from optimal, scanning through the list twice then sort the list once, and yet, ideally, just sorting the list once should be sufficient to get all min, max and median values!

If only the compiler knew how to solve cover set problem. The library could make function getMedian return not only the median value, but also the min and max values. Further, the library could inform the compiler that:

- Function getMin covers target (min).
- Function getMax covers target (max).
- Function getMedian covers targets (median, min, max).

Thus, when the user attempts to extract all 3 properties from an object, the compiler would figure out that by picking just the function getMedian, all the requested targets are covered. As such, it could generate nothing more than a getMedian function call, yielding optimal performance.

And that, would be super useful for C++ library developers.
