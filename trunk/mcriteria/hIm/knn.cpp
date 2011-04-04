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
 * �������Ŀ���ǲ��������㷨 knn����������Ҫ�����黹�����pre-computation�����ȼ����
 �� item-item�����ƶ�
 �� ���㲽�裺
 * ��1��Ԥ����ķ�ʽ������ÿ��item��ƽ����֣�����mean������
   ��2��ѭ��i��j�����ÿһ��ij�������ƶ�
   ��3������ÿһ��i��j���ҳ������ǹ�ͬ��ֵ�userlist������һ��set��
   ��4��Ӧ�ù�ʽRecommender system handbook��P125ҳ��ʽ4.21���Լ�������ƶȣ��洢�ڶ�ά������
   ����������ƶȱ�ȻӦ���ǶԳƵģ�������Գƣ�˵�������Ȼ�ǲ��Եġ�
 ��
 
      
 */
#include "movielens.h"
#include "mdefine.cpp"

namespace svd{
	//ʹ��һЩȫ�ֱ������洢��Ҫ���ƵĲ�����bu��bi,wij
    vector<double> mi(ITEM_NUM+1,0.0);         //�����洢ÿ��item��ƽ�����
    vector< vector<double> > w(ITEM_NUM+1);  //item-item���ƾ���
    vector< vector<int> >  userItems(USER_NUM+1);   //�û�����ֵ�items-----����
    //vector< vector <int> > rateMatrix(USER_NUM+1);  //��־���,�������ķ��˴������ڴ棬ʵ��ϵͳ�б����Ż������
                                                    //�Ż��أ�����������---->�Ѿ�ͨ���������һ��map��������
    map<int,int> rateMatrix[ITEM_NUM+1];           //ʹ��һ��map����洢ϡ��Ĵ�־���                     
	float mean = 0;                         //ȫ�ֵ�ƽ��ֵ
    
    //��������
    void RMSEProbe(void);
    void ratingAll(vector< Rating > & data);
    double rating(Rating  record);
    double predictRate(int user,int item);
    double getSim(int item1,int item2);
    
    void model(int dim, double & alpha, double & beta)
    {
        vector<Rating> data;
        cout << "begin initialization: " << endl;
        
        loadRating("../mldataset/u1.base",data,rateMatrix);  //��ʼ�������������load ��data��
       
        mean = setMeanRating(data); //��ƽ��ֵ����bu��bi��ֵ
        
        int i,u,j,k;
        
        //��ʼ��w
        for(int i = 1; i < ITEM_NUM+1; ++i){
            w[i].clear();            //������е�p[i]
            for(j = 1; j < ITEM_NUM+1; ++j) {
        		w[i].push_back(0);
        	}
        }
        
        //��mi����ÿ��item��ƽ�����
        for(i = 1; i < ITEM_NUM+1; ++i){  //����ÿһ����ֵ��Ž��
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
		
        //ѭ��ÿ��item �������ǵ����ƶ�
        
       	for(i = 1; i < ITEM_NUM+1; ++i){  //����ÿһ����ֵ��Ž��
        	//cout<<i<<':'<<'\t';
        	for(j = i; j < ITEM_NUM+1; ++j) {
        		w[i][j] = getSim(i,j);
        		if(j != i)w[j][i] = w[i][j];
        		//cout<<w[i][j]<<'\t';
        	}
        	//cout <<i<< "  over!"<<endl;
        }
        
        //���w��ֵ
        ofstream outputw("sim.txt");
	    for(i=1; i < ITEM_NUM+1; ++i)
	    {
	    	for(j=1; j < ITEM_NUM+1; ++j) {
	    		outputw << w[i][j] << "\t";
	    	}
	    	outputw << endl;
	    }
	    outputw.close();
	    
	    /*
	    //���bi��ֵ
        ofstream outputbi("bi.txt");
	    for(int i=1; i < ITEM_NUM+1; i++)
	    {
	    	outputbi << i << "\t"<< bi[i] << endl;
	    }
	    outputbu.close();
	    */
	    
    }
    
    double getSim(int item1, int item2) {
    	//(1)�ҵ���ֵĹ����û����ϣ�2�����չ�ʽ����
    	if(item1 == item2)return 1.0;
    	if(0 == rateMatrix[item1].size() || 0 == rateMatrix[item2].size() ) return 0.0;
    	map <int,int> ::iterator it = rateMatrix[item1].begin(); 
    	map <int,int> ::iterator end = rateMatrix[item1].end(); 
    	double sum1 = 0.0, sumSquare1 = 0.0, sumSquare2 = 0.0;
    	for(; it != end; ++it) {
    		int user = (*it).first;
    		if(rateMatrix[item2].find(user) != rateMatrix[item2].end()) {
    			//cout<<"common user:"<<user<<'\t'<<rateMatrix[item1][user]<<'\t'<<rateMatrix[item2][user]<<endl;
    			//�Ѿ��ҵ��˹���Ԫ��
    			sum1 += (rateMatrix[item1][user] - mi[item1]) *(rateMatrix[item2][user] - mi[item2]);
    			sumSquare1 += pow(rateMatrix[item1][user] - mi[item1],2); 
    			sumSquare2 += pow(rateMatrix[item2][user] - mi[item2],2);
    		}
    	}
    	double sim = 0.0;
    	if(sumSquare1 > 0.0000000001  && sumSquare2 > 0.0000000001)sim = sum1 / sqrt( sumSquare1 * sumSquare2);
    	return sim;
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
    	/*
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
        */
    }
    
    //�����Լ����
    void RMSEProbe(void){
    	
        /*	1��load test���ݼ���
        	2�����ÿһ�����ݣ�����Ԥ��ֵ��Ȼ���������ƽ���ͣ�����������
        	3�����rmse
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
