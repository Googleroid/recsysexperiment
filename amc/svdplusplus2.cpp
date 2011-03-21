/**
 *	����������ķ�����ֱ��ѧϰitem i ��j �����ƶȣ�
 *  
 */
#include "netflix.h"
#include "mdefineplus.cpp"

namespace svd{
    vector<float> bu(USER_NUM+1,0), bi(ITEM_NUM+1,0);    //�û�ƫ�ú�itemƫ��
    vector<int> un(USER_NUM+1,0), in(ITEM_NUM+1,0);    //�û�ƫ�ú�itemƫ��  //�ֱ��¼�û�u��ֵ�item������item q����ֵĴ���
    
    vector< vector<double> >  w(ITEM_NUM+1);  //�������ƾ���
    vector< vector<int> >  userItems(USER_NUM+1);   //�û�����ֵ�items-----����
    vector< vector <int> > rateMatrix(USER_NUM+1);  //��־���
	float mean = 0;
    
    /*
     *  Ԥ���֣�Ԥ�⹫ʽ
     *  r(ui) = (1/k)wi * (Ru)'
     */
    double rating(int user,int item, double &sumW)
    {  
    	sumW = 0;
    	double sumPui = 0.0;
    	//����1/k������Ru���������wi������
        for(int j=1; j< ITEM_NUM;++j) {
        	if(rateMatrix[user][j]) {
        		sumW += w[item][j];
        		sumPui += w[item][j] * rateMatrix[user][j];
        	}
        }
        
        double ret = sumPui/sumW;   
        
    	if(ret < 1) ret = 1;
    	if(ret > 5) ret = 5;
    	
    	//cout<<sumPui<<'\t'<<sumW<<'\t'<<endl;
    	return ret;
    }

	//�����Լ������������Լ�������ѵ�����õ��Ľ�����м��㡣
    void RMSEProbe(vector<Rating> &data){
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
        		double sumWij = 0.0;
        		predict = rating(atoi(rateDetail[0].c_str()), atoi(rateDetail[1].c_str()),sumWij);
        		realValue = atoi(rateDetail[2].c_str());
        		rmse += (predict-realValue) * (predict-realValue);
        		++num;
        		cout <<predict<<'\t'<< realValue<<endl;
        	}
        }
        cout << "test set rmse:"<<sqrt(rmse/num) << endl;
    }

	/*
    void ratingAll(vector< Rating > & data){
        for(int i = 0; i < data.size(); ++i){
            data[i].setPredict(rating(data[i]));
        }
    }
    */
     
    void model(int dim, double & alpha, double & beta)
    {
        vector<Rating> data;       
        
        //�ڳ�ʼ��֮ǰ�������ȶ�rateMatrix��ֵ�������еĴ�ָ�ֵΪ0
        for(int i = 1; i<USER_NUM+1;++i) {
        	for(int j = 0; j<ITEM_NUM+1; ++j) {
        		rateMatrix[i].push_back(0);
        	}
        }
        
        loadRating("u1.base",data,rateMatrix);  //��ʼ�����������ݣ���������ݴ洢��datavector��

        //1�����ȸ������������
        //��1����ƽ��ֵ
        //2��һ��һ��ʵ��ÿ������
        //3��ʲôʱ��Ū�꣬ʲôʱ���ȥ
        
        /*
        ���κ�����ʱ����Ҫƽ��ֵ���ȸĽ���ʱ������� 2011-2-22 by lvhl
        mean = setMeanRating(data); //��ƽ��ֵ����bu��bi��ֵ
        */
        
        //�м�ɾ��һ�δ��룬��xlvector����ƽ�����д����ʱ��Դ�ֵ�Ӱ��������õ�
        
        //��ʼ�����ƾ���w(ij)����0����random
        for(int i = 1; i < ITEM_NUM+1; ++i){
            w[i].clear();
            setRand(w[i],ITEM_NUM+1,0);
            //if(rateMatrix[1][i])cout << rateMatrix[1][i]<<endl;
        }
        //��ʼ��userItem���󣬽�ÿ��user����ֵ�item������
        for(int ii = 0; ii < data.size(); ++ii){ 
        	userItems[data[ii].user].push_back(data[ii].item);
        }
        
        //������������
        cout << "begin : " << endl;
        for(int step = 0; step < 100; ++step){  //�ȵ���25�ο���Ч��
            double rmse = 0;
            double n = 0;
            for(int ii = 0; ii < data.size(); ++ii){  //����ÿһ����ֵ��Ž��
            	//ÿ�ε����Ĳ��裺��1����ȡÿ�����д�ֵ�err��2������ÿ�����ݣ�������һ����������������
        		//�������µķ�����ѭ�������е�ÿ��Ԫ��
                int i = ii;  //���ڼ�¼���õڼ�����ּ�¼���Ż�
                
                int user = data[i].user;      //userId
                int item = data[i].item;      //itemId
                double rui = data[i].value(); //rate
                double sumWij = 0.0;
                double pui = rating(user,item, sumWij); //Ԥ���֣��������
                
                double err = rui - pui;
                //cout <<sumPui<<'\t'<<sumW<< '\t' <<rui<<'\t'<<pui<<'\t'<<err << endl;
                rmse += err * err; ++n;
                data[i].error = err;
                data[i].pRate = pui;
                
                //����Ĵ������ÿһ��wij
                for(int j=1; j < ITEM_NUM+1 ; ++j)
                {
                	// p[user][k] += alpha * (err * q[item][k] - beta * p[user][k]);
                	w[item][j] += alpha * (err * (1/sumWij) * rateMatrix[user][j] - beta * w[item][j]);
                }
            }
            cout << step << "\t" << sqrt(rmse / n) << endl;
            
            alpha *= 0.9;    //�𲽼�Сѧϰ����
        }
   		
        RMSEProbe(data);  // ���ѵ�������
        return ;
        /*
        ratingAll(data);  //Ԥ�����еĽ��
        
        
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
    svd::model(dim,alpha,beta);
    outputfile.close();
    end = clock();
    duration = (end-start)/CLOCKS_PER_SEC;
    cout << "duration:"<<duration <<" s!" <<endl;
    return 0;
}