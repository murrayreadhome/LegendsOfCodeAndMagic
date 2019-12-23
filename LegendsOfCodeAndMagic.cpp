// LegendsOfCodeAndMagic.cpp : Defines the entry point for the application.
//

#include "LegendsOfCodeAndMagic.h"
#include "GameController.h"
#include <chrono>

using namespace std;

double eval_params(const Params& a, const Params& b)
{
    int a_wins = 0;
    int seed = int(std::chrono::duration_cast<std::chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count());

    for (int i = 0; i < 500; i++)
    {
        for (int o = 0; o < 2; o++)
        {
            Player p1(o ? b : a);
            Player p2(o ? a : b);
            GameController gc(seed++);
            int winner = gc.play(p1, p2);
            if (winner == o)
                a_wins++;
        }
    }

    return a_wins / 1000.0;
}

void mutate_params(Params& p, mt19937& re)
{
    size_t ni = sizeof(p.i) / sizeof(int);
    size_t nd = sizeof(p.d) / sizeof(double);
    size_t n = uniform_int_distribution<size_t>(0, ni + nd -1)(re);
    if (n < ni)
    {
        int& i = reinterpret_cast<int*>(&p.i)[n];
        i += uniform_int_distribution<int>(-1, 1)(re);
    }
    else
    {
        n -= ni;
        double& d = reinterpret_cast<double*>(&p.d)[n];
        d += uniform_real_distribution<double>(-1.0, 1.0)(re);
    }
}

ostream& operator<<(ostream& out, const Params& p)
{
    size_t ni = sizeof(p.i) / sizeof(int);
    size_t nd = sizeof(p.d) / sizeof(double);
    const int* pi = reinterpret_cast<const int*>(&p.i);
    const double* pd = reinterpret_cast<const double*>(&p.d);

    const char* eon = "       \n";
    out << "{{" << endl;
    for (int i = 0; i < ni; i++)
        out << pi[i] << "," << eon[i%8];
    out << "\n},\n{" << endl;
    for (int i = 0; i < nd; i++)
        out << pd[i] << "," << eon[i%8];
    out << "\n}};" << endl;

    return out;
}

void improve_params(Params p, Params vs)
{
    int seed = int(std::chrono::duration_cast<std::chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count());
    mt19937 re(seed);
    double bestSc = eval_params(p, vs);
    size_t n = 1;
    double ltBestSc = bestSc;
    size_t ltN = n;
    Params ltBest = p;
    bool reverted = true;
    for (;;)
    {
        Params q = p;
        mutate_params(q, re);
        double scq = 0, tscq = 0;
        for (size_t i = 0; i < n; i++)
        {
            tscq += eval_params(q, vs);
            scq = tscq / (i+1);
            if (scq < bestSc)
                break;
        }
        double scp = eval_params(p, vs);
        bestSc = (bestSc * n + scp) / (n + 1);
        n++;
        cerr << scq << " " << bestSc << endl;
        if (scq >= bestSc)
        {
            reverted = false;
            p = q;
            n = 1;
            cout << scq << endl;
            if (scq > bestSc)
                cout << q;
            bestSc = scq;
        }
        if (bestSc > ltBestSc && n > 9)
        {
            ltBestSc = bestSc;
            ltN = n;
            ltBest = p;
        }
        else if (n > ltN && reverted)
        {
            ltBestSc = bestSc;
        }
        else if (bestSc < ltBestSc && uniform_int_distribution<int>(0, 10)(re) == 0)
        {
            reverted = true;
            p = ltBest;
            n = ltN;
            bestSc = ltBestSc;
            cout << "revert to lt best " << bestSc << " / " << n << endl;
            cout << p;
        }
    }
}

int main()
{
    Params last =
    { {
2, 9, 11, 9, 7, 4, 3, -2,
-3, 0, 4, 5, 2,
},
{
7.73817, 2.91513, 0.42475, 4.77937, -0.826599, 0.691071, 0.729992, -3.01847,
-5.33083, 4.26642, 4.8868, 2.18347, 1.09028, 2.01672, 1.89978, 3.09518,
1.35242, 4.52741, 3.01027, 0.144965, -0.440247, 4.34497, 5.36517, 0,
-4.13682, 0.775486, -2.53257, 0.964975, -2.90551, -3.59518, -2.44149, -1.07429,
2.50147, 0.327159, -2.1511, -2.62134, 4.22959, 1.87049, -2.11927,
} };

    Params prev =
    { {
    -3, 5, 6, 4, 2, 2, -3, -1,
    -1, 0, -1, 2, -4,
    },
    {
    4.79313, 1.17399, 0.424348, 4.70863, -0.796309, 0.0514385, 1.17727, -2.0296,
    1.59741, 0.485915, 0.602749, 0.102713, -2.07307, -1.08749, 0.890888, 1.43694,
    -0.0226625, 0.491488, 0.651959, 0.117335, -1.13037, 1.31927, 1.42461, 0,
    -0.97392, -0.973255, -0.627406, 0, 0.218332, -0.0336394, 4.79313, 1.17399, 
    0.424348, -2.0296, 1.59741, 0.485915, 0.602749, 0.102713, -2.07307,
    } };

    Params current =
    { {
3, 4, 4, 5, 3, 3, -1, 1,
-1, -4, -4, 0, -5,
},
{
1.47362, 1.24688, 0.0494033, 0.726908, -1.03512, 1.07404, 4.52013, -0.632891,
-0.812973, -0.928142, 0.675997, 1.67538, 2.31899, -0.503139, -0.655207, 3.98904,
2.02604, -1.12797, 1.03306, 0.0985854, 0.185162, 0.972665, 1.02857, 0,
1.37501, 2.19595, 2.36287, -1.14475, -0.21112, -1.72412, 1.65212, 1.36013,
0.674527, -2.89216, -3.29959, -1.26968, -2.4993, 2.27012, -1.56656,
} };
    improve_params(current, last);
	return 0;
}
