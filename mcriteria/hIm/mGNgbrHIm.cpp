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
 * 本程序的目的是测试 global Neighborhood 方法的扩展性，将Recommender system handbook中的P171页，5.35的
 *  公式，扩展到3维多指标打分的情况
 
    实现的方式也是渐进式增强（Progressive enhancement），不要求一步到位
    
    
    这里的数据id不是从1开始增加的，这个需要做一下调整，使其从1开始增加，一直到USER_NUM为止，这是所有处理的第一步
    这里采用的方法是php脚本处理，将所有原始数据处理一遍，进行变量替换
	使用的代码在../replaceUserHotelId.php中，目前已经处理完毕，不过目前的情况是数据量太少
  
 */
#include "rating.h"
#include "mdefine.cpp"
//#include "tripadvisor.cpp"
#include "yahooMovie.cpp"
namespace svd{
    //使用一些全局变量，存储需要估计的参数，bu，bi,bc,wijc
    vector<double> user(USER_NUM+1,0);   //用于存储原始Id
    vector<double> hotel(ITEM_NUM+1,0);
    
    double bu[USER_NUM+1] = {0}; 
	double bi[ITEM_NUM+1] = {0};
	double bc[CRI_NUM+1]  = {0};      //baseline预测器中的用户偏置,item偏置,criteria偏置
    vector<int> buNum(USER_NUM+1,0);	//用户u打分的item总数，
    vector<int> biNum(ITEM_NUM+1,0);   //打过item i分的用户总数
    vector<int> bcNum(ITEM_NUM+1,0);   //在这个criteria打过分的用户总数

	vector<double> buBase(USER_NUM+1,0);	//用户u打分的item总数，
    vector<double> biBase(ITEM_NUM+1,0);   //打过item i分的用户总数
    vector<double> bcBase(ITEM_NUM+1,0);   //在这个criteria打过分的用户总数
    
    double wMatrix[ITEM_NUM+1][ITEM_NUM+1][CRI_NUM+1] = {0};  //需要学习的item-item相似矩阵
	double cMatrix[ITEM_NUM+1][ITEM_NUM+1][CRI_NUM+1] = {0};  //隐含因子矩阵

    map<int,int> rateMatrix[USER_NUM+1][CRI_NUM+1];   //使用二维个map数组存储稀疏的打分矩阵，是否有更好的方法？？？
	double mean = 0;                         //全局的平均值
    
    //函数声明
    void RMSEProbe(void);
    void loadSim(char *fileName);
    void ratingAll(vector< Rating > & data);
    double rating(Rating  record);
    double predictRate(int user,int item, int cri);
    
    void model(int dim, double  alpha, double  beta)
    {
        cout << "begin initialization: " << endl;
        
        loadRating(RATING_FILE_NAME, rateMatrix, " ");  //load data
        
        mean = setMeanRating(rateMatrix); //calculate the mean value
		cout<<"mean:"<<mean<<endl;
        int i,u,j,cIndex,k;
        
		int itemId,rateValue;
        //initialize the bi ,bu and bc, calculte the specific mean value and subtract the mean value
		for( i = 1; i < USER_NUM+1; ++i) {
			for( j = 1; j < CRI_NUM+1; ++j) {
				//iteration the map
				map<int,int>::iterator it;
				map<int,int>::iterator end = rateMatrix[i][j].end();
				for ( it=rateMatrix[i][j].begin() ; it != end; ++it) {
					int itemId = it->first;
					int rateValue = it->second;
					/*
					bu[i] += rateMatrix[i][j].rate;
					buNum[i] += 1;
					bi[rateMatrix[i][j].item] += rateMatrix[i][j].rate;
					biNum[rateMatrix[i][j].item] += 1;
					*/
					bu[i] += rateValue;
					++buNum[i];
					bi[itemId] += rateValue;
					++biNum[itemId];
					bc[j] += rateValue;
					++bcNum[j];
				}
			}  			
		}

        //以下过程求平均值
        for(i = 1; i < USER_NUM+1; ++i) {
        	if(buNum[i]>1)bu[i] = bu[i]/buNum[i] - mean;
        	else bu[i] = 0.0;
			buBase[i] = bu[i];
        }
		cout<<"bu initialized end"<<endl;
        
        for(i = 1; i < ITEM_NUM+1; ++i) {
        	if(biNum[i] > 1)bi[i] = bi[i]/biNum[i] - mean;
        	else bi[i] = 0.0;
			biBase[i] = bi[i];
        }

		cout<<"bi initialized end"<<endl;
        for(i = 1; i < CRI_NUM+1; ++i) {
        	if(bcNum[i]>1)bc[i] = bc[i]/bcNum[i] - mean;
        	else bc[i] = 0.0;
			bcBase[i] = bc[i];
        }
        cout<<"bc initialized end"<<endl;

        //initialize the wMatrix
		for(i = 1; i < ITEM_NUM+1; ++i){
            //cout<< bu[i] << endl;
			for(j = 1; j < ITEM_NUM+1; ++j){
	            setRand(wMatrix[i][j],CRI_NUM+1,0);    //初始化w[i]
			}
        }
		cout<<"wMatrix initialized end"<<endl;
        //initialize the cMatrix        
        for(i = 1; i < ITEM_NUM+1; ++i){
			for(j = 1; j < ITEM_NUM+1; ++j){
	            setRand(cMatrix[i][j],CRI_NUM+1,0);    //初始化c[i]
			}
        }
        cout<<"cMatrix initialized end"<<endl; 
        cout <<"initialization end!"<<endl<< "begin iteration: " << endl;
        
        double pui = 0.0 ; // 预测的u对i的打分
        double preRmse = 1000000000000.0; //用于记录上一个rmse，作为终止条件的一种，如果rmse上升了，则停止
        double nowRmse = 0.0;
        for(int step = 0; step < 35; ++step){  //只迭代35次
            double rmse = 0;
            double n = 0;
            
            for( u = 1; u < USER_NUM+1; ++u) {   //循环处理每一个用户 
                for( cIndex = 1; cIndex < CRI_NUM+1; ++cIndex) {
					int ruNum = rateMatrix[u][cIndex].size();
					//if(ruNum > 0)cout<<u<<"	"<<cIndex<<"	"<<ruNum<<endl;
					double sqrtRuNum = sqrt(ruNum);
					map<int,int>::iterator it;
					map<int,int>::iterator end = rateMatrix[u][cIndex].end();
					double sum = 0;
					for ( it=rateMatrix[u][cIndex].begin() ; it != end; ++it) {
						int itemId = it->first;
						int rui = it->second;
						pui = predictRate(u,itemId,cIndex);
						double eui = rui - pui;
						if(eui >5 ) {
							cout<<u<<"	"<<cIndex<<"	"<<itemId<<"	"<<rui<<"	mean:"<<mean<<"	";
							cout<<pui<<'\t'<<rui<<"	"<<bu[u]<<"	"<<bi[itemId]<<"	"<<bc[cIndex]<<endl;
							exit(1);
						}
						rmse += eui * eui; ++n;  
						
						//update bu，bi,bc
						bu[u] += alpha * (eui - beta * bu[u]);
						bi[itemId] += alpha * (eui - beta * bi[itemId]);
						bc[cIndex] += alpha * (eui - beta * bc[cIndex]);
						sum += eui;
					}

					// update wMatrix[i][j][cIndex] cMatrix[i][j][cIndex]
					for ( it=rateMatrix[u][cIndex].begin() ; it != end; ++it) {
						int itemi = it->first;
						for(map<int,int>::iterator itj=rateMatrix[u][cIndex].begin(); itj != rateMatrix[u][cIndex].end(); ++itj) {
							int itemj = itj->first;
							double bujc = mean + buBase[u] + biBase[itemj] + bcBase[cIndex];
							wMatrix[itemi][itemj][cIndex] += alpha *(sum * sqrtRuNum * (itj->second-bujc) - beta * wMatrix[itemi][itemj][cIndex]);
							cMatrix[itemi][itemj][cIndex] += alpha *(sum * sqrtRuNum - beta * cMatrix[itemi][itemj][cIndex]);
						}
					} 
				}

            }
            nowRmse =  sqrt(rmse / n);
            if( nowRmse >= preRmse && step > 10) break; //如果rmse已经开始上升了，则跳出循环
            else
            	preRmse = nowRmse;
            cout << step << "\t" << nowRmse << endl;
            //alpha *= 0.992;    //逐步减小学习速率
            //RMSEProbe(); 
        }
        //ratingAll(data);  //预测所有的结果
        
        RMSEProbe();  // 检查训练集情况
        return;
        
        //输出bu的值
        ofstream outputbu("bu.txt");
	    for(int i=1; i < USER_NUM+1; i++)
	    {
	    	outputbu << i << "\t" << bu[i] << endl;
	    }
	    outputbu.close();
	    
	    //输出bi的值
        ofstream outputbi("bi.txt");
	    for(int i=1; i < ITEM_NUM+1; i++)
	    {
	    	outputbi << i << "\t"<< bi[i] << endl;
	    }
	    outputbu.close();
	    
	    
    }
    
    double predictRate(int user,int item, int cri)
    {
		double bui = mean + bu[user] + bi[item] + bc[cri];
		double ret = bui;
		return bui;
		double sqrtRuNum = 1/sqrt(rateMatrix[user][cri].size());
    	map<int,int>::iterator it;
		map<int,int>::iterator end = svd::rateMatrix[user][cri].end();
		double sumw = 0.0;
		double sumc = 0.0;

		for ( it=rateMatrix[user][cri].begin() ; it != end; ++it) {
			int itemId = it->first;
			int ruj = it->second;
			double buj = mean+ bu[user] + bc[cri] + bi[itemId];
			sumw += (ruj-buj) * wMatrix[item][itemId][cri];
			sumc += cMatrix[item][itemId][cri];
		}
		if(rateMatrix[user][cri].size()>=1) {
			ret = bui + sqrtRuNum * (sumw+sumc);
		}
		if(ret > 5) ret = 5.0;
		else if( ret < 1) ret = 1.0;
		return ret;
    }
    
    //检查测试集情况
    void RMSEProbe(void){
    	
        /*	1、load test数据集，
        	2、针对每一条数据，计算预测值，然后计算误差的平方和，计算总数量
        	3、输出rmse
        
        std::ifstream from("../../mldataset/u1.test");
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
		*/
    }

    void ratingAll(vector< Rating > & data){
    }
};

int main(int argc, char ** argv)
{
	time_t start,end;
    struct tm * timeStartInfo;
    struct tm * timeEndInfo;
    double duration;
	start = time(NULL);
    timeStartInfo = localtime(&start);
    string timeStartStr = asctime(timeStartInfo);
    double alpha = 0.005;  //经测试这个值比较好
    double beta = 0.002;   //经过测试这个也还行
    int dim = 100;//atoi(argv[1]);
    test_level = 1;//atoi(argv[2]);
    ofstream outputfile("parameter.txt");
    
    //for(int i=0; i < 10; i++)
    {
    //	beta = i*0.001 + 0.002;
    //	cout << beta << endl;
    	svd::model(dim,alpha,beta);
    	
    }
    end = time(NULL);
    timeEndInfo = localtime(&end);
    outputfile.close();
    duration = (end-start);
    cout << "Start at "<<timeStartStr<<", and end at "<< asctime(timeEndInfo);
    cout << "duration:"<<duration <<" s!" <<endl;
    return 0;
}
