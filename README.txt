compile with: 

gcc montecarloPI.c -lpthread -o monte

Run with:

./monte -n 4 -c

-n 4 -means create 4 threads 

-c   -means show counters

Improvements: use GMP library, add file to save count, add network capabilities, 
