/**
 * �������Ŀ����ʵ��koren��TKDD'09�����еķ�����learnFactorizedNeighborhoodModel�ķ���
 */
#include "netflix.h"
#include "mdefine.cpp"

namespace svd{
	//ʹ��һЩȫ�ֱ������洢��Ҫ���ƵĲ�����bu��bi,wij
    vector<float> bu(USER_NUM+1,0), bi(ITEM_NUM+1,0);       //baselineԤ�����е��û�ƫ�ú�itemƫ��
    vector<int> buNum(USER_NUM+1,0);	//�û�u��ֵ�item������
    vector<int> biNum(ITEM_NUM+1,0);   //���item i�ֵ��û�����
    
    vector< vector<float> > q(ITEM_NUM+1); //Item����
    vector< vector<float> > x(ITEM_NUM+1); //
    vector< vector<float> > y(ITEM_NUM+1); //
    
    vector< vector<int> >  userItems(USER_NUM+1);   //�û�����ֵ�items-----����
    map<int,int> rateMatrix[USER_NUM+1];           //ʹ��һ��map����洢ϡ��Ĵ�־���                     
	double mean = 0;                         //ȫ�ֵ�ƽ��ֵ
    
    //��������
    void RMSEProbe(void);
    
    double predictRate(int user,int item);
    
    void model(int dim, double & alpha, double & beta)
    {
        vector<Rating> data;
        cout << "begin initialization: " << endl;
        
        loadRating(DIR_PATH,data,rateMatrix);  //��ʼ�����
        
        mean = setMeanRating(data); //��ƽ��ֵ����bu��bi��ֵ
        
        vector<double> pu(K_NUM+1,0);   //���ڴ洢�м����puk
        int i,u,j,k;
        
        //��bu��bi���г�ʼ��,bu,bi�ĳ�ʼ���ķ�������ƽ��ֵ��Ȼ���ȥmean��
        //�ڼ���Ĺ����б���Ҫ�ǵ����е�ֵ���������еĴ�������ͷ����ܺ�
        cout<<data.size()<<endl;
        for(i = 0; i < data.size(); ++i){  //����ÿһ����ֵ��Ž��
           	bu[data[i].user] += data[i].rate;
           	++buNum[data[i].user];
        	bi[data[i].item] += data[i].rate;
        	++biNum[data[i].item];
        	
        	userItems[data[i].user].push_back(data[i].item);//��ʼ��userItems����
        	if(data[i].item > ITEM_NUM )cout<<"*********"<<data[i].item <<"*****"<<endl;
        } //���Ϲ��������ͣ�sigma��
        
        //���¹�����ƽ��ֵ
        for(i = 1; i < USER_NUM+1; ++i) {
        	if(buNum[i]>1)bu[i] = (bu[i]/buNum[i]) - mean;
        	else bu[i] = 0.0;
        }
        
        for(i = 1; i < ITEM_NUM+1; ++i) {
        	if(biNum[i] > 1)bi[i] = (bi[i]/biNum[i]) - mean;
        	else bi[i] = 0.0;
        }
        
        //@todo ��֪���Ƿ�����Գ�ʼ���Ĺ�����һЩ�Ż�
        //��w���г�ʼ������ʼ���ķ����������������֪�����ַ����Ƿ�ã��Ƿ��Ӱ������������������
        for(int i = 1; i < ITEM_NUM+1; ++i){
            q[i].clear();            //������е�q[i]
            setRand(q[i],K_NUM+1,0);    //��ʼ��q[i]
            x[i].clear();            //������е�x[i]
            setRand(x[i],K_NUM+1,0);    //��ʼ��x[i]
            y[i].clear();            //������е�y[i]
            setRand(y[i],K_NUM+1,0);    //��ʼ��y[i]
        }
       
        cout <<"initialization end!"<<endl<< "begin iteration: " << endl;
        
        double pui = 0.0 ; // Ԥ���u��i�Ĵ��
        double preRmse = 1000000000000.0; //���ڼ�¼��һ��rmse����Ϊ��ֹ������һ�֣����rmse�����ˣ���ֹͣ
        double nowRmse = 0.0;
        for(int step = 0; step < 35; ++step){  //ֻ����35��
            double rmse = 0;
            double n = 0;
            
            for( u = 1; u < USER_NUM+1; ++u) {   //ѭ������ÿһ���û� 
               	int RuNum = userItems[u].size(); //�û�u����ֵ�item��Ŀ
               	cout<<RuNum<<endl;
               	double sqrtRuNum = (1.0/sqrt(RuNum));
               	
               	//���pu
               	for( k=1; k< K_NUM+1; ++k) {
               		double sumx = 0.0;
               		double sumy = 0.0;
               		
               		for(i = 0; i < RuNum; ++i) { 
               			int itemI = userItems[u][i];
               			double bui = mean + bu[u] + bi[itemI];
               			sumx += (rateMatrix[u][itemI]-bui) * x[itemI][k];
               			sumy += y[itemI][k];
               		}
               		pu[k] = sqrtRuNum*(sumx + sumy);
               	}
                
                for(i = 0; i < RuNum; ++i) {  // ѭ������u��ֹ���ÿһ��item
                	int itemI = userItems[u][i];
                	double bui,rui;
                	bui = mean + bu[u] + bi[itemI];
                	if(RuNum >1) {
                		rui = bui + sqrtRuNum * dot(q[itemI],pu);
                	}
                	else rui = bui;
                		
                	if(pui < 1) pui = 1;
                	if(pui > 5) pui = 5;
                	
                	double eui = rateMatrix[u][itemI] - rui;
                	rmse += eui * eui; ++n;
                	
                	cout<< pui <<'\t'<<rateMatrix[u][itemI]<<'\t'<<eui<<endl;
                	
                	bu[u] += alpha * (eui - beta * bu[u]);
                	bi[itemI] += alpha * (eui - beta * bi[itemI]);
                	
	               	for( k=1; k< K_NUM+1; ++k) {
	               		q[itemI][k] += alpha * (eui*pu[k] - beta*q[itemI][k]);
	               		x[itemI][k] += alpha * (eui*sqrtRuNum*q[itemI][k]*(rateMatrix[u][itemI]-bui) - beta*x[itemI][k]);
	               		y[itemI][k] += alpha * (eui*sqrtRuNum*q[itemI][k] - beta*y[itemI][k]);
	               	}
                } 
            }
            nowRmse =  sqrt(rmse / n);
            if( nowRmse >= preRmse && step > 5) break; //���rmse�Ѿ���ʼ�����ˣ�������ѭ��
            else
            	preRmse = nowRmse;
            cout << step << "\t" << nowRmse << endl;
            //alpha *= 0.992;    //�𲽼�Сѧϰ����
            //RMSEProbe(); 
        }
        //ratingAll(data);  //Ԥ�����еĽ��
        
        //RMSEProbe();  // ���ѵ�������
        return;
	    
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
    	return 0.0;
    	/*
    	int u = user;
    	int i =  item;
    	double sumBias = 0.0;
    	int ruK = 0;
    	for(int j=0; j < userItems[u].size(); ++j) {
    		if((sim[i][userItems[u][j]]) < simLevel[i])continue;
    		++ruK;
    		double buj = mean + bu[u] + bi[userItems[u][j]];
    		double ruj = rateMatrix[u][userItems[u][j]];// u �� j�Ĵ����ο��ٻ���أ�
    		sumBias += (ruj - buj) * w[i][userItems[u][j]] + c[i][userItems[u][j]];
    	}
    	double ret = 0.0;
    	if(ruK>1)
    	{
    		ret = mean + bu[u] + bi[i] + (1.0/sqrt(ruK)) * sumBias;//�����Ȳ���k���б仯����ȡk=�����
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
        /*
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
        /*
        for(int i = 0; i < data.size(); ++i){
            data[i].setPredict(rating(data[i]));
        }
        */
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
