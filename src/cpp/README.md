# C++ counter

Build
g++ -O3 -march=native -std=c++17 -pthread -o counter counter.cpp

Run (rules as arguments, -jN threads)
./counter -j4 1 2 3 4 5 6 7 8 9 | python3 crt.py

Output of counter is the count modulo 4 primes, crt.py rebuilds the exact number and checks it against the 4th prime

Test mode
-x keeps only cases without castling and en passant
-c p:q:r:b:n,... replaces the material list (same list for both sides)
-w N runs only work group N

brute9.py counts the same small cases by checking every square one by one, to compare with -x -c
