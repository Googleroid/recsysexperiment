float test_level = 0.004;
using namespace std;
void explode(const char * probe, char * data,vector<string> &result);
Rating::Rating(int u_item, int u_user, int u_criteria, float u_rate)
{
    item = u_item;
    user = u_user;
    rate = u_rate;
    criteria = u_criteria;
}

double Rating::predict()
{
    return 0.0;
}

void Rating::setPredict(float rating)
{
    rate = rating;
}

float Rating::value()
{
    return rate;
}
int Rating::year()
{
    return yearValue;
}

int Rating::month()
{
    return monthValue;
    int good =10;
}


float dot(vector<float> &p, vector<float> &q)
{
    float result = 0.0;
    for (int i=0; i<p.size(); ++i){
        result += p[i]*q[i];
    }
    return result;
}

/**
 * load filePath�е����ݵ�data���vector�к� rateMatrix��
 * ����Ҫѭ��Ŀ¼
 */
void loadRating(char * dirPath, vector<Rating> &data, map<int,int> rateMatrixLocal[USER_NUM+1][CRI_NUM+1])
{
    char rateStr[256];
    DIR *dp;
 	struct dirent *dirp;
    vector<string> rateDetail(10);
    if((dp  = opendir(dirPath)) == NULL) {
        cout << "Error(" << errno << ") opening " << dirPath << endl;
   		return;
    }

    while ((dirp = readdir(dp)) != NULL) {
        
        string fileName = dirPath + string(dirp->d_name);
        //cout <<fileName<<endl;
        std::ifstream from (fileName.c_str());
        int itemId =  atoi(string(dirp->d_name).c_str());
        while(from.getline(rateStr,256)){
	    	string strTemp(rateStr);
	    	int pos = strTemp.find(":");
	    	string userId = strTemp.substr(0,pos-1);
	    	
	    	strcpy(rateStr,strTemp.substr(pos+1).c_str());
	        explode("	",rateStr,rateDetail);
	        
	        if(rateDetail.size()>=3){
	        	for(int c = 0; c<rateDetail.size() && c < 8; ++c) {
	        		Rating aRate(itemId,atoi(userId.c_str()),c+1,atof(rateDetail[c].c_str()));
					aRate.test = 0; //�������е����ݶ���ѵ�����ݣ�������������洢        	
	        		data.push_back(aRate);
	        		//��ʼ��rateMatrix
	        		rateMatrixLocal[atoi(userId.c_str())][c+1][itemId] = atof(rateDetail[c].c_str());
	        	}
	        }
	        
	    }
	    from.close();
    }
    return;
}

void explode(const char * probe,  char * data ,vector<string> &result)
{
    string dataStr(data);
    int pos1 = 0;
    int pos2 = 0;
    int i=0;
    while((pos2=dataStr.find(probe,pos1)) != string::npos){
        result[i] = (dataStr.substr(pos1,pos2-pos1));
        ++i;
        //if(i>=3)break;
        pos1=pos2+1;
    }
    result[i] = (dataStr.substr(pos1));
}

/**
 * ����ȫ�ֵ�ƽ��ֵ
 */
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
    	}  			
    }
    return sum/num;
}

double get_rand()
{
    return (rand()%1000-500)/5000.0;
}

/**
 * setRand��ֵ
 */
float setRand(vector<double> &p, int num, float base)
{
	srand((unsigned)time(0));
    for(int i=0;i<num;++i){
    	double temp = base+get_rand();
        p.push_back(temp);
        //p.push_back(base+0.05); //ȫ����ʼ��Ϊbase������Ӱ�����
        //cout << i <<"	"<< temp <<"	"<< endl;
    }
}




void ratingProbe(const char * filePath, vector<Rating> &data)
{

}

void saveRating(const char * filePath, vector<Rating> &data)
{
}

void ratingQuiz(const char * filePath, vector<Rating> &data)
{
}