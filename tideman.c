#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
}
pair;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
void print_winner(void);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    pair_count = 0;
    int voter_count = get_int("Number of voters: ");


    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's ith preference
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        record_preferences(ranks);

        printf("\n");
    }


    add_pairs();
    sort_pairs();
    lock_pairs();
    print_winner();
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    // if name is a match for the name of valid cadidate
    for (int x = 0; x < candidate_count; x++)
    {
        if (strcmp(name, candidates[x]) == 0)
        {
            ranks[rank] = (x);
            return true;
        }
    }
    // TODO
    return false;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    int xindex;
    int yindex;
    //for every row
    for (int x = 0; x < candidate_count; x++)
    {
        //find the position of candidate x
        for (int a = 0; a < candidate_count; a++)
        {
            if (ranks[a] == x)
            {
                xindex = a;
            }
        }

        //for every column
        for (int y = 0; y < candidate_count; y++)
        {
            //find the position of candidate y
            for (int b = 0; b < candidate_count; b++)
            {
                if (ranks[b] == y)
                {
                    yindex = b;
                }
            }

            if (xindex < yindex)
            {
                preferences[x][y]++;
            }
        }
    }

    return;
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    //add all pairs where one is preffered to the pairs array
    // if its not 0 means it is comparing between 2 different candidates
    for (int x = 0; x < candidate_count; x++)
    {
        for (int y = 0; y < candidate_count; y++)
        {
            //comparing if x or y is preferred
            if (preferences[x][y] != 0)
            {
                if (preferences[x][y] > preferences[y][x])
                {
                    pairs[pair_count].winner = x;
                    pairs[pair_count].loser = y;
                    pair_count++;
                }
            }
        }
    }
    return;
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    //for every digit in pair
    for (int x = 0; x < (pair_count); x++)
    {
        int variable = 0;
        int variableindex = 0;
        int temp = 0;

        //find the heightest difference
        //pairs array only show which one in the preferences array to look at
        for (int y = (0 + x); y < pair_count; y++)
        {
            int difference = preferences[pairs[y].winner][pairs[y].loser];
            if (difference > variable)
            {
                variable = difference;
                variableindex = y;
            }
        }

        temp = pairs[x].winner;
        pairs[x].winner = pairs[variableindex].winner;
        pairs[variableindex].winner = temp;

        temp = pairs[x].loser;
        pairs[x].loser = pairs[variableindex].loser;
        pairs[variableindex].loser = temp;
    }
    // TODO
    return;
}

void lock_pairs(void)
{
    // for every pair
    for (int x = 0; x < pair_count; x++)
    {
        bool noloop = true;
        // for every column
        for (int z = 0; z < candidate_count; z++)
        {
            // find where the true is in each row
            if (locked[pairs[x].loser][z] == true)
            {
                bool loop;
                int previousloser = z;

                int losersx [candidate_count];
                int losersy [candidate_count];
                int loserscounter = 0;
                int iterate = 0;
                int previousy = 0;
                // repeatedly go the the previous loser
                do
                {
                    for (int i = (0 + previousy); i < candidate_count; i++)
                    {
                        // if a previousloser is identified
                        if (locked[previousloser][i] == true)
                        {
                            loop = true;

                            // remember where you left off
                            if ((i + 1) <= candidate_count)
                            {
                                losersx [loserscounter - iterate] = previousloser;
                                losersy [loserscounter - iterate] = (i + 1);
                                loserscounter++;
                            }
                            previousloser = i;

                            // if it loops back to the winner, end the loop
                            if (previousloser == pairs[x].winner)
                            {
                                noloop = false;
                                loop = false;
                                break;
                            }

                            previousy = 0;
                            break;

                        }
                        else
                        {
                            // go back to where you left off
                            if ((loserscounter > iterate) && (i == (candidate_count - 1)))
                            {
                                previousloser = losersx[(loserscounter - 1)];
                                previousy = losersy[(loserscounter - 1)];
                                iterate++;
                                loop = true;
                            }
                            else
                            {
                                loop = false;
                            }
                            // cant be false here because it stops loop
                        }
                    }
                }
                while (loop == true);
            }
        }
        if (noloop == true)
        {
            locked[pairs[x].winner][pairs[x].loser] = true;
        }
    }
    return;
}

// Print the winner of the election
void print_winner(void)
{
    int final = candidate_count;

    for (int x = 0; x < candidate_count; x++)
    {
        int counter = 0;
        for (int y = 0; y < candidate_count; y++)
        {
            if (locked[y][x] == true)
            {
                counter++;
            }
        }

        if (counter <= final)
        {
            final = counter;
        }
    }

    for (int x = 0; x < candidate_count; x++)
    {
        int counter = 0;
        for (int y = 0; y < candidate_count; y++)
        {
            if (locked[y][x] == true)
            {
                counter++;
            }
        }
        if (counter == final)
        {
            printf("%s\n", candidates[x]);
        }
    }
    // end
    // TODO
    return;
}