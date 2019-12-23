// LegendsOfCodeAndMagic.cpp : Defines the entry point for the application.
//

#include "LegendsOfCodeAndMagic.h"
#include "GameController.h"
#include <chrono>
#include <conio.h>
#include <map>

using namespace std;

double eval_params(const Params& a, const Params& b)
{
    int a_wins = 0;
    static int seed = int(std::chrono::duration_cast<std::chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count());

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
    map<size_t, pair<Params, double>> ltBest;
    ltBest[1] = { p, bestSc };
    size_t last_revert = 1;
    double last_revert_sc = bestSc;

    while (!_kbhit())
    {
        Params q = p;
        mutate_params(q, re);
        double scq = 0, tscq = 0;
        size_t qn = 0;
        for (size_t i = 0; i < n; i++)
        {
            tscq += eval_params(q, vs);
            qn++;
            scq = tscq / qn;
            if (scq < bestSc)
                break;
        }
        double scp = eval_params(p, vs);
        bestSc = (bestSc * n + scp) / (n + 1);
        n++;
        cerr << scq << " vs " << bestSc << " / " << n << endl;
        if (scq >= bestSc || uniform_int_distribution<int>(0, 20)(re) == 0)
        {
            last_revert = 0;
            p = q;
            n = qn;
            if (scq > bestSc)
            {
                cout << q;
                bestSc = scq;
            }
        }

        if (last_revert != 0)
        {
//            cerr << "update " << last_revert << endl;
            ltBest[last_revert].second = bestSc;
        }

        if (n != 0 && (n & (n - 1)) == 0)
        {
//            cout << "check best for " << n << endl;
            if (bestSc > ltBest[n].second)
            {
                ltBest[n] = { p, bestSc };
            }
        }

        if (bestSc + 0.05 < last_revert_sc || n > 32 || uniform_int_distribution<size_t>(0, n*2)(re) == 0)
        {
            size_t r = uniform_int_distribution<size_t>(0, ltBest.size() - 1)(re);
            size_t rn = 1ull << r;
//            cout << "check update" << ltBest.size() << " " << r << " " << rn << endl;
            last_revert = rn;
            if (bestSc < ltBest[rn].second)
            {
                p = ltBest[rn].first;
                n = rn;
                bestSc = ltBest[rn].second;
                last_revert_sc = bestSc;
                ltBest[rn].second -= 0.001;
                cout << "revert to lt best " << bestSc << " / " << rn << endl;
                cout << p;
            }
        }
    }

    for (const auto& lt : ltBest)
    {
        cout << "n=" << lt.first << " score=" << lt.second.second << endl;
        cout << lt.second.first;
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
4, 4, 4, 5, 3, 3, -3, -1,
-1, -3, -4, -1, -6,
},
{
1.83846, 1.41196, 0.0494033, 0.726908, -1.03512, 1.07404, 4.62533, -0.8087,
-1.03991, 0.627651, 1.10518, 1.67538, 2.31899, -0.503139, -0.655207, 3.98904,
1.5107, -0.913315, 1.03306, 0.0985854, -0.0674146, 0.972665, 1.02857, 0,
1.89875, 2.77945, 2.36287, -1.87586, 0.0677042, -1.64044, 1.65212, 1.20588,
0.674527, -2.55321, -3.1398, -0.412947, -1.79981, 4.72006, -2.28103,
} };
    improve_params(current, last);
	return 0;
}
