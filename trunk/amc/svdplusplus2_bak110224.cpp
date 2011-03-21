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
    
    //����svd++��ʽԤ����ֵ
    double rating(int user,int item, vector<Rating> &data)
    {  
        //double pui = rating(data[i]); //Ԥ���֣��������
        //Ԥ���֣�Ԥ�ⷽ��Ϊ��1���ҳ�u������е�item��Ȼ������w(ij)������
        double sumPui = 0;
        double sumW = 0;
        
        for(int j=0; j< userItems[user].size();++j) {
        	//cout <<user<<'\t'<<item<<'\t'<<rateMatrix[user][userItems[user][j]]<<'\t'<<w[item][userItems[user][j]]<<endl;
        	sumPui += w[item][userItems[user][j]] * rateMatrix[user][userItems[user][j]]; //weight ����������ȷ��
        	sumW += w[item][userItems[user][j]];
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
        		predict = rating(atoi(rateDetail[0].c_str()), atoi(rateDetail[1].c_str()), data);
        		realValue = atoi(rateDetail[2].c_str());
        		rmse += (predict-realValue) * (predict-realValue);
        		++num;
        		//cout <<predict<<'\t'<< realValue<<endl;
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
        for(int step = 0; step < 25; ++step){  //ֻ����25��
        	
        	//ÿ�ε����Ĳ��裺��1����ȡÿ�����д�ֵ�err��2������ÿ��wij
            double rmse = 0;
            double n = 0;
            for(int ii = 0; ii < data.size(); ++ii){  //����ÿһ����ֵ��Ž��
                int i = ii;  //���ڼ�¼���õڼ�����ּ�¼���Ż�
                
                int user = data[i].user;      //userId
                int item = data[i].item;      //itemId
                double rui = data[i].value(); //rate
                //double pui = rating(data[i]); //Ԥ���֣��������
                //Ԥ���֣�Ԥ�ⷽ��Ϊ��1���ҳ�u������е�item��Ȼ������w(ij)������
                double sumPui = 0;
                double sumW = 0;
                for(int j=0; j< userItems[user].size();++j) {
                	//cout <<rateMatrix[user][item]<<'\t'<<w[item][userItems[user][j]]<<endl;return;
                	sumPui += w[item][userItems[user][j]] * rateMatrix[user][userItems[user][j]]; //weight ����������ȷ��
                	sumW += w[item][userItems[user][j]];
                }
                
                double pui = sumPui/sumW;
                double err = rui - pui;
                //cout <<sumPui<<'\t'<<sumW<< '\t' <<rui<<'\t'<<pui<<'\t'<<err << endl;
                rmse += err * err; ++n;
                data[i].error = err;
                data[i].pRate = pui;
                data[i].sumWij = sumW;
            }
            cout << step << "\t" << sqrt(rmse / n) << endl;
            
            //����wij�ķѵ�ʱ��ʵ��̫�����ܲ��ܼ���һ���أ���Ϊÿ�ε���ʱ�临�Ӷ�ΪITEM_NUM*USER_NUM*KnowRateNum
            //1682X943X80000 = 1269,6552,0000 ÿһ�ε�����Ҫ����1269�ڴΣ�������ļ����ٶ�2�ڴΣ�����Ҫ634.5�룬ʵ���Ͽ����ó˷�������һ�£�����ÿ�����˷����ٶ�ֻ��2000��Σ�����6345�룬��2��Сʱ��
            //Ӧ�����¿���
            
            for(int i=1; i< ITEM_NUM+1; ++i) {
            	for(int j=1; j< ITEM_NUM+1; ++j) {
            		//���ÿ��wij������Ҫ�������еĴ��
            		double sumTemp = 0;
            		for(int ii = 0; ii < data.size(); ++ii){  //����ÿһ����ֵ��Ž��
		                sumTemp += (data[ii].error*rateMatrix[data[ii].user][j])/data[ii].sumWij;
		            }
                	//ÿ�ΰ��չ�ʽ����wij
                	w[i][j] += alpha * ( sumTemp - beta * w[i][j]);
                	//cout << w[i][j]<< endl;
                }
            }
            alpha *= 0.92;    //�𲽼�Сѧϰ����
        }
   		//ΪʲôԤ������ʵ�ʲ����ôС�������ʲôԭ�򣿣�������ˣ�������ʲôԭ���أ��ܿ����ǹ�����ˣ��ò��Լ��������Ծ�֪����
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