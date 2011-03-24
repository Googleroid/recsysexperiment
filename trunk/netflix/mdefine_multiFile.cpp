float test_level = 0.004;
using namespace std;
void explode(const char * probe,  const char * data ,vector<string> &result);
void qsort(vector<float>& array, int start, int end);
int  partition(vector<float>& array, int start, int end);
bool cmp(float a, float b);
Rating::Rating(short u_item,int u_user,short u_rate)//float u_rate,int year=1970,int month=1,int day=1)
{
    item = u_item;
    user = u_user;
    rate = u_rate;
    //yearValue = year;
    //monthValue = month;
    //dayValue = day;
}


void Rating::setPredict(int rating)
{
    rate = rating;
}

short Rating::value()
{
    return rate;
}
/*
float Rating::predict()
{
    return 0.0;
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
*/

float dot(vector<float> &p, vector<float> &q)
{
	if(p.size() != q.size())return 0.0;
    float result = 0.0;
    for (int i=1; i<p.size(); ++i){
        result += p[i]*q[i];
    }
    return result;
}

/**
 * load filePath�е����ݵ�data���vector�к� rateMatrix��
 * 
 */
void loadRating(char * dirPath, vector<Rating> &data, map<int,int> rateMatrixLocal[USER_NUM+1])
{
    char rateStr[256];
    DIR *dp;
 	struct dirent *dirp;
    vector<string> rateDetail(10);
    vector<string> dayDetail(4);
    if((dp  = opendir(dirPath)) == NULL) {
        cout << "Error(" << errno << ") opening " << dirPath << endl;
   		return;
    } 
    int fileNum = 0;
    while ((dirp = readdir(dp)) != NULL) {
        if(string(dirp->d_name).length() < 3)continue;
        string fileName = dirPath + string(dirp->d_name);
        //cout <<fileName<<endl;
        std::ifstream from (fileName.c_str());
        string strTemp(dirp->d_name);
	    int pos = strTemp.find(".");
	    int itemId = atoi(strTemp.substr(0,pos).c_str());
	    //cout<< itemId<<'\t'<<strTemp<<"   end"<<endl;
	      
        while(from.getline(rateStr,256)){
	    	
	    	explode(",",rateStr,rateDetail);
	        if(rateDetail.size()>=3){
	        	int userId = atoi(rateDetail[0].c_str());
	        	Rating aRate(itemId,userId,atoi(rateDetail[1].c_str()));
	        	data.push_back(aRate);
	        	//��ʼ��rateMatrix
	        	//rateMatrixLocal[userId][itemId] = atoi(rateDetail[1].c_str());
	        }	        
	    }
	    from.close();
	    ++fileNum;	 
	    if(fileNum %100 ==0)cout<<"read file "<<fileNum<<" sucessfully!"<<endl;
    }
    //cout<<fileNum<<endl;
    //exit(1);
    return;
}

void explode(const char * probe,  const char * data ,vector<string> &result)
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
float setMeanRating(vector<Rating> &data)
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

float get_rand()
{
    return (rand()%1000-500)/5000.0;
}

/**
 * setRand��ֵ
 */
float setRand(vector<float> &p, int num, float base)
{
	srand((unsigned)time(0));
    for(int i=0;i<num;++i){
    	float temp = base+get_rand();
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


//����������������������С���ҳ���k���Ԫ��
float getKmax(vector<float>& array, int K)
{
	vector<float> heapTmp;
	for(int i=0; i < array.size(); ++i)
	{
		heapTmp.push_back(array[i]);
		if(i == K-1) {
			make_heap(heapTmp.begin(),heapTmp.end(),cmp);
		}
		else if(i >=K) {
			push_heap(heapTmp.begin(),heapTmp.end(),cmp);
			pop_heap(heapTmp.begin(),heapTmp.end(),cmp);
			heapTmp.pop_back();
		}
		//cout << i<<'\t'<<heapTmp.size()<<endl;
	}
	return heapTmp.front();
}

bool cmp(float a, float b)
{
	return a > b;
}

//����ļ�����������ʵ�ֿ�������
void qsort(vector<float>& array, int start, int end)
{
    if(start >= end)return;
    //cout<<"begin partition:"<<endl;
    int q = partition(array,start,end);
    //cout<< q<<endl;
    qsort(array,start,q-1);
    qsort(array,q+1,end);
}

int partition(vector<float>& array, int start,int end)
{
    if(start >= end) return start;
    if(start < 0) start = 0;
    int original  = start;
    float tmp = array[start];
    ++start;
    while(1) {
        while( start<=end && array[start] >= tmp)++start;
        while( start<=end && array[end] < tmp)--end;
        if(start < end) {
        	swap(array[start],array[end]);
        }
        else break;
    }
    swap(array[start-1],array[original]);
    return start-1;
}

void swap(float& a, float& b)
{
    float tmp = a;
    a = b;
    b = tmp;
}
