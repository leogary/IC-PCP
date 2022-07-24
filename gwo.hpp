#include <iostream>
#include <map>
#include <random>
#include <vector>
using namespace std;

#include "pcp.hpp";
std::random_device rd;
std::mt19937 generator(rd());
class gwo {
   public:
    int tasknum = 9, vmnum = 3, interval = 10, generation, population;
    vector<int> est;
    vector<int> eft;
    vector<int> lft;
    vector<int> mincomputation;
    vector<int> deadlinelist;
    vector<vector<int>> computationmatrix;
    vector<vector<int>> communicationmatrix;
    vector<int> vmprice;
    vector<int> presenttask;
    vector<int> successtask;

    map<int, int> entrytask;
    map<int, int> exittask;
    map<int, int> whichworkflow;
    map<int, vector<int>> front;
    map<int, vector<int>> parent;
    map<int, vector<int>> children;
    struct sol {
        int fitness;
        vector<double> doubleassignedinstance;
        map<int, vector<int>> assignedservice;
        vector<int> schedule;
    };
    map<int, sol> solset;
    gwo(int generation, int population, int tasknum, int vmnum, int interval, vector<int> est, vector<int> eft, vector<int> lft, vector<int> mincomputation, map<int, vector<int>> children, map<int, vector<int>> parent, map<int, vector<int>> front, map<int, int> entrytask, map<int, int> exittask, vector<int> deadlinelist, vector<int> vmprice, map<int, int> whichworkflow, vector<vector<int>> computationmatrix, vector<vector<int>> communicationmatrix) {
        this->generation = generation;
        this->population = population;
        this->tasknum = tasknum;
        this->vmnum = vmnum;
        this->interval = interval;
        this->children = children;
        this->parent = parent;
        this->entrytask = entrytask;
        this->front = front;
        this->exittask = exittask;
        this->est.assign(est.begin(), est.end());
        this->eft.assign(eft.begin(), eft.end());
        this->lft.assign(lft.begin(), lft.end());
        this->mincomputation.assign(mincomputation.begin(), mincomputation.end());
        this->deadlinelist.assign(deadlinelist.begin(), deadlinelist.end());
        this->vmprice.assign(vmprice.begin(), vmprice.end());
        this->whichworkflow = whichworkflow;
        this->computationmatrix.assign(computationmatrix.begin(), computationmatrix.end());
        this->communicationmatrix.assign(communicationmatrix.begin(), communicationmatrix.end());
    }
    void crossover();
    void mutation();
    int getcost();
    struct sol newsol() {
    }
    void init() {
        pcp t(tasknum, vmnum, interval, est, eft, lft, mincomputation, children, parent, entrytask, exittask, deadlinelist, vmprice, whichworkflow, computationmatrix, communicationmatrix);
        t.run();
        solset[0].fitness = t.cost;
        solset[0].assignedservice = t.scheduleinstance;
        for (int i = 0; i < tasknum; i++) {
            int a = solset[0].assignedservice[i][0];
            solset[0].doubleassignedinstance.push_back((double)a);
        }
        solset[0].schedule = t.permutation();
        for (int i = 1; i < population; i++) {
            std::uniform_real_distribution<double> unif(0, vmnum - 0.0000001);
            vector<int> temp;
            for (int j = 0; j < tasknum; j++) {
                solset[i].doubleassignedinstance.push_back(unif(generator));
                int a = solset[i].doubleassignedinstance[j] / 1;
                solset[i].assignedservice[j].push_back(a);
                temp.push_back(j);
            }

            while (temp.size() > 0) {
                std::uniform_int_distribution<int> unif2(0, temp.size() - 1);
                vector<int>::iterator it = find(temp.begin(), temp.end(), temp[unif2(generator)]);

                solset[i].schedule.push_back(*it);
                temp.erase(it);
            }
            cout << "before p" << i << " ";
            for (int k = 0; k < tasknum; k++) {
                cout << solset[i].schedule[k] + 1 << " ";
            }
            cout << endl;
            checkschedule(i);
            cout << "after p" << i << " ";
            for (int k = 0; k < tasknum; k++) {
                cout << solset[i].schedule[k] + 1 << " ";
            }
            cout << endl;
        }
    }
    void checkschedule(int solutionindex) {
        /*for (int i = 0; i < vmnum; i++) {
            cout << "vm=" << i << endl;
            int index1 = -1, index2 = -1;*/

        for (int j = 0; j < tasknum; j++) {
            int index1, index2;

            int t1 = solset[solutionindex].schedule[j];

            index1 = j;

            for (int k = j; k < tasknum; k++) {
                int t2 = solset[solutionindex].schedule[k];

                index2 = k;
                vector<int>::iterator it = find(front[t1].begin(), front[t1].end(), t2);
                // cout << "*it=" << *it << endl;
                if (it != front[t1].end()) {
                    int temp;
                    temp = solset[solutionindex].schedule[j];
                    solset[solutionindex].schedule[j] = solset[solutionindex].schedule[k];
                    solset[solutionindex].schedule[k] = temp;
                    j--;
                    break;
                }
            }
        }
        /*for (int k = 0; k < tasknum; k++) {
            int t = solset[solutionindex].schedule[k];
            if (solset[solutionindex].assignedservice[t][0] == i && !flag) {
                index1 = k;
            } else if (solset[solutionindex].assignedservice[t][0] == i && flag) {
                index2 = k;
                int t1 = solset[solutionindex].schedule[index1], t2 = solset[solutionindex].schedule[index2];
                vector<int>::iterator it;
                if(it==find(t1.begi))
            }
        }*/
        // }
    }
    bool check(int task1, int task2) {
    }
    void run() {
        cout << "intit" << endl;
        init();
        for (int i = 0; i < generation; i++) {
            cout << "generation" << i << ":" << endl;
            for (int j = 0; j < population; j++) {
                cout << "p" << j << " ";
                for (int k = 0; k < tasknum; k++) {
                    cout << solset[j].schedule[k] << " ";
                }
                cout << endl;
            }
        }
    };
};