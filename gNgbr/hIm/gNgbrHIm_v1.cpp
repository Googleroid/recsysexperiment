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
 * �������Ŀ���ǲ��� global Neighborhood ������׼ȷ�ԣ�ʹ�õĹ�ʽ��Recommender system handbook�е�P171ҳ��5.35��
 *  ��ʽ����ȥ��Cij��cij��ʾ�ľ������η��������������Է�������Ч��
 */
#include "movielens.h"
#include "mdefine.cpp"

namespace svd{
	//ʹ��һЩȫ�ֱ������洢��Ҫ���ƵĲ�����bu��bi,wij
    vector<double> bu(USER_NUM+1,0), bi(ITEM_NUM+1,0);       //baselineԤ�����е��û�ƫ�ú�itemƫ��
    vector<int> buNum(USER_NUM+1,0);	//�û�u��ֵ�item������
    vector<int> biNum(ITEM_NUM+1,0);   //���item i�ֵ��û�����
    vector< vector<double> > w(ITEM_NUM+1);  //item-item���ƾ���
    vector< vector<double> > c(ITEM_NUM+1);  //item-item ���Է����������Թ�ϵ
    vector< vector<int> >  userItems(USER_NUM+1);   //�û�����ֵ�items-----����
    vector< vector <int> > rateMatrix(USER_NUM+1);  //��־���,�������ķ��˴������ڴ棬ʵ��ϵͳ�б����Ż������
                                                    //�Ż��أ�����������
	float mean = 0;                         //ȫ�ֵ�ƽ��ֵ
    
    //��������
    void RMSEProbe(void);
    void ratingAll(vector< Rating > & data);
    double rating(Rating  record);
    double predictRate(int user,int item);
    
    void model(int dim, double & alpha, double & beta)
    {
        vector<Rating> data;
        cout << "begin initialization: " << endl;
        
        //��rateMatix���г�ʼ�����ڳ�ʼ��֮ǰ�������ȶ�rateMatrix��ֵ�������еĴ�ָ�ֵΪ0
        for(int i = 1; i<USER_NUM+1;++i) {
        	for(int j = 0; j<ITEM_NUM+1; ++j) {
        		rateMatrix[i].push_back(0);
        	}
        }
        
        loadRating("../../mldataset/u1.base",data,rateMatrix);  //��ʼ�������������load ��data��
        
        mean = setMeanRating(data); //��ƽ��ֵ����bu��bi��ֵ
        int i,u,j,k;
        
        //��bu��bi���г�ʼ��,bu,bi�ĳ�ʼ���ķ�������ƽ��ֵ��Ȼ���ȥmean��
        //�ڼ���Ĺ����б���Ҫ�ǵ����е�ֵ���������еĴ�������ͷ����ܺ�
        for(i = 0; i < data.size(); ++i){  //����ÿһ����ֵ��Ž��
           	bu[data[i].user] += data[i].rate;
           	++buNum[data[i].user];
        	bi[data[i].item] += data[i].rate;
        	++biNum[data[i].item];
        	
        	userItems[data[i].user].push_back(data[i].item);//��ʼ��userItems����
        } //���Ϲ��������ͣ�sigma��
        
        //���¹�����ƽ��ֵ
        for(i = 1; i < USER_NUM+1; ++i) {
        	if(buNum[i]>1)bu[i] = bu[i]/buNum[i] - mean;
        	else bu[i] = 0.05;
        }
        
        for(i = 1; i < ITEM_NUM+1; ++i) {
        	if(biNum[i] > 1)bi[i] = bi[i]/biNum[i] - mean;
        	else bi[i] = 0.05;
        }
        
        //@todo ��֪���Ƿ�����Գ�ʼ���Ĺ�����һЩ�Ż�
        //��w���г�ʼ������ʼ���ķ����������������֪�����ַ����Ƿ�ã��Ƿ��Ӱ������������������
        for(int i = 1; i < ITEM_NUM+1; ++i){
            //cout<< bu[i] << endl;
            w[i].clear();            //������е�w[i]
            setRand(w[i],ITEM_NUM+1,0);    //��ʼ��w[i]
        }
        
        //@todo ��֪���Ƿ�����Գ�ʼ���Ĺ�����һЩ�Ż�
        //��c���г�ʼ������ʼ���ķ����������������֪�����ַ����Ƿ�ã��Ƿ��Ӱ������������������
        for(int i = 1; i < ITEM_NUM+1; ++i){
            //cout<< bu[i] << endl;
            c[i].clear();            //������е�c[i]
            setRand(c[i],ITEM_NUM+1,0);    //��ʼ��c[i]
        }
       
        cout <<"initialization end!"<<endl<< "begin iteration: " << endl;
        
        double pui = 0.0 ; // Ԥ���u��i�Ĵ��
        double preRmse = 1000000000000.0; //���ڼ�¼��һ��rmse����Ϊ��ֹ������һ�֣����rmse�����ˣ���ֹͣ
        double nowRmse = 0.0;
        for(int step = 0; step < 35; ++step){  //ֻ����25��
            double rmse = 0;
            double n = 0;
            
            for( u = 1; u < USER_NUM+1; ++u) {   //ѭ������ÿһ���û� 
                for(i = 0; i < userItems[u].size(); ++i) {
                	//ΪԤ��u��i�Ĵ����һЩ׼��
                	double sumBias = 0;
                	for(j=0; j < userItems[u].size(); ++j) {
                		double buj = mean + bu[u] + bi[userItems[u][j]];
                		double ruj = rateMatrix[u][userItems[u][j]];// u �� j�Ĵ����ο��ٻ���أ�
                		sumBias += (ruj - buj) * w[userItems[u][i]][userItems[u][j]] + c[userItems[u][i]][userItems[u][j]];
                	}
                	if( userItems[u].size() > 1)pui = mean + bu[u] + bi[userItems[u][i]] + (1.0/sqrt(userItems[u].size()))*sumBias;//�����Ȳ���k���б仯����ȡk=�����
                	else pui = mean;
                	if(pui < 1) pui = 1;
                	if(pui > 5) pui = 5;
                	//cout <<pui<<"	"<< rateMatrix[u][userItems[u][i]]<<endl;
                	double eui = rateMatrix[u][userItems[u][i]] - pui;
                	rmse += eui * eui; ++n;
                	
                	//����bu��bi
                	bu[u] += alpha * (eui - beta * bu[u]);
                	bi[userItems[u][i]] += alpha * (eui - beta * bi[userItems[u][i]]);
                	
                	//����wij
                	for(j=0; j < userItems[u].size(); ++j) {
                		double buj = mean + bu[u] + bi[userItems[u][j]];
                		w[userItems[u][i]][userItems[u][j]] += alpha * ( (1/sqrt(userItems[u].size())) * eui * (rateMatrix[u][userItems[u][j]] - buj) -  beta * w[userItems[u][i]][userItems[u][j]]);
                		c[userItems[u][i]][userItems[u][j]] += alpha * ( (1/sqrt(userItems[u].size())) * eui - beta * c[userItems[u][i]][userItems[u][j]]);
                	}	
                } 
            }
            nowRmse =  sqrt(rmse / n);
            if( nowRmse >= preRmse && step > 5) break; //���rmse�Ѿ���ʼ�����ˣ�������ѭ��
            else
            	preRmse = nowRmse;
            cout << step << "\t" << nowRmse << endl;
            alpha *= 0.992;    //�𲽼�Сѧϰ����
            RMSEProbe(); 
        }
        //ratingAll(data);  //Ԥ�����еĽ��
        
        RMSEProbe();  // ���ѵ�������
        return;
        
        //���bu��ֵ
        ofstream outputbu("bu.txt");
	    for(int i=1; i < USER_NUM+1; i++)
	    {
	    	outputbu << i << "\t" << bu[i] << endl;
	    }
	    outputbu.close();
	    
	    //���bi��ֵ
        ofstream outputbi("bi.txt");
	    for(int i=1; i < ITEM_NUM+1; i++)
	    {
	    	outputbi << i << "\t"<< bi[i] << endl;
	    }
	    outputbu.close();
	    
    }
    
    //����svd++��ʽԤ����ֵ
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
    	int u = user;
    	int i =  item;
    	double sumBias = 0.0;
    	for(int j=0; j < userItems[u].size(); ++j) {
    		double buj = mean + bu[u] + bi[userItems[u][j]];
    		double ruj = rateMatrix[u][userItems[u][j]];// u �� j�Ĵ����ο��ٻ���أ�
    		sumBias += (ruj - buj) * w[i][userItems[u][j]] + c[i][userItems[u][j]];
    	}
    	double ret = 0.0;
    	if(userItems[u].size()>1)
    	{
    		ret = mean + bu[u] + bi[i] + (1.0/sqrt(userItems[u].size())) * sumBias;//�����Ȳ���k���б仯����ȡk=�����
    	}
    	else ret  = mean;
    	if(ret < 1.0) ret = 1;
        if(ret > 5.0) ret = 5;
        //cout <<u<<"	"<<i<<"	"<<mean <<"	"<<bu[u]<<"	"<<bi[i]<<"	"<<userItems[u].size()<<"	"<<sumBias<<"	"<<ret<<endl;
        return ret;
    }
    
    //�����Լ����
    void RMSEProbe(void){
    	
        /*	1��load test���ݼ���
        	2�����ÿһ�����ݣ�����Ԥ��ֵ��Ȼ���������ƽ���ͣ�����������
        	3�����rmse
        */
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
    double alpha = 0.013;  //���������ֵ�ȽϺ�
    double beta = 0.001;   //�����������Ҳ����
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
