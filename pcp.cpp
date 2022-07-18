#include <algorithm>
#include <iostream>
#include <map>
#include <vector>
using namespace std;

int tasknum = 9, vmnum = 3;
vector<double> est;
vector<double> eft;
vector<double> lft;
vector<double> mincomputation;
vector<int> scheduled;
vector<int> unassigned;
int deadline;
vector<vector<double> > computationmatrix;
vector<vector<double> > communicationmatrix;
vector<int> presenttask;
vector<int> successtask;
map<int, int> notentrytask;
map<int, int> notexittask;
map<int, int> entrytask;
map<int, int> exittask;
map<int, vector<int> > parent;
map<int, vector<int> > children;
map<int, int> schedulevm;
map<int, vector<int> > vmpcp;
int pcpnum = 0;
map<int, int> instancenum;  // each vm's instancenum;
map<vector<int>, int> vmidletime;

// vector<int> successor;
// vector<int> predecssors;
void findpartial(int task, vector<int> v);
void updatesuccessor(int task) {
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
void updatepredecessor(int task) {
    for (int i = 0; i < parent[task].size(); i++) {
        double temp;
        if (scheduled[parent[task][i]] == 1 && scheduled[task] == 1) {
            if (children[parent[task][i]].size() == 1 && schedulevm[task] == schedulevm[parent[task][i]]) {
                lft[parent[task][i]] = lft[task] - computationmatrix[task][schedulevm[task]];
                updatepredecessor(parent[task][i]);
                continue;
            } else if (schedulevm[task] == schedulevm[parent[task][i]]) {
                temp = lft[task] - computationmatrix[task][schedulevm[task]];
            } else {
                temp = lft[task] - computationmatrix[task][schedulevm[task]] - communicationmatrix[parent[task][i]][task];
            }
        } else {
            temp = lft[task] - computationmatrix[task][schedulevm[task]] - communicationmatrix[parent[task][i]][task];
        }
        if (temp < lft[parent[task][i]] || children[parent[task][i]].size() == 1) {
            cout << task << "update" << parent[task][i] << endl;
            lft[parent[task][i]] = temp;
        }
        updatepredecessor(parent[task][i]);
    }
}
void pretreatment() {
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
        double min = 9999;
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
            lft[i] = deadline;
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
vector<int> findcriticalpath() {
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
void schedulepath(vector<int> cp) {
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
        if (parent[cp[j]].size() == 1 && schedulevm[parent[cp[j]][0]] == i) {
            fronttime = eft[parent[cp[j]][0]];
            first = eft[parent[cp[j]][0]];
        }

        while (j >= 0) {
            cout << cp[j] << "first=" << first << "=   " << fronttime + computationmatrix[cp[j]][i] << "    lft=" << lft[cp[j]] << endl;
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
                    if (parent[cp[k]].size() == 1 && schedulevm[parent[cp[k]][0]] == i) {
                        est[cp[k]] = first;
                        eft[cp[k]] = first + computationmatrix[cp[k]][i];
                    } else {
                        eft[cp[k]] = est[cp[k]] + computationmatrix[cp[k]][i];
                    }
                    // eft[cp[k]] = est[cp[k]] + computationmatrix[cp[k]][i];

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
void findpartial(int task, vector<int> v) {
    cout << "partial task=   " << task << endl;
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
void scheduleandcost() {
    for (int i = 0; i < pcpnum; i++) {
        int vm = vmpcp[i][0];
        if (instancenum.count(vm)) {
            for (int j = 0; j < instancenum[vm]; j++) {
            }
        } else {
            instancenum[vm] = 1;
        }
    }
}
int main() {
    eft.assign(tasknum, 0);
    est.assign(tasknum, 0);
    lft.assign(tasknum, 9999);
    scheduled.assign(tasknum, 0);
    deadline = 30;
    tasknum = 9;
    vmnum = 3;
    computationmatrix = {{2, 5, 8}, {5, 12, 16}, {3, 5, 9}, {4, 6, 10}, {3, 8, 11}, {4, 8, 11}, {5, 8, 11}, {3, 6, 8}, {5, 8, 14}};
    communicationmatrix = {{0, 0, 0, 1, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 2, 2, 0, 0, 0}, {0, 0, 0, 0, 0, 2, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 1, 1, 0}, {0, 0, 0, 0, 0, 0, 0, 4, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 3}, {0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0}};
    freopen("E:\\c++\\researchcode\\test.txt", "r", stdin);
    /*for (int i = 0; i < tasknum; i++) {
        for (int j = 0; j < tasknum; j++) {
            cin >> communicationmatrix[i][j];
        }
    }*/
    pretreatment();
    for (int i = 0; i < tasknum; i++) {
        cout << i << "  :  est=" << est[i] << "   eft=" << eft[i] << "   lft=" << lft[i] << endl;
    }
    for (int i = 0; i < tasknum; i++) {
        unassigned.push_back(i);
    }
    while (unassigned.size() >= 1) {
        schedulepath(findcriticalpath());
        /*cout << "play" << endl;
        for (int i = 0; i < tasknum; i++) {
            cout << i << "  :  est=" << est[i] << "   eft=" << eft[i] << "   lft=" << lft[i] << endl;
        }*/
    }
    for (int i = 0; i < pcpnum; i++) {
        for (int j = 0; j < vmpcp[i].size(); j++) {
            cout << vmpcp[i][j] << " ";
        }
        cout << endl;
    }
    for (int i = 0; i < tasknum; i++) {
        cout << i << " : " << schedulevm[i] << endl;
    }
    fclose(stdin);
}