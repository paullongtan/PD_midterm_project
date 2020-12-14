//
//  main.cpp
//  Project_fix_ver3
//
//  Created by 王彥普 on 2020/12/11.
//

#include <iostream>
#include <cmath>
using namespace std;

void insertionSort(double* priority, int* rank, int num);
void orderRanking(bool* orderFinished, double* orderPriority,
                  int* orderRank, double* orderQuantity,
                  double* orderDDL, int orderNum, int currentTime);
void machineRanking(int* machineJobClearTime, double* machinePriority,
                    int* machineRank, int machineNum, double* output,
                    double* init, double* decrease, double* low);

int main()
{
    // 輸入
    int machineNum = 0, orderNum = 0, fixLim = 0;
    cin >> machineNum >> orderNum >> fixLim;
    double* output = new double[machineNum];
    double* decrease = new double[machineNum];
    double* fixTime = new double[machineNum];
    double* init = new double[machineNum];
    double* low = new double[machineNum];
    
    for(int i = 0; i < machineNum; i++)
        cin >> output[i] >> decrease[i] >> fixTime[i] >> init[i] >> low[i];
    
    double* orderQuantity = new double[orderNum];
    double* orderDDL = new double[orderNum];
    
    for (int i = 0; i < orderNum; i++)
        cin >> orderQuantity[i] >> orderDDL[i];
    
    int currentTime = 0;
    // 計算訂單急迫性
    double* orderPriority = new double[orderNum];
    for (int i = 0; i < orderNum; i++)
    {
        orderPriority[i] = orderQuantity[i] / orderDDL[i];
//        cout << i << ":" << orderPriority[i] << endl;
    }
    
    // 排序訂單急迫性
    int* orderRank = new int[orderNum];
    for (int i = 0; i < orderNum; i++)
        orderRank[i] = i;
    insertionSort(orderPriority, orderRank, orderNum);
//    for (int i = 0; i < orderNum; i++)
//        cout << orderRank[i] << ",";
//    cout << endl;
    
    // 計算機器的priority值，並存到一個dynamic array中
    double* machinePriority = new double[machineNum];
    for(int i = 0; i < machineNum; i ++)
    {
        machinePriority[i] = output[i] * init[i] * (1 / (decrease[i] + 15)) * (low[i] / 10);
//        cout << i << ":" << machinePriority[i] << endl;
    }
    
    // 排序機器品質
    int* machineRank = new int[machineNum];
    for (int i = 0; i < machineNum; i++)
        machineRank[i] = i;
    insertionSort(machinePriority, machineRank, machineNum);
//    for (int i = 0; i < machineNum; i++)
//        cout << machineRank[i] << ",";
//    cout << endl;

    // 儲存訂單是否完成
    bool* orderFinish = new bool[orderNum];
    for (int i = 0; i < orderNum; i++)
        orderFinish[i] = 0;
    
    
    // 目前可以儲存幾個工作
    int proportion = orderNum / machineNum + 2;
    int* machineProportion = new int[machineNum];
    for (int i = 0; i < machineNum; i++)
        machineProportion[i] = proportion;
    
    // 儲存每個機器對應到的工作
    int** machineJob = new int*[machineNum];
    for (int i = 0; i < machineNum; i++)
        machineJob[i] = new int[proportion];
    
    // 目前每個機台存了幾個工作
    int* jobNum = new int[machineNum];
    for (int i = 0; i < machineNum; i++)
        jobNum[i] = 0;
    
    // 判斷機台是否需要考慮維修
    bool* considerFix = new bool[machineNum];
    for (int i = 0; i < machineNum; i++)
    {
        if (low[i] * fixTime[i] >= (output[i] - low[i]) / decrease[i] * output[i] /2 || low[i] >= 80)
            considerFix[i] = 0;
        else
            considerFix[i] = 1;
    }
    
    
    // 機器距離當前訂單完成的時間
    int* machineJobClearTime = new int[machineNum];
    for (int i = 0; i < machineNum; i++)
        machineJobClearTime[i] = 1;
    
    
    
    // 完成的訂單總數
    int finish = 0;
    
    int* workerTime = new int[fixLim];
    for (int i = 0; i < fixLim; i++)
        workerTime[i] = 0;
    
    do
    {
        // 先把訂單排序
        orderRanking(orderFinish, orderPriority, orderRank, orderQuantity, orderDDL, orderNum, currentTime);
//        machineRanking(machineJobClearTime, machinePriority, machineRank, machineNum, output, init,             decrease, low);
//        cout << currentTime << endl;
//        for (int i = 0; i < orderNum; i++)
//            cout << orderRank[i] << ",";
//        cout << endl;
        // 此期已分配的訂單數
        int distributedNum = 0;
        
        int workerNum = fixLim;
        for (int i = 0; i < fixLim; i++)
        {
            if (workerTime[i] == 1)
            {
                workerTime[i]--;
            }
            else if(workerTime[i] > 1)
            {
                workerNum--;
                workerTime[i]--;
            }
        }
        
        for (int i = 0; i < machineNum; i++)
        {
            if (finish == orderNum)
                break;
            // 進入新的一期，機器距離當前訂單完成的時間-1
            machineJobClearTime[machineRank[i]] -= 1;
            // 如果機器空出
            if (machineJobClearTime[machineRank[i]] == 0)
            {
                
                if(considerFix[machineRank[i]] == 1 && (init[machineRank[i]] <= low[machineRank[i]] + 20 || init[machineRank[i]] <= 70) && workerNum > 0)
                {
                    for (int j = 0; j < fixLim; j++)
                    {
                        if (workerTime[j] == 0)
                        {
                            workerTime[j] = fixTime[machineRank[i]];
                            workerNum--;
                            break;
                        }
                    }
                    
                    // 確認儲存陣列的長度
                    if (jobNum[machineRank[i]] + 2 <= machineProportion[machineRank[i]])
                    {
    //                    cout << "long enough" << orderRank[distributedNum] << endl;
                        // 將訂單分配給該機器，儲存於機器的負責訂單陣列中
                        machineJob[machineRank[i]][jobNum[machineRank[i]]] = -1;
                        jobNum[machineRank[i]]++;
                        
                        machineJob[machineRank[i]][jobNum[machineRank[i]]] = orderRank[distributedNum];
                        jobNum[machineRank[i]]++;
                    }
                    else
                    {
    //                    cout << " not long enough" << orderRank[i] << endl;
                        // 儲存陣列不夠長時，把陣列長度變兩倍
                        int* temp = new int[machineProportion[machineRank[i]] * 2];
                        for (int j = 0; j < jobNum[machineRank[i]]; j++)
                            temp[j] = machineJob[machineRank[i]][j];
                        
                        delete machineJob[machineRank[i]];
                        machineJob[machineRank[i]] = temp;
                        // 更新陣列長度及以該機器已分配的工作數量
                        machineProportion[machineRank[i]] *= 2;
                        machineJob[machineRank[i]][jobNum[machineRank[i]]] = -1;
                        jobNum[machineRank[i]]++;
                        machineJob[machineRank[i]][jobNum[machineRank[i]]] = orderRank[distributedNum];
                        jobNum[machineRank[i]]++;
                    }
                
                    int count = 0;
                    // 產量可能出現小數，因此設定以生產量也為double，在題目未說明下我們假設產品可分割，每階段小數皆保留
                    double made = 0;
                    // 當為此訂單製造的量小於訂單要求時，持續生產
                    while (made < orderQuantity[orderRank[distributedNum]])
                    {
                        count += 1;
                        // 良率有最低限制
                        made += max(output[machineRank[i]] * (100 - decrease[machineRank[i]] * (count - 1)) * 0.01, output[machineRank[i]] * low[machineRank[i]] * 0.01);
                    }
                    // 機台剩餘運作時間
                    machineJobClearTime[machineRank[i]] = count + fixTime[machineRank[i]];
                    // 機台完成訂單後良率
                    init[machineRank[i]] = max(100 - decrease[machineRank[i]] * (count - 1), low[machineRank[i]]);
                    // 該訂單完成
                    orderFinish[orderRank[distributedNum]] = 1;
                    distributedNum++;
                    // 訂單完成數加一
                    finish++;

                }
                else
                {
                    // 確認儲存陣列的長度
                    if (jobNum[machineRank[i]] + 1 <= machineProportion[machineRank[i]])
                    {
    //                    cout << "long enough" << orderRank[distributedNum] << endl;
                        // 將訂單分配給該機器，儲存於機器的負責訂單陣列中
                        machineJob[machineRank[i]][jobNum[machineRank[i]]] = orderRank[distributedNum];
                        jobNum[machineRank[i]]++;
                    }
                    else
                    {
    //                    cout << " not long enough" << orderRank[i] << endl;
                        // 儲存陣列不夠長時，把陣列長度變兩倍
                        int* temp = new int[machineProportion[machineRank[i]] * 2];
                        for (int j = 0; j < jobNum[machineRank[i]]; j++)
                            temp[j] = machineJob[machineRank[i]][j];
                        
                        delete machineJob[machineRank[i]];
                        machineJob[machineRank[i]] = temp;
                        // 更新陣列長度及以該機器已分配的工作數量
                        machineProportion[machineRank[i]] *= 2;
                        machineJob[machineRank[i]][jobNum[machineRank[i]]] = orderRank[distributedNum];
                        jobNum[machineRank[i]]++;
                    }
                
                    int count = 0;
                    // 產量可能出現小數，因此設定以生產量也為double，在題目未說明下我們假設產品可分割，每階段小數皆保留
                    double made = 0;
                    // 當為此訂單製造的量小於訂單要求時，持續生產
                    while (made < orderQuantity[orderRank[distributedNum]])
                    {
                        count += 1;
                        // 良率有最低限制
                        made += max(output[machineRank[i]] * (init[machineRank[i]] - decrease[machineRank[i]] * count) * 0.01, output[machineRank[i]] * low[machineRank[i]] * 0.01);
                    }
                    // 機台剩餘運作時間
                    machineJobClearTime[machineRank[i]] = count;
                    // 機台完成訂單後良率
                    init[machineRank[i]] = max(init[machineRank[i]] - decrease[machineRank[i]] * count, low[machineRank[i]]);
                    // 該訂單完成
                    orderFinish[orderRank[distributedNum]] = 1;
                    distributedNum++;
                    // 訂單完成數加一
                    finish++;
                }
            }
        }
        currentTime++;
        
    }while (finish < orderNum);
    
    
    for (int i = 0; i < machineNum; i++)
    {
        if (jobNum[i] == 0)
            cout << 0 << endl;
        for (int j = 0; j < jobNum[i]; j++)
        {
            if (j == jobNum[i] - 1)
                cout << machineJob[i][j] + 1 << endl;
            else
            {
                if (machineJob[i][j] == -1)
                    cout << "M" << ",";
                else
                    cout << machineJob[i][j] + 1 << ",";
            }
        }
    }
    
    
    delete [] output;
    delete [] decrease;
    delete [] fixTime;
    delete [] init;
    delete [] low;
    delete [] orderQuantity;
    delete [] orderDDL;

    return 0;
}

void insertionSort(double* priority, int* rank, int num)
{
    for (int i = 1; i < num; i++)
    {
        for (int j = 1; j <= i; j++)
        {
            if (priority[rank[i]] > priority[rank[i - j]])
            {
                if (i - j == 0)
                {
                    int temp = i;
                    for (int k = i; k > i - j; k--)
                        rank[k] = rank[k - 1];
                    rank[i - j] = temp;
                }
                continue;
            }
            else if (priority[rank[i]] <= priority[rank[i - j]])
            {
                int temp = i;
                for (int k = i; k > i - j + 1; k--)
                    rank[k] = rank[k - 1];
                rank[i - j + 1] = temp;
                break;
            }
            
        }
    }
}

void orderRanking(bool* orderFinished, double* orderPriority, int* orderRank, double* orderQuantity, double* orderDDL, int orderNum, int currentTime)
{
    for (int i = 0; i < orderNum; i++)
    {
        if (orderFinished[i] == 0)
        {
            if (orderQuantity[i] / (orderDDL[i] - currentTime) > 0)
                orderPriority[i] = orderQuantity[i] / (orderDDL[i] - currentTime);
            else
                orderPriority[i] = currentTime - orderDDL[i];
        }
        else
            orderPriority[i] = 0;
    }
    for (int i = 0; i < orderNum; i++)
        orderRank[i] = i;
    insertionSort(orderPriority, orderRank, orderNum);
}

void machineRanking(int* machineJobClearTime, double* machinePriority,
                    int* machineRank, int machineNum, double* output,
                    double* init, double* decrease, double* low)
{
    for (int i = 0; i < machineNum; i++)
    {
        if (machineJobClearTime[i] == 1)
            machinePriority[i] = output[i] * init[i];
        else
            machinePriority[i] = 0;
    }

    // default
    for (int i = 0; i < machineNum; i++)
        machinePriority[i] = i;

    insertionSort(machinePriority, machineRank, machineNum);
}
