/**
 * �������Ŀ����ʵ��koren��TKDD'09�����еķ�����learnFactorizedNeighborhoodModel�ķ���
 */
#include "netflix.h"
#include "mdefine.cpp"

namespace svd{
	//ʹ��һЩȫ�ֱ������洢��Ҫ���ƵĲ�����bu��bi,wij
    double bu[USER_NUM+1] = {0};
    double bi[ITEM_NUM+1] = {0};       //baselineԤ�����е��û�ƫ�ú�itemƫ��
    int buNum[USER_NUM+1] = {0};	//�û�u��ֵ�item������
    int biNum[ITEM_NUM+1] = {0};   //���item i�ֵ��û�����
    
    float q[ITEM_NUM+1][K_NUM+1]; //Item����
    float x[ITEM_NUM+1][K_NUM+1]; //
    float y[ITEM_NUM+1][K_NUM+1]; //
    
    vector < vector<rateNode> > rateMatrix(USER_NUM+1);           //ʹ��һ��map����洢ϡ��Ĵ�־���                     
	float mean = 0;                         //ȫ�ֵ�ƽ��ֵ
    
    //��������
    void RMSEProbe(void);
    
    float predictRate(int user,int item);
    
    void model(int dim, float  alpha, float  beta)
    {
        cout << "begin initialization: " << endl;
        ofstream log("log.txt");
        ofstream logbi("log_bi.txt");
        
        loadRating(DIR_PATH,rateMatrix);  //��ʼ�����
        
        
        mean = setMeanRating(rateMatrix); //��ƽ��ֵ����bu��bi��ֵ
        
        
        float pu[K_NUM+1] = {0};   //���ڴ洢�м����puk
        int i,u,j,k;
        
        //��bu��bi���г�ʼ��,bu,bi�ĳ�ʼ���ķ�������ƽ��ֵ��Ȼ���ȥmean��
        //�ڼ���Ĺ����б���Ҫ�ǵ����е�ֵ���������еĴ�������ͷ����ܺ�
        int tmpIndex = 0;
	    for(i = 1; i < USER_NUM+1; ++i){
	    	int vSize = rateMatrix[i].size();
			for(j=0; j < vSize; ++j) {
				bu[i] += rateMatrix[i][j].rate;
				buNum[i] += 1;
				bi[rateMatrix[i][j].item] += rateMatrix[i][j].rate;
				biNum[rateMatrix[i][j].item] += 1;
        		
			}			
	    }
        
        //���¹�����ƽ��ֵ
        for(i = 1; i < USER_NUM+1; ++i) {
        	if(buNum[i]>=1)bu[i] = (bu[i]/buNum[i]) - mean;
        	else bu[i] = 0.0;
        	//log<<i<<'\t'<<bu[i]<<endl;
        }
        
        for(i = 1; i < ITEM_NUM+1; ++i) {
        	if(biNum[i] >=1)bi[i] = (bi[i]/biNum[i]) - mean;
        	else bi[i] = 0.0;
        	//logbi<<i<<'\t'<<bi[i]<<endl;
        }
        //@todo ��֪���Ƿ�����Գ�ʼ���Ĺ�����һЩ�Ż�
        //��w���г�ʼ������ʼ���ķ����������������֪�����ַ����Ƿ�ã��Ƿ��Ӱ������������������
        for(int i = 1; i < ITEM_NUM+1; ++i){
            setRand(q[i],K_NUM+1,0);    //��ʼ��q[i]
            setRand(x[i],K_NUM+1,0);    //��ʼ��x[i]
            setRand(y[i],K_NUM+1,0);    //��ʼ��y[i]
        }
       
        cout <<"initialization end!"<<endl<< "begin iteration: " << endl;
        
        float pui = 0.0 ; // Ԥ���u��i�Ĵ��
        double preRmse = 1000000000000.0; //���ڼ�¼��һ��rmse����Ϊ��ֹ������һ�֣����rmse�����ˣ���ֹͣ
        double nowRmse = 0.0;
        
       
        for(int step = 0; step < 35; ++step){  //ֻ����35��
            long double rmse = 0.0;
            int n = 0;
            for( u = 1; u < USER_NUM+1; ++u) {   //ѭ������ÿһ���û� 
               	int RuNum = rateMatrix[u].size(); //�û�u����ֵ�item��Ŀ
               	float sqrtRuNum = 0.0;
               	if(RuNum>1) sqrtRuNum = (1.0/sqrt(RuNum));
               	
               	//���pu
               	for( k=1; k< K_NUM+1; ++k) {
               		float sumx = 0.0;
               		float sumy = 0.0;
               		
               		for(j=0; j < RuNum; ++j) {
               			int itemI = rateMatrix[u][j].item;
               			short rui =  rateMatrix[u][j].rate;
               			float bui = mean + bu[u] + bi[itemI];
               			sumx += (rui-bui) * x[itemI][k];
               			sumy += y[itemI][k];
               		}
               		pu[k] = sqrtRuNum*(sumx + sumy);
               	}
                //��������
                for(i=0; i < RuNum; ++i) {// ѭ������u��ֹ���ÿһ��item
                	int itemI = rateMatrix[u][i].item;
                	short rui = rateMatrix[u][i].rate; //ʵ�ʵĴ��
                	double bui = mean + bu[u] + bi[itemI];
                	if(RuNum >1) {
                		pui = bui + dot(q[itemI],pu);
                	}
                	else pui = bui;
                		
                	if(pui < 1) pui = 1;
                	if(pui > 5) pui = 5;
                	
                	//cout<<u<<'\t'<<i<<'\t'<<pui<<'\t'<<rui<<endl;
                	
                	float eui = rui - pui;
                	if( fabs(eui) >= 3 && step > 1) {
                		cout<<u<<'\t'<<i<<'\t'<<pui<<'\t'<<rui<<"	"<<bu[u]<<"	"<<bi[itemI]<<"	"<<mean<<endl;
                	}
                	rmse += eui * eui; ++n;
                	if(n % 1000000 == 0)cout<<"step:"<<step<<"	n:"<<n<<" dealed!"<<endl;
                	bu[u] += alpha * (eui - beta * bu[u]);
                	bi[itemI] += alpha * (eui - beta * bi[itemI]);
                	
	               	for( k=1; k< K_NUM+1; ++k) {
	               		q[itemI][k] += alpha * (eui*pu[k] - beta*q[itemI][k]);
	               		x[itemI][k] += alpha * (eui*sqrtRuNum*q[itemI][k]*(rui-bui) - beta*x[itemI][k]);
	               		y[itemI][k] += alpha * (eui*sqrtRuNum*q[itemI][k] - beta*y[itemI][k]);
	               	}
                } 
            }
            nowRmse =  sqrt( rmse / n);
            if( nowRmse >= preRmse && step > 5) break; //���rmse�Ѿ���ʼ�����ˣ�������ѭ��
            else
            	preRmse = nowRmse;
            cout << step << "\t" << nowRmse <<'\t'<< rmse<<endl;
            //alpha *= 0.992;    //�𲽼�Сѧϰ����
            //RMSEProbe(); 
        }
        //ratingAll(data);  //Ԥ�����еĽ��
        log.close();
        logbi.close();
        //RMSEProbe();  // ���ѵ�������
        return;
	    
    }
    
    //����svd++��ʽԤ����ֵ
    float rating(Rating  record){
        /*
        int user = record.user;
        int item = record.item;
        float ret = mean + bu[user] + bi[item] + dot(p[user], q[item]);
        if(ret < 1) ret = 1;
        if(ret > 5) ret = 5;
        return ret;
        */
        return  0.0;
    } 
    
    float predictRate(int user,int item)
    {
    	return 0.0;
    	/*
    	int u = user;
    	int i =  item;
    	float sumBias = 0.0;
    	int ruK = 0;
    	for(int j=0; j < userItems[u].size(); ++j) {
    		if((sim[i][userItems[u][j]]) < simLevel[i])continue;
    		++ruK;
    		float buj = mean + bu[u] + bi[userItems[u][j]];
    		float ruj = rateMatrix[u][userItems[u][j]];// u �� j�Ĵ����ο��ٻ���أ�
    		sumBias += (ruj - buj) * w[i][userItems[u][j]] + c[i][userItems[u][j]];
    	}
    	float ret = 0.0;
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
        float predict = 0.0;
        float rmseSum = 0.0;
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
	
    void ratingAll(vector< Rating*> & data){
        /*
        for(int i = 0; i < data.size(); ++i){
            data[i].setPredict(rating(data[i]));
        }
        */
    }
};

int main(int argc, char ** argv)
{
	float start,end,duration; 
	start = clock();
    float alpha = 0.005;  //���������ֵ�ȽϺ�
    float beta = 0.002;   //�����������Ҳ����
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
