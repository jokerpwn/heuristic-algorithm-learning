#include <iostream>
#include <ctime>
#include <string>
#include <cstdlib>
#include <vector>
#include <fstream>
#include <ostream>

using namespace std;

const int MaxIter=100000;
void string2arc(const string &a,vector<int> &b,const string &delim){
    string::size_type pos1=a.find(delim),pos2=a.find(delim,pos1+1);
    string str1=a.substr(pos1,pos2-pos1),str2=a.substr(pos2);
    const char *num_str1=str1.c_str(),*num_str2=str2.c_str();
    b[0]=atoi(num_str1);
    b[1]=atoi(num_str2);

}
void string2vex(const string &a,vector<int> &b,const string &delim){
    string::size_type pos0=a.find(delim),pos1=a.find(delim,pos0+1)+1,pos2=a.find(delim,pos1+1);
    string str1=a.substr(pos1,pos2-pos1);
    string str2=a.substr(pos2);

    const char *num_str1=str1.c_str(),*num_str2=str2.c_str();
    b.push_back(atoi(num_str1));
    b.push_back(atoi(num_str2));
    cout<<atoi(num_str1)<<" "<<atoi(num_str2)<<endl;

}

class tabu_search{
private:
    bool tag=false;
    int *Sol;//节点对应颜色值
    int f;//冲突值
    int f_table[2][1000]={0};//冲突维护表
    int *BestSol;//最优解
    int best_f;//历史最优冲突值
    long long **TabuTenure;//禁忌表
    int **Adjacent_Color_Table;//邻接颜色表
    int N,e;//点数，边数
    int K;//颜色数

    int **graph;//图--记录邻接边,方便更新邻接颜色表
    long long iter;//步数记录
    int tabu_best_move[2];//记录当前禁忌表中BEST_MOVE
    int non_tabu_best_move[2];//记录当前非禁忌表中BEST_MOVE
public:
    void put_color_num(int color_num){
        K=color_num;
    }
//初始化各数据结构
    void Initialization(){
        ifstream infile("DSJC500.5.col.txt",ios::in);
        string delim=" ";
        string str;
        if(!infile.good())
        {
            cout<<"file open error"<<endl;
            return;
        }
        vector<int> num;
        f=0;

        while(!infile.fail()){
            getline(infile,str);

            if(str.find("p")==0){

                string2vex(str,num,delim);
                N=num[0];
                e=num[1];

                //内存分配
                Sol=new int[N];
                Adjacent_Color_Table=new int *[N];
                graph=new int*[N];
                TabuTenure=new long long*[N];


                //数值初始化
                for(int i=0;i<N+1;i++){
                    Adjacent_Color_Table[i]=new int[K];
                    TabuTenure[i]=new long long[K];
                    graph[i]=new int[N];

                    for(int j=0;j<K;j++) {
                        Adjacent_Color_Table[i][j] = 0;
                        TabuTenure[i][j] = 0;
                    }
                    graph[i][0]=0;

                    for(int j=1;j<N;j++)
                        graph[i][j]=-1;

                }

                for(int i=0;i<N;i++){
                    Sol[i]=rand()%K;//初始解
                }
            }
            else if(str.find("e")==0){
                string2arc(str,num,delim);
                int first=num[0]-1,next=num[1]-1;//对应数组下标

                //邻接颜色表统计+图数据初始化
                graph[first][0]++;
                graph[next][0]++;
                graph[first][graph[first][0]]=next;
                graph[next][graph[next][0]]=first;

                Adjacent_Color_Table[first][Sol[next]]++;
                Adjacent_Color_Table[next][Sol[first]]++;

                if(Sol[first]==Sol[next]){
                    //冲突数记录
                    f++;
                    if(!f_table[1][first]){
                        f_table[0][0]++;
                        f_table[0][f_table[0][0]]=first;
                        f_table[1][first]=f_table[0][0];
                    }
                    if(!f_table[1][next]){
                        f_table[0][0]++;
                        f_table[0][f_table[0][0]]=next;
                        f_table[1][next]=f_table[0][0];
                    }

                }
            }

        }
        best_f=f;
        infile.close();
    }

    void FindMove(int &u,int &vi,int &vj,int &delt){
        delt=10000;
        int delt1=10000;

        for(int i=0;i<N;i++){//全部查找一遍

            if(Adjacent_Color_Table[i][Sol[i]]>0){//存在颜色冲突

                int count=1;//记录delt相同情况

                for(int k=0;k<K;k++){//遍历颜色搜索冲突min

                    if(k==Sol[i]) continue;

                    int tmp_delt=Adjacent_Color_Table[i][k]-Adjacent_Color_Table[i][Sol[i]];//计算DELT

                    if(tmp_delt>delt)
                        continue;

                    //非禁忌移动
                        if(TabuTenure[i][k]<iter)
                        {
                            bool move=true;
                            if(tmp_delt==delt)//与当前DELt重合,1/count概率选择
                            {
                                count++;
                                float possibility=rand()%(1000)/(float)(1000.0);
                                if(possibility<(float)(1.0/count)) {
                                    move =false;
                                }
                            }
                            else
                                count=1;

                            if(move){
                                delt=tmp_delt;
                                //记录移动情况
                                non_tabu_best_move[0]=i;
                                non_tabu_best_move[1]=k;
                            }

                        }
                     //禁忌移动
                        else{
                            if(tmp_delt<=delt1&&tmp_delt<=delt){
                                bool move=true;
                                if(tmp_delt==delt1)
                                {
                                    count++;
                                    float possibility=rand()%(1000)/(float)(1000.0);
                                    if(possibility<(float)(1.0/count))
                                        move=false;
                                }
                                else
                                    count=1;

                                if(move)
                                {
                                    delt1=tmp_delt;
                                    tabu_best_move[0]=i;
                                    tabu_best_move[1]=k;
                                }
                            }
                        }
                }//结束当前节点的颜色判断
            }

        }//结束统计比较


        //最终判断
        if(f+delt1<best_f)
        {
            delt=delt1;
            u=tabu_best_move[0];
            vj=tabu_best_move[1];
        }
        else{
            u=non_tabu_best_move[0];
            vj=non_tabu_best_move[1];
        }

        vi=Sol[u];
        //更新禁忌表
        TabuTenure[u][vi]+=iter;
    }

    void MakeMove(int &u,int &vi,int &vj, int &delt){

        Sol[u]=vj;
        f=f+delt;
        if(best_f>f)
            best_f=f;
        TabuTenure[u][vi]=iter+f+rand()%10;

        //邻接颜色表+冲突表更新
        for(int m=1;graph[u][m]!=-1;m++){
               int i=graph[u][m];

                Adjacent_Color_Table[i][vi]--;
                Adjacent_Color_Table[i][vj]++;

                if(vi==Sol[i]&&f_table[1][i]&&!Adjacent_Color_Table[i][vi]){//若修改颜色后冲突消除,在冲突表中删除
                    int pos=f_table[1][i],count=f_table[0][0];
                    f_table[0][pos]=f_table[0][count];
                    f_table[1][i]=0;
                    f_table[0][0]--;
                }
                else if(vj==Sol[i]&&!f_table[1][i]){//修改颜色后产生新冲突
                    int pos=f_table[0][0]+1;
                    f_table[0][pos]=i;
                    f_table[1][i]=pos;
                    f_table[0][0]++;
                }

        }

        if(!Adjacent_Color_Table[u][vj]&&f_table[1][u]){
            int pos=f_table[1][u],count=f_table[0][0];
            f_table[0][pos]=f_table[0][count];
            f_table[1][u]=0;
            f_table[0][0]--;
        }


    }
    void run(){
        int u,vi,vj;
        iter=0;
        int delt=10000;
        int cur_count;
        int pause;
        srand(clock());

        Initialization();
        cur_count=f_table[0][0];

        double  start_time,end_time,elapsed_time;
        start_time=clock();

        while(cur_count>0){//冲突仍存在
            iter++;
            FindMove(u,vi,vj,delt);
            MakeMove(u,vi,vj,delt);
            cur_count=f_table[0][0];
            if(iter%10000==0)
                cout<<u<<" "<<vi<<" "<<vj<<" "<<iter<<" "<<cur_count<<endl;
        }
        end_time=clock();
        elapsed_time=(end_time - start_time)/CLOCKS_PER_SEC;

        cout<<"颜色数"<<K<<"解具体情况："<<endl;
        for(int i=0;i<N;i++)
            cout<<i+1<<" "<<Sol[i]<<endl;
        cout<<"时间"<<elapsed_time<<endl;

    }

};
int main(){
    tabu_search init;
    int color_num;
    cin>>color_num;
    init.put_color_num(color_num);

    init.run();
}