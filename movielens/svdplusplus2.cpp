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
����Ӧ�ý���һ��svd��svd++������Ӧ�ý�����������svd++���ǵ������η�����p(u)����һ���򵥵�������
������������������������������item������һЩ������
*/
#include "netflix.h"
#include "mdefineplus.cpp"

namespace svd{
    vector<float> bu(USER_NUM+1,0), bi(ITEM_NUM+1,0);    //�û�ƫ�ú�itemƫ��
    vector<int> un(USER_NUM+1,0), in(ITEM_NUM+1,0);    //�û�ƫ�ú�itemƫ��  //�ֱ��¼�û�u��ֵ�item������item q����ֵĴ���
    vector< vector<float> > p(USER_NUM+1), q(ITEM_NUM+1);  //�����ƫ�þ���
	float mean = 0;
    
    //����svd++��ʽԤ����ֵ
    double rating(Rating  record){
        int user = record.user;
        int item = record.item;
        double ret = mean + bu[user] + bi[item] + dot(p[user], q[item]);
        if(ret < 1) ret = 1;
        if(ret > 5) ret = 5;
        return ret;
    }
    
    double rating(int user,int item)
    {
    	double ret = mean + bu[user] + bi[item] + dot(p[user], q[item]);
    	if(ret < 1) ret = 1;
    	if(ret > 5) ret = 5;
    	return ret;
    }
    
    int setMeanRating(vector<Rating> &data)
	{
	    //����ƽ��ֵ;
	    float sum = 0;
	    int num = 0;
	    int tmp = 0;
	    for(int i = 0; i < data.size(); ++i){
	    	tmp = data[i].value();
	    	if(data[i].value() > 0){
	    		sum += data[i].value();
	    		++num;
	    		
	    		++un[data[i].user];
	    		++in[data[i].item];
	    		bu[data[i].user] += data[i].rate;
	    		bi[data[i].item] += data[i].rate;
	    	}		
	    }
	    float totalMean = sum/num;
	    for(int i = 1; i < USER_NUM+1; ++i){
	    	cout<< bu[i]<<"\t"<<un[i]<<"\t";
	    	bu[i] = (bu[i]/un[i]) - totalMean;
	    	cout << bu[i] << endl;
	    }
	    for(int i = 1; i < ITEM_NUM+1; ++i){
	    	cout<< bi[i]<<"\t"<<in[i]<<"\t";
	    	bi[i] = (bi[i]/in[i]) - totalMean;
	    	cout << bi[i] << endl;
	    }
	    
	    return totalMean;
	}

	//�����Լ����
    void RMSEProbe(void){
    	
        /*	1��load test���ݼ���
        	2�����ÿһ�����ݣ�����Ԥ��ֵ��Ȼ���������ƽ���ͣ�����������
        	3�����rmse
        */
        std::ifstream from("u1.test");
        char rateStr[100];
        vector<string> rateDetail(4);
        int predict = 0;
        double rmse = 0.0;
        int num = 0;
        int realValue = 0;
        while(from.getline(rateStr,100)){
        	explode("	",rateStr,rateDetail);
        	if(rateDetail.size()>=3){
        		predict = rating(atoi(rateDetail[0].c_str()), atoi(rateDetail[1].c_str()));
        		realValue = atoi(rateDetail[2].c_str());
        		rmse += (predict-realValue) * (predict-realValue);
        		++num;
        	}
        }
        cout << "test set rmse:"<<sqrt(rmse/num) << endl;
    }

    void ratingAll(vector< Rating > & data){
        for(int i = 0; i < data.size(); ++i){
            data[i].setPredict(rating(data[i]));
        }
    }
     
    void model(int dim, double & alpha, double & beta)
    {
        vector<Rating> data;
        loadRating("u1.base",data);  //��ʼ��
        
        //1�����ȸ������������
        //��1����ƽ��ֵ
        //2��һ��һ��ʵ��ÿ������
        //3��ʲôʱ��Ū�꣬ʲôʱ���ȥ
        
        mean = setMeanRating(data); //��ƽ��ֵ����bu��bi��ֵ
        
        //�м�ɾ��һ�δ��룬��xlvector����ƽ�����д����ʱ��Դ�ֵ�Ӱ��������õ�
        
        double cc = 1 / (double)(dim);
        //cout << cc << endl;exit(1);
        
        //��ʼ��bu������user��bias��ƫ�ã�
        for(int i = 1; i < USER_NUM+1; ++i){
            //cout<< bu[i] << endl;
            //if(bu[i] < 0) continue;  //����bu[i]С��0��Զ����������������Զ����ִ��
            p[i].clear();            //������е�p[i]
            setRand(p[i],dim,cc);    //��ʼ��p[i]��������Ϊ��ʼ���ͳ�Ϊsvd++�ɣ��������������ӵ�
        }
        cout << bi.size() << endl;
        for(int i = 1; i < ITEM_NUM+1; ++i){
            //if(bi[i] < 0) continue;  
            //cout << i << endl;
            q[i].clear();
            setRand(q[i],dim,cc);
        }
        cout << "begin : " << endl;
        for(int step = 0; step < 25; ++step){  //ֻ����25��
            double rmse = 0;
            double n = 0;
            for(int ii = 0; ii < data.size(); ++ii){  //����ÿһ����ֵ��Ž��
                int i = ii;
                
                //�������䲻֪����ʲô�ã�
                //if(data[i].value() > 3) continue;
                //if(data[i].test > test_level) continue;
                
                int user = data[i].user;
                int item = data[i].item;
                double rui = data[i].value();
                double pui = rating(data[i]); //Ԥ���֣��������
                double err = rui - pui;
                rmse += err * err; ++n;
                //��������
                bu[user] += alpha * (err - beta * bu[user]);
                bi[item] += alpha * (err - beta * bi[item]);
                for(int k = 0; k < dim; ++k){
                    p[user][k] += alpha * (err * q[item][k] - beta * p[user][k]);
                    q[item][k] += alpha * (err * p[user][k] - beta * q[item][k]);
                }
            }
            cout << step << "\t" << sqrt(rmse / n) << endl;
            alpha *= 0.92;    //�𲽼�Сѧϰ����
        }
        ratingAll(data);  //Ԥ�����еĽ��
        RMSEProbe();  // ���ѵ�������
        
        //���bu��ֵ
        ofstream outputbu("svdbu.txt");
	    for(int i=1; i < USER_NUM+1; i++)
	    {
	    	outputbu << i << "\t" << bu[i] << endl;
	    }
	    outputbu.close();
	    
	    //���bi��ֵ
        ofstream outputbi("svdbi.txt");
	    for(int i=1; i < ITEM_NUM+1; i++)
	    {
	    	outputbi << i << "\t"<< bi[i] << endl;
	    }
	    outputbu.close();
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
    svd::model(dim,alpha,beta);
    outputfile.close();
    end = clock();
    duration = (end-start)/CLOCKS_PER_SEC;
    cout << "duration:"<<duration <<" s!" <<endl;
    return 0;
}