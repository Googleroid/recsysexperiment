/**
 * �������Ŀ����ʵ��koren��TKDD'09�����еķ�����learnFactorizedNeighborhoodModel�ķ���
 */
#include "movielens.h"
#include "mdefine.cpp"

namespace svd{
	//ʹ��һЩȫ�ֱ������洢��Ҫ���ƵĲ�����bu��bi,wij
    double bu[USER_NUM+1] = {0};
    double bi[ITEM_NUM+1] = {0};       //baselineԤ�����е��û�ƫ�ú�itemƫ��
    
    int buNum[USER_NUM+1] = {0};	//�û�u��ֵ�item������
    int biNum[ITEM_NUM+1] = {0};   //���item i�ֵ��û�����
    
    double p[USER_NUM+1][K_NUM+1] = {0};   //���ڴ洢�û�����������p
    double q[ITEM_NUM+1][K_NUM+1] = {0};   //����item����������q
    
    vector < vector<rateNode> > rateMatrix(USER_NUM+1);           //ʹ��һ��map����洢ϡ��Ĵ�־���                     
	float mean = 0;                         //ȫ�ֵ�ƽ��ֵ
    
    //��������
    void RMSEProbe(void);
    
    float predictRate(int user,int item);
    
    void model(int dim, float  alpha1, float alpha2, float beta1, float beta2)
    {
        cout << "begin initialization: " << endl;
        ofstream log("log.txt");
        ofstream logbi("log_bi.txt");
        
        loadRating(DIR_PATH,rateMatrix);  //��ʼ�����
        //getProbeReal();
        //exit(0);
        
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
        */
        srand((unsigned)time(0));
        //@todo ��֪���Ƿ�����Գ�ʼ���Ĺ�����һЩ�Ż�
        //��w���г�ʼ������ʼ���ķ����������������֪�����ַ����Ƿ�ã��Ƿ��Ӱ������������������
        for(int i = 1; i < ITEM_NUM+1; ++i){
            setRand(q[i],K_NUM+1,0);    //��ʼ��q[i]
        }
        
        for(int i = 1; i < USER_NUM+1; ++i){
            setRand(p[i],K_NUM+1,0);    //��ʼ��p[i]
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
            for( u = 1; u < USER_NUM+1; ++u) {   //ѭ������ÿһ���û� 
            	
               	int RuNum = rateMatrix[u].size(); //�û�u����ֵ�item��Ŀ
               	float sqrtRuNum = 0.0;
               	if(RuNum>1) sqrtRuNum = (1.0/sqrt(RuNum));
               	
                //��������
                for(i=0; i < RuNum; ++i) {// ѭ������u��ֹ���ÿһ��item
                	int itemI = rateMatrix[u][i].item;
                	short rui = rateMatrix[u][i].rate; //ʵ�ʵĴ��
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
	               		double tempPu = p[u][k];
	               		p[u][k] += alpha2 * (eui*q[itemI][k] - beta2*p[u][k]);
	               		q[itemI][k] += alpha2 * (eui*tempPu - beta2*q[itemI][k]);
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
            //RMSEProbe(); 
        }
        //ratingAll(data);  //Ԥ�����еĽ��
        log.close();
        logbi.close();
        RMSEProbe();  // ���ѵ�������
        return;
	    
    }
    
    //����svd++��ʽԤ����ֵ
    float rating(Rating  record){
        return  0.0;
    } 
    
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
	
    void ratingAll(vector< Rating*> & data){
        /*
        for(int i = 0; i < data.size(); ++i){
            data[i].setPredict(rating(data[i]));
        }
        */
    }
    
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
	    	rateValue = getRate(rateMatrix[userId],itemId);
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
    float alpha1 = 0.05;  //according the paper of koren: Factorization Meets the Neighborhood: SIGKDD'08
    float alpha2 = 0.05;    //according the paper of koren: Factorization Meets the Neighborhood: SIGKDD'08
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
    startInfo = localtime(&start);
    endInfo =   localtime(&end);
    cout << "start at" << asctime(startInfo) << ". And end at "<< asctime(endInfo) <<endl;
    cout << "duration:"<<duration <<" s!" <<endl;
    return 0;
}
