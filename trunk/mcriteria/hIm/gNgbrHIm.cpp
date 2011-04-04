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
 * �������Ŀ���ǲ��� global Neighborhood ��������չ�ԣ���Recommender system handbook�е�P171ҳ��5.35��
 *  ��ʽ����չ��3ά��ָ���ֵ����
 
    ʵ�ֵķ�ʽҲ�ǽ���ʽ��ǿ��Progressive enhancement������Ҫ��һ����λ
    
    
    ���������id���Ǵ�1��ʼ���ӵģ������Ҫ��һ�µ�����ʹ���1��ʼ���ӣ�һֱ��USER_NUMΪֹ���������д���ĵ�һ��
    ������õķ�����php�ű�����������ԭʼ���ݴ���һ�飬���б����滻
	ʹ�õĴ�����../replaceUserHotelId.php�У�Ŀǰ�Ѿ�������ϣ�����Ŀǰ�������������̫��
  
 */
#include "tripadvisor.h"
#include "mdefine.cpp"

namespace svd{
	//ʹ��һЩȫ�ֱ������洢��Ҫ���ƵĲ�����bu��bi,wij
    vector<double> user(USER_NUM+1,0);   //���ڴ洢ԭʼId
    vector<double> hotel(ITEM_NUM+1,0);
    
    double bu[USER_NUM+1] = {0}; 
	double bi[ITEM_NUM+1] = {0};
	double bc[CRI_NUM+1]  = {0};      //baselineԤ�����е��û�ƫ��,itemƫ��,criteriaƫ��
    vector<int> buNum(USER_NUM+1,0);	//�û�u��ֵ�item������
    vector<int> biNum(ITEM_NUM+1,0);   //���item i�ֵ��û�����
    vector<int> bcNum(ITEM_NUM+1,0);   //�����criteria����ֵ��û�����

	vector<double> buBase(USER_NUM+1,0);	//�û�u��ֵ�item������
    vector<double> biBase(ITEM_NUM+1,0);   //���item i�ֵ��û�����
    vector<double> bcBase(ITEM_NUM+1,0);   //�����criteria����ֵ��û�����
    
    double wMatrix[ITEM_NUM+1][ITEM_NUM+1][CRI_NUM+1] = {0};  //��Ҫѧϰ��item-item���ƾ���
	double cMatrix[ITEM_NUM+1][ITEM_NUM+1][CRI_NUM+1] = {0};  //�������Ӿ���

    map<int,int> rateMatrix[USER_NUM+1][CRI_NUM+1];   //ʹ�ö�ά��map����洢ϡ��Ĵ�־����Ƿ��и��õķ���������
	double mean = 0;                         //ȫ�ֵ�ƽ��ֵ
    
    //��������u
    void RMSEProbe(void);
    void  loadSim(char *fileName);
    void ratingAll(vector< Rating > & data);
    double rating(Rating  record);
    double predictRate(int user,int item, int cri);
    
    void model(int dim, double  alpha, double  beta)
    {
        cout << "begin initialization: " << endl;
        
        loadRating("../../tripDataset_t/",rateMatrix);  //load data
        
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

        //���¹�����ƽ��ֵ
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
	            setRand(wMatrix[i][j],CRI_NUM+1,0);    //��ʼ��w[i]
			}
        }
		cout<<"wMatrix initialized end"<<endl;
        //initialize the cMatrix        
        for(i = 1; i < ITEM_NUM+1; ++i){
			for(j = 1; j < ITEM_NUM+1; ++j){
	            setRand(cMatrix[i][j],CRI_NUM+1,0);    //��ʼ��c[i]
			}
        }
        cout<<"cMatrix initialized end"<<endl; 
        cout <<"initialization end!"<<endl<< "begin iteration: " << endl;
        
        double pui = 0.0 ; // Ԥ���u��i�Ĵ��
        double preRmse = 1000000000000.0; //���ڼ�¼��һ��rmse����Ϊ��ֹ������һ�֣����rmse�����ˣ���ֹͣ
        double nowRmse = 0.0;
        for(int step = 0; step < 35; ++step){  //ֻ����25��
            double rmse = 0;
            double n = 0;
            
            for( u = 1; u < USER_NUM+1; ++u) {   //ѭ������ÿһ���û� 
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
						
						//update bu��bi,bc
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
            if( nowRmse >= preRmse && step > 10) break; //���rmse�Ѿ���ʼ�����ˣ�������ѭ��
            else
            	preRmse = nowRmse;
            cout << step << "\t" << nowRmse << endl;
            //alpha *= 0.992;    //�𲽼�Сѧϰ����
            //RMSEProbe(); 
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
    
    //�����Լ����
    void RMSEProbe(void){
    	
        /*	1��load test���ݼ���
        	2�����ÿһ�����ݣ�����Ԥ��ֵ��Ȼ���������ƽ���ͣ�����������
        	3�����rmse
        
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
	double start,end,duration; 
	start = clock();
    double alpha = 0.005;  //���������ֵ�ȽϺ�
    double beta = 0.002;   //�����������Ҳ����
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
