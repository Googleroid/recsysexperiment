/*
* This file is Copyright (C) 2011 Lv Hongliang. All Rights Reserved.
* please maitain the copyright information completely when you redistribute the code.
* 
* Please contact me via email honglianglv@gmail.com
* my blog: http://lifecrunch.biz
* my twitter: http://twitter.com/honglianglv
*
* It is free software; you can redistribute it and/or modify it under 
* the GNU General Public License as published by the Free Software
* Foundation; either version 1, or (at your option) any later version.
*/

/**
 * load filePath中的数据到data这个vector中和 rateMatrix中
 * 主要从单个文件中获取
 */
void loadRating(char * fileName,  map<int,int> rateMatrixLocal[USER_NUM+1][CRI_NUM+1],
        const char* separator=" ")
{
    char rateStr[256];
    char* pch;   
    int fileNum = 0;
    std::ifstream from (fileName);
    if (!from.is_open()) {
        cout << "can't open input file!\n";
        exit(1);
    }
    cout << "open file ok!" << endl;    
    int itemId = 0, userId = 0, rate = 0;
    string strTemp = "";
    
    while(from.getline(rateStr,256)){
        string strTemp(rateStr);
        if(strTemp.length() < 3)continue;
            
        int i = 0;
        pch = strtok (rateStr,separator);
        while (pch != NULL) {
            if(0 == i) itemId = atoi(pch);
            else if(1 == i) userId = atoi(pch);
            else if(i > 1) {
                rateMatrixLocal[userId][i-1][itemId] = atoi(pch);
//                cout<<strTemp<<"#####################userId:"<<userId<<" itemId:"<<itemId<<" rate:"<<atoi(pch)<<endl;
            }
            else if(i > 1+CRI_NUM) break;
            ++i;
            pch = strtok (NULL,separator);
          }
        if(0 == itemId || 0 == userId) {
            cout<<strTemp<<"#####################userId:"<<userId<<" itemId:"<<itemId<<" rate:"<<rate<<endl;
            exit(1);
        }
        //initial rateMatrix
        try {
            //rateNode tmpNode;
            //tmpNode.item = itemId;
            //tmpNode.rate = (short)rate;
            //rateMatrixLocal[userId].push_back(tmpNode);
        }
        catch (bad_alloc& ba){
            cerr << "bad_alloc caught: " << ba.what() << endl;
        }
    }
    from.close();
    cout<<"read training set file sucessfully!"<<endl;
    return;
}
/*
//load test set
void loadProbe(char * fileName,vector<testSetNode>& probeSet, const char* separator)
{
    ifstream in(fileName);
    if (!in.is_open()) {
        cout << "can't open test set file!\n";
        exit(1);
    }
    char rateStr[256];
    char* pch ; // store a token of a string

    string strTemp;
    int rateValue,itemId,userId,probeNum=0;
    
    while(in.getline(rateStr,256)){
        strTemp = rateStr;
        if(strTemp.length() < 4) continue;
        int i = 0;
        pch = strtok (rateStr,separator);
        while (pch != NULL) {
            if(0 == i) userId = atoi(pch);
            else if(1 == i) itemId = atoi(pch);
            else if(2 == i) rateValue = atoi(pch);
            else if(i > 2) break;
            ++i;
            pch = strtok (NULL,separator);
        }
        try {
            testSetNode tmpNode;
            tmpNode.item = itemId;
            tmpNode.rate = (short)rateValue;
            tmpNode.user = userId;
            probeSet.push_back(tmpNode);
            ++probeNum;
        }
        catch (bad_alloc& ba) {
            cerr << "bad_alloc caught: " << ba.what() << endl;
            cout << "Can't allocate the momery!" << endl; exit(1);
        }
    }
    cout << "Load " << probeNum << " test ratings successfully!"<<endl;
    in.close(); 
}
*/
