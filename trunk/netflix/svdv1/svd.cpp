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
 * �������Ŀ����ʵ��koren��TKDD'09�����еķ�����learnFactorizedNeighborhoodModel�ķ���
 * the difference between the code in this file  and the code in netflix/svd/svd.cpp is the way of storing the rate
 * in this file, I store the rate in the sparse matrix ,the row is item and the column is user.
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
    
    vector < vector<rateNode> > rateMatrix(ITEM_NUM+1);           //ʹ�ö�άrateNode����洢ϡ��Ĵ�־���                     
	double mean = 0;                         //ȫ�ֵ�ƽ��ֵ
    
    //��������
    void RMSEProbe(void);
    
    float predictRate(int user,int item);
    
    void model(int dim, float  alpha1, float alpha2, float beta1, float beta2)
    {
        cout << "begin initialization: " << endl;
        
        loadRating(DIR_PATH,rateMatrix);  //��ʼ�����
        
        //getProbeReal(); //get the probe_t.txt
        //exit(0);
        mean = setMeanRating(rateMatrix); //��ƽ��ֵ����bu��bi��ֵ
        int i,j,k,itemI,v;
        
        //��bu��bi���г�ʼ��,bu,bi�ĳ�ʼ���ķ�������ƽ��ֵ��Ȼ���ȥmean��
        //�ڼ���Ĺ����б���Ҫ�ǵ����е�ֵ���������еĴ�������ͷ����ܺ�
        int tmpIndex = 0;
	    for(i = 1; i < ITEM_NUM+1; ++i){
	    	int vSize = rateMatrix[i].size();
			for(j=0; j < vSize; ++j) {
				bi[rateMatrix[i][j].item] += (rateMatrix[i][j].rate - mean);
				biNum[rateMatrix[i][j].item] += 1;
			}			
	    }
        
       
        
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
        }
        
        srand((unsigned)time(0));
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
        double preRmse = 1000000000000.0; //���ڼ�¼��һ��rmse����Ϊ��ֹ������һ�֣����rmse�����ˣ���ֹͣ
        double nowRmse = 0.0;
        
        cout <<"begin testRMSEProbe(): " << endl;
        RMSEProbe();
        
        //main loop
        for(int step = 0; step < 60; ++step){  //ֻ����60��
            long double rmse = 0.0;
            int n = 0;
            for( itemI = 1; itemI < ITEM_NUM+1; ++itemI) {   //ѭ������ÿһ���û� 	
               	int RiNum = rateMatrix[itemI].size(); //  the user num who rated the item itemI
               	float sqrtRiNum = 0.0;
               	if(RiNum>1) sqrtRiNum = (1.0/sqrt(RiNum));
               	
                //��������
                for( v=0; v < RiNum; ++v) {// ѭ������u��ֹ���ÿһ��item
                	int u = rateMatrix[itemI][v].user;
                	short rui = rateMatrix[itemI][v].rate; //ʵ�ʵĴ��
                	double bui = mean + bu[u] + bi[itemI];
                	pui = predictRate(u,itemI);
                	
                	float eui = rui - pui;
                	
                	if( isnan(eui) ) {// fabs(eui) >= 4.2 || 
                		cout<<u<<'\t'<<i<<'\t'<<pui<<'\t'<<rui<<"	"<<bu[u]<<"	"<<bi[itemI]<<"	"<<mean<<endl;
                		//printArray(q[itemI],p[u],K_NUM+1);
                		exit(1);
                	}
                	rmse += eui * eui; ++n;
                	if(n % 10000000 == 0)cout<<"step:"<<step<<"	n:"<<n<<" dealed!"<<endl;
                	
                	bu[u] += alpha1 * (eui - beta1 * bu[u]);
                	bi[itemI] += alpha1 * (eui - beta1 * bi[itemI]);
                	
                	for( k=1; k< K_NUM+1; ++k) {
	               		//double tempPu = p[u][k];
	               		p[u][k] += alpha2 * (eui*q[itemI][k] - beta2*p[u][k]);
	               		q[itemI][k] += alpha2 * (eui*p[u][k] - beta2*q[itemI][k]);
	               	}
                } 
            }
            nowRmse =  sqrt( rmse / n);
            
            if( nowRmse >= preRmse && step >= 3) break; //���rmse�Ѿ���ʼ�����ˣ�������ѭ��
            else
            	preRmse = nowRmse;
            cout << step << "\t" << nowRmse <<'\t'<< preRmse<<" 	n:"<<n<<endl;
            RMSEProbe();  // ���ѵ�������
            
            alpha1 *= 0.9;    //�𲽼�Сѧϰ����
            alpha2 *= 0.9;
        }
        RMSEProbe();  // ���ѵ�������
        return;
	    
    }
    
    float predictRate(int user,int item)
    {
    	int RiNum = rateMatrix[item].size(); //�û�u����ֵ�item��Ŀ
       	double ret; 
    	if(RiNum > 1)
    	{
    		ret = mean + bu[user] + bi[item] +  dot(p[user],q[item]);//�����Ȳ���k���б仯����ȡk=�����
    	}
    	else ret  = mean+bu[user] + bi[item];
    	if(ret < 1.0) ret = 1;
        if(ret > 5.0) ret = 5;
        return ret;
    }
    
    //�����Լ����
    void RMSEProbe(void){
    	
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
		    pos1 = strTemp.find(",");
		    pos2 = strTemp.find(",",pos1+1);
	    	itemId = atoi(strTemp.substr(0,pos1).c_str());
	    	userId = atoi(strTemp.substr(pos1+1,pos2-pos1-1).c_str());
	    	rateValue = atoi(strTemp.substr(pos2+1).c_str());
	    	
	    	pRate = predictRate(userId,itemId);
	    	err = pRate-rateValue;
	    	
	    	rmse += err*err;++probeNum;
	    }
	    rmse = sqrt( rmse / probeNum);
	    cout<<"RMSE:"<<rmse<<" probeNum:"<<probeNum<<endl;
		in.close(); //load userId map���
    }
    
    /*
     *  find the real rate of the probe.txt
     */
    void getProbeReal()
	{
		//���ȶ�ȡprobe_t.txt �ļ�������ÿһ��rate���ҳ���Ӧ��ʵ�ʴ��
		ifstream in("probe_t.txt");
		ofstream out("probe_real.txt");
		ofstream logNP("n_p.txt");
		char rateStr[256];
		int pos1,pos2;
		string strTemp;
		int rateValue,itemId,userId;
		//load userIdMap
		while(in.getline(rateStr,256)){
	    	strTemp = rateStr;
		    pos1 = strTemp.find(",");
	    	itemId = atoi(strTemp.substr(0,pos1).c_str());
	    	userId = atoi(strTemp.substr(pos1+1).c_str());
	    	//cout<<itemId<<'\t'<<userId<<'\t'<<endl;	    	exit(0);
	    	rateValue = getRate(rateMatrix[itemId],userId);
	    	if(rateValue == 0) {
	    		logNP<<itemId<<','<<userId<<','<<rateValue<<endl;
	    	}
	    	out<<itemId<<','<<userId<<','<<rateValue<<endl;
	    }
		in.close(); //load userId map���
		out.close();
		logNP.close();
	}
};

int main(int argc, char ** argv)
{
	time_t start,end;
    struct tm* startInfo;
    struct tm* endInfo;
    double duration;
	start = time(NULL);
    startInfo = localtime(&start);
    string startStr = asctime(startInfo);
    float alpha1 = 0.008;  //according the paper of koren: Factorization Meets the Neighborhood: SIGKDD'08
    float alpha2 = 0.008;    //according the paper of koren: Factorization Meets the Neighborhood: SIGKDD'08
    float beta1  = 0.01;
    float beta2  = 0.01;
    
    //for(int i=0; i < 10; i++)
    {
    //	beta = i*0.001 + 0.002;
    //	cout << beta << endl;
    	svd::model(K_NUM,alpha1,alpha2,beta1,beta2);
    	
    }
    end = time(NULL);
    duration = end-start;
    endInfo =   localtime(&end);
    cout << "start at" << startStr << ". And end at "<< asctime(endInfo) <<endl;
    cout << "duration:"<<duration <<" s!" <<endl;
    return 0;
}
