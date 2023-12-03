#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define CYLINDERS 5000
#define REQUESTS 1000

// function declartion
int FCFS(int *reqs, int first);
int SSTF(int *reqs, int first);
int SCAN(int *reqs, int first);
int CSCAN(int *reqs, int first);
int look(int *reqs, int first);
int Clook(int *reqs, int first);

int array[CYLINDERS];
int reqs[REQUESTS];

int main(int argc, char *argv[])
{

    if (argc < 2 || argc > 2)
    {
        printf("too many or too less arguments");
        exit(1);
    }
    else
    {
        int first = atoi(argv[1]);
        if (first > 4999 || first < 0)
        {
            printf("the range is between 0 and 4999!\n");
            exit(1);
        }
    }

    int first = atoi(argv[1]);
    int taken[CYLINDERS];

    for (int i = 0; i < CYLINDERS; i++)
    {
        array[i] = i;
        taken[i] = 0;
    }

    int a = 0;
    while (a < REQUESTS)
    {
        int random = (rand() % 5000);
        if (taken[random] == 0)
        {
            reqs[a] = random;
            taken[random] = 1;
            a++;
        }
    }

    printf("FCFS: TOTAL HEAD MOVEMENT: %d\n", FCFS(reqs, first));
    // segmentation fault
    printf("SSTF: TOTAL HEAD MOVEMENT: %d\n", SSTF(reqs, first));
    // seems to be stuck somewhere
    printf("SCAN: TOTAL HEAD MOVEMENT: %d\n", SCAN(reqs, first));
    printf("C-SCAN: TOTAL HEAD MOVEMENT: %d\n", CSCAN(reqs, first));
    printf("LOOK: TOTAL HEAD MOVEMENT: %d\n", look(reqs, first));
    printf("C-LOOK: TOTAL HEAD MOVEMENT: %d\n", Clook(reqs, first));

    return 0;
}

// first come first served.
int FCFS(int *reqs, int first)
{
    int total = 0;
    int current = first;
    for (int i = 0; i < REQUESTS; i++)
    {
        // the number of headmovements it takes to the next request
        total += abs(current - reqs[i]);
        current = reqs[i];
    }
    return total;
}

// Shortest seek time first
int SSTF(int *reqs, int first)
{
    int total = 0;
    int current = first;

    // make a check list to mark the requests that are done
    int check[REQUESTS];
    memset(&check, 0, REQUESTS * sizeof(int));

    for (int i = 0; i < REQUESTS; i++)
    {

        // to find the next request with minimal difference
        int min = CYLINDERS;
        int min_req = -1;

        // go through the list of randomized request
        for (int j = 0; j < REQUESTS; j++)
        {
            if (check[j] == 0 && min > abs(current - reqs[j]))
            {
                min = abs(current - reqs[j]);
                min_req = j;
            }
        }

        // minimal is found, add it to the total
        total += abs(current - reqs[min_req]);
        current = reqs[min_req];
        // mark
        check[min_req] = 1;
    }

    return total;
}

// move head towards one end of the disk and then to the other
int SCAN(int *reqs, int first)
{
    int total = 0;
    int current = first;
    // make a check list to mark the requests that are done
    int check[REQUESTS];
    memset(&check, 0, REQUESTS * sizeof(int));

    int direction = 0;
    int i = 0;
    // ends when all requests are found
    while (i < REQUESTS)
    {
        // check if current head is on a request in the randomized list of requests
        for (int j = 0; j < REQUESTS; j++)
        {
            if (check[j] == 0 && reqs[j] == current)
            {
                check[j] = 1;
                // when a request in the list is found
                i++;
            }
        }

        total++;

        // when moving towards the lower end
        if (direction == 0)
        {
            current--;
            // when it is the lower end, reverse
            if (current == -1)
            {
                current += 2;
                direction = 1;
            }
            // when moving towards the higher end.
        }
        else
        {
            current++;
            // when it is the higher end, reverse
            if (current == CYLINDERS)
            {
                current -= 2;
                direction = 0;
            }
        }
    }

    return total;
}

// In SCAN, some path is being checked twice,
// in CSCAN, when it reaches an end, it continues from the other end, like a circle
int CSCAN(int *reqs, int first)
{
    int total = 0;
    int current = first;
    int check[REQUESTS];
    memset(&check, 0, REQUESTS * sizeof(int));

    int i = 0;
    while (i < REQUESTS)
    {
        for (int j = 0; j < REQUESTS; j++)
        {
            if (check[j] == 0 && reqs[j] == current)
            {
                check[j] = 1;
                i++;
            }
        }
        total++;

        current++;

        // when it is the end, go to the other!
        if (current == CYLINDERS)
        {
            current = current % CYLINDERS;
        }
    }

    return total;
}

// same as SCAN but stops at the last request at a certain direction, not the end
int look(int *reqs, int first)
{
    int total = 0;
    int current = first;
    int check[REQUESTS];
    memset(&check, 0, REQUESTS * sizeof(int));
    int direction = 0;

    int i = 0;
    while (i < REQUESTS)
    {
        for (int j = 0; j < REQUESTS; j++)
        {
            if (check[j] == 0 && reqs[j] == current)
            {
                check[j] = 1;
                i++;
            }
        }

        // a variable used to check if any request remain on the path
        int remain = 0;
        total++;
        if (direction == 0)
        {
            for (int j = 0; j < REQUESTS; j++)
            {
                // if any request that is lower is found, then the header continues on the path
                if (check[j] == 0 && reqs[j] < current)
                {
                    remain = 1;
                    current--;
                    break;
                }
            }
            // if not, change direction
            if (remain == 0)
            {
                direction = 1;
                current--;
            }
        }
        else
        {
            for (int j = 0; j < REQUESTS; j++)
            {
                if (check[j] == 0 && reqs[j] > current)
                {
                    remain = 1;
                    current++;
                    break;
                }
            }

            /*if(remain == 0){
                direction = 0;
                total++;
                current--;
            }*/
        }
    }
    return total;
}

// Same as C-scan but stops at the last request on the path and swithes to the last on the other end
int Clook(int *reqs, int first)
{
    int total = 0;
    int current = first;
    int check[REQUESTS];
    memset(&check, 0, REQUESTS * sizeof(int));

    int i = 0;
    while (i < REQUESTS)
    {
        for (int j = 0; j < REQUESTS; j++)
        {
            if (check[j] == 0 && reqs[j] == current)
            {
                check[j] = 1;
                i++;
            }
        }
        int remain = 0;
        total++;

        for (int j = 0; j < REQUESTS; j++)
        {
            if (check[j] == 0 && reqs[j] < current)
            {
                remain = 1;
                current--;
                break;
            }
        }

        // when no request remains on the path toward the lower end
        if (remain == 0)
        {
            int max = -1;
            int max_req = CYLINDERS;

            for (int j = 0; j < REQUESTS; j++)
            {
                if (check[j] == 0 && max < reqs[j])
                {
                    max = reqs[j];
                    max_req = j;
                }
            }
            if (max_req != CYLINDERS)
            {
                current = reqs[max_req];
            }
        }
    }
    return total;
}
