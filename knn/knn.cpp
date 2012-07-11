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
 * 本程序的目的是测试最经典的算法 knn，但是最主要的事情还是完成pre-computation，事先计算出
 × item-item的相似度
 × 计算步骤：
 * （1）预处理的方式，计算每个item的平均打分，存在mean数组中
   （2）循环i，j，针对每一个ij计算相似度
   （3）对于每一个i和j，找出对他们共同打分的userlist，放入一个set中
   （4）应用公式Recommender system handbook的P125页公式4.21可以计算出相似度，存储在二维数组中
   求出来的相似度必然应该是对称的，如果不对称，说明结果显然是不对的。
 ×
 
      
 */
#include "movielens.h"
#include "mdefine.cpp"

namespace svd{
	//使用一些全局变量，存储需要估计的参数，bu，bi,wij
    vector<double> mi(ITEM_NUM+1,0.0);         //用来存储每个item的平均打分
    vector< vector<double> > w(ITEM_NUM+1);  //item-item相似矩阵
    vector< vector<int> >  userItems(USER_NUM+1);   //用户打过分的items-----数组
    //vector< vector <int> > rateMatrix(USER_NUM+1);  //打分矩阵,这个矩阵耗费了大量的内存，实际系统中必须优化，如何
                                                    //优化呢？？？？？？---->已经通过了下面的一个map数组解决了
    map<int,int> rateMatrix[ITEM_NUM+1];           //使用一个map数组存储稀疏的打分矩阵                     
	float mean = 0;                         //全局的平均值
    
    //函数声明
    void RMSEProbe(void);
    void ratingAll(vector< Rating > & data);
    double rating(Rating  record);
    double predictRate(int user,int item);
    double getSim(int item1,int item2);
    
    void model(int dim, double & alpha, double & beta)
    {
        vector<Rating> data;
        cout << "begin initialization: " << endl;
        
        loadRating("../mldataset/u1.base",data,rateMatrix);  //初始化，将打分数据load 入data中
       
        mean = setMeanRating(data); //求平均值，求bu和bi的值
        
        int i,u,j,k;
        
        //初始化w
        for(int i = 1; i < ITEM_NUM+1; ++i){
            w[i].clear();            //清空所有的p[i]
            for(j = 1; j < ITEM_NUM+1; ++j) {
        		w[i].push_back(0);
        	}
        }
        
        //求mi，即每个item的平均打分
        for(i = 1; i < ITEM_NUM+1; ++i){  //利用每一个打分调优结果
        	map <int,int> ::iterator it = rateMatrix[i].begin(); 
        	//if(rateMatrix[i].size() < 1) continue;
        	map <int,int> ::iterator end = rateMatrix[i].end();
        	while(it != end) {
        		mi[i] += (*it).second;
        		++it;
        	}
        	if(rateMatrix[i].size() > 0)mi[i] = mi[i]/rateMatrix[i].size();
        	else mi[i] = mean;
        	//cout <<i<<'\t'<< mi[i]<<endl;//<<rateMatrix[i].size()
        } 
		
        //循环每个item ，求它们的相似度
        
       	for(i = 1; i < ITEM_NUM+1; ++i){  //利用每一个打分调优结果
        	//cout<<i<<':'<<'\t';
        	for(j = i; j < ITEM_NUM+1; ++j) {
        		w[i][j] = getSim(i,j);
        		if(j != i)w[j][i] = w[i][j];
        		//cout<<w[i][j]<<'\t';
        	}
        	//cout <<i<< "  over!"<<endl;
        }
        
        //输出w的值
        ofstream outputw("sim.txt");
	    for(i=1; i < ITEM_NUM+1; ++i)
	    {
	    	for(j=1; j < ITEM_NUM+1; ++j) {
	    		outputw << w[i][j] << "\t";
	    	}
	    	outputw << endl;
	    }
	    outputw.close();
    }
    
    double getSim(int item1, int item2) {
    	//(1)找到打分的公共用户集合（2）按照公式计算
    	if(item1 == item2)return 1.0;
    	if(0 == rateMatrix[item1].size() || 0 == rateMatrix[item2].size() ) return 0.0;
    	map <int,int> ::iterator it = rateMatrix[item1].begin(); 
    	map <int,int> ::iterator end = rateMatrix[item1].end(); 
    	double sum1 = 0.0, sumSquare1 = 0.0, sumSquare2 = 0.0;
    	for(; it != end; ++it) {
    		int user = (*it).first;
    		if(rateMatrix[item2].find(user) != rateMatrix[item2].end()) {
    			//cout<<"common user:"<<user<<'\t'<<rateMatrix[item1][user]<<'\t'<<rateMatrix[item2][user]<<endl;
    			//已经找到了公共元素
    			sum1 += (rateMatrix[item1][user] - mi[item1]) *(rateMatrix[item2][user] - mi[item2]);
    			sumSquare1 += pow(rateMatrix[item1][user] - mi[item1],2); 
    			sumSquare2 += pow(rateMatrix[item2][user] - mi[item2],2);
    		}
    	}
    	double sim = 0.0;
    	if(sumSquare1 > 0.0000000001  && sumSquare2 > 0.0000000001)sim = sum1 / sqrt( sumSquare1 * sumSquare2);
    	return sim;
    }
    
    
    
    //根据svd++公式预测打分值
    double rating(Rating  record){
        /*
        int user = record.user;
        int item = record.item;
        double ret = mean + bu[user] + bi[item] + dot(p[user], q[item]);
        if(ret < 1) ret = 1;
        if(ret > 5) ret = 5;
        return ret;
        */
        return  0.0;
    } 
    
    double predictRate(int user,int item)
    {
    	/*
    	int u = user;
    	int i =  item;
    	double sumBias = 0.0;
    	for(int j=0; j < userItems[u].size(); ++j) {
    		double buj = mean + bu[u] + bi[userItems[u][j]];
    		double ruj = rateMatrix[u][userItems[u][j]];// u 对 j的打分如何快速获得呢？
    		sumBias += (ruj - buj) * w[i][userItems[u][j]] + c[i][userItems[u][j]];
    	}
    	double ret = 0.0;
    	if(userItems[u].size()>1)
    	{
    		ret = mean + bu[u] + bi[i] + (1.0/sqrt(userItems[u].size())) * sumBias;//这里先不对k进行变化，先取k=无穷大
    	}
    	else ret  = mean;
    	if(ret < 1.0) ret = 1;
        if(ret > 5.0) ret = 5;
        //cout <<u<<"	"<<i<<"	"<<mean <<"	"<<bu[u]<<"	"<<bi[i]<<"	"<<userItems[u].size()<<"	"<<sumBias<<"	"<<ret<<endl;
        return ret;
        */
    }
    
    //检查测试集情况
    void RMSEProbe(void){
    	
        /*	1、load test数据集，
        	2、针对每一条数据，计算预测值，然后计算误差的平方和，计算总数量
        	3、输出rmse
        */
        std::ifstream from("../mldataset/u1.test");
        char rateStr[100];
        vector<string> rateDetail(4);
        double predict = 0.0;
        double rmseSum = 0.0;
        int num = 0;
        int realValue = 0;
        while(from.getline(rateStr,100)){
        	explode("	",rateStr,rateDetail);
        	if(rateDetail.size()>=3){
        		predict = predictRate(atoi(rateDetail[0].c_str()), atoi(rateDetail[1].c_str()));
        		realValue = atoi(rateDetail[2].c_str());
        		rmseSum =  rmseSum + pow(predict-realValue,2);
        		//cout << predict <<"	"<<realValue<<"	"<<rmseSum<<endl;
        		++num;
        	}
        }
        cout << "test set rmse:"<<sqrt(rmseSum/num) <<"	"<< rmseSum <<"	"<< num<< endl;
    }

    void ratingAll(vector< Rating > & data){
        for(int i = 0; i < data.size(); ++i){
            data[i].setPredict(rating(data[i]));
        }
    }
};

int main(int argc, char ** argv)
{
	double start,end,duration; 
	start = clock();
    double alpha = 0.013;  //经测试这个值比较好
    double beta = 0.001;   //经过测试这个也还行
    int dim = 100;//atoi(argv[1]);
    test_level = 1;//atoi(argv[2]);
    ofstream outputfile("parameter.txt");
    
    //for(int i=0; i < 10; i++)
    {
    //	beta = i*0.001 + 0.002;
    //	cout << beta << endl;
    	svd::model(dim,alpha,beta);
    	
    }
    outputfile.close();
    end = clock();
    duration = (end-start)/CLOCKS_PER_SEC;
    cout << "duration:"<<duration <<" s!" <<endl;
    return 0;
}
