#include "netflix.h"
#include "mdefine.cpp"

namespace svd{
    vector< vector<float> > p(USER_NUM+1), q(ITEM_NUM+1);  //��������Ծ���
    float mean = 0;
    
    //����svd++��ʽԤ����ֵ
    double rating(Rating  record){
        int user = record.user;
        int item = record.item;
        //double ret = record.predict() + bu[user] + bi[item] + dot(p[user], q[item]);
        double ret = mean + dot(p[user], q[item]);
        if(ret < 1) ret = 1;
        if(ret > 5) ret = 5;
        return ret;
    }
    
    double rating(int user,int item)
    {
    	double ret = mean + dot(p[user], q[item]);
    	if(ret < 1) ret = 1;
    	if(ret > 5) ret = 5;
    	return ret;
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
        float rmse = 0.0;
        int num = 0;
        int realValue = 0;
        while(from.getline(rateStr,100)){
        	explode("	",rateStr,rateDetail);
        	if(rateDetail.size()>=3){
        		predict = rating(atoi(rateDetail[0].c_str()), atoi(rateDetail[1].c_str()));
        		realValue = atoi(rateDetail[2].c_str());
        		rmse += pow(predict-realValue,2);
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
        if(data.size() == 0)loadRating("u1.base",data);  //��ʼ��
        
        //1�����ȸ������������
        //��1����ƽ��ֵ
        //2��һ��һ��ʵ��ÿ������
        //3��ʲôʱ��Ū�꣬ʲôʱ���ȥ
        
        if(mean == 0)mean = setMeanRating(data); //��ƽ��ֵ����bu��bi��ֵ
        
        //�м�ɾ��һ�δ��룬��xlvector����ƽ�����д�����õ�
        
        double cc = 1 / (double)(dim);
        //cout << cc << endl;exit(1);
        
        //��ʼ��bu������user��bias��ƫ�ã�
        for(int i = 1; i < USER_NUM+1; ++i){
            //if(bu[i] < 0) continue;  //����bu[i]С��0��Զ����������������Զ����ִ��
            p[i].clear();            //������е�p[i]
            setRand(p[i],dim,cc);    //��ʼ��p[i]
        }
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
                for(int k = 0; k < dim; ++k){
                    p[user][k] += alpha * (err * q[item][k] - beta * p[user][k]);
                    q[item][k] += alpha * (err * p[user][k] - beta * q[item][k]);
                }
            }
            if(step == 24)cout << alpha <<"	"<<step << ":\t" << sqrt(rmse / n) << endl;  
            alpha *= 0.98;    //�𲽼�Сѧϰ����
        }
        ratingAll(data);  //Ԥ�����еĽ��
        RMSEProbe();  // ���ѵ�������
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
    cout << "duration:"<<duration <<" s!" << CLOCKS_PER_SEC<<endl;
    return 0;
} 