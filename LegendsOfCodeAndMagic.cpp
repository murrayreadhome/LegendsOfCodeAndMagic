// LegendsOfCodeAndMagic.cpp : Defines the entry point for the application.
//

#include "LegendsOfCodeAndMagic.h"
#include "GameController.h"
#include <chrono>
#include <conio.h>
#include <map>

using namespace std;

double eval_params(const Params& a, const Params& b, int n=1000)
{
    int a_wins = 0;
    static int seed = int(std::chrono::duration_cast<std::chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count());

    for (int i = 0; i < n/2; i++)
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

    return double(a_wins) / n;
}

void mutate_params(Params& p, mt19937& re)
{
    do
    {
        size_t ni = sizeof(p.i) / sizeof(int);
        size_t nd = sizeof(p.d) / sizeof(double);
        size_t n = uniform_int_distribution<size_t>(0, ni + nd - 1)(re);
        if (n < ni)
        {
            int& i = reinterpret_cast<int*>(&p.i)[n];
            i += uniform_int_distribution<int>(-2, 2)(re);
        }
        else
        {
            n -= ni;
            double& d = reinterpret_cast<double*>(&p.d)[n];
            if (uniform_int_distribution<size_t>(0, 1)(re) == 0)
                d += uniform_real_distribution<double>(-1.0, 1.0)(re);
            else
                d *= exp(uniform_real_distribution<double>(-1.0, 1.0)(re));
        }
    } while (uniform_int_distribution<size_t>(0, 1)(re) == 0);
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

void tournament(const vector<Params>& params)
{
    size_t n = params.size();
    vector<double> scores(n, 0);
    for (size_t i = 0; i < n; i++)
    {
        for (size_t j = i + 1; j < n; j++)
        {
            double i_wins = eval_params(params[i], params[j], 10000);
            cout << i << " vs " << j << " " << i_wins << endl;
            scores[i] += i_wins;
            scores[j] += 1.0 - i_wins;
        }
    }

    for (size_t i = 0; i < n; i++)
        cout << i << " " << scores[i]/(n-1) << endl;
}

int main()
{
    Params gold32 =
    { {
2, 9, 11, 9, 7, 4, 3, -2,
-3, 0, 4, 5, 2,
},
{
7.73817, 2.91513, 0.42475, 4.77937, -0.826599, 0.691071, 0.729992, -3.01847,
-5.33083, 4.26642, 4.8868, 2.18347, 1.09028, 2.01672, 1.89978, 3.09518,
1.35242, 4.52741, 3.01027, 0.144965, -0.440247, 4.34497, 5.36517, 0,
-4.13682, 0.775486, -2.53257, 0.964975, -2.90551, -3.59518, -2.44149, -1.07429,
2.50147, 0.327159, -2.1511, -2.62134, 4.22959, 1.87049, -2.11927, 0, 0, 0.327159, -2.1511, -2.62134, 4.22959, 1.87049, -2.11927,
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

    Params gold57=
    { {
2, 3, 5, 5, 3, 2, 2, -2,
-5, -2, -1, 0, -2,
},
{
2.69287, 1.93944, 0.218916, 0.954512, -1.04002, 0.99281, 4.60915, -1.16954,
2.53758, 2.43279, 0.635511, 4.82829, 2.74237, 0.634413, -0.941901, 4.66539,
2.1327, -2.81415, 1.3494, 0.174386, -2.43276, 1.75129, 1.93385, 0,
1.97353, 0.584058, 2.94098, -0.893542, 7.60946, 1.6964, 3.13547, 1.5531,
0.703979, -3.93893, -1.93636, -4.59371, 0.132084, 8.15254, -2.93104, 0, 0,  -3.93893, -1.93636, -4.59371, 0.132084, 8.15254, -2.93104,
} };

    Params gold56 =
    { {
-6, 6, 9, 9, 7, 6, 4, 4,
6, -8, 0, 4, -7,
},
{
8.25737, 3.45518, 0.568089, 0.0932142, -1.04358, 0.98962, 8.93013, -0.201248,
0.0830147, 1.89789, 3.23594, 6.99142, 2.3462, -1.91313, 4.03476, 8.45027,
-3.43596, 1.16945, 2.50136, 0.975953, 0.321074, 2.11444, 1.59877, 0,
0.394601, 6.98325, 2.68533, -8.51471, 12.2219, 1.04509, 7.00152, 1.48917,
0.303177, 1.8464, -2.42281, -3.00746, 0.320811, 7.63241, 0.196139, 1.2679,
1.39347, -7.002, -0.217405, -0.640002, 1.04721, 4.44642, 2.53409, -1.544,
2.54769, 2.59733, -3.83357, 2.70376, 1.19074, -0.286425, -0.0729417, -2.06092,
-3.54413,
} };
    Params gold31 =
    { {
    7, 6, 12, 10, 10, 8, 6, 4,
    5, -10, -10, 5, -4,
    },
    {
    7.61404, 6.73621, 0.00703978, -0.605314, -1.18427, -25.1146, 0.726403, -0.719495,
    4.99645, 0.130773, 0.244524, 0.974274, 0.0252914, -0.709927, 0.97464, 0.737504,
    -1.5623, -5.09058, -1.53143, 0.224828, -0.0458732, 1.22855, 1.57064, 0,
    2.83786, 4.61971, 0.2521, 2.94954, 15.5086, 3.53728, 0.690261, -1.44891,
    2.04015, -0.983451, -1.55338, 2.19156, 3.15911, 9.07261, -0.884952, 0.727166,
    1.26622, -62.8933, -6.71968, -0.970392, -9.02535, -0.597951, 1.06167, -1.37738,
    3.29726, 9.94, -2.68016, 1.65906, 0.378562, 2.34008, 0.0208961, -1.55224,
    0.236862,
    } };

    Params current = 
    { {
    0, 4, 4, 3, 3, 3, 0, 0,
    -2, 0, 0, -1, 0,
    },
    {
    1.38956, 1.87169, 1, 1, -1.17939, 6.60737e-05, 1, 0,
    0, 0, 0.445612, 0, 0, 0, 0.433479, 0,
    0, 0, 0, 1, 0.5, 1, 2.49716, 0,
    0, 0, 0, 0, 0, 0, 1, 1,
    1.89977, 0, 0, 0, 0.088106, 0, 0.482735, 0,
    0, 0, 0, 0.0141146, 0, 0, 0, 1.05411,
    1.83387, 1, 0, 0.420305, 0, 0, 0, 0,
    -7,
    } };

    improve_params(current, gold31);

    //tournament({ gold32, gold57, gold56, gold31 });
	return 0;
}
