
#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;

class pcp {
   public:
    int tasknum = 9, vmnum = 3, interval = 10;
    vector<int> est;
    vector<int> eft;
    vector<int> lft;
    vector<int> mincomputation;
    vector<int> scheduled;
    vector<int> unassigned;
    int deadline;
    vector<int> deadlinelist;
    vector<vector<int> > computationmatrix;
    vector<vector<int> > communicationmatrix;
    vector<int> vmprice;
    vector<int> presenttask;
    vector<int> successtask;
    map<int, int> notentrytask;
    map<int, int> notexittask;
    map<int, int> entrytask;
    map<int, int> exittask;
    map<int, int> whichworkflow;
    map<int, vector<int> > parent;
    map<int, vector<int> > children;
    map<int, int> schedulevm;
    map<int, vector<int> > vmpcp;  // each partial critical path
    map<int, vector<int> > scheduleinstance;
    int pcpnum = 0;
    map<int, int> instancenum;               // each vm's instancenum;
    map<vector<int>, int> instanceinterval;  // each instances'intervalnum
    map<vector<int>, int> instanceidletime;
    map<vector<int>, int> instanceremaintime;

    int cost = 0;
    pcp(int tasknum, int vmnum, int interval, vector<int> est, vector<int> eft, vector<int> lft, vector<int> mincomputation, map<int, vector<int> > children, map<int, vector<int> > parent, map<int, int> entrytask, map<int, int> exittask, vector<int> deadlinelist, vector<int> vmprice, map<int, int> whichworkflow, vector<vector<int> > computationmatrix, vector<vector<int> > communicationmatrix) {
        scheduled.assign(tasknum, -1);

        this->tasknum = tasknum;
        this->vmnum = vmnum;
        this->interval = interval;
        this->children = children;

        this->parent = parent;
        this->entrytask = entrytask;
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

    // vector<int> successor;
    // vector<int> predecssors;
    // int findwhichpcp(int task);
    // void findpartial(int task, vector<int> v);

    void updatesuccessor(int task) {  // update est,eft of successor after assinged service to pcp
        for (int i = 0; i < children[task].size(); i++) {
            if (scheduled[children[task][i]] == 1) {
                continue;
            }
            int temp = eft[task] + communicationmatrix[task][children[task][i]];
            if (temp > est[children[task][i]]) {
                est[children[task][i]] = temp;
                eft[children[task][i]] = est[children[task][i]] + mincomputation[children[task][i]];
            }

            updatesuccessor(children[task][i]);
        }
    }
    void updatepredecessor(int task) {  // update lft of predecessor after assinged service to pcp
        // cout << "task=" << task;
        for (int i = 0; i < parent[task].size(); i++) {
            /*int temp;
            if (scheduled[parent[task][i]] == 1 && scheduled[task] == 1) {
                if (children[parent[task][i]].size() == 1 && schedulevm[task] == schedulevm[parent[task][i]]) {
                    cout << "a" << endl;
                    lft[parent[task][i]] = lft[task] - computationmatrix[task][schedulevm[task]];
                    updatepredecessor(parent[task][i]);
                    continue;
                } else if (schedulevm[task] == schedulevm[parent[task][i]]) {
                    cout << "b" << endl;
                    temp = lft[task] - computationmatrix[task][schedulevm[task]];
                } else {
                    cout << "c" << endl;
                    temp = lft[task] - computationmatrix[task][schedulevm[task]] - communicationmatrix[parent[task][i]][task];
                }
            } else {
                temp = lft[task] - computationmatrix[task][schedulevm[task]] - communicationmatrix[parent[task][i]][task];
                lft[parent[task][i]] = temp;
            }
            if (temp < lft[parent[task][i]] || children[parent[task][i]].size() == 1) {
                // cout << task << "update" << parent[task][i] << endl;
                lft[parent[task][i]] = temp;
            }
            cout << "temp=" << temp << endl;*/
            // cout << "parent[task]=" << parent[task][i] << endl;
            int min = deadlinelist[whichworkflow[task]];
            for (int j = 0; j < children[parent[task][i]].size(); j++) {
                int temp = deadlinelist[whichworkflow[task]];
                if (scheduled[parent[task][i]] == 1 && scheduled[children[parent[task][i]][j]] == 1) {
                    if (findwhichpcp(parent[task][i]) == findwhichpcp(children[parent[task][i]][j])) {
                        // cout << "a" << endl;
                        temp = lft[children[parent[task][i]][j]] - computationmatrix[children[parent[task][i]][j]][schedulevm[children[parent[task][i]][j]]];
                    } else {
                        // cout << "b" << endl;
                        temp = lft[children[parent[task][i]][j]] - computationmatrix[children[parent[task][i]][j]][schedulevm[children[parent[task][i]][j]]] - communicationmatrix[parent[task][i]][children[parent[task][i]][j]];
                        ;
                    }
                } else if (scheduled[children[parent[task][i]][j]] == 1) {
                    // cout << "c" << endl;
                    temp = lft[children[parent[task][i]][j]] - computationmatrix[children[parent[task][i]][j]][schedulevm[children[parent[task][i]][j]]] - communicationmatrix[parent[task][i]][children[parent[task][i]][j]];
                } else {
                    // cout << "d" << endl;
                    temp = lft[children[parent[task][i]][j]] - mincomputation[children[parent[task][i]][j]] - communicationmatrix[parent[task][i]][children[parent[task][i]][j]];
                    ;
                }
                if (min > temp) {
                    min = temp;
                }
            }
            lft[parent[task][i]] = min;
            updatepredecessor(parent[task][i]);
        }
    }
    int getcost() {
        return cost;
    }
    /*void pretreatment() {  // calculate the est,eft,lft
        for (int i = 0; i < tasknum; i++) {
            schedulevm[i] = -1;
            for (int j = 0; j < tasknum; j++) {
                if (communicationmatrix[i][j] != 0) {
                    children[i].push_back(j);
                }
            }
        }
        for (int i = 0; i < tasknum; i++) {
            cout << i + 1 << "=";
            for (int j = 0; j < tasknum; j++) {
                if (communicationmatrix[j][i] != 0) {
                    parent[i].push_back(j);
                    cout << j + 1 << " ";
                }
            }
            cout << endl;
        }
        for (int i = 0; i < tasknum; i++) {  // find entrytask,exittask
            if (parent[i].size() == 0) {
                entrytask[i] = 1;
            }
            if (children[i].size() == 0) {
                exittask[i] = 1;
            }
        }
        for (int i = 0; i < tasknum; i++) {  // find min computaion time on each vm
            int min = 9999;
            for (int j = 0; j < vmnum; j++) {
                if (computationmatrix[i][j] < min) {
                    min = computationmatrix[i][j];
                }
            }
            mincomputation.push_back(min);
        }

        for (int i = 0; i < tasknum; i++) {  // calculate est,eft
            if (entrytask.count(i)) {
                est[i] = 0;
                eft[i] = est[i] + mincomputation[i];
                continue;
            }
            for (int j = 0; j < parent[i].size(); j++) {
                int sum = est[parent[i][j]] + communicationmatrix[parent[i][j]][i] + mincomputation[parent[i][j]];
                if (est[i] < sum) {
                    est[i] = sum;
                }
            }
            eft[i] = est[i] + mincomputation[i];
        }
        for (int i = tasknum - 1; i >= 0; i--) {  // calculate lft
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
        }
    }
    */

    vector<int> findcriticalpath() {  // find pcp from exitnode
        bool finished = false;
        vector<int> pcp;
        int max = 0, back = -1;
        for (int i = 0; i < unassigned.size(); i++) {
            int temp;
            if (exittask[unassigned[i]] == 1) {
                temp = eft[unassigned[i]];

            } else {
                temp = 0;
                for (int j = 0; j < children[unassigned[i]].size(); j++) {
                    int sum = eft[unassigned[i]] + communicationmatrix[unassigned[i]][children[unassigned[i]][j]];
                    if (sum > temp) {
                        temp = sum;
                    }
                }
            }
            if (max < temp) {
                max = temp;
                back = unassigned[i];
            }
        }
        cout << "back=" << back << endl;
        pcp.push_back(back);
        // predecssors.push_back(back);
        scheduled[back] = 1;

        vector<int>::iterator it = find(unassigned.begin(), unassigned.end(), back);
        cout << "erase first" << *it << endl;
        // unassigned.erase(it);
        for (auto i = unassigned.begin(); i != unassigned.end(); i++) {
            // cout << *i << endl;
        }

        // cout << unassigned.size() << endl;
        while (parent[back].size() != 0) {
            max = 0;
            int cur = -1;
            for (int i = 0; i < parent[back].size(); i++) {
                int sum;
                if (scheduled[parent[back][i]] == 1) {
                    continue;
                }
                // cout << parent[back][i] + 1 << " =" << eft[parent[back][i]] << " +" << communicationmatrix[parent[back][i]][back] << endl;
                sum = eft[parent[back][i]] + communicationmatrix[parent[back][i]][back];
                if (sum > max) {
                    max = sum;
                    cur = i;
                }
            }
            if (cur != -1) {
                int a = back;
                back = parent[back][cur];
                cout << "back=" << back << endl;
                // predecssors.push_back(back);
                // successor.push_back(back);
                it = find(unassigned.begin(), unassigned.end(), back);
                cout << "loop erase" << *it << endl;
                // unassigned.erase(it);
                pcp.push_back(back);
                scheduled[back] = 1;
                // parent[a].erase(parent[a].begin() + cur);  // remove assigned to critical path's task
            } else {
                break;
            }
        }
        cout << "END PCP" << endl;
        return pcp;
    }
    void schedulepath(vector<int> cp) {  // assigned service  for pcp
        if (cp.size() == 0) {
            return;
        }
        cout << "schedule  path";
        for (int i = 0; i < cp.size(); i++) {
            cout << cp[i] << endl;
        }
        int max = 0;
        for (int i = vmnum - 1; i >= 0; i--) {
            bool flag = true;
            int j = cp.size() - 1, fronttime = est[cp[j]], first;
            /*if (parent[cp[j]].size() == 1 && schedulevm[parent[cp[j]][0]] == i) {
                fronttime = eft[parent[cp[j]][0]];
                first = eft[parent[cp[j]][0]];
            }*/

            while (j >= 0) {  // if all task on pcp can finish before its lft return true
                cout << cp[j] << "first=" << est[cp[j]] << "=   " << fronttime + computationmatrix[cp[j]][i] << "    lft=" << lft[cp[j]] << endl;
                if (fronttime + computationmatrix[cp[j]][i] > lft[cp[j]]) {
                    flag = false;
                    break;
                }
                fronttime = fronttime + computationmatrix[cp[j]][i];
                j--;
            }
            if (flag) {
                for (int k = cp.size() - 1; k >= 0; k--) {
                    int a = cp[k];
                    vmpcp[pcpnum].push_back(a);
                    schedulevm[cp[k]] = i;
                    if (k == cp.size() - 1) {
                        /*if (parent[cp[k]].size() == 1 && schedulevm[parent[cp[k]][0]] == i) {
                            est[cp[k]] = first;
                            eft[cp[k]] = first + computationmatrix[cp[k]][i];
                        } else {
                            eft[cp[k]] = est[cp[k]] + computationmatrix[cp[k]][i];
                        }*/
                        eft[cp[k]] = est[cp[k]] + computationmatrix[cp[k]][i];

                    } else {
                        est[cp[k]] = eft[cp[k + 1]];
                        eft[cp[k]] = est[cp[k]] + computationmatrix[cp[k]][i];
                    }
                    scheduled[cp[k]] = 1;
                    vector<int>::iterator it = find(unassigned.begin(), unassigned.end(), cp[k]);
                    cout << "erase" << *it << endl;
                    unassigned.erase(it);
                }
                pcpnum++;
                cout << "scheduled vm =" << i << endl;

                break;
            }
        }
        /*for (int i = 0; i < tasknum; i++) {
            cout << i << "  :  est=" << est[i] << "   eft=" << eft[i] << "   lft=" << lft[i] << endl;
        }*/

        for (int i = 0; i < cp.size(); i++) {
            updatesuccessor(cp[i]);
        }

        for (int i = 0; i < cp.size(); i++) {
            updatepredecessor(cp[i]);
        }
        for (int i = 0; i < tasknum; i++) {
            cout << i << "  :  est=" << est[i] << "   eft=" << eft[i] << "   lft=" << lft[i] << endl;
        }
        cout << "find partial" << endl;
        for (int i = cp.size() - 1; i >= 0; i--) {
            vector<int> temp;
            findpartial(cp[i], temp);
        }
        cout << "update" << endl;
        for (int i = 0; i < tasknum; i++) {
            cout << i << "  :  est=" << est[i] << "   eft=" << eft[i] << "   lft=" << lft[i] << endl;
        }
    }
    void findpartial(int task, vector<int> v) {  // find pcp not from exitnode

        for (auto it = v.begin(); it != v.end(); it++) {
            cout << *it << endl;
        }
        vector<int> partial;
        int max = 0, maxtask = -1;
        for (int i = 0; i < parent[task].size(); i++) {
            if (scheduled[parent[task][i]] == 1) {
                continue;
            }
            int temp = est[parent[task][i]] + communicationmatrix[parent[task][i]][task];
            if (max < temp) {
                max = temp;
                maxtask = parent[task][i];
            }
        }
        if (maxtask != -1) {
            v.push_back(maxtask);
        }

        for (int i = 0; i < parent[task].size(); i++) {
            vector<int> notcp;
            if (scheduled[parent[task][i]] == 1) {
                continue;
            } else if (parent[task][i] == maxtask) {
                cout << "max" << endl;
                findpartial(parent[task][i], v);
            } else {
                cout << "notmax" << endl;
                notcp.push_back(parent[task][i]);
                findpartial(parent[task][i], notcp);
            }
        }
        cout << "task=" << task << "vsize=" << v.size() << endl;
        for (int i = 0; i < v.size(); i++) {
            cout << "v[]=" << v[i] << endl;
            if (scheduled[v[i]] == 1) {
                vector<int>::iterator it = v.begin() + i;
                v.erase(it);
                i--;
            }
        }
        cout << "show end" << endl;
        cout << v.size() << endl;
        if (v.size() > 0) {
            schedulepath(v);
        }
    }
    int findwhichpcp(int task) {  // find this task belong to which pcp

        for (int i = 0; i < pcpnum; i++) {
            for (int j = 0; j < vmpcp[i].size(); j++) {
                if (task == vmpcp[i][j]) {
                    return i;
                }
            }
        }
    }
    bool check(int fronttime, int task, vector<int> instance) {  // check if assigned is validable
        bool flag = true;

        for (int i = 0; i < children[task].size(); i++) {
            if (scheduleinstance.count(children[task][i])) {
                if (scheduleinstance[children[task][i]][0] == instance[0] && scheduleinstance[children[task][i]][1] == instance[1]) {
                    int temp = fronttime + computationmatrix[children[task][i]][instance[0]];
                    if (temp > deadlinelist[whichworkflow[task]]) {
                        flag = false;
                        if (!flag) {
                            return false;
                        }
                    } else {
                        flag = check(temp, children[task][i], instance);
                        if (!flag) {
                            return false;
                        }
                    }
                } else {
                    int temp = fronttime + computationmatrix[children[task][i]][scheduleinstance[children[task][i]][0]] + communicationmatrix[task][children[task][i]];
                    if (temp > deadlinelist[whichworkflow[task]]) {
                        flag = false;
                        if (!flag) {
                            return false;
                        }
                    } else {
                        flag = check(temp, children[task][i], {scheduleinstance[children[task][i]][0], scheduleinstance[children[task][i]][1]});
                        if (!flag) {
                            return false;
                        }
                    }
                }

            } else if (schedulevm[children[task][i]] == instance[0]) {
                int temp = fronttime + computationmatrix[children[task][i]][instance[0]];
                if (temp > deadlinelist[whichworkflow[task]]) {
                    flag = false;
                    if (!flag) {
                        return false;
                    }
                } else {
                    if (exittask[task] != 1) {
                        flag = check(temp, children[task][i], {instance[0], -1});
                    }

                    if (!flag) {
                        return false;
                    }
                }

            } else {
                int temp = est[children[task][i]] + computationmatrix[children[task][i]][instance[0]] + communicationmatrix[task][children[task][i]];
                if (temp > deadlinelist[whichworkflow[task]]) {
                    flag = false;
                    if (!flag) {
                        return false;
                    }
                } else {
                    if (exittask[task] != 1) {
                        flag = check(temp, children[task][i], {scheduleinstance[children[task][i]][0], -1});
                    }
                    if (!flag) {
                        return false;
                    }
                }
            }
        }

        return true;
    }
    void update(int task) {  // update est after assigned instance
        for (int i = 0; i < children[task].size(); i++) {
            if (scheduleinstance.count(children[task][i])) {
                if (eft[task] > est[children[task][i]] && scheduleinstance[task][0] == scheduleinstance[children[task][i]][0] && scheduleinstance[task][1] == scheduleinstance[children[task][i]][1]) {
                    est[children[task][i]] = eft[task];
                    eft[children[task][i]] = est[children[task][i]] + computationmatrix[children[task][i]][schedulevm[children[task][i]]];
                } else {
                    est[children[task][i]] = eft[task] + communicationmatrix[task][children[task][i]];
                    eft[children[task][i]] = est[children[task][i]] + computationmatrix[children[task][i]][schedulevm[children[task][i]]];
                }
                update(children[task][i]);
            } else {
                if (eft[task] > est[children[task][i]] && findwhichpcp(task) == findwhichpcp(children[task][i])) {
                    est[children[task][i]] = eft[task];
                    eft[children[task][i]] = est[children[task][i]] + computationmatrix[children[task][i]][schedulevm[children[task][i]]];
                } else {
                    int temp = findwhichpcp(children[task][i]);
                    int sum = 0;
                    for (int k = 0; k < vmpcp[temp].size(); k++) {
                        sum += computationmatrix[vmpcp[temp][k]][schedulevm[task]];
                    }
                    if (lft[children[task][i]] > instanceidletime[{scheduleinstance[task][0], scheduleinstance[task][1]}] + sum && schedulevm[task] == schedulevm[children[task][i]]) {
                        est[children[task][i]] = instanceidletime[{scheduleinstance[task][0], scheduleinstance[task][1]}];
                        eft[children[task][i]] = instanceidletime[{scheduleinstance[task][0], scheduleinstance[task][1]}] + sum;
                    } else {
                        est[children[task][i]] = eft[task] + communicationmatrix[task][children[task][i]];
                        eft[children[task][i]] = est[children[task][i]] + computationmatrix[children[task][i]][schedulevm[children[task][i]]];
                    }
                }
                update(children[task][i]);
            }
        }
    }
    void scheduleandcost() {  // assigned instance  and update instance status
        cout << "pcpnum=" << pcpnum << endl;

        for (int i = 0; i < pcpnum; i++) {
            int vm = schedulevm[vmpcp[i][0]];
            cout << "start" << endl;
            for (int j = 0; j < vmpcp[i].size(); j++) {
                cout << vmpcp[i][j] << "  ";
            }
            cout << endl;

            if (instancenum.count(vm)) {
                bool flag = false, flag2 = false;
                int max = 0, index1 = -1, index2 = -2, min = 99999;

                for (int j = 0; j < instancenum[vm]; j++) {
                    bool f = false;

                    for (int a = 0; a < children[vmpcp[i][vmpcp[i].size() - 1]].size(); a++) {  // arrange former critical path before later critical path if they can arrange to same instance

                        if (scheduleinstance.count(children[vmpcp[i][vmpcp[i].size() - 1]][a])) {
                            if (scheduleinstance[children[vmpcp[i][vmpcp[i].size() - 1]][a]][0] == vm && scheduleinstance[children[vmpcp[i][vmpcp[i].size() - 1]][a]][1] == j) {
                                int fronttime = 0;
                                for (int k = 0; k < vmpcp[i].size(); k++) {
                                    fronttime += computationmatrix[vmpcp[i][k]][vm];
                                }
                                f = check(fronttime, vmpcp[i][vmpcp[i].size() - 1], {vm, j});
                                if (f) {
                                    int time = 0;
                                    for (int s = 0; s < children[vmpcp[i][vmpcp[i].size() - 1]].size(); s++) {
                                        if (scheduleinstance[children[vmpcp[i][vmpcp[i].size() - 1]][s]][0] == vm && scheduleinstance[children[vmpcp[i][vmpcp[i].size() - 1]][s]][1] == j) {
                                            time = instanceidletime[{vm, j}] - computationmatrix[children[vmpcp[i][vmpcp[i].size() - 1]][s]][vm];
                                            break;
                                        }
                                    }
                                    instanceidletime[{vm, j}] = instanceidletime[{vm, j}] + fronttime;

                                    if (fronttime <= instanceremaintime[{vm, j}]) {
                                        instanceremaintime[{vm, j}] -= fronttime;
                                    } else {
                                        instanceinterval[{vm, j}] += ((fronttime - instanceremaintime[{vm, j}]) / interval) + 1;
                                        instanceremaintime[{vm, j}] = interval * (((fronttime - instanceremaintime[{vm, j}]) / interval) + 1) - (fronttime - instanceremaintime[{vm, j}]);
                                    }
                                    for (int p = 0; p < vmpcp[i].size(); p++) {
                                        scheduleinstance[vmpcp[i][p]].push_back(vm);
                                        scheduleinstance[vmpcp[i][p]].push_back(j);
                                    }

                                    est[vmpcp[i][0]] = time;
                                    eft[vmpcp[i][0]] = est[vmpcp[i][0]] + computationmatrix[vmpcp[i][0]][vm];
                                    update(vmpcp[i][0]);
                                    cout << "arrange instance=" << vm << j << endl;
                                    break;
                                }
                            }
                        }
                    }

                    if (f) {
                        flag2 = true;
                        break;
                    }
                    for (int a = 0; a < parent[vmpcp[i][0]].size(); a++) {  // arrange later critical path after former critical path if they can arrange to same instance
                        if (scheduleinstance[parent[vmpcp[i][0]][a]][0] == vm && scheduleinstance[parent[vmpcp[i][0]][a]][1] == j && parent[vmpcp[i][0]].size() == 1) {
                            int fronttime = eft[parent[vmpcp[i][0]][a]];

                            if (instanceidletime[{vm, j}] <= eft[parent[vmpcp[i][0]][a]] || instanceidletime[{vm, j}] <= est[vmpcp[i][0]] || instanceidletime[{vm, j}] <= lft[vmpcp[i][0]]) {
                                f = check(fronttime, parent[vmpcp[i][0]][a], {vm, j});
                            }
                            if (f) {
                                int temp = 0;
                                for (int k = 0; k < vmpcp[i].size(); k++) {
                                    temp += computationmatrix[vmpcp[i][k]][vm];
                                }
                                est[vmpcp[i][0]] = instanceidletime[{vm, j}];
                                eft[vmpcp[i][0]] = est[vmpcp[i][0]] + computationmatrix[vmpcp[i][0]][vm];
                                update(vmpcp[i][0]);
                                int front = eft[parent[vmpcp[i][0]][a]] + temp, back = instanceidletime[{vm, j}] + instanceremaintime[{vm, j}];
                                if (instanceidletime[{vm, j}] + instanceremaintime[{vm, j}] > eft[parent[vmpcp[i][0]][a]]) {
                                    if (front < back) {
                                        instanceremaintime[{vm, j}] -= temp;
                                        instanceidletime[{vm, j}] += +temp;

                                    } else {
                                        if ((front - back) % interval == 0) {
                                            instanceinterval[{vm, j}] += (front - back) / interval;
                                            instanceremaintime[{vm, j}] = 0;
                                        } else {
                                            instanceinterval[{vm, j}] = instanceinterval[{vm, j}] + (front - back) / interval + 1;

                                            instanceremaintime[{vm, j}] = interval * ((front - back) / interval + 1) - (front - back);
                                        }
                                        instanceidletime[{vm, j}] = eft[parent[vmpcp[i][0]][a]] + temp;
                                    }

                                } else {
                                    if ((front - back) % interval == 0) {
                                        instanceinterval[{vm, j}] += temp / interval;
                                        instanceremaintime[{i, j}] = 0;
                                    } else {
                                        instanceinterval[{vm, j}] += (temp / interval) + 1;
                                        instanceremaintime[{i, j}] = interval * ((temp / interval) + 1) - temp;
                                    }

                                    instanceidletime[{vm, j}] = eft[parent[vmpcp[i][0]][a]] + temp;
                                }

                                for (int p = 0; p < vmpcp[i].size(); p++) {
                                    scheduleinstance[vmpcp[i][p]].push_back(vm);
                                    scheduleinstance[vmpcp[i][p]].push_back(j);
                                }

                                cout << "arrange instance=" << vm << "," << j << endl;
                                break;
                            }
                        }
                    }
                    if (f) {
                        flag2 = true;
                        break;
                    }
                    if (instanceidletime[{vm, j}] < est[vmpcp[i][0]] && instanceidletime[{vm, j}] + instanceremaintime[{vm, j}] > est[vmpcp[i][0]]) {  // if there is an applicable and remaining time instance exist
                        int temp = instanceidletime[{vm, j}] + instanceremaintime[{vm, j}] - est[vmpcp[i][0]];
                        if (temp > max) {
                            max = temp;
                            flag = true;
                            index1 = j;
                        }
                    } else if (instanceidletime[{vm, j}] + instanceremaintime[{vm, j}] < est[vmpcp[i][0]]) {  // if there is an applicable instance exist
                        int temp = est[vmpcp[i][0]] - instanceidletime[{vm, j}] - instanceremaintime[{vm, j}];
                        if (temp < min) {
                            max = temp;
                            flag = true;
                            index1 = j;
                        }
                    }
                }

                int sum = 0;
                if (flag2 == true) {
                    for (int i = 0; i < tasknum; i++) {
                        cout << i << "  :  est=" << est[i] << "   eft=" << eft[i] << "   lft=" << lft[i] << endl;
                    }
                    cout << "break" << endl;
                    for (int s = 0; s < vmnum; s++) {
                        for (int k = 0; k < instancenum[s]; k++) {
                            cout << s + 1 << "," << k + 1 << endl;
                            cout << "instanceidletime=" << instanceidletime[{s, k}] << "   instanceinterval=" << instanceinterval[{s, k}] << "    instanceremaintime=" << instanceremaintime[{s, k}] << endl;
                        }
                    }
                    continue;
                }
                if (index1 != -1) {
                    for (int j = 0; j < vmpcp[i].size(); j++) {
                        scheduleinstance[vmpcp[i][j]].push_back(vm);
                        scheduleinstance[vmpcp[i][j]].push_back(index1);
                        sum += computationmatrix[vmpcp[i][j]][vm];
                    }
                    int front = est[vmpcp[i][0]] + sum, back = instanceidletime[{vm, index1}] + instanceremaintime[{vm, index1}];
                    if (front <= back) {
                        instanceremaintime[{vm, index1}] -= sum;
                        instanceidletime[{vm, index1}] += sum;
                    } else {
                        if ((front - back) % interval == 0) {
                            instanceinterval[{vm, index1}] += (front - back) / interval;
                            instanceremaintime[{vm, index1}] = interval * ((front - back) / interval) - (front) - (back);
                            instanceidletime[{vm, index1}] = est[vmpcp[i][0]] + sum;
                        } else {
                            instanceinterval[{vm, index1}] += (front - back) / interval + 1;
                            instanceremaintime[{vm, index1}] = interval * ((front - back) / interval + 1) - (front) - (back);
                            instanceidletime[{vm, index1}] = est[vmpcp[i][0]] + sum;
                        }
                    }
                    cout << "if there is an applicable and remaining time instance exist" << endl;
                    cout << "new instance=" << vm << "," << index1;
                } else if (index2 != -2) {
                    for (int j = 0; j < vmpcp[i].size(); j++) {
                        scheduleinstance[vmpcp[i][j]].push_back(vm);
                        scheduleinstance[vmpcp[i][j]].push_back(index2);
                        sum += computationmatrix[vmpcp[i][j]][vm];
                    }
                    instanceinterval[{vm, index2}] = (sum / interval) + 1;
                    instanceidletime[{vm, index2}] = est[vmpcp[i][0]] + sum;
                    instanceremaintime[{vm, index2}] = interval * ((sum / interval) + 1) - sum;
                    cout << "if there is an applicable instance exist" << endl;
                    cout << "new instance=" << vm << "," << index2;
                } else {
                    instancenum[vm]++;
                    for (int j = 0; j < vmpcp[i].size(); j++) {
                        scheduleinstance[vmpcp[i][j]].push_back(vm);
                        scheduleinstance[vmpcp[i][j]].push_back(instancenum[vm] - 1);
                        sum += computationmatrix[vmpcp[i][j]][vm];
                    }
                    instanceinterval[{vm, instancenum[vm] - 1}] = (sum / interval) + 1;
                    instanceidletime[{vm, instancenum[vm] - 1}] = est[vmpcp[i][0]] + sum;
                    instanceremaintime[{vm, instancenum[vm] - 1}] = interval * ((sum / interval) + 1) - sum;
                    cout << "new instance=" << vm << "," << instancenum[vm] - 1;
                }
                cout << endl;

            } else {
                instancenum[vm] = 1;
                int sum = 0;
                for (int j = 0; j < vmpcp[i].size(); j++) {
                    scheduleinstance[vmpcp[i][j]].push_back(vm);
                    scheduleinstance[vmpcp[i][j]].push_back(instancenum[vm] - 1);
                    sum += computationmatrix[vmpcp[i][j]][vm];
                    // cout << "sum=" << sum << endl;
                }
                if (entrytask[vmpcp[i][0]] != 1) {
                    sum += est[vmpcp[i][0]];
                }
                instanceinterval[{vm, instancenum[vm] - 1}] = instanceinterval[{vm, instancenum[vm] - 1}] + (sum / interval) + 1;
                instanceidletime[{vm, instancenum[vm] - 1}] = sum;
                instanceremaintime[{vm, instancenum[vm] - 1}] = interval * ((sum / interval) + 1) - sum;

                cout << "new instance=" << vm << (instancenum[vm] - 1) << endl;
            }
            for (int s = 0; s < vmpcp[i].size(); s++) {
                updatepredecessor(vmpcp[i][s]);
            }
            for (int s = 0; s < vmnum; s++) {
                for (int k = 0; k < instancenum[s]; k++) {
                    cout << s + 1 << "," << k + 1 << endl;
                    cout << "end pcp instanceidletime=" << instanceidletime[{s, k}] << "   instanceinterval=" << instanceinterval[{s, k}] << "    instanceremaintime=" << instanceremaintime[{s, k}] << endl;
                }
            }
        }
        cout << "end cost" << endl;
        for (int s = 0; s < vmnum; s++) {
            for (int k = 0; k < instancenum[s]; k++) {
                cost += vmprice[s] * instanceinterval[{s, k}];
                cout << s + 1 << "," << k + 1 << endl;
                cout << "instanceidletime=" << instanceidletime[{s, k}] << "   instanceinterval=" << instanceinterval[{s, k}] << "    instanceremaintime=" << instanceremaintime[{s, k}] << endl;
            }
        }
        for (int i = 0; i < tasknum; i++) {
            cout << i << "  :  est=" << est[i] << "   eft=" << eft[i] << "   lft=" << lft[i] << endl;
        }
    }
    void run() {
        /*for (int i = 0; i < tasknum; i++) {
            schedulevm[i] = -1;
        }*/
        cout << "run" << endl;
        cout << "tasknum=" << tasknum << endl;
        // pretreatment();
        cout << "pretreatment end" << endl;

        for (int i = 0; i < tasknum; i++) {
            unassigned.push_back(i);
        }
        while (unassigned.size() >= 1) {
            schedulepath(findcriticalpath());
        }
        for (int i = 0; i < tasknum; i++) {
            cout << i << ":" << schedulevm[i] << endl;
        }

        for (int i = 0; i < pcpnum; i++) {
            for (int j = 0; j < vmpcp[i].size(); j++) {
                cout << vmpcp[i][j] << " ";
            }
            cout << endl;
        }
        cout << "scheduleandcost" << endl;
        scheduleandcost();
        for (int i = 0; i < tasknum; i++) {
            cout << i + 1 << "=" << scheduleinstance[i][0] + 1 << "," << scheduleinstance[i][1] + 1 << endl;
        }
    }
    vector<int> permutation() {
        vector<int> temp;
        for (int i = 0; i < tasknum; i++) {
            temp.push_back(i);
        }
        for (int i = 0; i < tasknum; i++) {
            for (int j = 0; j < tasknum - 1; j++) {
                if (est[j] > est[j + 1]) {
                    int temp1;
                    temp1 = est[j];
                    est[j] = est[j + 1];
                    est[j + 1] = temp1;
                    temp1 = temp[j];
                    temp[j] = temp[j + 1];
                    temp[j + 1] = temp1;
                }
            }
        }
        return temp;
    }
};
