/* This file is Copyright (C) 2011 Lv Hongliang. All Rights Reserved.
 * please maitain the copyright information completely when you redistribute the code.
 * 
 * Please contact me via email honglianglv@gmail.com
 * my blog: http://lifecrunch.biz
 * my twitter: http://twitter.com/honglianglv
 *
 * It is free software; you can redistribute it and/or modify it under 
 * the GNU General Public License as published by the Free Software
 * Foundation; either version 1, or (at your option) any later version.
 *
 * �������Ŀ����ʵ��koren��SIGKDD'08�����еķ�����svd model
 */
#include "movielens.h"
#include "mdefine.cpp"

namespace svd{
	//ʹ��һЩȫ�ֱ������洢��Ҫ���ƵĲ�����bu��bi,p,q
    double bu[USER_NUM+1] = {0};
    double bi[ITEM_NUM+1] = {0};       //baselineԤ�����е��û�ƫ�ú�itemƫ��
    
    int buNum[USER_NUM+1] = {0};	//�û�u��ֵ�item������
    int biNum[ITEM_NUM+1] = {0};   //���item i�ֵ��û�����
    
    double p[USER_NUM+1][K_NUM+1] = {0};   //���ڴ洢�û�����������p
    double q[ITEM_NUM+1][K_NUM+1] = {0};   //����item����������q
    
    vector < vector<rateNode> > rateMatrix(USER_NUM+1);           //ʹ��һ��vector����洢ϡ��Ĵ�־���                     
	float mean = 0;                         //ȫ�ֵ�ƽ��ֵ
    
    //��������
    double RMSEProbe(void);
    
    float predictRate(int user,int item);
    
    void model(int dim, float  alpha, float  beta)
    {
        cout << "begin initialization: " << endl;
        
        loadRating(DIR_PATH,rateMatrix);  //��ʼ�����

        mean = setMeanRating(rateMatrix); //��ƽ��ֵ����bu��bi��ֵ
        
        int i,u,j,k;
        
       
        /*
        //��bu��bi���г�ʼ��,bu,bi�ĳ�ʼ���ķ�������ƽ��ֵ��Ȼ���ȥmean��
        //�ڼ���Ĺ����б���Ҫ�ǵ����е�ֵ���������еĴ�������ͷ����ܺ�
        int tmpIndex = 0;
	    for(i = 1; i < USER_NUM+1; ++i){
	    	int vSize = rateMatrix[i].size();
			for(j=0; j < vSize; ++j) {
				bi[rateMatrix[i][j].item] += (rateMatrix[i][j].rate - mean);
				biNum[rateMatrix[i][j].item] += 1;
			}			
	    }
        
       
        //ofstream logbi("bi.txt");
        for(i = 1; i < ITEM_NUM+1; ++i) {
        	if(biNum[i] >=1)bi[i] = bi[i]/(biNum[i]+25);
        	else bi[i] = 0.0;
        	
        }
       
         for(i = 1; i < USER_NUM+1; ++i){
	    	int vSize = rateMatrix[i].size();
			for(j=0; j < vSize; ++j) {
				bu[i] += (rateMatrix[i][j].rate - mean - bi[rateMatrix[i][j].item]);
				buNum[i] += 1;
			}			
	    }        
        
         //���¹�����ƽ��ֵ
        for(i = 1; i < USER_NUM+1; ++i) {
        	if(buNum[i]>=1)bu[i] = bu[i]/(buNum[i]+10);
        	else bu[i] = 0.0;
        	//logbi<<i<<"	"<<buNum[i]<<"	"<<bu[i]<<endl;
        }
        //logbi.close();
        */

        //@todo ��֪���Ƿ�����Գ�ʼ���Ĺ�����һЩ�Ż�
        //��w���г�ʼ������ʼ���ķ����������������֪�����ַ����Ƿ�ã��Ƿ��Ӱ������������������
        for(int i = 1; i < ITEM_NUM+1; ++i){
           setRand(q[i],K_NUM,0);    //��ʼ��q[i]
        }
        
        for(int i = 1; i < USER_NUM+1; ++i){
           setRand(p[i],K_NUM,0);    //��ʼ��p[i]
        }
       
        cout <<"initialization end!"<<endl<< "begin iteration: " << endl;
        
        float pui = 0.0 ; // Ԥ���u��i�Ĵ��
        double preRmse = 100000000.0;
        double preProbRmse = RMSEProbe();//���ڼ�¼��һ��rmse����Ϊ��ֹ������һ�֣����rmse�����ˣ���ֹͣu 
        double nowRmse = 0.0;
        double nowProbRmse = 0.0;
        
        cout <<"begin testRMSEProbe(): " << endl;
        
        //main loop
        for(int step = 0; step < 290; ++step){  //ֻ����60��
            long double rmse = 0.0;
            int n = 0;
            for( u = 1; u < USER_NUM+1; ++u) {   //ѭ������ÿһ���û� 
            	
               	int RuNum = rateMatrix[u].size(); //�û�u����ֵ�item��Ŀ
               	float sqrtRuNum = 0.0;
               	if(RuNum>1) sqrtRuNum = (1.0/sqrt(RuNum));
               	
                //��������
                for(i=0; i < RuNum; ++i) {// ѭ������u��ֹ���ÿһ��item
                	int itemI = rateMatrix[u][i].item;
                	short rui = rateMatrix[u][i].rate; //ʵ�ʵĴ��
                	pui = predictRate(u,itemI);
                	
                	float eui = rui - pui;
                	
                	if( isnan(eui) ) {// fabs(eui) >= 4.2 || 
                		cout<<u<<'\t'<<i<<'\t'<<pui<<'\t'<<rui<<"	"<<bu[u]<<"	"<<bi[itemI]<<"	"<<mean<<endl;
                		//printArray(q[itemI],p[u],K_NUM+1);
                		exit(1);
                	}
                	rmse += eui * eui; ++n;
                	if(n % 10000000 == 0)cout<<"step:"<<step<<"	n:"<<n<<" dealed!"<<endl;
                	
                	//bu[u] += alpha * (eui - beta * bu[u]);
                	//bi[itemI] += alpha * (eui - beta * bi[itemI]);
                	
                	for( k=1; k< K_NUM+1; ++k) {
	               		double tempPu = p[u][k];
	               		p[u][k] += alpha * (eui*q[itemI][k] - beta*p[u][k]);
	               		q[itemI][k] += alpha * (eui*tempPu - beta*q[itemI][k]);
	               	}
                } 
            }
            nowRmse =  sqrt( rmse / n);
            
            cout << step << "\t" << nowRmse <<'\t'<< preRmse<<" 	n:"<<n<<endl;
            
            nowProbRmse = RMSEProbe(); // ���ѵ�������
            if( nowRmse >= preRmse && step >= 3) break; //���rmse�Ѿ���ʼ�����ˣ�������ѭ��
            else
            	preRmse = nowRmse;
            
            if(alpha > 0.01)alpha *= 0.9;    //�𲽼�Сѧϰ����
            else if( alpha > 0.003) alpha *= 0.999;
            //RMSEProbe(); 
        }
        RMSEProbe();  // ���ѵ�������
        return;    
    }
    
    //����svd��ʽԤ����ֵ
    float predictRate(int user,int item)
    {
    	int RuNum = rateMatrix[user].size(); //�û�u����ֵ�item��Ŀ
       	float sqrtRuNum = 0.0;
       	double ret; 
    	if(RuNum > 1)
    	{
    		ret = mean + bu[user] + bi[item] +  dot(p[user],q[item]);//�����Ȳ���k���б仯����ȡk=�����
    	}
    	else ret  = mean+bu[user] + bi[item];
    	if(ret < 1.0) ret = 1;
        if(ret > 5.0) ret = 5;
        //cout<<ret<<"    "<<mean<<"    "<<bu[user]<<"     "<<bi[item]<<endl;
        return ret;
    }
    
    //�����Լ����
    double RMSEProbe(void){
    	
        /*	1��load test���ݼ���
        	2�����ÿһ�����ݣ�����Ԥ��ֵ��Ȼ���������ƽ���ͣ�����������
        	3�����rmse
        
        */
         //���ȶ�ȡprobe_t.txt �ļ�������ÿһ��rate�����Ԥ��Ĵ�֣����ݶ�Ӧ��ʵ�ʴ�ּ���RMSE
		ifstream in(PROBE_SET);
		char rateStr[256];
		int pos1,pos2,pos3;
		string strTemp;
		int rateValue,itemId,userId,probeNum=0;
		float pRate,err;
		long double rmse = 0;
		
		while(in.getline(rateStr,256)){
	    	strTemp = rateStr;
	    	if(strTemp.length() < 4) continue;
		    pos1 = strTemp.find("	");
		    pos2 = strTemp.find("	",pos1+1);
		    pos3 = strTemp.find("	",pos2+1);
	    	userId = atoi(strTemp.substr(0,pos1).c_str());
	    	itemId = atoi(strTemp.substr(pos1+1,pos2-pos1-1).c_str());
	    	rateValue = atoi(strTemp.substr(pos2+1,pos3-pos2-1).c_str());
	    	
	    	pRate = predictRate(userId,itemId);
	    	err = pRate-rateValue;
	    	
	    	rmse += err*err;++probeNum;
	    }
	    rmse = sqrt( rmse / probeNum);
	    cout<<"RMSE:"<<rmse<<" probeNum:"<<probeNum<<endl;
		in.close(); //load userId map���
        return rmse;
    }
};

int main(int argc, char ** argv)
{
	time_t start,end;
    struct tm * timeStartInfo;
    struct tm * timeEndInfo;
    double duration; 
	start = time(NULL);
    float alpha = 0.01;  //0.0045according to the paper of "a guide to SVD for CF"
    float beta = 0.05;   //0.015 according to the paper of "a guide to SVD for CF"
    					 //0.0005 according the experiment
    
    //for(int i=0; i < 10; i++)
    {
    	//alpha = i*0.0005 + 0.0025;
    	//cout << alpha << endl;
    	svd::model(K_NUM,alpha,beta);	
    }
    end = time(NULL);
    duration = (end-start);
    timeStartInfo = localtime(&start);
    timeEndInfo = localtime(&end);
    cout << "Start at "<<asctime(timeStartInfo)<<", and end at "<< asctime(timeEndInfo)<<"!"<<endl;
    cout << "duration:"<<duration <<" s!" <<endl;
    return 0;
}
