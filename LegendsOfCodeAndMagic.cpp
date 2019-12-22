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
    size_t nd = sizeof(p.d) / sizeof(int);
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
    size_t nd = sizeof(p.d) / sizeof(int);
    const int* pi = reinterpret_cast<const int*>(&p.i);
    const double* pd = reinterpret_cast<const double*>(&p.d);

    out << "{{" << endl;
    for (int i = 0; i < ni; i++)
        out << "    " << pi[i] << "," << endl;
    out << "},\n{" << endl;
    for (int i = 0; i < nd; i++)
        out << "    " << pd[i] << "," << endl;
    out << "}};" << endl;

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
        if (bestSc > ltBestSc && n > 5)
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
    improve_params(Params(), Params());
	return 0;
}
