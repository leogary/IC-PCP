#include <fstream>
#include <iostream>
#include <map>
#include <vector>

#include "gwo.hpp"
//#include "pcp.hpp"
using namespace std;
int workflownum, vmnum, temp, interval = 10, totaltasknum;
vector<int> vmprice, tasknum, deadlinelist, est, eft, lft, mincomputation, entrynode, exitnode;
vector<int> uninit;
vector<vector<vector<int>>> fcomputationmatrix;
vector<vector<vector<int>>> fcommunicationmatrix;
vector<vector<int>> computationmatrix;
vector<vector<int>> communicationmatrix;
map<int, vector<int>> front;
map<int, vector<int>> taskinterval, children, parent;
map<int, int> whichworkflow, entrytask, exittask, init;
void readdata(string s) {  // read data from file
    ifstream fin(s);
    fin >> vmnum;
    for (int i = 0; i < vmnum; i++) {
        fin >> temp;
        vmprice.push_back(temp);
    }
    fin >> workflownum;
    int sum = 0;
    for (int i = 0; i < workflownum; i++) {
        fin >> temp;
        tasknum.push_back(temp);
        for (int j = 0; j < temp; j++) {
            whichworkflow[sum] = i;
        }
        sum++;
    }
    for (int i = 0; i < workflownum; i++) {
        fin >> temp;
        deadlinelist.push_back(temp);
    }

    int tempsum = 0;
    int cur = 0;
    for (int i = 0; i < workflownum; i++) {
        for (int j = 0; j < tasknum[i]; j++) {
            vector<int> temp1;
            for (int k = 0; k < vmnum; k++) {
                fin >> temp;
                temp1.push_back(temp);
            }
            computationmatrix.push_back(temp1);
        }

        vector<vector<int>> temp3;
        for (int j = 0; j < tasknum[i]; j++) {
            vector<int> temp1;
            for (int k = 0; k < tasknum[i]; k++) {
                fin >> temp;
                temp1.push_back(temp);
            }
            temp3.push_back(temp1);
        }
        fcommunicationmatrix.push_back(temp3);
    }
    fin.close();
}
void mergeallworkflow() {  // merge all workflow to one
    int sum = 0;
    for (int i = 0; i < workflownum; i++) {
        taskinterval[i].push_back(totaltasknum);
        totaltasknum += tasknum[i];
        taskinterval[i].push_back(totaltasknum);
    }
    cout << "totaltasknum" << totaltasknum << endl;
    /*for (int i = 0; i < workflownum; i++) {
        for (int j = 0; j < tasknum[i]; j++) {
            for (int s = 0; s < tasknum[i]; s++) {
                cout << fcommmunicationmatrix[i][j][s] << " ";
            }
            cout << endl;
        }
    }*/
    for (int i = 0; i < totaltasknum; i++) {
        int cur = 0;
        vector<int> temp;
        for (int j = 0; j < totaltasknum; j++) {
            if (j < taskinterval[cur][0] || j >= taskinterval[cur][1] || i < taskinterval[cur][0] || i >= taskinterval[cur][1]) {
                temp.push_back(0);
            } else {
                temp.push_back(fcommunicationmatrix[cur][i - taskinterval[cur][0]][j - taskinterval[cur][0]]);
            }

            if (j >= taskinterval[cur][1] - 1) {
                cur++;
            }
        }
        communicationmatrix.push_back(temp);
    }
    for (int i = 0; i < totaltasknum; i++) {
        for (int j = 0; j < totaltasknum; j++) {
            cout << communicationmatrix[i][j] << " ";
        }
        cout << endl;
    }
}
void pretreatment() {  // calculate the est,eft,lft
    est.assign(totaltasknum, 0);
    eft.assign(totaltasknum, 0);
    lft.assign(totaltasknum, 999999);

    for (int i = 0; i < totaltasknum; i++) {
        uninit.push_back(i);
        // schedulevm[i] = -1;
        for (int j = 0; j < totaltasknum; j++) {
            if (communicationmatrix[i][j] != 0) {
                children[i].push_back(j);
            }
        }
    }
    for (int i = 0; i < totaltasknum; i++) {
        cout << i + 1 << "=";
        for (int j = 0; j < totaltasknum; j++) {
            if (communicationmatrix[j][i] != 0) {
                parent[i].push_back(j);
                cout << j + 1 << " ";
            }
        }
        cout << endl;
    }

    for (int i = 0; i < totaltasknum; i++) {  // find entrytask,exittask
        if (parent[i].size() == 0) {
            entrytask[i] = 1;
            entrynode.push_back(i);
        }
        if (children[i].size() == 0) {
            exittask[i] = 1;
            exitnode.push_back(i);
            lft[i] = deadlinelist[whichworkflow[i]];
        }
    }

    for (int i = 0; i < totaltasknum; i++) {  // find min computaion time on each vm
        int min = 9999;
        for (int j = 0; j < vmnum; j++) {
            if (computationmatrix[i][j] < min) {
                min = computationmatrix[i][j];
            }
        }
        mincomputation.push_back(min);
    }
    for (int i = entrynode.size() - 1; i >= 0; i--) {
        eft[entrynode[i]] = mincomputation[entrynode[i]];
        init[entrynode[i]] = 1;
        vector<int>::iterator it = find(uninit.begin(), uninit.end(), entrynode[i]);
        uninit.erase(it);
        // calculateest(entrynode[i]);
    }

    while (uninit.size() > 0) {  // init all est,eft
        for (int i = 0; i < uninit.size(); i++) {
            bool flag = true;
            for (int j = 0; j < parent[uninit[i]].size(); j++) {
                if (!init.count(parent[uninit[i]][j])) {
                    flag = false;
                    break;
                }
            }
            if (flag) {
                int max = 0;
                for (int j = 0; j < parent[uninit[i]].size(); j++) {
                    int temp1 = est[parent[uninit[i]][j]] + mincomputation[parent[uninit[i]][j]] + communicationmatrix[parent[uninit[i]][j]][uninit[i]];
                    if (max < temp1) {
                        max = temp1;
                    }
                }
                est[uninit[i]] = max;
                eft[uninit[i]] = est[uninit[i]] + mincomputation[uninit[i]];
                init[uninit[i]] = 1;
                vector<int>::iterator it = find(uninit.begin(), uninit.end(), uninit[i]);
                uninit.erase(it);
                i--;
            }
        }
    }
    init.clear();
    for (int i = 0; i < totaltasknum; i++) {
        uninit.push_back(i);
    }

    for (int i = 0; i < exitnode.size(); i++) {
        vector<int>::iterator it = find(uninit.begin(), uninit.end(), exitnode[i]);
        uninit.erase(it);
        init[exitnode[i]] = 1;
    }
    cout << "reset" << endl;
    while (uninit.size() > 0) {  // init all lft
        for (int i = uninit.size() - 1; i >= 0; i--) {
            bool flag = true;
            for (int j = 0; j < children[uninit[i]].size(); j++) {
                if (!init.count(children[uninit[i]][j])) {
                    flag = false;
                    break;
                }
            }
            if (flag) {
                int min = 9999;
                for (int j = 0; j < children[uninit[i]].size(); j++) {
                    int temp1 = lft[children[uninit[i]][j]] - mincomputation[children[uninit[i]][j]] - communicationmatrix[uninit[i]][children[uninit[i]][j]];
                    if (min > temp1) {
                        min = temp1;
                    }
                }
                lft[uninit[i]] = min;
                init[uninit[i]] = 1;
                vector<int>::iterator it = find(uninit.begin(), uninit.end(), uninit[i]);
                uninit.erase(it);

                i = uninit.size() - 1;
            }
        }
    }
    for (int i = 0; i < totaltasknum; i++) {
        cout << i << "=lftt=" << lft[i] << endl;
    }

    /*for (int k = 0; k < entrynode.size(); k++) {

        eft[temp] = mincomputation[temp];
        for (int i = 0; i < children[temp].size(); i++) {
            int max = 0;
            for (int j = 0; j < parent[children[temp][i]].size(); j++) {
                int temp1 = est[parent[children[temp][i]][j]] + mincomputation[parent[children[temp][i]][j]] + communicationmatrix[parent[children[temp][i]][j]][children[temp][i]];
                if (max < temp1) {
                    max = temp1;
                }
            }
            est[children[temp][i]] = max;
            eft[children[temp][i]] = est[children[temp][i]] + mincomputation[children[temp][i]];
        }
    }
    for (int i = 0; i < totaltasknum; i++) {
        cout << i << ":  est=" << est[i] << "   eft=" << eft[i] << endl;
    }
    for (int k = 0; k < exitnode.size(); k++) {
        temp = exitnode[k];
        for (int i = 0; i < parent[temp].size(); i++) {
            int min = lft[temp];
            for (int j = 0; j < children[children[temp][i]].size(); j++) {
                int temp1 = lft[children[parent[temp][i]][j]] -mincomputation[parent[children[temp][i]][j]] -communicationmatrix[parent[children[temp][i]][j]][children[temp][i]];
                if (min < temp1) {
                    min = temp1;
                }
            }
            est[children[temp][i]] = max;
            eft[children[temp][i]] = est[children[temp][i]] + mincomputation[children[temp][i]];
        }
    }*/
    /*for (int i = 0; i < totaltasknum; i++) {  // calculate est,eft
        cout << "i=" << i << endl;
        if (entrytask.count(i)) {
            est[i] = 0;
            eft[i] = est[i] + mincomputation[i];
            cout << "a" << endl;
            continue;
        }
        for (int j = 0; j < parent[i].size(); j++) {
            cout << "b" << endl;
            int sum = est[parent[i][j]] + communicationmatrix[parent[i][j]][i] + mincomputation[parent[i][j]];
            if (est[i] < sum) {
                est[i] = sum;
            }
        }
        eft[i] = est[i] + mincomputation[i];
    }*/
    /*cout << "aaa" << endl;
    for (int i = totaltasknum - 1; i >= 0; i--) {  // calculate lft
        if (exittask.count(i)) {
            lft[i] = deadlinelist[whichworkflow[i]];
            continue;
        }
        for (int j = 0; j < children[i].size(); j++) {
            int dif = lft[children[i][j]] - mincomputation[children[i][j]] - communicationmatrix[i][children[i][j]];
            if (lft[i] > dif) {
                lft[i] = dif;
            }
        }
    }*/
}
void parentcheck(int p, int c) {
    for (int j = 0; j < parent[p].size(); j++) {
        vector<int>::iterator it = find(front[c].begin(), front[c].end(), parent[p][j]);
        if (it == front[c].end()) {
            front[c].push_back(parent[p][j]);
        }
        if (parent[parent[p][j]].size() > 0) {
            parentcheck(parent[p][j], c);
        }
    }
}
void check(int task) {
    for (int j = 0; j < parent[task].size(); j++) {
        vector<int>::iterator it = find(front[task].begin(), front[task].end(), parent[task][j]);
        if (it == front[task].end()) {
            front[task].push_back(parent[task][j]);
        }
        check(parent[task][j]);
    }
}
void findfront() {
    for (int i = 0; i < totaltasknum; i++) {
        if (entrytask[i] == 1) {
            continue;

        } else {
            for (int j = 0; j < parent[i].size(); j++) {
                check(i);
                parentcheck(parent[i][j], i);
            }
        }
    }
}

int main() {
    readdata("D:\\c++\\c++\\researchcode\\paperexample.txt");
    mergeallworkflow();
    pretreatment();
    findfront();

    cout << "end pretreatment" << endl;
    for (int i = 0; i < totaltasknum; i++) {
        cout << i + 1 << "  :  est=" << est[i] << "   eft=" << eft[i] << "   lft=" << lft[i] << endl;
    }
    cout << "vmnum=" << vmnum << endl;
    for (int i = 0; i < vmnum; i++) {
        cout << vmprice[i] << " ";
    }
    cout << endl;
    cout << workflownum << endl;

    gwo t(5, 10, totaltasknum, vmnum, interval, est, eft, lft, mincomputation, children, parent, front, entrytask, exittask, deadlinelist, vmprice, whichworkflow, computationmatrix, communicationmatrix);
    t.run();
    // vector<int> a = t.permutation();
    /* for (int i = 0; i < totaltasknum; i++) {
         cout << a[i] + 1 << " ";
     }*/
    cout << endl;
    // cout << "cost=" << t.getcost() << endl;
}