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
    
    void getProbeReal();          //���probe��ʵ�ʴ��
    
    vector < vector<rateNode> > rateMatrix(USER_NUM+1);           //ʹ��һ��map����洢ϡ��Ĵ�־���                     
	float mean = 0;                         //ȫ�ֵ�ƽ��ֵ
    
    //��������
    void RMSEProbe(void);
    
    float predictRate(int user,int item);
    
    void model(int dim, float  alpha, float  beta)
    {
        cout << "begin initialization: " << endl;
        
        loadRating(DIR_PATH,rateMatrix);  //��ʼ�����
        //getProbeReal();
        //exit(0);
        
        mean = setMeanRating(rateMatrix); //��ƽ��ֵ����bu��bi��ֵ
        
        

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
        
        cout <<"initialization end!"<<endl<< "begin iteration: " << endl;
        
        float pui = 0.0 ; // Ԥ���u��i�Ĵ��
        double preRmse = 1000000000000.0; //���ڼ�¼��һ��rmse����Ϊ��ֹ������һ�֣����rmse�����ˣ���ֹͣ
        double nowRmse = 0.0;
        
        cout <<"begin testRMSEProbe(): " << endl;
        RMSEProbe();
        for(int step = 0; step < 60; ++step){  //ֻ����35��
            long double rmse = 0.0;
            int n = 0;
            for( u = 1; u < USER_NUM+1; ++u) {   //ѭ������ÿһ���û� 
        		float sum[K_NUM+1] = {0};   //���ڴ洢�м����sum
               	int RuNum = rateMatrix[u].size(); //�û�u����ֵ�item��Ŀ
               	float sqrtRuNum = 0.0;
               	if(RuNum>1) sqrtRuNum = (1.0/sqrt(RuNum));
               	
                //��������
                for(i=0; i < RuNum; ++i) {// ѭ������u��ֹ���ÿһ��item
                	int itemI = rateMatrix[u][i].item;
                	short rui = rateMatrix[u][i].rate; //ʵ�ʵĴ��
                	double bui = mean + bu[u] + bi[itemI];
                	pui = predictRate(u,itemI);
					
                	//cout<<u<<'\t'<<i<<'\t'<<pui<<'\t'<<rui<<endl;
                	
                	float eui = rui - pui;
                	if( isnan(eui) ) {// fabs(eui) >= 4.2 || 
                		cout<<u<<'\t'<<i<<'\t'<<pui<<'\t'<<rui<<"	"<<bu[u]<<"	"<<bi[itemI]<<"	"<<mean<<endl;
                		exit(1);
                	}
                	rmse += eui * eui; ++n;
                	if(n % 10000000 == 0)cout<<"step:"<<step<<"	n:"<<n<<" dealed!"<<endl;
                	
                	bu[u] += alpha * (eui - beta * bu[u]);
                	bi[itemI] += alpha * (eui - beta * bi[itemI]);
                } 
            }
            nowRmse =  sqrt( rmse / n);
            
            if( nowRmse >= preRmse && step >= 3) break; //���rmse�Ѿ���ʼ�����ˣ�������ѭ��
            else
            	preRmse = nowRmse;
            cout << step << "\t" << nowRmse <<'\t'<< preRmse<<" 	n:"<<n<<endl;
            RMSEProbe();  // ���ѵ�������
            
            //alpha *= 0.992;    //�𲽼�Сѧϰ����
            //RMSEProbe(); 
        }
        
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
	    outputbi.close();
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
       	float ret  = mean+bu[user] + bi[item];
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
